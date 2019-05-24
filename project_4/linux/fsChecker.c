#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/mman.h>
#include "fs.h"
#include "types.h"


#define T_DIR  1   // Directory
#define T_FILE 2   // File
#define T_DEV  3   // Device
#define T_UNALLOC   0
#define SUPERBLOCK  1
// #define stat XV6_stat
// File descriptor of file system
int fsfd;
struct superblock sb;
struct dinode di;
// Disk layout:
// [ boot block | sb block | log | inode blocks | free bit map | data blocks ]
void printERR(char *msg){
    printf("%s\n", msg);
}

void
rsect(uint sec, void *buf)
{
  if(lseek(fsfd, sec * BSIZE, 0) != sec * BSIZE){
    perror("lseek");
    exit(1);
  }
  if(read(fsfd, buf, BSIZE) != BSIZE){
    perror("read");
    exit(1);
  }
}

// Helper for reading the inodes
void
rinode(uint inum, struct dinode *ip)
{
  char buf[BSIZE];
  uint bn;
  struct dinode *dip;

  bn = IBLOCK(inum, sb);
  rsect(bn, buf);
  dip = ((struct dinode*)buf) + (inum % IPB);
  *ip = *dip;
}

int getInum(uint inum, char* name){
    struct dinode inode;
    rinode(inum, &inode);
    struct dirent dirs[DPB];
    char buf[BSIZE];

    if (inode.type != T_DIR) {
        return 0;
    }
    int i,j;
    for(i=0; i<NDIRECT; i++){
        if(inode.addrs[i] == 0)
            continue;
        rsect(inode.addrs[i], buf);
        memmove(&dirs, buf, sizeof(dirs));
        for(j = 0; j < DPB; j++){
            // keep searching until find the right name.
            if(strncmp(name, dirs[j].name, DIRSIZ) == 0){
                return dirs[j].inum;
            }
        }
    }
    if(inode.addrs[NDIRECT] != 0){
        uint addrs[NINDIRECT];
        rsect(inode.addrs[NDIRECT], buf);
        memmove(&addrs, buf, sizeof(addrs));
        for(i = 0; i < NINDIRECT; i++){
            if(addrs[i] == 0){
                continue;
            }
            rsect(addrs[i], buf);
            memmove(&dirs, buf, sizeof(dirs));
            // iterate to find the name match
            for(j = 0; j < DPB; j++){
                if(strncmp(name, dirs[j].name, DIRSIZ) == 0){
                    return dirs[j].inum;
                }
            }
        }
    }
    return -1;
}

int isNthBitTrue(unsigned char c, int n){
    static unsigned char mask[]={1,2,4,8,16,32,64,128};
    return((c & mask[n])!=0);
}

//Each inode is either unallocated or one of the valid types (T_FILE, T_DIR, T_DEV). 
//ERROR: bad inode.

//Root directory exists, and it is inode number 1.
//ERROR MESSAGE: root directory does not exist.
int checkBadInode(){
    int i;
    for(i = 0; i < sb.ninodes; i++)
    {
        struct dinode inode;
        rinode(i,&inode);
        if (inode.type != T_UNALLOC && inode.type!=T_DIR && inode.type!= T_FILE && inode.type!= T_DEV) {
            char* msg = "ERROR: bad inode.";
            printERR(msg);
            return -1;
        }

        //Checking the free node
        if (inode.type != T_UNALLOC && i ==1) {
            if (inode.type!=T_DIR) {
                printERR("ERROR: root directory does not exist.");
                return -1;
            }
        }        
    }
    return 1;
}
//For in-use inodes, each address that is used by inode is valid 
//(points to a valid datablock address within the image).
//Note: must check indirect blocks too, when they are in use.
//ERROR: bad address in inode.
int checkValidAddr(){
    int i,j;
    uint maxinum = sb.size-1;
    uint mininum = BBLOCK(sb.size, sb);
    struct dinode inode;
    for(i = 0; i < sb.ninodes; i++)
    {
        rinode(i, &inode);
        //Check for bad direct inode address
        for(j = 0; j < NDIRECT+1; j++)
        {
            if ((inode.addrs[j]< mininum || inode.addrs[j] > maxinum) && inode.addrs[j]!=0 ) {
                return -1;
            }
        }
        //Check for bad indirect inode address
        if (inode.addrs[NDIRECT] != 0) {
            char buf[BSIZE];
            rsect(inode.addrs[NDIRECT], buf);
            uint indiraddrs[NINDIRECT];
            memmove(&indiraddrs, buf, sizeof(indiraddrs));
            for(int j = 0; j < NINDIRECT; j++){

                if(indiraddrs[j] != 0 && (indiraddrs[j] < mininum || indiraddrs[j] > maxinum)){
                return -1;
                }
            }
        } 
    }
    return 1;
}


int getBit(uint addr){
    uchar buf[BSIZE];
    uint bNumber = BBLOCK(addr, sb);
    rsect(bNumber,buf);
    uint index = (addr % BPB) / 8; // bitmap block number
    uint shift = addr % 8; // addr shift bit
      // the bit in bitmap is marked return 1, not mark in bitmap return -1
    if((buf[index] >> shift) % 2 == 1){
        return 1;
    }
    else{
        printf("%d\n", addr);
        return -1;
    }
}   


//For in-use inodes, each address in use is also marked in use in the bitmap.
//ERROR: address used by inode but marked free in bitmap.
int checkBitmap(uint addresses[]){
    struct dinode inode;
    uchar buf[BSIZE];
    int i, j;
    for(i = 0; i < sb.ninodes; i++)
    {
        rinode(i, &inode);
        if (inode.type ==T_UNALLOC) {
            continue;
        }
        // find directory inode and to check the direct directory format
        for(j = 0; j < NDIRECT; j++)
        {
            if (inode.addrs[j] == 0) {
                continue;
            }
            addresses[inode.addrs[j]] += 1;
            if (getBit(inode.addrs[j])!=1) {
                return -1;
            }   
        }
        // search the indirect part to find the name
        if (inode.addrs[NDIRECT]!=0) {
            // Record Indirect data block;
            addresses[inode.addrs[j]] += 1;
            uint indirect_addr[NINDIRECT];
            rsect(inode.addrs[NDIRECT],buf);
            memmove(&indirect_addr,buf,sizeof(indirect_addr));
            //iterat every address
            for( j = 0; j < NINDIRECT; j++)
            {
                if (indirect_addr[j] == 0) {
                    continue;
                }
                addresses[indirect_addr[j]] += 1;
                if (getBit(indirect_addr[j]) != 1) {
                    return -1;
                }   
            }
        }
    }
    return 1;
}
//For blocks marked in-use in bitmap, actually is in-use in an inode or indirect block somewhere.
//ERROR: bitmap marks block in use but it is not in use.
int checkUsedBlock(uint addresses[]){
    uchar buf[BSIZE];
    rsect(sb.bmapstart, buf);
    uint checkStart = sb.bmapstart+1;

    for(uint i = checkStart; i < sb.size; i++)
    {
        uint index = i / 8;
        uint shift = i % 8;
        if((buf[index] >> shift) % 2 == 1){
            if(addresses[i] == 0){
                printf("lal:%d\n", i);
                return -1;
            }
        }
    }
    return 1;
}

//Each directory contains . and .. entries.
//ERROR: directory not properly formatted.
int checkDirFormat(char* name){
    struct dinode inode;
    int i;
    for(i = 0; i < sb.ninodes; i++)
    {
        rinode(i, &inode);
        if (inode.type !=T_DIR) {
            continue;
        }
        if (getInum(i, name) == -1) {
            return -1;
        }
    }
    
    return 1;
}
//Each .. entry in directory refers to the proper parent inode, 
//and parent inode points back to it.
//ERROR: parent directory mismatch.
int checkParentDir(){
    struct dinode inode;
    uchar buf[BSIZE];
    struct dirent dirs[DPB];
    int i,j,k;
    for(i = 0; i < sb.ninodes; i++)
    {
        rinode(i, &inode);
        if (inode.type != T_DIR) {
            continue;
        }
        for( j = 0; j < NDIRECT; j++)
        {
            if (inode.addrs[j] == 0) {
                continue;
            }
            rsect(inode.addrs[j],buf);
            memmove(&dirs, buf, sizeof(dirs));
            for(k = 0; k < DPB; k++)
            {
                uint inum = getInum((uint)dirs[k].inum,"..");
                if (inum>0&&inum!=i) {
                    printf("parent check inum:%d, %d\n", inum, i);
                    return -1;
                }   
            } 
        }
        //indirect
        if(inode.addrs[NDIRECT] != 0){
            uint addrs[NINDIRECT];
            rsect(inode.addrs[NDIRECT], buf);
            memmove(&addrs, buf, sizeof(addrs));
            for(j = 0; j < NINDIRECT; j++){
                if(addrs[j] == 0){
                    continue;
                }
                rsect(addrs[j], buf);
                memmove(&dirs, buf, sizeof(dirs));
                // read each dirent to find the name match
                for(k = 0; k < DPB; k++){
                    uint inum = getInum((uint)dirs[k].inum, "..");
                    if(inum > 0 && inum != i){
                        printf("parent check inum(indirect):%d, %d\n", inum, i);
                        return -1;
                    }
                }
            }
        }
    }
    return 1;
}



//For in-use inodes, any address in use is only used once.
//ERROR: address used more than once.
int checkUsedTimes(uint addresses[])
{
    int i;
    for(i = 0; i < sb.size; i++){
        if(addresses[i] > 1){
            return -1;
        }
    }
    return 1;
}
//For inodes marked used in inode table, must be referred to in at least one directory.
//ERROR: inode marked use but not found in a directory.
int checkInodeRef(uint ins[]){
    struct dinode inode;
    int i,j,k;
    for(i = 0; i < sb.ninodes; i++)
    {
        rinode(i, &inode);
        if (inode.type != T_DIR) {
            continue;
        }
        struct dirent dirs[DPB];
        char buf[BSIZE];
        // find directory inode and to check the direct directory format
        for(j = 0; j < NDIRECT; j++)
        {
            if (inode.addrs[j] == T_UNALLOC) {
                continue;
            }
            rsect(inode.addrs[j],buf);
            memmove(&dirs, buf, sizeof(dirs));
            for(k = 0; k < DPB; k++)
            {
                // count linked directory inode
                if(dirs[k].inum != 0 && strncmp("..", dirs[k].name, DIRSIZ) != 0 && strncmp(".", dirs[k].name, DIRSIZ) != 0){
                    ins[dirs[k].inum]++;
                }
            }
            
        }
        // search the indirect part to find the name
        if (inode.addrs[NDIRECT] != T_UNALLOC) {
            uint indirect_addr[NINDIRECT];
            rsect(inode.addrs[NDIRECT], buf);
            memmove(&indirect_addr, buf, sizeof(indirect_addr));
            for(j = 0; j < NINDIRECT; j++)
            {
                if (indirect_addr[j] == T_UNALLOC) {
                    continue;
                }
                rsect(indirect_addr[j],buf);
                memmove(&dirs, buf, sizeof(dirs));

                for( k = 0; k < DPB; k++)
                {
                    if(dirs[k].inum != 0 && strncmp("..", dirs[k].name, DIRSIZ) != 0 && strncmp(".", dirs[k].name, DIRSIZ) != 0){
                        ins[dirs[k].inum]++;
                    }
                }
                
            }
            
        }
        for( i = 2; i < sb.ninodes; i++)
        {
            rinode(i, &inode);
            if (inode.type != T_UNALLOC && ins[i] == 0) {
                printf("wrong root ref: %d, %d\n", ins[i], i);
                return -1;
            }     
        }
    }
    return 1;
}
//For inode numbers referred to in a valid directory, actually marked in use in inode table.
//ERROR: inode referred to in directory but marked free.
int checekValidDir(uint ins[]){
    struct dinode inode;
    int i;
    for(i=0; i<sb.ninodes;i++){

      if (ins[i]>0) {
        rinode(i, &inode);
        if (inode.type == T_UNALLOC) {
            printf("Wrong Inode Type: %d, %d\n", i, ins[i]);
            return -1;
        }
      }
      
        
    }
    return 1;
}
//Reference counts (number of links) for regular files match the number
//of times file is referred to in directories (i.e., hard links work correctly).
//ERROR: bad reference count for file.
int checkBadLinks(uint ins[]){
    struct dinode inode;
    int i;
    for(i=0; i< sb.ninodes; i++){
        rinode(i, &inode);
        if (inode.type !=T_FILE) {
            continue;
        }
        if (inode.nlink != ins[i]) {
            printf("bad reference link %d, %d, %d\n", i, ins[i], inode.nlink);
            return -1;
        }
    }
    return 1;
}
//No extra links allowed for directories (each directory only appears in one other directory).
//ERROR: directory appears more than once in file system.
int checkExtraLink(uint ins[]){
    struct dinode inode;
    int i;
    for( i = 0; i < sb.ninodes; i++)
    {
        rinode(i, &inode);
        if (inode.type != T_DIR) {
            continue;
        }
        if (ins[i] > 1) {
            printf("extra link %d, %d, %d\n", i,  ins[i], inode.nlink);
            return -1;
        }
    }
    return 1;
}

int main(int argc, char *argv[]){
    // Show USAGE
    if(argc < 2){
        fprintf(stderr, "ERROR: image not found. Usage: fschecker fs.img\n");
        return 1;
    } 
    // Open file system image
    fsfd = open(argv[1], O_RDONLY);
    
    if (fsfd < 0) { 
        perror(argv[1]);
        return 1;
    }
    uchar buf[BSIZE];
    //Read SuperBlock
    rsect(SUPERBLOCK, buf);
    memmove(&sb,buf,sizeof(sb));
    printf("sb.size: %d, sb.nblocks: %d, sb.ninodes:%d, sb.nlog: %d, sb.logstart: %d, sb.inodestart: %d, sb.bmapstart: %d\n",
            sb.size, sb.nblocks, sb.ninodes, sb.nlog, sb.logstart, sb.inodestart, sb.bmapstart); 
    rsect(sb.bmapstart, &buf);
    //Check bad inode and whether root directory exist.
    if(checkBadInode()==-1){
        close(fsfd);
        return 1;
    }
    //Check Valid address in inode
    if (checkValidAddr()==-1) {
        close(fsfd);
        printERR("ERROR: bad address in inode.");
        return 1;
    }
    //Check the validation of directory format.
    if (checkDirFormat(".")==-1 || checkDirFormat("..")==-1) {
        close(fsfd);
        printERR("ERROR: directory not properly formatted.");
        return 1;
    }
    //Check whether match the parent directory
    if (checkParentDir()==-1) {
        close(fsfd);
        printERR("ERROR: bad address in inode.");
        return 1;
    }
    uint addresses[sb.size];
    int i;
    for(i = 0; i < sb.size; i++)
    {
        addresses[i] = 0;
    }
    // Check dirty bimap Address
    if (checkBitmap(addresses)==-1) {
        close(fsfd);
        printERR("ERROR: address used by inode but marked free in bitmap.");
        return 1;
    }
    // Check dirty bitmap block
    if (checkUsedBlock(addresses)==-1) {
        close(fsfd);
        printERR("ERROR: bitmap marks block in use but it is not in use.");
        return 1;
    }
    // Check the use times of the same address
    if (checkUsedTimes(addresses)==-1) {
        close(fsfd);
        printERR("ERROR: address used more than once.");
        return 1;
    }
    uint inodes[sb.ninodes];
    for(i=0;i<sb.ninodes;i++){
        inodes[i] = 0;
    }
    //Check Inode Reference
    if (checekValidDir(inodes) == -1) {
        close(fsfd);
        printERR("ERROR: inode referred to in directory but marked free.");
        return 1;
    }
    //ERROR: inode marked use but not found in a directory.
    if (checkInodeRef(inodes) == -1) {
        close(fsfd);
        printERR("ERROR: inode marked use but not found in a directory.");
        return 1;
    }
    
    // Check the bad reference count
    if (checkBadLinks(inodes) == -1) {
        close(fsfd);
        printERR("ERROR: bad reference count for file.");
        return 1;
    }
    //The the time of directory appears
    if (checkExtraLink(inodes) == -1) {
        close(fsfd);
        printERR("directory appears more than once in file system.");
        return 1;
    }

    printf("All Test Passed\n");
    return 0;
}