/* Calvin Aduma 
    Christopher J. Broom
 */
#include "tfs.h"
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>

/* implementation of assigned functions */

char* substr(const char *src, int m, int n)
{
    // get the length of the destination string
    int len = n - m;
 
    // allocate (len + 1) chars for destination (+1 for extra null character)
    char *dest = (char*)malloc(sizeof(char) * (len + 1));
 
    // extracts characters between m'th and n'th index from source string
    // and copy them into the destination string
    for (int i = m; i < n && (*(src + i) != '\0'); i++)
    {
        *dest = *(src + i);
        dest++;
    }
 
    // null-terminate the destination string
    *dest = '\0';
 
    // return the destination string
    return dest - len;
}

unsigned int get_file_descriptor (char *filename){
  int i=0;
  for (i=0; i<16; i++)
   if (strcmp(directory[i].name,filename)==0)
    return i;
}

_Bool file_is_readable( char *filename){
  unsigned int file_descriptor = get_file_descriptor(filename);
  if (strncmp(directory[file_descriptor].permissions,"R",1)==0) return TRUE;
  else return FALSE;
}

_Bool file_is_writeable( char *filename){
  unsigned int file_descriptor = get_file_descriptor(filename);
  if (strncmp(directory[file_descriptor].permissions,"C",1)==0) {
    strcpy(directory[file_descriptor].permissions,"W");
    return TRUE;
  }
  if (strncmp(directory[file_descriptor].permissions,"W",1)==0)
    return TRUE;
  else 
    return FALSE;
}

void file_make_readable( char *filename){
  unsigned int file_descriptor = get_file_descriptor(filename);
  if (strncmp(directory[file_descriptor].permissions,"R",1)!=0) strcpy(directory[file_descriptor].permissions,"R");
}

void file_make_writeable( char *filename){
  unsigned int file_descriptor = get_file_descriptor(filename);
  if (strncmp(directory[file_descriptor].permissions,"W",1)==0) strcpy(directory[file_descriptor].permissions,"W");
}

/* you are welcome to use helper functions of your own */


/* tfs_delete()
 *
 * deletes a closed directory entry having the given file descriptor
 *   (changes the status of the entry to unused) and releases all
 *   allocated file blocks
 *
 * preconditions:
 *   (1) the file descriptor is in range
 *   (2) the directory entry is closed
 *
 * postconditions:
 *   (1) the status of the directory entry is set to unused
 *   (2) all file blocks have been set to free
 *
 * input parameter is a file descriptor
 *
 * return value is TRUE when successful or FALSE when failure
 */

unsigned int tfs_delete( unsigned int file_descriptor ){

  /* your code here */
  strcpy(directory[file_descriptor].permissions,"C");
  if (directory[file_descriptor].status == UNUSED){
    return FALSE;
  }else{
    int current = directory[file_descriptor].first_block;
    directory[file_descriptor].first_block = FREE;
    int next = file_allocation_table[current];
    while (current != 1 && current != 0){
      memset(blocks[current].bytes,0,255);
      file_allocation_table[current] = FREE;
      current = next;
      next = file_allocation_table[next];
    }
    return TRUE;
  }
}


/* tfs_read()
 *
 * reads a specified number of bytes from a file starting
 *   at the byte offset in the directory into the specified
 *   buffer; the byte offset in the directory entry is
 *   incremented by the number of bytes transferred
 *
 * depending on the starting byte offset and the specified
 *   number of bytes to transfer, the transfer may cross two
 *   or more file blocks
 *
 * the function will read fewer bytes than specified if the
 *   end of the file is encountered before the specified number
 *   of bytes have been transferred
 *
 * preconditions:
 *   (1) the file descriptor is in range
 *   (2) the directory entry is open
 *   (3) the file has allocated file blocks
 *
 * postconditions:
 *   (1) the buffer contains bytes transferred from file blocks
 *   (2) the specified number of bytes has been transferred
 *         except in the case that end of file was encountered
 *         before the transfer was complete
 *
 * input parameters are a file descriptor, the address of a
 *   buffer of bytes to transfer, and the count of bytes to
 *   transfer
 *
 * return value is the number of bytes transferred
 */

unsigned int tfs_read( unsigned int file_descriptor,
                       char *buffer,
                       unsigned int byte_count ){

  /* your code here */
  if (!file_is_readable(directory[file_descriptor].name)) return FREE;

  unsigned int available_data = directory[file_descriptor].size - directory[file_descriptor].byte_offset;
  unsigned int num_of_bytes_transferred = (byte_count > available_data ? available_data:byte_count);

  // returns number of full pages
  double r = floor((double)directory[file_descriptor].byte_offset/BLOCK_SIZE);
  int start_index = directory[file_descriptor].first_block;
  int counter = 1;
  // finds start of where transfer will be made
  while (counter < r){
    start_index = file_allocation_table[start_index];
    counter++;
  }
  int remaining_amount = num_of_bytes_transferred;
  int start = directory[file_descriptor].byte_offset%BLOCK_SIZE;
  int end = (BLOCK_SIZE < (start+byte_count) ? BLOCK_SIZE:(start+byte_count));
  int diff = BLOCK_SIZE - start;
  // start of transfer
  while (remaining_amount > 0){
    strncat(buffer, substr(blocks[start_index].bytes, start, end), diff);
    start_index = file_allocation_table[start_index];
    remaining_amount -= diff;
    diff = (remaining_amount < BLOCK_SIZE ? remaining_amount:BLOCK_SIZE);
    start = 0;
    end = diff;  
  }
  // sets byte offset to end of size for easier write next time
  directory[file_descriptor].byte_offset = directory[file_descriptor].size % BLOCK_SIZE;
  
  return num_of_bytes_transferred;
}

/* tfs_write()
 *
 * writes a specified number of bytes from a specified buffer
 *   into a file starting at the byte offset in the directory;
 *   the byte offset in the directory entry is incremented by
 *   the number of bytes transferred
 *
 * depending on the starting byte offset and the specified
 *   number of bytes to transfer, the transfer may cross two
 *   or more file blocks
 *
 * furthermore, depending on the starting byte offset and the
 *   specified number of bytes to transfer, additional file
 *   blocks, if available, will be added to the file as needed;
 *   in this case, the size of the file will be adjusted
 *   based on the number of bytes transferred beyond the
 *   original size of the file
 *
 * the function will read fewer bytes than specified if file
 *   blocks are not available
 *
 * preconditions:
 *   (1) the file descriptor is in range
 *   (2) the directory entry is open
 *
 * postconditions:
 *   (1) the file contains bytes transferred from the buffer
 *   (2) the specified number of bytes has been transferred
 *         except in the case that file blocks needed to
 *         complete the transfer were not available
 *   (3) the size of the file is increased as appropriate
 *         when transferred bytes extend beyond the previous
 *         end of the file
 *
 * input parameters are a file descriptor, the address of a
 *   buffer of bytes to transfer, and the count of bytes to
 *   transfer
 *
 * return value is the number of bytes transferred
 */

unsigned int tfs_write( unsigned int file_descriptor,
                        char *buffer,
                        unsigned int byte_count ){

  /* your code here */


  if (!file_is_writeable(directory[file_descriptor].name)) return FREE;
  else file_make_writeable(directory[file_descriptor].name);

  file_make_readable(directory[file_descriptor].name);

  unsigned int difference = byte_count;
  int original_start, dummy_variable;
  // needed to calculate how much blocks will be used in future allocation
  if(directory[file_descriptor].byte_offset != 0){
    // difference is how many blocks will be used in the future
    difference = byte_count - directory[file_descriptor].byte_offset;
    dummy_variable = 0;
  }

  unsigned int num_of_bytes_transferred = byte_count;
  // records the size of the file descriptor
  if ((int)(directory[file_descriptor].size + byte_count) < MAX_FILE_SIZE){
    directory[file_descriptor].size += byte_count;
  } else {
    directory[file_descriptor].size += MAX_FILE_SIZE - (byte_count + directory[file_descriptor].size);
    num_of_bytes_transferred = MAX_FILE_SIZE - (byte_count + directory[file_descriptor].size);
  }

  // finds number of blocks used in file
  double r = ceil((double)difference/BLOCK_SIZE);
  int num_of_blocks_used = r;
  int remainder = (int)difference%BLOCK_SIZE;
  int counter = 0;
  int startRange=0, endRange;
  endRange = (byte_count < BLOCK_SIZE ? byte_count:BLOCK_SIZE);
  
  int i,j=0,k=0;
  // finds start of file if file is not new
  int start=directory[file_descriptor].first_block;
  while (file_allocation_table[start] != 1 && start !=0) {
    start = file_allocation_table[start];
    original_start = start;
  }
  // makes start = 4 since index starts at 4
  if ( start == 0 ) start = 4;
  for (i=start; i+j<N_BLOCKS; i++){
    while (k<num_of_blocks_used){
      // if block is free
      if (file_allocation_table[i+j] == 0){
        // necessary for when block is not new
        if (dummy_variable == 0) {
          dummy_variable++;
          file_allocation_table[original_start] = i+j;
          endRange = difference;
          strncat(blocks[i+j].bytes, substr(buffer,startRange,endRange), endRange);
        }
        // when reached last block
        if (k == num_of_blocks_used-1){
          // records the first block in the case where only 1 block is used for file
          if (directory[file_descriptor].first_block == 0) directory[file_descriptor].first_block = i+j;
          file_allocation_table[i+j] = 1;
          strncat(blocks[i+j].bytes, substr(buffer,startRange,endRange), endRange);
          if ((int)difference%BLOCK_SIZE != 0) {
            directory[file_descriptor].byte_offset = ((int)difference%BLOCK_SIZE); // records remainder of file used
          }
        }else{
          // records the first block in the case where n blocks are used for file
          if (directory[file_descriptor].first_block == 0) directory[file_descriptor].first_block = i+j;
          file_allocation_table[i+j] = i+j+1;
          strncat(blocks[i+j].bytes,substr(buffer,startRange,endRange),endRange);
        }
        k++;
        //startRange = endRange+1;
        byte_count -= BLOCK_SIZE;
        endRange = (BLOCK_SIZE < byte_count ? BLOCK_SIZE:byte_count);
      }
      j++;
    }
    if (j==num_of_blocks_used) break;
  }
  return num_of_bytes_transferred;
}