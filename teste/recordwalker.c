#include <stdio.h>
#include "t2fs.h"
#include "walker.h"
#include "utils.h"

int main(int argc, char** argv){
    RECORD_WALKER rec_walk;
    char code_types[5][20] = {DEBUG_OUT_REC_WALKER};
    int code;

    code = getRecordFromPath("/sub/arq2", &rec_walk);
    printf("\n/sub/arq2  ANSWER: %s\n", code_types[code]);
    recordWalkerPrint(rec_walk);

    code = getRecordFromPath("/arq", &rec_walk);
    printf("\n/arq  ANSWER: %s\n", code_types[code]);
    recordWalkerPrint(rec_walk);

    code = getRecordFromPath("/arq/arq2", &rec_walk);
    printf("\n/arq/arq2  ANSWER: %s\n", code_types[code]);
    recordWalkerPrint(rec_walk);

    return TRUE;
}
