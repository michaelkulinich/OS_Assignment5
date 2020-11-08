# OS_Assignment5

## Todo
Figure out how to do the dynamic memory allocation for nitems. Right now, nitems (which is just BUFFERSIZE in sharedBuffer.c) is set at 1750 as the limit because memsize cant exceed 64000 bytes. I tried estimating what the limit of nitems actually is but I was way wrong. We need the user to enter how many items, nitems, they want in the buffer, and if nitems > 1750, then we should say "not enough memory for that many items". 



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
<pred><code>
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
