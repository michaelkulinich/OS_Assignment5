# Operating Systems: Assignment5

## Task: Using Threads and Mutex/Conting Semaphores for the producer-consumer problem
The idea is to write a C/C++ program that creates two threads. The first thread is the consumer thread that consumes the data written to a shared memory buffer. The second thread is the producer thread that “produces” the data for the shared memory buffer. In order to prevent a race condition (e.g. the consumer reading before the producer writing) use a mutex semaphore and counting semaphores to coordinate when each thread can safely write or read to/from a common shared memory region. 



**Linux**
- gcc compiler


### Compile and Deployment
In order to compile the project, it is required to have gcc

```
gcc -c ip_checksum.c 

ar -rc libip_checksum.a ip_checksum.o

gcc prodcon.c -L. -lip_checksum -o prodcon -lrt -lpthread

./prodcon [buffer size]

```

### Sample output
<pre><code>
 [mk@amk OS_Assignment5]$ ./prodcon 10
Successfully created shared memory object 
 Producer is in red 
 Consumer is in green 
Produced: Item -> shm (no.0, cksum: 0xc3f9)  
Produced: Item -> shm (no.1, cksum: 0x11a2)  
Produced: Item -> shm (no.2, cksum: 0xc967)  
Produced: Item -> shm (no.3, cksum: 0xaf6e)  
Produced: Item -> shm (no.4, cksum: 0x4851)  
Produced: Item -> shm (no.5, cksum: 0xe34c)  
Produced: Item -> shm (no.6, cksum: 0xb885)  
Produced: Item -> shm (no.7, cksum: 0xdf77)  
Consumed: Item -> shm (no.0, cksum received: 0xc3f9. cksum expected: 0xc3f9, Successful  
Produced: Item -> shm (no.8, cksum: 0x8da5)  
Produced: Item -> shm (no.9, cksum: 0x19cf)  
Produced: Item -> shm (no.10, cksum: 0xc621)  
Consumed: Item -> shm (no.1, cksum received: 0x11a2. cksum expected: 0x11a2, Successful  
Produced: Item -> shm (no.11, cksum: 0x2f68)  
Consumed: Item -> shm (no.2, cksum received: 0xc967. cksum expected: 0xc967, Successful  
Produced: Item -> shm (no.12, cksum: 0xccca)  
Consumed: Item -> shm (no.3, cksum received: 0xaf6e. cksum expected: 0xaf6e, Successful  
Produced: Item -> shm (no.13, cksum: 0xacd7)  
Consumed: Item -> shm (no.4, cksum received: 0x4851. cksum expected: 0x4851, Successful  
Produced: Item -> shm (no.14, cksum: 0x3b13)  
Consumed: Item -> shm (no.5, cksum received: 0xe34c. cksum expected: 0xe34c, Successful  
Produced: Item -> shm (no.15, cksum: 0x9601)  
Consumed: Item -> shm (no.6, cksum received: 0xb885. cksum expected: 0xb885, Successful  
Produced: Item -> shm (no.16, cksum: 0x8c8)  
Consumed: Item -> shm (no.7, cksum received: 0xdf77. cksum expected: 0xdf77, Successful  
Produced: Item -> shm (no.17, cksum: 0xeedd)  
Consumed: Item -> shm (no.8, cksum received: 0x8da5. cksum expected: 0x8da5, Successful  
Produced: Item -> shm (no.18, cksum: 0x4ebf)  
Consumed: Item -> shm (no.9, cksum received: 0x19cf. cksum expected: 0x19cf, Successful  
Produced: Item -> shm (no.19, cksum: 0xff49)  
Consumed: Item -> shm (no.10, cksum received: 0xc621. cksum expected: 0xc621, Successful  
Produced: Item -> shm (no.20, cksum: 0x79f1)  
^Csig_handler:: Got the Signal 2 
  </code></pre>
