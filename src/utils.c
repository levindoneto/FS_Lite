#include <stdio.h>
#include <stdlib.h>

#include "diskio.h"
#include "fslite.h"
#include "constants.h"
#include "bitmap2.h"
#include "utils.h"



/**
  * Functions for the print the structures
  * DEBUG stuff
  */

void superblockPrint (SUPERBLOCK_T superblock) {
    char ID[5] = {superblock.id[0], superblock.id[1], superblock.id[2], superblock.id[3], '\0'};
    printf("SUPERBLOCK:\n");
    printf(" File system identification: %s\n", ID);
    printf(" Currently version: %x\n", superblock.version);
    printf(" Quantity of sectors that form the superblock: %u\n", superblock.superblockSize);
    printf(" Quantity of sectors used to store the bitmap of blocks: %u\n", superblock.freeBlocksBitmapSize);
    printf(" Quantity of sectors used to store the bitmap of blocks: %u\n", superblock.freeInodeBitmapSize);
    printf(" Quantity of sectors used to store the i-nodes: %u\n", superblock.inodeAreaSize);
    printf(" Quantity of sectors that form the logic block: %u\n", superblock.blockSize);
    printf(" Disk size: %u\n", superblock.diskSize);
}

void recordPrint (RECORD_T record) {
    char types[3][10] = {"INVALID\0", "FILE\0", "DIR\0"};
    printf("T2FS_RECORD:\n");
    printf(" TypeVal: %s\n", types[(int)record.TypeVal]);
    printf(" Name of file: %s\n", record.name);
    printf(" Size of the File [Data Blocks]: %d\n", record.blocksFileSize);
    printf(" Size of the File [Number of bytes]: %d\n", record.bytesFileSize);
    printf(" Inode: %i\n", record.inodeNumber);
}

void inodePrint (INODE_T inode) {
    printf("T2FS_INODE:\n");
    printf(" Direct pointer[0]: %i\n", inode.dataPtr[0]);
    printf(" Direct pointer[1]: %i\n", inode.dataPtr[1]);
    printf(" Indirection single pointer: %i\n", inode.singleIndPtr);
    printf(" Indirection double pointer: %i\n", inode.doubleIndPtr);
}

void direntryPrint (DIRENT2 direntry) {
    printf("T2FS_DIRENTRY:\n");
    printf(" Name: %s\n", direntry.name);
    printf(" Type file: %hu\n", direntry.fileType);
    printf(" Size file: %d\n", direntry.fileSize);
}

void recordWalkerPrint(RECORD_WALKER walker){
    printf("--PARENT RECORD-- STATUS %s AT block: %i index: %i\n",
        walker.parent_code == VALID_REC ? "valid": "invalid",
        walker.parent_pos.block, walker.parent_pos.index);
    recordPrint(walker.parent);
    printf("--CHILD RECORD--  STATUS %s AT block: %i index: %i\n",
        walker.child_code == VALID_REC ? "valid": "invalid",
        walker.child_pos.block, walker.child_pos.index);
    recordPrint(walker.child);
}

void listDirWithInode (int inode_index){
    RECORD_T records[RECORD_PER_BLOCK];
    INODE_WALKER walker;
    BLOCK_T mem_block;
    int i, found, next_ptr;

    walker = initInodeWalker(inode_index);
    found = 0;
    printf("                Name               | Type | Size(blocks) |  Size(bytes)  | Inode\n");
    next_ptr = getNextBlockIndex(&walker);
    while(next_ptr != INVALID_PTR){
        if (readBlock(next_ptr, &mem_block) == FALSE)
            break;
        blockToRecords(&mem_block, records);
        for (i = 0; i < RECORD_PER_BLOCK; i++){
            if(records[i].TypeVal != INVALID_TYPE){

                printf("%-3i %-30s | %4.4s | %12i | %13i | %5i\n", found++, records[i].name,
                records[i].TypeVal == FILE_TYPE ? "FILE\0": "DIR\0", records[i].blocksFileSize,
                records[i].bytesFileSize, records[i].inodeNumber);
            }
        }
        next_ptr = getNextBlockIndex(&walker);
    }
}

void tree (int inode_index, int level){
    RECORD_T records[RECORD_PER_BLOCK];
    INODE_WALKER walker;
    BLOCK_T mem_block;
    int i, k, cursor, next_ptr;
    walker = initInodeWalker(inode_index);
    next_ptr = getNextBlockIndex(&walker);
    while(next_ptr != INVALID_PTR){
        if (readBlock(next_ptr, &mem_block) == FALSE)
            break;
        blockToRecords(&mem_block, records);
        for (i = 0; i < RECORD_PER_BLOCK; i++){
            if(records[i].TypeVal != INVALID_TYPE){
				for(k = 0; k < (level+1); k++){
				printf("--");
				}
				printf("%s%s\n", records[i].name, records[i].TypeVal == DIR_TYPE ? "/\0" : "");
				if(records[i].TypeVal == DIR_TYPE){
					cursor = walker.cursor;
					tree(records[i].inodeNumber, level + 1);
					walker.cursor = cursor;
				}
            }
        }
        next_ptr = getNextBlockIndex(&walker);
    }
}

void bitmapPrint (int handle){
    int location_first = 0;
    int location_last = 0;
    int last_location_value = 1;
    int temp_value = 0;
    int current_bit = 0;
    int size = 0;
    printf("BITMAP OF %s:\n", handle == BITMAP_DADOS ? "DATA\0": "INODES\0");
    handle == BITMAP_DADOS ? (size = BITMAP_DATA_SIZE): (size = BITMAP_INODE_SIZE);
    while(current_bit<size){
        temp_value = getBitmap2(handle, current_bit);
        if(temp_value == last_location_value){
            location_last++;
        }else{
            printf(" %4.4i .. %4.4i %s \n", location_first, location_last, last_location_value == USED ? "USED\0": "FREE\0");
            last_location_value = temp_value;
            location_first = current_bit + 1;
            location_last = current_bit + 1;
        }
        current_bit++;
    }
	if(location_first<=size-1)
	printf(" %4.4i .. %4.4i %s \n", location_first, location_last, last_location_value == USED ? "USED\0": "FREE\0");


}

void handlesPrint(){
    int i;
    printf("ALL HANDLES\n");
    printf("___________________________________________________________________\n");
    printf(" FILE | 0| 1| 2| 3| 4| 5| 6| 7| 8| 9|10|11|12|13|14|15|16|17|18|19|\n");
    printf("      ");
    for(i = 0; i < ENTRY_MAX; i++){
        printf("| %c", handles.files[i].valid == TRUE ? 'V': 'I');
    }
    printf("|\n___________________________________________________________________\n");
    printf(" DIR  | 0| 1| 2| 3| 4| 5| 6| 7| 8| 9|10|11|12|13|14|15|16|17|18|19|\n");
    printf("      ");
    for(i = 0; i < ENTRY_MAX; i++){
        printf("| %c", handles.dirs[i].valid == TRUE ? 'V': 'I');
    }
    printf("|\n___________________________________________________________________\n");
}
