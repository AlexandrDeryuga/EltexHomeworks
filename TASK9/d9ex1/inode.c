#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

int main(int argc, char *argv[]) {
 struct stat file_stats;

 if(argc != 2)
  fprintf(stderr, "Usage: fstat FILE...\n"), exit(EXIT_FAILURE);

 if((stat(argv[1], &file_stats)) == -1) {
  perror("fstat"); 
  return 1;
 }

 printf("filename: %s\n", argv[1]);
 printf(" device: %lld\n",                       file_stats.st_dev);
 printf(" inode: %ld\n",                         file_stats.st_ino);
 printf(" protection: %o\n",                     file_stats.st_mode);
 printf(" number of hard links: %d\n",           file_stats.st_nlink);
 printf(" user ID of owner: %d\n",               file_stats.st_uid);
 printf(" group ID of owner: %d\n",              file_stats.st_gid);
 printf(" device type (if inode device): %lld\n",file_stats.st_rdev);
 printf(" total size, in bytes: %ld\n",          file_stats.st_size);
 printf(" blocksize for filesystem I/O: %ld\n",  file_stats.st_blksize);
 printf(" number of blocks allocated: %ld\n",    file_stats.st_blocks);
 printf(" time of last access: %ld : %s",        file_stats.st_atime, ctime(&file_stats.st_atime));
 printf(" time of last modification: %ld : %s",  file_stats.st_mtime, ctime(&file_stats.st_mtime));
 printf(" time of last change: %ld : %s",        file_stats.st_ctime, ctime(&file_stats.st_ctime));

 return 0;
}
