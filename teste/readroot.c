#include <stdio.h>

#include "t2fs.h"
#include "diskio.h"
#include "utils.h"

int main(){
    INODE_T inode;
    SUPERBLOCK_T superblock;
    /* READING SUPERBLOCK*/
    if (readSuperbloco (&superblock) == FALSE){
        printf ("ERROR: Impossible to read disk superblock.\n");
        return FALSE;
    }
    superblockPrint (superblock);

    /* READING ROOT INODE*/
    if (readInode (0, &inode) == FALSE){
        printf ("ERROR: Impossible to read root inode.\n");
        return FALSE;
    }
    printf("**ROOT** \n");
    inodePrint (inode);
    listDirWithInode(0);

    /* READING SUB DIR INODE*/
    if (readInode (2, &inode) == FALSE){
        printf ("ERROR: Impossible to read dir inode.\n");
        return FALSE;
    }
    printf("**SUB** \n");
    inodePrint (inode);
    listDirWithInode(2);
    return TRUE;
}
