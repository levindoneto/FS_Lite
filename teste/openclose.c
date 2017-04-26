#include <stdio.h>
#include "t2fs.h"
#include "fslite.h"
#include "walker.h"
#include "utils.h"

int main(int argc, char** argv){
    FILE2 file;
    DIR2 dir;
    DIRENT2 dentry;

    file = open2("/arq/");
    handlesPrint();
    dir = opendir2("/");
    handlesPrint();

    while(readdir2(dir, &dentry) == TRUE)
        direntryPrint(dentry);
    printf("current_entry : %i\n", handles.dirs[dir].current_entry);
    close2(file);
    closedir2(dir);
    handlesPrint();
    return TRUE;
}
