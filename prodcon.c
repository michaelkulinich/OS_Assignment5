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
// #include "ip_checksum.c"

// name of shared memory
const char *name = "OS-IPC";

// number of items in the shared buffer
int nitems;  // change this

// shared buffer of limited size 100
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
/* main method */
int main(int argc, char *argv[])
{
    // ensure file is executed corrctly
    if (argc != 2) {
        printf("Usage Error: inlcude number of bits \n");
        return -1;
    }
    if (atoi(argv[1]) > 480) {
        printf("Usage Error: not enough memory size for that many nitems\n");
        return -1;
    }
    nitems = atoi(argv[1]);

    // set up signal handler to exit peacefully
    struct sigaction act;
    act.sa_handler = sig_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGINT, &act, 0);

    ////////////////
    // Initialize//
    ///////////////

    // shared memory file descriptor
    int   shm_fd;

    // pointer to shared buffer
    void  *ptr;
 
    /////////////////////////////////
    // Set up shared memory object //
    /////////////////////////////////

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
    printf("size of struct: %lu", sizeof(sBuffer));
    

    printf("Producer successfully created shared memory object \n");

    // set of thread attrivutes
    // pthread_attr_t attr; 

    // get the default attributes 
    // pthread_attr_init(&attr);


    // Semaphore set up //
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

    // Mutex set up //
    // create the mutex
    if (pthread_mutex_init(&mutex, 0)) {
        perror("Semaphore initialization failed");
        return -1;
    }
    
    // thread identifier for producer and consumer
    pthread_t prod_id, cons_id;

    // create the threads
    printf("\33[31m Producer is in red \33[0m\n");
    printf("\33[32m Consumer is in green \33[0m\n");
    pthread_create(&prod_id, NULL, producer, NULL);
    // sleep(5);
    pthread_create(&cons_id, NULL, consumer, NULL);
    printf("After thread\n");

    // join the threads
    pthread_join(prod_id, NULL);
    pthread_join(cons_id, NULL);

    // destroy the semiphores
    sem_destroy(&empty);
    sem_destroy(&full);

    // destroy the mutex
    pthread_mutex_destroy(&mutex);



 

}
  
  

/* Producer method */
void *producer(void *arg)
{
    printf("Inside prod\n");
    item next_produced;
    next_produced.item_no = -1;   // changed this from -1
    while (1) {
      //  sleep(1);
        /* 1. Increment the buffer count (item_no)  */
        next_produced.item_no+=1;

        /* 2. Generate the payload data             */
        for (int i=0 ; i<34 ; i++)
            next_produced.payload[i] = (unsigned char) rand() % 256;

        /* 3. Calculate the 16-bit checksum (cksum) */
        next_produced.cksum = ip_checksum(next_produced.payload, PAYLOAD_SIZE);
        /*
        // check if buffer is full, wait until it isnt
        while (!sbuffFull(sBuffer)){
        ////printf("buffer full: in=%i, out=%i waiting... \n", sBuffer->in, sBuffer->out);
            sleep(1);
        }
        */
        // wait
        sem_wait(&empty);
        pthread_mutex_lock(&mutex);

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
        sem_post(&full);
        pthread_mutex_unlock(&mutex);
    }
}


/* consumer method */
void *consumer(void *arg)
{
    printf("Inside cons\n");
    item next_consumed;
    next_consumed.item_no = -1;
    int item_no_prev;
    int repeat = 0;
    while (1) {
        // usleep(2e5);  /////// change for final version ///////

        // store item number to check with later
        item_no_prev = next_consumed.item_no;
        /*
        // if its empty, wait
        while (!sbuffEmpty(sBuffer)){
            sleep(1); 
        }
        */

        // Wait
        sem_wait(&full);
        pthread_mutex_lock(&mutex);
        // critical section

        // remove item
        next_consumed = sbuffPop(sBuffer);
        // printf("Pulled item no. %i: payload[0]=%d \n", next_consumed.item_no+1, next_consumed.payload[0]);

        // verify noskip
        if (next_consumed.item_no != item_no_prev+1){
            printf("Error: Item skipped. Expected: %i, recieved: %i \n", item_no_prev+1, next_consumed.item_no);
            break;
        }

        // verify with checksum
        cksum1 = ip_checksum(next_consumed.payload, PAYLOAD_SIZE);
        cksum2 = next_consumed.cksum;

        if (cksum1 != cksum2){
            printf("Checksum mismatch: received 0x%x, expected 0x%x \n", cksum2, cksum1);
            exit(1);
        }

        printf("\33[32mConsumed: Item -> shm (no.%i, cksum received: 0x%x. cksum expected: 0x%x, Successful \33[0m \n", sBuffer->buffer[sBuffer->out].item_no, cksum2, cksum1);

        // increment index of out
        incOut(sBuffer);

        sem_post(&empty);
        pthread_mutex_unlock(&mutex);
        usleep(5e4);
    }

}


void sig_handler(int sig)
{
     printf("%s:: Got the Signal %d \n", __FUNCTION__, sig);
    // remove the shared memory object
    shm_unlink(name);

    exit(1);
}