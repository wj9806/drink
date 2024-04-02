//
// Created by xsy on 2024/3/21.
//

#ifndef DRINK_PARSER_H
#define DRINK_PARSER_H

#include "vm.h"
#include "common.h"
#include "meta_obj.h"

typedef enum
{
    TOKEN_UNKNOWN,
    // 数据类型
    TOKEN_NUM,		            //数字
    TOKEN_STRING,     	        //字符串
    TOKEN_ID,	     	        //变量名
    TOKEN_INTERPOLATION,        //内嵌表达式

    // 关键字(系统保留字)
    TOKEN_VAR,		            //'var'
    TOKEN_FUN,		            //'fun'
    TOKEN_IF,		            //'if'
    TOKEN_ELSE,	     	        //'else'
    TOKEN_TRUE,	     	        //'true'
    TOKEN_FALSE,	     	    //'false'
    TOKEN_WHILE,	     	    //'while'
    TOKEN_FOR,	     	        //'for'
    TOKEN_BREAK,	     	    //'break'
    TOKEN_CONTINUE,             //'continue'
    TOKEN_RETURN,     	        //'return'
    TOKEN_NULL,	     	        //'null'

    //以下是关于类和模块导入的token
    TOKEN_CLASS,	     	    //'class'
    TOKEN_THIS,	     	        //'this'
    TOKEN_STATIC,     	        //'static'
    TOKEN_IS,		            // 'is'
    TOKEN_SUPER,	     	    //'super'
    TOKEN_IMPORT,     	        //'import'

    //分隔符
    TOKEN_COMMA,		        //','
    TOKEN_COLON,		        //':'
    TOKEN_LEFT_PAREN,	        //'('
    TOKEN_RIGHT_PAREN,	        //')'
    TOKEN_LEFT_BRACKET,	        //'['
    TOKEN_RIGHT_BRACKET,	    //']'
    TOKEN_LEFT_BRACE,	        //'{'
    TOKEN_RIGHT_BRACE,	        //'}'
    TOKEN_DOT,		            //'.'
    TOKEN_DOT_DOT,	            //'..'

    //简单双目运算符
    TOKEN_ADD,		            //'+'
    TOKEN_SUB,		            //'-'
    TOKEN_MUL,		            //'*'
    TOKEN_DIV,		            //'/'
    TOKEN_MOD,		            //'%'

    //赋值运算符
    TOKEN_ASSIGN,	            //'='

    // 位运算符
    TOKEN_BIT_AND,	            //'&'
    TOKEN_BIT_OR,	            //'|'
    TOKEN_BIT_NOT,	            //'~'
    TOKEN_BIT_SHIFT_RIGHT,      //'>>'
    TOKEN_BIT_SHIFT_LEFT,       //'<<'

    // 逻辑运算符
    TOKEN_LOGIC_AND,	        //'&&'
    TOKEN_LOGIC_OR,	            //'||'
    TOKEN_LOGIC_NOT,	        //'!'

    //关系操作符
    TOKEN_EQUAL,		        //'=='
    TOKEN_NOT_EQUAL,	        //'!='
    TOKEN_GREAT,	            //'>'
    TOKEN_GREAT_EQUAL,	        //'>='
    TOKEN_LESS,		            //'<'
    TOKEN_LESS_EQUAL,	        //'<='

    TOKEN_QUESTION,	            //'?'

    //文件结束标记,仅词法分析时使用
    TOKEN_EOF		   //'EOF'
} token_type;

typedef struct {
    token_type type;
    const char * start;
    uint32_t length;
    uint32_t line_no;
    value value;
} token;

struct parser {
    const char * file;
    const char * source_code;
    const char * next_char_ptr;
    char cur_char;
    token cur_token;
    token pre_token;
    //当前正在编译的模块
    obj_module * cur_module;

    //处理内嵌表达式，期望右括号的数量
    int interpolation_expect_right_paren_num;
    VM* vm;
};

#define PEEK_TOKEN(parser_ptr) parser-parser_ptr->cur_token.type

char look_ahead_char(parser * parser);

void get_next_token(parser * parser);

bool match_token(parser * parser, token_type expected);

void consume_cur_token(parser * parser, token_type expected, const char * errMsg);

void consume_next_token(parser * parser, token_type expected, const char * errMsg);

uint32_t get_byte_num_of_encode_utf8(int value);

uint8_t encode_utf8(uint8_t * buf, int value);

void init_parser(VM* vm, parser * parser, const char * file, const char * source_code, obj_module * obj_module);


#endif //DRINK_PARSER_H
