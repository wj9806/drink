//
// Created by xsy on 2024/3/21.
//
#include "cli.h"
#include "vm.h"
#include "core.h"
#include "parser.h"
#include <stdio.h>
#include <string.h>
#include "core.h"
#include "class.h"

static void run_file(const char * path)
{
    //strrchr 搜索最后一次出现字符，指针并指向该字符
    const char * last_slash = strrchr(path, '/');
    if (last_slash != NULL)
    {
        char * root = (char *) malloc(last_slash - path + 2);
        memcpy(root, path, last_slash - path + 1);

        root[last_slash - path + 1] = '\0';
        root_dir = root;
    }

    VM* vm = new_vm();

    const char * source_code = read_file(path);

    execute_module(vm, OBJ_TO_VALUE(new_obj_string(vm, path, strlen(path))), source_code);
}

int main(int argc, char** argv)
{
    if (argc == 1) {
    }
    run_file(argv[1]);
    return 0;
}