//
// Created by wj on 24-3-21.
//

#include "core.h"
#include "utils.h"
#include <sys/stat.h>

//根目录
char * root_dir = NULL;

/**
 * 读取源码文件
 * @param path 源码文件路径
 * @return 源码文件内容
 */
char * read_file(const char * path)
{
    FILE * file = fopen(path, "r");
    if (file == NULL)
    {
        IO_ERROR("Couldn't open file %s .\n", path);
    }

    struct stat file_stat;
    stat(path, &file_stat);
    //通过stat获取文件大小
    size_t file_size = file_stat.st_size;
    char * file_content = (char *) malloc(file_size + 1);
    if (file_content == NULL)
    {
        MEM_ERROR("Couldn't alloc mem for read file: %s .\n", path);
        return (char *)0;
    }

    size_t read_size = fread(file_content, sizeof(char), file_size, file);
    if (read_size < file_size)
    {
        IO_ERROR("Read file: %s failed. \n", path);
    }
    file_content[file_size] = '\0';
    fclose(file);
    return file_content;
}
