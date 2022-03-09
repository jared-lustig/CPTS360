/************** lab5base.c file ******************/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ext2fs/ext2_fs.h>
#include <string.h>

typedef struct ext2_super_block SUPER;
typedef struct ext2_group_desc  GD;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;

SUPER *sp;
GD    *gp;
INODE *ip;
DIR   *dp;   

#define BLK 1024

int fd;             // opened vdisk for READ
int inodes_block;   // inodes start block number
int ninodes;
int iblock;

char gpath[128];    // token strings
char *name[32];
int n, i;

int get_block(int fd, int blk, char *buf)
{
   lseek(fd, blk*BLK, 0);
   read(fd, buf, BLK);
}   

int search(INODE *ip, char *name)
{
  // Chapter 11.4.4  int i; 
  // Exercise 6
   int i;
   char *cp, temp[256], sbuf[BLK];
   for (i=0; i<12; i++){ // search DIR direct blocks only
      if (ip->i_block[i] == 0){
         return 0;
      }
      get_block(fd, ip->i_block[i], sbuf);
      dp = (DIR *)sbuf;
      cp = sbuf;

      while (cp < sbuf + BLK){
         strncpy(temp, dp->name, dp->name_len);
         temp[dp->name_len] = 0;
         printf("%8d %8d %8u %s\n", dp->inode, dp->rec_len, dp->name_len, temp);
         printf("name = %s, temp = %s\n", dp->name, temp);
         if (strcmp(name, temp)==0){
            printf("found %s : inumber = %d\n", name, dp->inode);
            return dp->inode;
         }
         cp += dp->rec_len;
         dp = (DIR *)cp;
      }
   }
   return 0;
}

/*************************************************************************/
int tokenize(char *pathname)
{
   char *s;
   strcpy(gpath, pathname);
   n = 0;
   s = strtok(gpath, "/");

   while(s){
      name[n++] = s;
      s = strtok(0, "/");
   }
} 

char *disk = "vdisk";

int main(int argc, char *argv[])   // a.out pathname
{
  char buf[BLK];  // use more bufs if you need them
  char supbuf[BLK];
  char ibuf[BLK];
  char ibbuf[BLK];

   //   1. open vdisk for READ: print fd value
   int fd = open(disk, O_RDONLY);
   printf("fd = %d\n", fd);
   //   2. read SUPER block #1 to verify EXT2 fs : print s_magic in HEX
   get_block(fd, 1, buf);
   sp = (SUPER *)buf;
   int firstdata = sp->s_first_data_block;
   int inodesize = sp->s_inode_size; // get inodes count

   printf("s_magic = %x\n", ninodes);

   //   3. read GD block #2 to get inodes_block=bg_inode_table: print inodes_block 
   get_block(fd, (firstdata + 1), buf);
   gp = (GD *)buf;
   inodes_block = gp->bg_inode_bitmap; // get imap block number

   printf("inodes_block = %d\n", inodes_block);

   //   4. read inodes_block into buf[] to get root INODE #2 : set ino=2 
   //           INODE *ip = (INODE *)buf + 1;

   get_block(fd, inodes_block, buf);
   int ino;
   ip = (INODE *)buf + 1; 
   ino = 2;

   //   5. tokenize pathame=argv[1]);
   tokenize(argv[1]);
   printf("tokenize -> ");
   for(i = 0; name[i] != NULL; i++)
        printf("/%s", name[i]);
    printf("\n");
   //   6.   
   //printf("name0 = %s, name1 = %s, name2 = %s", name[0], name[1], name[2]);
   int blk, offset;
   char newbuf[BLK];
   for (i=0; i<n; i++){
      printf("===========================================\n");
      printf("search name[%d]=%s in ino=%d\n", i, name[i], ino);
      ino = search(ip, name[i]);

      if (ino==0){
         printf("name %s does not exist\n", name[i]);
         exit(1);
      }
      // MAILman's algrithm:
      blk    = (ino-1) / 8 + inodes_block;
      offset = (ino-1) % 8;

      get_block(fd, blk, newbuf);

      ip = (INODE *)newbuf + offset;
   } 


   // 7. HERE, ip->INODE of pathname
   printf("****************  DISK BLOCKS  *******************\n");
   for (i=0; i<15; i++){
      if (ip->i_block[i]) 
         printf("block[%2d] = %d\n", i, ip->i_block[i]);
   }

   printf("================ Direct Blocks ===================\n");
   
   if (ip->i_block[12]){
      printf("===============  Indirect blocks   ===============\n"); 
   }

   if (ip->i_block[13]){
     printf("===========  Double Indirect blocks   ============\n"); 
   }
}
