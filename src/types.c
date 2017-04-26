#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"

#include "t2fs.h"
#include "fslite.h"


void blockToRecords(BLOCK_T* block_buffer, RECORD_T* vector){
    int i;
    for (i = 0; i < RECORD_PER_BLOCK; i++)
        memcpy((void*) &vector[i], (void*) &block_buffer->at[i*RECORD_SIZE], sizeof(RECORD_T));
}

void blockToPointers(BLOCK_T* block_buffer, int *vector){
    memcpy((void*) vector, (void*)block_buffer, sizeof(BLOCK_T));
}

void pointersToBlock(BLOCK_T* block_buffer, int *vector){
    memcpy((void*)block_buffer, (void*) vector, sizeof(BLOCK_T));
}

int sectorToRecord(BLOCK_T block_buffer, int entry_index, RECORD_T* record){
    /*Verify if index is on the sector*/
    if(entry_index < BLOCK_SIZE/RECORD_SIZE){
        void* buffer = (void*) &block_buffer.at[entry_index*RECORD_SIZE];
        memcpy((void*) record, buffer, sizeof(RECORD_T));
        return TRUE;
    }
    return FALSE;
}

int sectorToInode(SECTOR_T sector_buffer, int inode_index, INODE_T* inode){
    /*Verify if index is on the sector*/
    if(inode_index < SECTOR_SIZE/INODE_SIZE){
        void* buffer = (void*) &sector_buffer.at[inode_index*INODE_SIZE];
        memcpy((void*) inode, buffer, sizeof(INODE_T));
        return TRUE;
    }
    return FALSE;
}

void sectorToSuperblock(SECTOR_T* sector_buffer, SUPERBLOCK_T* superblock){
    void* buffer = (void*) sector_buffer;
    memcpy((void*) superblock, buffer, sizeof(SUPERBLOCK_T));
}


void recordToDirEnt(RECORD_T* record, DIRENT2* direntry){
    strncpy(direntry->name, record->name, MAX_FILE_NAME_SIZE+1);
    direntry->fileType = record->TypeVal;
    direntry->fileSize = record->bytesFileSize;
}

BLOCK_T newBlockWithInvalidPtr(){
    BLOCK_T block;
    int i;
    int* data;
    for(i = 0; i < PTR_PER_BLOCK; i++){
        data = (int*) &block.at[i*sizeof(int)];
        *data = INVALID_PTR;
    }
    return block;
}

BLOCK_T newBlockWithInvalidRecord(){
    BLOCK_T block;
    RECORD_T* data;
    int i;
    for(i = 0; i < RECORD_PER_BLOCK; i++){
        data = (RECORD_T*) &block.at[i*RECORD_SIZE];
        *data = initRecord();
    }
    return block;
}

INODE_T initInode(){
    INODE_T inode;
    inode.dataPtr[0] = INVALID_PTR;
    inode.dataPtr[1] = INVALID_PTR;
    inode.singleIndPtr = INVALID_PTR;
    inode.doubleIndPtr = INVALID_PTR;

    return inode;
}

RECORD_T initRecord(){
    RECORD_T record;
    record.TypeVal = INVALID_TYPE;
    record.name[0] = '\0';
    record.blocksFileSize = 1;
    record.bytesFileSize = 0;
    record.inodeNumber = INVALID_PTR;

    return record;
}
