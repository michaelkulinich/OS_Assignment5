#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include "sharedBuffer.c"
#include "ip_checksum.c"



///////////////////////
// Global Objects
//

    // name of shared memory
    const char *name = "OS-IPC";

    // number of items in the shared buffer
    int nitems;  // change this

    // shared buffer of limited size of [argument 2]
    sharedBuffer* sBuffer;

    // semaphores
    sem_t empty, full;

    // mutex
    pthread_mutex_t mutex;

    // check sums
    unsigned short cksum1, cksum2;

    void *producer(void *arg);
    void *consumer(void *arg);
    void sig_handler(int sig);

///////////////////////



///////////////////////
// Initialize Main Method
//
  int main(int argc, char *argv[])
  {
    // ensure file is executed corrctly
    if (argc != 2) {
        printf("Usage Error: inlcude number of bits \n");
        return -1;
    }
    if (atoi(argv[1]) > 480 || atoi(argv[1]) <= 0) {
        printf("Usage Error: enter a positive number of items under 480\n");
        return -1;
    }
    nitems = atoi(argv[1]);

    // set up signal handler to exit peacefully
    struct sigaction act;
    act.sa_handler = sig_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGINT, &act, 0);
///////////////////////



///////////////////////
// Initialize
//

    // shared memory file descriptor
    int   shm_fd;

    // pointer to shared buffer
    void  *ptr;

///////////////////////



///////////////////////
// Set up shared memory object
//

    /* create the shared memory object */
    shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("Error creating shared memory");
        return -1;
    }

    // configure the size of the shared memory object
    ftruncate(shm_fd, MMAP_SIZE);

    // memory map the shared memory object
    ptr = mmap(0, MMAP_SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);

    // dynamically allocate memory for the buffer
    sBuffer = (sharedBuffer*)ptr; // we have the pointer to a blank object, just cast it to the abstraction we want to use!
    sBuffer->buffer = (item *) malloc(nitems * sizeof(item));
    sBuffer->buffer_size = nitems;
    sBuffer->in = 0;
    sBuffer->out = 0;

    /////printf("Successfully created shared memory object \n");

///////////////////////



///////////////////////
// Locking Mechanics
//

    // create the empty semaphore
    if (sem_init(&empty, 0, nitems) != 0) {
        perror("Semaphore initialization failed");
        return -1;
    }

    // create the full semaphore
    if (sem_init(&full, 0, 0) != 0) {
        perror("Semaphore initialization failed");
        return -1;
    }

    // create the mutex
    if (pthread_mutex_init(&mutex, 0)) {
        perror("Semaphore initialization failed");
        return -1;
    }

///////////////////////



///////////////////////
// Threads and Process Execution
//
    // thread identifier for producer and consumer
    pthread_t prod_id, cons_id;

    printf("\33[31m Producer is in red \33[0m\n");
    printf("\33[32m Consumer is in green \33[0m\n");

    // create the threads
    pthread_create(&prod_id, NULL, producer, NULL);
    // sleep(5);
    pthread_create(&cons_id, NULL, consumer, NULL);

    // join the threads
    pthread_join(prod_id, NULL);
    pthread_join(cons_id, NULL);
}
///////////////////////



///////////////////////
// Producer Behavior
//

void *producer(void *arg)
{
    item next_produced;
    next_produced.item_no = -1;
    while (1) {
        /* 1. Increment the buffer count (item_no)  */
        next_produced.item_no+=1;

        /* 2. Generate the payload data             */
        for (int i=0 ; i<34 ; i++)
            next_produced.payload[i] = (unsigned char) rand() % 256;

        /* 3. Calculate the 16-bit checksum (cksum) */
        next_produced.cksum = ip_checksum(next_produced.payload, PAYLOAD_SIZE);

        // wait for open space in buffer
        sem_wait(&empty);

        // lock critical section
        pthread_mutex_lock(&mutex);

        /////////////////////
        // critical section

        // push item to buffer
        sbuffPush(sBuffer, next_produced);

        // verify checksum
        if (sBuffer->buffer[sBuffer->in].cksum = ip_checksum(next_produced.payload, PAYLOAD_SIZE)){
            printf("\33[31mProduced: Item -> shm (no.%d, cksum: 0x%x) \33[0m \n", sBuffer->buffer[sBuffer->in].item_no, sBuffer->buffer[sBuffer->in].cksum);
        } else {
            printf("Corruption Error: item corrupted while being pushed to shared memory \n");
            exit(1);
        }
        incIn(sBuffer);

        //
        /////////////////////

        // signal item has been added
        sem_post(&full);

        // unlock critical section
        pthread_mutex_unlock(&mutex);
        usleep(5e3);
    }
}
///////////////////////



///////////////////////
// Consumer Behavior
//

void *consumer(void *arg)
{
    item next_consumed;
    next_consumed.item_no = -1;
    int item_no_prev;
    int repeat = 0;
    while (1) {

        // store item number to check with later
        item_no_prev = next_consumed.item_no;


        // wait for item in the buffer
        sem_wait(&full);

        // lock critical section
        pthread_mutex_lock(&mutex);

        /////////////////////
        // critical section

        // retrieve item
        next_consumed = sbuffPop(sBuffer);

        // verify noskip
        if (next_consumed.item_no != item_no_prev+1){
            printf("Error: Item skipped. Expected: %i, recieved: %i \n", item_no_prev+1, next_consumed.item_no);
            break;
        }

        // verify checksums
        cksum1 = ip_checksum(next_consumed.payload, PAYLOAD_SIZE);
        cksum2 = next_consumed.cksum;
        if (cksum1 != cksum2){
            printf("Checksum mismatch: received 0x%x, expected 0x%x \n", cksum2, cksum1);
            exit(1);
        }

        printf("\33[32mConsumed: Item -> shm (no.%i, cksum received: 0x%x. cksum expected: 0x%x, Successful \33[0m \n", sBuffer->buffer[sBuffer->out].item_no, cksum2, cksum1);

        // increment index of out
        incOut(sBuffer);

        //
        /////////////////////

        // signal that we removed from buffer
        sem_post(&empty);
        // unlock critical section
        pthread_mutex_unlock(&mutex);

        usleep(5e4);
    }

}
///////////////////////



///////////////////////
// On Exit - Cleanup
//

void sig_handler(int sig)
{
     printf("%s:: Got the Signal %d \n", __FUNCTION__, sig);
    // remove the shared memory object
    shm_unlink(name);

    // destroy the semiphores
    sem_destroy(&empty);
    sem_destroy(&full);

    // destroy the mutex
    pthread_mutex_destroy(&mutex);

    exit(1);
}
///////////////////////
