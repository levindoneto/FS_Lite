#include <stdio.h>
#include "t2fs.h"
#include "fslite.h"
#include "walker.h"
#include "utils.h"
#include "diskio.h"
#include "constants.h"
#include "bitmap2.h"

#define READ_LEN 150
int main(int argc, char** argv){
    FILE2 file, file2;
    DIR2 dir;
    int i = 0, read;
    char str[12] = "/sub/arqXXX\0";
	char str2[10] = "/sub/dir/\0";
	char str3[18] = "/sub/dir/filesXXX\0";
	char str4[19] = "/sub/dir/anotherd/\0";
	char str5[13] = "/sub/subdir/\0";
    char buffer[READ_LEN + 1];
	char original[READ_LEN + 1];
    /*
    RECORD_T records[RECORD_PER_BLOCK];
    BLOCK_T block;
    readBlock(0, &block);
    blockToRecords(&block, records);
    records[0].bytesFileSize = 20;
    writeRecord(0, 0, &records[0]);
    records[RECORD_PER_BLOCK-1].TypeVal = FILE_TYPE;
    strcpy(records[RECORD_PER_BLOCK-1].name, "TESTE");
    writeRecord(0, RECORD_PER_BLOCK-1, &records[RECORD_PER_BLOCK-1]);
    */
	printf("Creating empty files and dirs...\n");
    for (i = 2; i < 15; i++){
        str[8] = i / 100 + 48;
        str[9] = (i % 100) / 10 + 48;
		str[10] = (i % 100) % 10 + 48;
        file = create2(str);
		if(file != FALSE)
        	close2(file);
    }

	mkdir2(str2);


	for (i = 1; i < 5; i++){
        str3[14] = i / 100 + 48;
        str3[15] = (i % 100) / 10 + 48;
		str3[16] = (i % 100) % 10 + 48;
        file = create2(str3);
		if(file != FALSE)
        	close2(file);
    }
	mkdir2(str4);
	mkdir2(str5);


    mkdir2("/test/");
	printf("Files Created!\n");
	printf("Tree of the File System:\n");
	tree(0,0);
    
	
	printf("Testing Open, Close, Read, Write, Seek...\n");
    

	printf("Opening '/sub/arq2'...\n");
    file = open2("/sub/arq2");
	printf("Reading '/sub/arq2'...\n");
    read = read2(file, original, READ_LEN);
	seek2(file, 0);
	read = read2(file, buffer, READ_LEN);
	original[read] = '\0';
    buffer[read] = '\0';
    printf("READ '/sub/arq2': %s (%i chars)\n", buffer, read);
    printf("FILE CURSOR: %i\n", handles.files[file].cursor);
	printf("Seek to third byte in '/sub/arq2'...\n");
    seek2(file, 3);
    printf("FILE CURSOR: %i\n", handles.files[file].cursor);
	printf("Writing on file '/sub/arq2'...\n");
	write2(file, "I wrote in this file!", read);
	seek2(file, 0);
	read = read2(file, buffer, READ_LEN);
    buffer[read] = '\0';
    printf("READ '/sub/arq2': %s (%i chars)\n", buffer, read);
	seek2(file, 0);
    printf("Writing back the original value of '/sub/arq2'...\n");
	write2(file, original, read);
	seek2(file, 0);
	read = read2(file, buffer, READ_LEN);
    buffer[read] = '\0';
    printf("READ '/sub/arq2': %s (%i chars)\n", buffer, read);
	printf("Closing '/sub/arq2'...\n");
    close2(file);
/*
    file = open2("/arq");
    read = read2(file, buffer, READ_LEN);
    buffer[read] = '\0';
    printf("READ '/ARQ': %s (%i chars)\n", buffer, read);
    file2 = create2("/arq3");
    bitmapPrint (BITMAP_DADOS);

    write2(file2, buffer, read);
    bitmapPrint (BITMAP_DADOS);


    read = read2(file2, buffer, read);
    buffer[read] = '\0';
    printf("READ '/ARQ3': %s (%i chars)\n", buffer, read);
    close2(file);
    close2(file2);
	listDirWithInode(2);
*/
    
    return TRUE;
}
    
