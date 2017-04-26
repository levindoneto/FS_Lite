#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "diskio.h"

#include "t2fs.h"
#include "apidisk.h"
#include "fslite.h"

int readSector(int sector_index, SECTOR_T* buffer){
    if(sector_index < 0)
        return FALSE;
    if (read_sector((unsigned int)sector_index, (unsigned char*)buffer) == 0)
        return TRUE;
    return FALSE;
}

int readBlock(int block_index, BLOCK_T* buffer){
    int i, sector_index;

    if (block_index < 0)
        return FALSE;

    sector_index = BLOCK_DATA_OFFSET + block_index*SECTOR_PER_BLOCK;

    for(i = 0; i < SECTOR_PER_BLOCK; i++){
        if (readSector(sector_index, (SECTOR_T*) &buffer->at[i*SECTOR_SIZE]) == FALSE)
            return FALSE;
        sector_index++;
    }
    return TRUE;
}

int readInode(int inode_index, INODE_T* buffer){
    SECTOR_T sector_buffer;
    int sector_index;

    if (inode_index < 0)
        return FALSE;

    sector_index = INODE_DATA_OFFSET + inode_index/INODE_PER_SECTOR;

    if (readSector(sector_index, (SECTOR_T*) &sector_buffer) != 0)
        return FALSE;

    sectorToInode(sector_buffer, inode_index % INODE_PER_SECTOR, buffer);

    return TRUE;
}

int readSuperbloco(SUPERBLOCK_T* buffer){
    SECTOR_T sector;
    if (readSector (0, &sector) == FALSE)
        return FALSE;
    sectorToSuperblock (&sector, buffer);
    return TRUE;
}

int writeSector(int sector_index, SECTOR_T* buffer){
    if(sector_index < 0)
        return FALSE;
    if (write_sector((unsigned int)sector_index, (unsigned char*)buffer) == 0)
        return TRUE;
    return FALSE;
}

int writeInode(int inode_index, INODE_T* buffer){
    SECTOR_T sector_buffer;
    int sector_index;
    int inode_sector_index;

    if (inode_index < 0)
        return FALSE;

    sector_index = INODE_DATA_OFFSET + inode_index/INODE_PER_SECTOR;
    inode_sector_index = inode_index%INODE_PER_SECTOR;

    if (readSector(sector_index, &sector_buffer) == FALSE)
        return FALSE;

    memcpy((void*)&sector_buffer.at[inode_sector_index*INODE_SIZE],
        (void*) buffer, sizeof(INODE_T));

    if (writeSector(sector_index, (SECTOR_T*) &sector_buffer) == FALSE)
        return FALSE;

    return TRUE;
}

int writeBlock(int block_index, BLOCK_T* buffer){
    int i, sector_index;

    if (block_index < 0)
        return FALSE;

    sector_index = BLOCK_DATA_OFFSET + block_index*SECTOR_PER_BLOCK;

    for(i = 0; i < SECTOR_PER_BLOCK; i++){
        if (writeSector(sector_index++, (SECTOR_T*)&buffer->at[i*SECTOR_SIZE]) == FALSE)
            return FALSE;
    }

    return TRUE;
}

int writeRecord(int block, int index, RECORD_T* buffer){
    SECTOR_T sector_buffer;
    int sector_index, record_sector_index;

    if (block < 0 || index < 0)
        return FALSE;

    sector_index = BLOCK_DATA_OFFSET + block*SECTOR_PER_BLOCK
        + index/RECORD_PER_SECTOR;
    record_sector_index = index % RECORD_PER_SECTOR;

    if (readSector(sector_index, (SECTOR_T*) &sector_buffer) == FALSE)
        return FALSE;

    memcpy((void*)&sector_buffer.at[record_sector_index*RECORD_SIZE],
        (void*) buffer, sizeof(RECORD_T));

    if (writeSector(sector_index, (SECTOR_T*) &sector_buffer) == FALSE)
        return FALSE;

    return TRUE;
}
