//
// Created by xsy on 2024/3/21.
//
#include "cli.h"
#include "vm.h"
#include "core.h"
#include "parser.h"
#include <stdio.h>
#include <string.h>
#include "token.list"

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

    struct parser parser;
    init_parser(vm, &parser, path, source_code);

    while (parser.cur_token.type != TOKEN_EOF)
    {
        get_next_token(&parser);
        printf("%dL: %s [", parser.cur_token.line_no, token_array[parser.cur_token.type]);

        uint32_t idx = 0;
        while (idx < parser.cur_token.length)
        {
            printf("%c", *(parser.cur_token.start+idx++));

        }
        printf("]\n");
    }
}

int main(int argc, char** argv)
{
    if (argc == 1) {
        ;
    }
    run_file(argv[1]);
    return 0;
}