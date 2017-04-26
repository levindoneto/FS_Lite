#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bitmap2.h"
#include "diskio.h"
#include "fslite.h"

HANDLES handles = {.init = FALSE};

int newHandle(int option){
    int i;
    if (handles.init == FALSE)
        handles = initHandles();
    if (option == FILE_TYPE){
        for(i = 0; i < ENTRY_MAX; i++){
            if (handles.files[i].valid == FALSE)
                return i;
        }
    }
    else if (option == DIR_TYPE){
        for(i = 0; i < ENTRY_MAX; i++){
            if (handles.dirs[i].valid == FALSE)
                return i;
        }
    }
    return INVALID_HANDLE;
}

/*
    Create new Inode and first data block.
    BE CAREFULL: it's change bitmaps.
    Return:
    INVALID_PTR if inode/block
        are all used,
    Else return inode_index
        of this new inode
*/
int newInode(INODE_T* inode){
    int inode_index, block_index;

    inode_index = searchBitmap2(BITMAP_INODE, FREE);
    if (inode_index <= 0)
        return INVALID_PTR;
    setBitmap2(BITMAP_INODE, inode_index, USED);

    block_index = searchBitmap2(BITMAP_DADOS, FREE);
    if (block_index <= 0)
        return INVALID_PTR;
    setBitmap2(BITMAP_DADOS, block_index, USED);

    *inode = initInode();
    inode->dataPtr[0] = block_index;
    return inode_index;
}

void deleteInode(int inode_index){
    /* Free inode*/
    setBitmap2(BITMAP_INODE, inode_index, FREE);
}
/*
    Search for a invalid record starting in block index 0.
    If found return TRUE, else FALSE
*/
int getNextInvalidRecord(INODE_WALKER* walker, RECORD_MAP* rec_pos){
    RECORD_T records[RECORD_PER_BLOCK];
    BLOCK_T mem_block;
    int i, next_block;

    /* Initialing walker cursor at block 0*/
    next_block = getBlockIndexAt(walker, 0);
	while(next_block != INVALID_PTR){
        if (readBlock(next_block, &mem_block) == FALSE)
            return FALSE;
        blockToRecords(&mem_block, records);
        for (i = 0; i < RECORD_PER_BLOCK; i++){
            if(records[i].TypeVal == INVALID_TYPE){
                rec_pos->block = next_block;
                rec_pos->index = i;
                return TRUE;
            }
        }
        next_block = getNextBlockIndex(walker);
	}
    return FALSE;
}
/*
    Search for a valid record initialing at 'last_index'
    If found return record_index + 1 to be set as new last_index
    If not return INVALID_PTR
*/
int getNextValidRecord(INODE_WALKER* walker, RECORD_T *buffer, int last_index){
    RECORD_T records[RECORD_PER_BLOCK];
    BLOCK_T mem_block;
    int i, next_block, initial_record;

    if (last_index < 0)
        return INVALID_PTR;

    initial_record = last_index % RECORD_PER_BLOCK;
    next_block = getBlockIndexAt(walker, last_index/RECORD_PER_BLOCK);
	while(next_block != INVALID_PTR){
        if (readBlock(next_block, &mem_block) == FALSE)
            return FALSE;
        blockToRecords(&mem_block, records);
        for (i = initial_record; i < RECORD_PER_BLOCK; i++){
            if(records[i].TypeVal != INVALID_TYPE){
                *buffer = records[i];
                return walker->cursor*RECORD_PER_BLOCK + i + 1;
            }
        }
        next_block = getNextBlockIndex(walker);
		initial_record = 0;
	}
    return INVALID_PTR;
}

/*
    Search for a valid record initialing at 'last_index'
    If found return record_index + 1 to be set as new last_index
    If not return INVALID_PTR
*/
int getLastValidRecord(INODE_WALKER* walker, RECORD_T *buffer){
    RECORD_T records[RECORD_PER_BLOCK];
    BLOCK_T mem_block;
    int i, next_block, block_counter, last_block;

    block_counter = 0;
    last_block = 0;
    next_block = getBlockIndexAt(walker, 0);
	while(next_block != INVALID_PTR){
        readBlock(next_block, &mem_block);
        blockToRecords(&mem_block, records);
        block_counter++;
        for (i = 0; i < RECORD_PER_BLOCK; i++){
            if(records[i].TypeVal != INVALID_TYPE){
                *buffer = records[i];
                last_block = block_counter;
            }
        }
        next_block = getNextBlockIndex(walker);
	}
    return last_block;
}



int pathValidation(char* pathname){
    int i, flag = FALSE;
    if(strlen(pathname) == 0)
        return FALSE;
    if(pathname[0] != '/')
        return FALSE;
    for(i = 0; i < strlen(pathname); i++){
        if(pathname[i] == '/'){
            if (flag == TRUE)
                return FALSE;
            flag = TRUE;
        }
        else
            flag = FALSE;
    }
    return TRUE;
}

HANDLES initHandles(){
    HANDLES hand;
    int i;

    for(i = 0; i < ENTRY_MAX; i++){
        hand.files[i].valid = FALSE;
        hand.dirs[i].valid = FALSE;
    }
    hand.init = TRUE;
    return hand;
}

FILE_T initFile(INODE_WALKER inode_w, RECORD_WALKER record_w){
    FILE_T file;

    file.valid =  TRUE;
    file.cursor = 0;
    file.modified = FALSE;
    file.record_walker = record_w;
    file.inode_walker = inode_w;
    return file;
}

DIR_T initDir(INODE_WALKER inode_w, RECORD_WALKER record_w){
    DIR_T dir;

    dir.valid =  TRUE;
    dir.current_entry = 0;
    dir.record_walker = record_w;
    dir.inode_walker = inode_w;
    return dir;
}
