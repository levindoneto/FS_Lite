# File System

## Distrubuição de tarefas

### Funções da API

**1° Grupo:** _Eduardo_
* FILE2 open2 (char* filename)
* DIR2 opendir2 (char* pathname)
* FILE2 create2 (char *filename)
* int mkdir2 (char *pathname)

**2° Grupo:** _Béuren_
* int close2 (FILE2 handle)
* int closedir2 (DIR2 handle)
* int delete2 (char *filename)
* int rmdir2 (char *pathname)
* int truncate2 (FILE2 handle)

**3° Grupo:** _Levindo_
* int read2 (FILE2 handle, char *buffer, int size)
* int write2 (FILE2 handle, char *buffer, int size)
* int readdir2 (DIR2 handle, DIRENT2 *dentry)
* int seek2 (FILE2 handle, DWORD offset)

### Funções Auxiliares

* int getRecordFromPath (char* pathname, t2fs_record* parent, t2fs_record* child)
* int freeDataBlocks (int inode_index, int blocks)
* int readMemBytes (MEM_COORD position, char* buffer, int lenght)
* int writeMemBytes (MEM_COORD position, char* buffer, int lenght)
