# OS_Assignment5

## Todo
Figure out how to do the dynamic memory allocation for nitems. Right now, nitems (which is just BUFFERSIZE in sharedBuffer.c) is set at 1750 as the limit because memsize cant exceed 64000 bytes. I tried estimating what the limit of nitems actually is but I was way wrong. We need the user to enter how many items, nitems, they want in the buffer, and if nitems > 1750, then we should say "not enough memory for that many items". 
