/* Boolean types*/
#define FALSE -1
#define TRUE   0

#define ENTRY_MAX 20

#define MAX_SIZE  4229169792

/* Define sizes in Bytes*/
#define BLOCK_SIZE  4096
#define RECORD_SIZE   64
#define INODE_SIZE    16

/* Important quantities */
#define SECTOR_PER_BLOCK   16   /* DEFINITION */
#define PTR_PER_BLOCK    1024   /* 4096 Bytes /  4 Bytes */
#define INODE_PER_SECTOR   16   /*  256 Bytes / 16 Bytes */
#define RECORD_PER_BLOCK   64   /* 4096 Bytes / 64 Bytes */
#define RECORD_PER_SECTOR   4   /*  256 Bytes / 64 Bytes */

/* Define size in Bits*/
#define BITMAP_DATA_SIZE 2040
#define BITMAP_INODE_SIZE 2000

/* WALKER output constants*/
#define CHILD_PARENT_FOUND 0
#define CHILD_FOUND        1
#define PARENT_FOUND       2
#define NOT_FOUND          3
#define INVALID_PATH       4

#define VALID_REC          0
#define INVALID_REC       -1
#define ROOT_REC          -2

/* Offsets in sector width in sectors*/
#define SUPERBLOCK_OFFSET   0
#define BLOCK_BIT_OFFSET    1
#define INODE_BIT_OFFSET    2
#define INODE_DATA_OFFSET   3
#define BLOCK_DATA_OFFSET 128

/* Inode info*/
#define INODE_DIR_P  2
#define INODE_IND_P  1
#define INODE_DIND_P 1

#define DEBUG_OUT_REC_WALKER "CHILD_PARENT_FOUND\0", "CHILD_FOUND\0", "PARENT_FOUND\0", "NOT_FOUND\0", "INVALID_PATH\0"

/* Record TypeVal definition*/
#define INVALID_TYPE 0x00
#define FILE_TYPE    0x01
#define DIR_TYPE     0x02

#define INVALID_HANDLE -1

/* BITMAP*/
#define FREE 0
#define USED 1
