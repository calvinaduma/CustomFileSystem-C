/* Calvin Aduma 
    Christopher J. Broom
 */
/* test driver */

#include "tfs.h"

int main(){
  unsigned int fd[20];
  char buffer1[1024], buffer2[1024], buffer3[1024];
  unsigned int length1, length2, count1, count2, count3;

  sprintf( buffer1, "%s",
    "This is a simple-minded test for the trivial file system code.  " );
  sprintf( buffer1 + 64, "%s",
    "This is a simple-minded test for the trivial file system code.  " );
  sprintf( buffer1 + 128, "%s",
    "This is a simple-minded test for the trivial file system code.  " );
  sprintf( buffer1 + 192, "%s",
    "This is a simple-minded test for the trivial file system code.  " );
  sprintf( buffer1 + 256, "%s",
    "This is a simple-minded test for the trivial file system code.  " );

  sprintf( buffer2, "%s",
    "And now for something completely different." );

  length1 = strlen( buffer1 );
  length2 = strlen( buffer2 );
  printf( "length of buffer1 is %d\n", length1 );
  printf( "length of buffer2 is %d\n", length2 );

  tfs_init();

  tfs_list_directory();

  fd[0] = tfs_create( "file.txt" );
  if( fd[0] == 0 ) printf( "first create failed\n" );

  fd[1] = tfs_create( "my_file" );
  if( fd[1] == 0 ) printf( "second create failed\n" );

  /* this read should fail because file is not readable */
  count3 = tfs_read( fd[0], buffer3, 640 );
  printf("\nthis read should fail because file is not readable\n");
  if (count3 == 0) printf( "first file is not readable\n\n");
  else printf( "%d bytes read from first file\n", count3 );

  tfs_list_directory();

  /* this write will pass */
  count1 = tfs_write( fd[0], buffer1, length1 );
  printf("\nthis write will pass\n");
  if (count1 == 0) printf( "first file is not writeable\n\n");
  else printf( "%d bytes written to first file\n", count1 );

  tfs_list_directory();

  tfs_seek( fd[0], 600 );
  /* this read will pass */
  printf("this read will pass\n");
  count2 = tfs_read( fd[0], buffer3, 640 );
  printf( "%d bytes read from first file\n", count2 );

  tfs_list_directory();

  /* this write will fail because it has no permissions */
  count1 = tfs_write( fd[0], buffer1, length1 );
  printf("this write should fail because file is not writable\n");
  if (count1 == 0) printf( "first file is not writeable\n");
  else printf( "%d bytes written to first file\n", count1 );

  tfs_close( fd[1] );
  tfs_close( fd[0] );

  tfs_list_directory();
  tfs_list_blocks();

  return 0;
}

/* this test driver should print

length of buffer1 is 320
length of buffer2 is 43
-- directory listing --
  fd =  1: unused
  fd =  2: unused
  fd =  3: unused
  fd =  4: unused
  fd =  5: unused
  fd =  6: unused
  fd =  7: unused
  fd =  8: unused
  fd =  9: unused
  fd = 10: unused
  fd = 11: unused
  fd = 12: unused
  fd = 13: unused
  fd = 14: unused
  fd = 15: unused
-- end --

this read should fail because file is not readable
first file is not readable

-- directory listing --
  fd =  1: file.txt, currently open, 0 bytes in size
           FAT: no blocks in use
  fd =  2: my_file, currently open, 0 bytes in size
           FAT: no blocks in use
  fd =  3: unused
  fd =  4: unused
  fd =  5: unused
  fd =  6: unused
  fd =  7: unused
  fd =  8: unused
  fd =  9: unused
  fd = 10: unused
  fd = 11: unused
  fd = 12: unused
  fd = 13: unused
  fd = 14: unused
  fd = 15: unused
-- end --

this write will pass
320 bytes written to first file
-- directory listing --
  fd =  1: file.txt, currently open, 320 bytes in size
           FAT: 4 5 6
  fd =  2: my_file, currently open, 0 bytes in size
           FAT: no blocks in use
  fd =  3: unused
  fd =  4: unused
  fd =  5: unused
  fd =  6: unused
  fd =  7: unused
  fd =  8: unused
  fd =  9: unused
  fd = 10: unused
  fd = 11: unused
  fd = 12: unused
  fd = 13: unused
  fd = 14: unused
  fd = 15: unused
-- end --
this read will pass
256 bytes read from first file
-- directory listing --
  fd =  1: file.txt, currently open, 320 bytes in size
           FAT: 4 5 6
  fd =  2: my_file, currently open, 0 bytes in size
           FAT: no blocks in use
  fd =  3: unused
  fd =  4: unused
  fd =  5: unused
  fd =  6: unused
  fd =  7: unused
  fd =  8: unused
  fd =  9: unused
  fd = 10: unused
  fd = 11: unused
  fd = 12: unused
  fd = 13: unused
  fd = 14: unused
  fd = 15: unused
-- end --
this write should fail because file is not writable
first file is not writeable
-- directory listing --
  fd =  1: file.txt, currently closed, 320 bytes in size
           FAT: 4 5 6
  fd =  2: my_file, currently closed, 0 bytes in size
           FAT: no blocks in use
  fd =  3: unused
  fd =  4: unused
  fd =  5: unused
  fd =  6: unused
  fd =  7: unused
  fd =  8: unused
  fd =  9: unused
  fd = 10: unused
  fd = 11: unused
  fd = 12: unused
  fd = 13: unused
  fd = 14: unused
  fd = 15: unused
-- end --
-- file allocation table listing of used blocks --
  block   4 is used and points to   5
  block   5 is used and points to   6
  block   6 is used and points to   1
-- end --


*/
