//
// Created by xsy on 2024/3/21.
//

#include "parser.h"
#include "utils.h"
#include <string.h>
#include <ctype.h>
#include "class.h"
#include "header_obj.h"
#include "compile.h"

//关键字、保留字结构
typedef struct
{
    char * keyword;
    uint8_t length;
    token_type token;
} keyword_token;

keyword_token keywords_token[] = {
        {"var",       3,      TOKEN_VAR},
        {"fun",       3,      TOKEN_FUN},
        {"if",       2,      TOKEN_IF},
        {"else",       4,      TOKEN_ELSE},
        {"true",       4,      TOKEN_TRUE},
        {"false",       5,      TOKEN_FALSE},
        {"while",       5,      TOKEN_WHILE},
        {"for",       3,      TOKEN_FOR},
        {"break",       5,      TOKEN_BREAK},
        {"continue",       8,      TOKEN_CONTINUE},
        {"return",       6,      TOKEN_RETURN},
        {"null",       4,      TOKEN_NULL},
        {"class",       5,      TOKEN_CLASS},
        {"is",       2,      TOKEN_IS},
        {"static",       6,      TOKEN_STATIC},
        {"this",       4,      TOKEN_THIS},
        {"super",       5,      TOKEN_SUPER},
        {"import",       6,      TOKEN_IMPORT},
        {NULL,       0,      TOKEN_UNKNOWN},
};

#define OPCODE_SLOTS(op_code, effect) effect,
static const int op_code_slots_used[] = {
#include "opcode.inc"
};
#undef OPCODE_SLOTS

//初始化compile_unit函数
static void init_compile_unit(parser * parser, compile_unit * cu,
                              compile_unit * enclosing_unit, bool is_method)
{
    parser->cur_compile_unit = cu;
    cu->cur_parser = parser;
    cu->enclosing_unit = enclosing_unit;
    cu->cur_loop = NULL;
    cu->enclosing_class_bk = NULL;
    if (enclosing_unit == NULL)
    {
        cu->scope_depth = -1;
        cu->local_var_num = 0;
    }
    else
    {
        if (is_method)
        {
            cu->local_vars[0].name = "this";
            cu->local_vars[0].length = 4;
        }
        else
        {
            cu->local_vars[0].name = NULL;
            cu->local_vars[0].length = 0;
        }
        cu->local_vars[0].scope_depth = -1;
        cu->local_vars[0].is_upvalue = false;
        cu->local_var_num = 1;
        cu->scope_depth = 0;
    }

    cu->stack_slot_num = cu->local_var_num;
    cu->fn = new_obj_fn(cu->cur_parser->vm,
                        cu->cur_parser->cur_module, cu->local_var_num);
}

//往函数的指令流中写入1字节，返回其索引
static int write_byte(compile_unit * cu, int byte)
{
#ifdef DEBUG
    int_buffer_add(cu->cur_parser->vm, &cu->fn->debug->line_no, cu->cur_parser->pre_token.line_no);
#endif
    byte_buffer_add(cu->cur_parser->vm, &cu->fn->instr_stream, (uint8_t)byte);
    return cu->fn->instr_stream.count - 1;
}

//写入操作码
static void write_opcode(compile_unit * cu, op_code opCode)
{
    write_byte(cu, opCode);
    cu->stack_slot_num += op_code_slots_used[opCode];
    if (cu->stack_slot_num > cu->fn->max_stack_slot_used_num)
    {
        cu->fn->max_stack_slot_used_num = cu->stack_slot_num;
    }
}

//写入1个字节的操作数
static int write_byte_operand(compile_unit * cu, int operand)
{
    return write_byte(cu, operand);
}

//／写入2个字节的操作数按大端字节序写入参数 低地址写高位，高地址写低位
inline static void write_short_operand(compile_unit * cu, int operand)
{
    write_byte(cu, (operand >> 8) & 0xFF);
    write_byte(cu, operand & 0xFF);
}

//写入操作数为1字节大小的指令
static int write_opcode_byte_operand(compile_unit * cu, op_code opCode, int operand)
{
    write_opcode(cu, opCode);
    return write_byte_operand(cu, operand);
}

//写入操作数为2字节大小的指令
static void write_opcode_short_operand(compile_unit * cu, op_code opCode, int operand)
{
    write_opcode(cu, opCode);
    write_short_operand(cu, operand);
}

static token_type id_or_keyword(const char * start, uint8_t length)
{
    uint32_t idx = 0;
    while (keywords_token[idx].keyword != NULL)
    {
        keyword_token candidate_token = keywords_token[idx++];
        if (candidate_token.length == length && memcmp(candidate_token.keyword, start, length) == 0) {
            return candidate_token.token;
        }
    }
    return TOKEN_ID;
}

//获取下一个字符
static void get_next_char(parser * parser)
{
    parser->cur_char = *parser->next_char_ptr++;
}

//查看下一个字符是否符合期望，如果是就读进来，返回true
static bool match_next_char(parser * parser, const char expected_char)
{
    if (look_ahead_char(parser) == expected_char)
    {
        get_next_char(parser);
        return true;
    }

    return false;
}

//跳过连续的空格
static void skip_blanks(parser * parser)
{
    while (isspace(parser->cur_char))
    {
        if (parser->cur_char == '\n')
        {
            //计算行号
            parser->cur_token.line_no++;
        }
        //跳过
        get_next_char(parser);
    }
}

/**
 * 解析标识符
 * 1.标识符 由数字、字母、下划线组成，首字符不能为数字
 *
 */
static void parse_id(parser * parser, token_type type)
{
    //isalnum函数: 如果 c 是一个数字或一个字母，则该函数返回非零值，否则返回 0。
    while (isalnum(parser->cur_char) || parser->cur_char == '_')
    {
        get_next_char(parser);
    }

    uint32_t length = (uint32_t)(parser->next_char_ptr - parser->cur_token.start - 1);
    if (type != TOKEN_UNKNOWN)
    {
        parser->cur_token.type = type;
    }
    else
    {
        parser->cur_token.type = id_or_keyword(parser->cur_token.start, length) ;
    }
    parser->cur_token.length = length;
}

//解析unicode码点
static void parse_unicode_code_point(parser * parser, byte_buffer * buf)
{
    uint32_t idx = 0;
    int value = 0;
    uint8_t digit = 0;

    while (idx ++ < 4)
    {
        get_next_char(parser);

        if (parser->cur_char == '\0')
        {
            LEX_ERROR(parser, "unterminated unicode!");
        }

        if (parser->cur_char >= '0' && parser->cur_char <= '9')
        {
            digit = parser->cur_char - '0';
        }
        else if (parser->cur_char >= 'a' && parser->cur_char <= 'f')
        {
            digit = parser->cur_char - 'a' + 10;
        }
        else if (parser->cur_char >= 'A' && parser->cur_char <= 'F')
        {
            digit = parser->cur_char - 'A' + 10;
        }
        else
        {
            LEX_ERROR(parser, "invalid unicode!");
        }
        value = value * 16 | digit;
    }

    uint32_t byte_num = get_byte_num_of_encode_utf8(value);
    ASSERT(byte_num !+ 0, "utf8 encode bytes should be between 1 and 4");

    byte_buffer_fill_write(parser->vm, buf, 0, byte_num);

    encode_utf8(buf->datas + buf->count - byte_num, value);
}

//解析字符串
static void parse_string(parser * parser)
{
    byte_buffer str;
    byte_buffer_init(&str);

    while (true)
    {
        get_next_char(parser);

        if (parser->cur_char == '\0')
        {
            LEX_ERROR(parser, "unterminated string");
        }
        if (parser->cur_char == '"')
        {
            parser->cur_token.type = TOKEN_STRING;
            break;
        }

        //字符串结束
        if (parser->cur_char == '%')
        {
            if (!match_next_char(parser, '('))
            {
                LEX_ERROR(parser, "'%' should followed by '('");
            }

            if (parser->interpolation_expect_right_paren_num > 0)
            {
                COMPILE_ERROR(parser, "This version is not supported nest interpolate expression");
            }

            parser->interpolation_expect_right_paren_num = 1;
            parser->cur_token.type = TOKEN_INTERPOLATION;
            break;
        }
        //转义字符
        if (parser->cur_char == '\\')
        {
            get_next_char(parser);
            switch (parser->cur_char) {
                case '0':
                    byte_buffer_add(parser->vm, &str, '\0');
                    break;
                case 'a':
                    byte_buffer_add(parser->vm, &str, '\a');
                    break;
                case 'b':
                    byte_buffer_add(parser->vm, &str, '\b');
                    break;
                case 'f':
                    byte_buffer_add(parser->vm, &str, '\f');
                    break;
                case 'n':
                    byte_buffer_add(parser->vm, &str, '\n');
                    break;
                case 'r':
                    byte_buffer_add(parser->vm, &str, '\r');
                    break;
                case 't':
                    byte_buffer_add(parser->vm, &str, '\t');
                    break;
                case 'u':
                    parse_unicode_code_point(parser, &str);
                    break;
                case '"':
                    byte_buffer_add(parser->vm, &str, '"');
                    break;
                case '\\':
                    byte_buffer_add(parser->vm, &str, '\\');
                    break;
                default:
                    LEX_ERROR(parser, "unsupported escape \\%c", parser->cur_char);
                    break;
            }
        }
        else
        {
            byte_buffer_add(parser->vm, &str, parser->cur_char);
        }
    }
    //字符串对象存储到value中
    obj_string * objString = new_obj_string(parser->vm, (const char *)str.datas, str.count);
    parser->cur_token.value = OBJ_TO_VALUE(objString);
    byte_buffer_clear(parser->vm, &str);
}

//跳过一行
static void skip_aline(parser * parser)
{
    get_next_char(parser);
    while (parser->cur_char != '\0')
    {
        if (parser->cur_char == '\n')
        {
            parser->cur_token.line_no++;
            get_next_char(parser);
            break;
        }
        get_next_char(parser);
    }
}

//处理注释
static void skip_comment(parser * parser)
{
    char next_char = look_ahead_char(parser);

    //行注释
    if (parser->cur_char == '/')
    {
        skip_aline(parser);
    }
    else
    {
        //块注释
        while (next_char != '*' && next_char != '\0')
        {
            get_next_char(parser);
            if (parser->cur_char == '\n')
            {
                parser->cur_token.line_no++;
            }
            next_char = look_ahead_char(parser);
        }
        if (match_next_char(parser, '*'))
        {
            if (!match_next_char(parser, '/'))
            {
                LEX_ERROR(parser, "expect '\' after '*' ");
            }
            get_next_char(parser);
        }
        else
        {
            LEX_ERROR(parser, "expect '*/' before file end!");
        }
    }

    skip_blanks(parser);
}

//十六进制
static void parse_hex_num(parser * parser)
{
    while (isxdigit(parser->cur_char))
    {
        get_next_char(parser);
    }
}

//十进制
static void parse_dec_num(parser * parser)
{
    while (isdigit(parser->cur_char))
    {
        get_next_char(parser);
    }

    if (parser->cur_char == '.' && isdigit(look_ahead_char(parser)))
    {
        get_next_char(parser);
        while (isdigit(parser->cur_char))
        {
            get_next_char(parser);
        }
    }
}

//八进制
static void parse_oct_num(parser * parser)
{
    while (parser->cur_char >= '0' && parser->cur_char < '8')
    {
        get_next_char(parser);
    }
}

//解析十六进制、十进制、八进制
static void parse_num(parser * parser)
{
    //strtol 可以给定字符串按照给定进制转成长整数
    //strtod 将字符串转成浮点数
    if (parser->cur_char == '0' && match_next_char(parser, 'x'))
    {
        get_next_char(parser);
        //十六进制
        parse_hex_num(parser);
        parser->cur_token.value = NUM_TO_VALUE(strtol(parser->cur_token.start, NULL, 16));
    }
    else if (parser->cur_char == '0' && isdigit(look_ahead_char(parser)))
    {
        //八进制
        parse_oct_num(parser);
        parser->cur_token.value = NUM_TO_VALUE(strtol(parser->cur_token.start, NULL, 8));
    }
    else
    {
        //十进制
        parse_dec_num(parser);
        parser->cur_token.value = NUM_TO_VALUE(strtod(parser->cur_token.start, NULL));
    }

    parser->cur_token.length = (uint32_t) (parser->next_char_ptr - parser->cur_token.start - 1);
    parser->cur_token.type = TOKEN_NUM;
}

//查看下一个字符
char look_ahead_char(parser * parser)
{
    return *parser->next_char_ptr;
}


void get_next_token(parser * parser)
{
    parser->pre_token = parser->cur_token;
    skip_blanks(parser);
    parser->cur_token.type = TOKEN_EOF;
    parser->cur_token.length = 0;
    parser->cur_token.start = parser->next_char_ptr - 1;
    while (parser->cur_char != '\0')
    {
        switch (parser->cur_char) {
            case ',':
                parser->cur_token.type = TOKEN_COMMA;
                break;
            case ':':
                parser->cur_token.type = TOKEN_COLON;
                break;
            case '(':
                if (parser->interpolation_expect_right_paren_num > 0)
                {
                    parser->interpolation_expect_right_paren_num++;
                }
                parser->cur_token.type = TOKEN_LEFT_PAREN;
                break;

            case ')':
                if (parser->interpolation_expect_right_paren_num > 0)
                {
                    parser->interpolation_expect_right_paren_num--;
                    if (parser->interpolation_expect_right_paren_num == 0)
                    {
                        parse_string(parser);
                        break;
                    }
                }
                parser->cur_token.type = TOKEN_RIGHT_PAREN;
                break;
            case '[':
                parser->cur_token.type = TOKEN_LEFT_BRACKET;
                break;
            case ']':
                parser->cur_token.type = TOKEN_RIGHT_BRACKET;
                break;
            case '{':
                parser->cur_token.type = TOKEN_LEFT_BRACE;
                break;
            case '}':
                parser->cur_token.type = TOKEN_RIGHT_BRACE;
                break;
            case '.':
                if (match_next_char(parser, '.'))
                {
                    parser->cur_token.type = TOKEN_DOT_DOT;
                }
                else
                {
                    parser->cur_token.type = TOKEN_DOT;
                }
                break;
            case '=':
                if (match_next_char(parser, '='))
                {
                    parser->cur_token.type = TOKEN_EQUAL;
                }
                else
                {
                    parser->cur_token.type = TOKEN_ASSIGN;
                }
                break;
            case '+':
                parser->cur_token.type = TOKEN_ADD;
                break;
            case '-':
                parser->cur_token.type = TOKEN_SUB;
                break;
            case '*':
                parser->cur_token.type = TOKEN_MUL;
                break;
            case '/':
                if (match_next_char(parser,'/') || match_next_char(parser, '*'))
                {
                    skip_comment(parser);
                    parser->cur_token.start = parser->next_char_ptr - 1;
                    continue;
                }
                else
                {
                    parser->cur_token.type = TOKEN_DIV;
                }
                break;
            case '%':
                parser->cur_token.type = TOKEN_MOD;
                break;
            case '&':
                if (match_next_char(parser, '&'))
                {
                    parser->cur_token.type = TOKEN_LOGIC_AND;
                }
                else
                {
                    parser->cur_token.type = TOKEN_BIT_AND;
                }
                break;
            case '|':
                if (match_next_char(parser, '|'))
                {
                    parser->cur_token.type = TOKEN_LOGIC_OR;
                }
                else
                {
                    parser->cur_token.type = TOKEN_BIT_OR;
                }
                break;
            case '~':
                parser->cur_token.type = TOKEN_BIT_NOT;
                break;
            case '?':
                parser->cur_token.type = TOKEN_QUESTION;
                break;
            case '>':
                if (match_next_char(parser, '='))
                {
                    parser->cur_token.type = TOKEN_GREAT_EQUAL;
                }
                else if(match_next_char(parser, '>'))
                {
                    parser->cur_token.type = TOKEN_BIT_SHIFT_RIGHT;
                }
                else
                {
                    parser->cur_token.type = TOKEN_GREAT;
                }
                break;
            case '<':
                if (match_next_char(parser, '='))
                {
                    parser->cur_token.type = TOKEN_LESS_EQUAL;
                }
                else if(match_next_char(parser, '<'))
                {
                    parser->cur_token.type = TOKEN_BIT_SHIFT_LEFT;
                }
                else
                {
                    parser->cur_token.type = TOKEN_LESS;
                }
                break;
            case '!':
                if (match_next_char(parser, '='))
                {
                    parser->cur_token.type = TOKEN_NOT_EQUAL;
                }
                else
                {
                    parser->cur_token.type = TOKEN_LOGIC_NOT;
                }
                break;
            case '"':
                parse_string(parser);
                break;
            default:
                //处理变量及数字
                //如果以字母或者下划线开头，则是变量名
                if (isalpha(parser->cur_char) || parser->cur_char == '_')
                {
                    parse_id(parser, TOKEN_UNKNOWN);
                }
                else if (isdigit(parser->cur_char))
                {
                    parse_num(parser);
                }
                else
                {
                    if (parser->cur_char == '#' && match_next_char(parser, '!'))
                    {
                        skip_aline(parser);
                        parser->cur_token.start = parser->next_char_ptr - 1;
                        continue;
                    }
                    LEX_ERROR(parser, "unsupported char: \'%c\'", parser->cur_char);
                }
                return;
        }

        parser->cur_token.length = (uint32_t)(parser->next_char_ptr - parser->cur_token.start);
        get_next_char(parser);
        return;
    }
}

bool match_token(parser * parser, token_type expected)
{
    if (parser->cur_token.type == expected)
    {
        get_next_token(parser);
        return true;
    }

    return false;
}

void consume_cur_token(parser * parser, token_type expected, const char * errMsg)
{
    if (parser->cur_token.type != expected)
    {
        COMPILE_ERROR(parser, errMsg);
    }
    get_next_token(parser);
}

void consume_next_token(parser * parser, token_type expected, const char * errMsg)
{
    get_next_token(parser);
    if (parser->cur_token.type != expected)
    {
        COMPILE_ERROR(parser, errMsg);
    }
}

void init_parser(VM* vm, parser * parser, const char * file, const char * source_code, obj_module * obj_module)
{
    parser->file = file;
    parser->source_code = source_code;
    parser->cur_char = *parser->source_code;
    parser->next_char_ptr = parser->source_code + 1;
    parser->cur_token.line_no = 1;
    parser->cur_token.type = TOKEN_UNKNOWN;
    parser->cur_token.start = NULL;
    parser->cur_token.length = 0;
    parser->pre_token = parser->cur_token;
    parser->interpolation_expect_right_paren_num = 0;
    parser->vm = vm;
    parser->cur_module = obj_module;
}
