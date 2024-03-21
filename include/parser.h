//
// Created by xsy on 2024/3/21.
//

#ifndef DRINK_PARSER_H
#define DRINK_PARSER_H

#include "vm.h"
#include "common.h"

typedef enum
{
    Token_EOF,          //EOF
} TokenType;

typedef struct {
    TokenType type;
    const char * start;
    uint32_t length;
    uint32_t lineNo;
} Token;

struct parser {
    const char * file;
    const char * sourceCode;
    const char * nextCharPtr;
    char curChar;
    Token curToken;
    Token preToken;
    int interpolationExpectRightParenNum;
    VM* vm;
};

#endif //DRINK_PARSER_H
