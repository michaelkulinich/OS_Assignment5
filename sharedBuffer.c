#define MMAP_SIZE 64000
#define PAYLOAD_SIZE 34

// a payloaded item to send
typedef struct {
    unsigned char payload[PAYLOAD_SIZE];  // random generated data 

    int item_no;  // number of the item produced
    unsigned short cksum;  // 16-bit Internet checksum    

     // total bytes, (34 * 1) + 4 + 2  = 40 bytes
     // plus 28 bytes of padding, 68 total bytes
} item;

// a circular buffer with a size limit of 100
typedef struct {
  // item buffer[buffer_size];
  item *buffer;
  int buffer_size;
  int in;
  int out;

  // total bytes (64000) <= (68 * nitems) + 4 + 4 + 4 
  // nitems <= 941
  // however because of padding, the buffer takes up the most space so 'in', 'out' will need a padding 
  // amount of sizeof(buffer - 12)
  // -> buffer size limit (nitems) 480
  // Theoretically... By trial an error, I can have a limit of about 1750 nitems
} sharedBuffer;

// move the first buffer item index forward
void incIn(sharedBuffer* buff){
  buff->in = (buff->in + 1) % buff->buffer_size;
}

// move the last buffer item index forward
void incOut(sharedBuffer* buff){
  buff->out = (buff->out + 1) % buff->buffer_size;
}

// returns 1 if okay, returns 0 if full
int sbuffFull(sharedBuffer* buff){
  if (((buff->in + 1) % buff->buffer_size) == buff->out ){
    return 0;
  }
  return 1;
}

// returns 1 if okay, returns 0 if empty
int sbuffEmpty(sharedBuffer* buff){
  if ((buff->in == buff->out)){
    return 0;
  }
  return 1;
}

// pushes item to buffer
void sbuffPush(sharedBuffer* buff, item item){
  // incIn(buff); 
  buff->buffer[buff->in] = item;
}


// returns item on success, no error handling yet!
item sbuffPop(sharedBuffer* buff){
  item temp = buff->buffer[buff->out];
  // incOut(buff);
  return temp;
}
