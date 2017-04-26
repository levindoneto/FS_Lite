#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fslite.h"
#include "diskio.h"
#include "constants.h"
#include "walker.h"

#include "t2fs.h"

int identify2 (char *name, int size) {
	char* str ="\tBéuren Felipe Bechlin\t\t(00230321)\n\
\tEduardo Stein Brito\t\t(00243657)\n\
\tLevindo Gabriel Taschetto Neto\t(00243685)\n\0";

	memcpy(name, str, size);
	if (size == 0)
		name[0] = '\0';
	else
		name[size-1] = '\0';

	if (strlen(str) != strlen(name))
		return FALSE;
	return TRUE;
}

FILE2 create2 (char *filename){
	RECORD_WALKER rec_walk;
	RECORD_MAP record_pos;
	INODE_WALKER inode_walk;
	INODE_T  inode;
	int handle, inode_index;

	handle = newHandle(FILE_TYPE);
	if (handle == INVALID_HANDLE)
		return FALSE;

	/* Verify path and test if it's refers a directory*/
	if(pathValidation(filename) == FALSE || filename[strlen(filename)-1] == '/')
		return FALSE;

	if (getRecordFromPath(filename, &rec_walk) == PARENT_FOUND){
		if(rec_walk.parent.TypeVal != DIR_TYPE)
		    return FALSE;
		/* Creating inode walker for parent to search for a invalid record*/
		inode_walk = initInodeWalker(rec_walk.parent.inodeNumber);
		/* No invalid records, need to create a new block*/
		if(getNextInvalidRecord(&inode_walk, &record_pos) == FALSE){
		    BLOCK_T block = newBlockWithInvalidRecord();
		    int block_index = getNewBlockIndex(&inode_walk);
		    if (block_index == INVALID_PTR)
		        return FALSE;
		    writeBlock(block_index, &block);
		    /* Set parent dir info*/
		    rec_walk.parent.blocksFileSize ++;
		    rec_walk.parent.bytesFileSize += BLOCK_SIZE;
		    /* If it's root will not save*/

		    writeRecord(rec_walk.parent_pos.block, rec_walk.parent_pos.index,
		        &rec_walk.parent);
		    record_pos.block = block_index;
		    record_pos.index = 0;
		}
		/* Creating inode and saving on memory*/
		inode_index = newInode(&inode);
		if(inode_index == INVALID_PTR)
		    return FALSE;
		writeInode(inode_index, &inode);
		/* Creating inode walker and set record file and parent dir*/
		inode_walk = initInodeWalker(inode_index);
		rec_walk.child.TypeVal = FILE_TYPE;
		rec_walk.child.inodeNumber = inode_index;
		rec_walk.child_pos = record_pos;
		writeRecord(rec_walk.child_pos.block, rec_walk.child_pos.index,
		    &rec_walk.child);
		handles.files[handle] = initFile(inode_walk, rec_walk);
		return (FILE2) handle;
	}
	return FALSE;
}

int delete2 (char *filename){
	RECORD_WALKER rec_walk;
	RECORD_T record;
	INODE_WALKER inode_walk;
	int valid_blocks;

	/* Verify path and test if it's refers a directory*/
	if(pathValidation(filename) == FALSE || filename[strlen(filename)-1] == '/')
		return FALSE;

	if (getRecordFromPath(filename, &rec_walk) == CHILD_PARENT_FOUND){
		if(rec_walk.child.TypeVal != FILE_TYPE)
		    return FALSE;

		/* Free entry at PARENT*/
		rec_walk.child.TypeVal = INVALID_TYPE;
		writeRecord(rec_walk.child_pos.block, rec_walk.child_pos.index,
			&rec_walk.child);
		inode_walk = initInodeWalker(rec_walk.parent.inodeNumber);

		valid_blocks = getLastValidRecord(&inode_walk, &record);
		if (valid_blocks == 0)
			rec_walk.parent.blocksFileSize = 1;
		else
			rec_walk.parent.blocksFileSize = valid_blocks;
		rec_walk.parent.bytesFileSize = rec_walk.parent.blocksFileSize*BLOCK_SIZE;
		deleteBlocksStartingAt(&inode_walk, (int)rec_walk.parent.blocksFileSize);
		writeRecord(rec_walk.parent_pos.block, rec_walk.parent_pos.index,
			&rec_walk.parent);

		/* Creating inode walker for child to FREE all data blocks*/
		inode_walk = initInodeWalker(rec_walk.child.inodeNumber);
		deleteBlocksStartingAt(&inode_walk, 0);
		deleteInode((int)rec_walk.child.inodeNumber);
		return TRUE;
	}
	return FALSE;
}

FILE2 open2 (char *filename){
	RECORD_WALKER rec_walk;
	INODE_WALKER inode_walk;
	int handle;

	handle = newHandle(FILE_TYPE);
	if (handle == INVALID_HANDLE)
		return FALSE;

	/* Verify path and test if it's refers a directory*/
	if(pathValidation(filename) == FALSE || filename[strlen(filename)-1] == '/')
		return FALSE;

	/* NORMAL FILE OR ROOT FILE*/
	if (getRecordFromPath(filename, &rec_walk) == CHILD_PARENT_FOUND){
		if(rec_walk.child.TypeVal != FILE_TYPE){
			return FALSE;
		}
		inode_walk = initInodeWalker(rec_walk.child.inodeNumber);
		handles.files[handle] = initFile(inode_walk, rec_walk);
		return (FILE2) handle;
	}
	return FALSE;
}

int close2 (FILE2 handle){

	if (handle < 0 || handle >= ENTRY_MAX)
	return FALSE;

	if (handles.files[handle].valid == FALSE)
		return FALSE;

	if (handles.files[handle].modified == TRUE)
		writeRecord(
			handles.files[handle].record_walker.child_pos.block,
			handles.files[handle].record_walker.child_pos.index,
			&handles.files[handle].record_walker.child);
	handles.files[handle].valid = FALSE;
	return TRUE;
}

int read2 (FILE2 handle, char *buffer, int size){
	BLOCK_T block;
	int next_block;
	int initial_pos, look_size, read_size;

	if (handle < 0 || handle >= ENTRY_MAX)
		return FALSE;

	if (handles.files[handle].valid == FALSE)
		return FALSE;

	if (size <= 0)
		return FALSE;

	if (handles.files[handle].cursor + size >
		handles.files[handle].record_walker.child.bytesFileSize)
		size = handles.files[handle].record_walker.child.bytesFileSize -
			handles.files[handle].cursor;

	read_size = 0;
	look_size = 0;
	initial_pos = handles.files[handle].cursor % BLOCK_SIZE;
	next_block = getBlockIndexAt(&handles.files[handle].inode_walker,
			(handles.files[handle].cursor)/BLOCK_SIZE);

	while(next_block != INVALID_PTR && size > read_size){
		if (readBlock(next_block, &block) == FALSE)
			return FALSE;

		if(size - read_size < BLOCK_SIZE)
			look_size = size - read_size;
		else
			look_size = BLOCK_SIZE - initial_pos;

		memcpy((void*) &buffer[read_size], (void*) &block.at[initial_pos],
			look_size*sizeof(char));

		initial_pos = 0;
		read_size += look_size;
		next_block = getNextBlockIndex(&handles.files[handle].inode_walker);
	}
	handles.files[handle].cursor += read_size;
	return read_size;
}

int write2 (FILE2 handle, char *buffer, int size){
	BLOCK_T block;
	int i, final_block, last_block, next_block;
	int initial_pos, wrote_size, write_size;
	if (handle < 0 || handle >= ENTRY_MAX)
		return FALSE;

	if (handles.files[handle].valid == FALSE)
		return FALSE;

	if (size <= 0)
		return FALSE;

	final_block = (handles.files[handle].cursor + size - 1)/BLOCK_SIZE;
	last_block = getLastBlockCursor(&handles.files[handle].inode_walker) - 1;
	/* Alocating necessary blocks and set walker cursor to first INVALID_PTR*/
	for(i = last_block; i < final_block; i++){
		if (getNewBlockIndex(&handles.files[handle].inode_walker) == INVALID_PTR)
			return FALSE;
	}

	wrote_size = 0;
	write_size = 0;
	next_block = getBlockIndexAt(&handles.files[handle].inode_walker,
			(handles.files[handle].cursor)/BLOCK_SIZE);
	initial_pos = handles.files[handle].cursor % BLOCK_SIZE;
	while(wrote_size < size){
		if (readBlock(next_block, &block) == FALSE)
			return FALSE;

		if(size - wrote_size < BLOCK_SIZE)
			write_size = size - wrote_size;
		else
			write_size = BLOCK_SIZE - initial_pos;
		memcpy((void*) &block.at[initial_pos], (void*) &buffer[wrote_size],
			write_size*sizeof(char));

		if (writeBlock(next_block, &block) == FALSE)
			return FALSE;

		initial_pos = 0;
		wrote_size += write_size;
		next_block = getNextBlockIndex(&handles.files[handle].inode_walker);
	}
	handles.files[handle].cursor += size;
	handles.files[handle].record_walker.child.bytesFileSize += size;
	handles.files[handle].record_walker.child.blocksFileSize =
		(handles.files[handle].record_walker.child.bytesFileSize -1)/BLOCK_SIZE + 1;
	handles.files[handle].modified = TRUE;
	return TRUE;
}

int truncate2 (FILE2 handle){
	int file_size;

	if (handle < 0 || handle >= ENTRY_MAX)
		return FALSE;

	if (handles.files[handle].valid == FALSE)
		return FALSE;

	file_size = handles.files[handle].record_walker.child.bytesFileSize;

	if (handles.files[handle].cursor == file_size)
		return TRUE;
	deleteBlocksStartingAt(&handles.files[handle].inode_walker,
		handles.files[handle].cursor/BLOCK_SIZE + 1);
	/* Seet size as same cursor*/
	handles.files[handle].record_walker.child.bytesFileSize = handles.files[handle].cursor;
	handles.files[handle].record_walker.child.blocksFileSize =
		(handles.files[handle].record_walker.child.bytesFileSize-1)/BLOCK_SIZE + 1;
	handles.files[handle].modified = TRUE;
	return TRUE;
}

int seek2 (FILE2 handle, DWORD offset){

	if (handle < 0 || handle >= ENTRY_MAX)
		return FALSE;
	if (handles.files[handle].valid == FALSE)
		return FALSE;


	if((int)offset == -1){
		handles.files[handle].cursor = handles.files[handle].record_walker.child.bytesFileSize;
		return TRUE;
	}
	else if((int)offset > handles.files[handle].record_walker.child.bytesFileSize){
		return FALSE;
	}
	else{
		handles.files[handle].cursor = (int) offset;
		return TRUE;
	}
}

int mkdir2 (char *pathname){
	RECORD_WALKER rec_walk;
	RECORD_MAP record_pos;
	INODE_WALKER inode_walk;
	INODE_T  inode;
	int handle, inode_index;

	handle = newHandle(DIR_TYPE);
	if (handle == INVALID_HANDLE)
		return FALSE;

	/* Verify path and test if it's refers a directory*/
	if(pathValidation(pathname) == FALSE || pathname[strlen(pathname)-1] != '/')
		return FALSE;

	if (getRecordFromPath(pathname, &rec_walk) == PARENT_FOUND){
		if(rec_walk.parent.TypeVal != DIR_TYPE)
		    return FALSE;
		/* Creating inode walker for parent to search for a invalid record*/
		inode_walk = initInodeWalker(rec_walk.parent.inodeNumber);
		/* No invalid records, need to create a new block*/
		if(getNextInvalidRecord(&inode_walk, &record_pos) == FALSE){
		    BLOCK_T block = newBlockWithInvalidRecord();
		    int block_index = getNewBlockIndex(&inode_walk);
		    if (block_index == INVALID_PTR)
		        return FALSE;
		    writeBlock(block_index, &block);
		    /* Set parent dir info*/
		    rec_walk.parent.blocksFileSize ++;
		    rec_walk.parent.bytesFileSize += BLOCK_SIZE;
		    /* If it's root will not save*/

		    writeRecord(rec_walk.parent_pos.block, rec_walk.parent_pos.index,
		        &rec_walk.parent);
		    record_pos.block = block_index;
		    record_pos.index = 0;
		}
		/* Creating inode and saving on memory*/
		inode_index = newInode(&inode);
		if(inode_index == INVALID_PTR)
		    return FALSE;
		writeInode(inode_index, &inode);
		/* Creating inode walker and set record file and parent dir*/
		inode_walk = initInodeWalker(inode_index);
		rec_walk.child.TypeVal = DIR_TYPE;
		rec_walk.child.bytesFileSize = BLOCK_SIZE;
		rec_walk.child.inodeNumber = inode_index;
		rec_walk.child_pos = record_pos;
		writeRecord(rec_walk.child_pos.block, rec_walk.child_pos.index,
		    &rec_walk.child);
		handles.dirs[handle] = initDir(inode_walk, rec_walk);
		return (DIR2) handle;
	}
	return FALSE;
}

int rmdir2 (char *pathname){
	RECORD_WALKER rec_walk;
	RECORD_T record;
	INODE_WALKER inode_walk;
	int valid_blocks;

	/* Verify path and test if it's refers a directory*/
	if(pathValidation(pathname) == FALSE || pathname[strlen(pathname)-1] != '/')
		return FALSE;

	if (getRecordFromPath(pathname, &rec_walk) == CHILD_PARENT_FOUND){
		if(rec_walk.child.TypeVal != DIR_TYPE)
			return FALSE;

		/* Search for a at least one entry*/
		inode_walk = initInodeWalker(rec_walk.child.inodeNumber);
		valid_blocks = getLastValidRecord(&inode_walk, &record);
		if(valid_blocks != 0){
			return FALSE;
		}

		/* Free entry at PARENT*/
		rec_walk.child.TypeVal = INVALID_TYPE;
		writeRecord(rec_walk.child_pos.block, rec_walk.child_pos.index,
			&rec_walk.child);
		inode_walk = initInodeWalker(rec_walk.parent.inodeNumber);

		valid_blocks = getLastValidRecord(&inode_walk, &record);
		if (valid_blocks == 0)
			rec_walk.parent.blocksFileSize = 1;
		else
			rec_walk.parent.blocksFileSize = valid_blocks;
		rec_walk.parent.bytesFileSize = rec_walk.parent.blocksFileSize*BLOCK_SIZE;
		deleteBlocksStartingAt(&inode_walk, (int)rec_walk.parent.blocksFileSize);
		writeRecord(rec_walk.parent_pos.block, rec_walk.parent_pos.index,
			&rec_walk.parent);

		/* Creating inode walker for child to FREE all data blocks*/
		inode_walk = initInodeWalker(rec_walk.child.inodeNumber);
		deleteBlocksStartingAt(&inode_walk, 0);
		deleteInode((int)rec_walk.child.inodeNumber);
		return TRUE;
	}
	return FALSE;
}

DIR2 opendir2 (char *pathname){
	RECORD_WALKER rec_walk;
	INODE_WALKER inode_walk;
	int code, handle;

	handle = newHandle(DIR_TYPE);
	if (handle == INVALID_HANDLE)
		return FALSE;

	/* Verify path and test if it's refers a directory*/
	if(pathValidation(pathname) == FALSE || pathname[strlen(pathname)-1] != '/')
		return FALSE;

	code = getRecordFromPath(pathname, &rec_walk);
	/* NORMAL DIR OR ROOT DIR*/
	if (code == CHILD_PARENT_FOUND || code == CHILD_FOUND){
		if(rec_walk.child.TypeVal != DIR_TYPE){
			return FALSE;
		}
		inode_walk = initInodeWalker(rec_walk.child.inodeNumber);
		handles.dirs[handle] = initDir(inode_walk, rec_walk);
		return (DIR2) handle;
	}
	return FALSE;
}

int readdir2 (DIR2 handle, DIRENT2 *dentry){
	RECORD_T record;
    INODE_WALKER* walker;

	if (handle < 0 || handle >= ENTRY_MAX)
		return FALSE;

	if (handles.dirs[handle].valid == FALSE)
		return FALSE;

	walker = &handles.dirs[handle].inode_walker;
	handles.dirs[handle].current_entry =
		getNextValidRecord(walker, &record, handles.dirs[handle].current_entry);

	if (handles.dirs[handle].current_entry == INVALID_PTR)
		return FALSE;
	recordToDirEnt(&record, dentry);
	return TRUE;
}

int closedir2 (DIR2 handle){
	if (handles.dirs[handle].valid == FALSE)
		return FALSE;

	handles.dirs[handle].valid = FALSE;
	return TRUE;
}
