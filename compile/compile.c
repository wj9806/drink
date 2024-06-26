//
// Created by wj on 2024/4/8.
//

#include "compile.h"
#include "obj_fn.h"
#include <string.h>
#include "vm.h"
#include "core.h"
#include "class.h"
#include "parser.h"

//操作符的绑定权值，及优先级
typedef enum
{
    BP_NONE,
    BP_LOWEST,
    BP_ASSIGN,  // =
    BP_CONDITION, // ? :
    BP_LOGIC_OR, // ||
    BP_LOGIC_AND, // &&
    BP_EQUAL, //== !=
    BP_IS, // is
    BP_CMP, // <> <= >=
    BP_BIT_OR, // |
    BP_BIT_AND, // &
    BP_BIT_SHIFT, //<< >>
    BP_RANGE, // ..
    BP_TERM, // + -
    BP_FACTOR,// * / %
    BP_UNARY, // - ! ~
    BP_CALL, // . () []
    BP_HIGHEST
} bind_power;

typedef void (*denotation_fn) (compile_unit * cu, bool can_assign);

typedef void (*method_signature_fn) (compile_unit * cu, signature * signature);

//符号绑定规则
typedef struct {
    //id
    const char * id;
    //左绑定权值，不关注左边操作数的符号此值为0
    bind_power lbp;
    //字面量、变量、前缀运算符等不关注左操作数的token调用的方法
    denotation_fn nud;
    //中缀运算符等关注左操作数的token调用的方法
    denotation_fn led;
    //表示奔赴好在类中被视为一个方法
    //为其生成一个方法签名
    method_signature_fn method_sign;
} symbol_bind_rule;

int define_module_var(VM * vm, obj_module* objModule, const char * name, uint32_t length, value value)
{
    if (length > MAX_ID_LEN)
    {
        char id[MAX_ID_LEN] = {'\0'};
        memcpy(id, name, length);
        if (vm->cur_parser != NULL)
        {
            COMPILE_ERROR(vm->cur_parser, "length of identifier \"%s\" should be no more than %d", id, MAX_ID_LEN);
        }
        else
        {
            MEM_ERROR("length of identifier \"%s\" should be no more than %d", id, MAX_ID_LEN);
        }
    }

    int symbol_index = get_index_from_symbol_table(&objModule->module_var_name, name, length);
    if (symbol_index == -1)
    {
        //不存在，则添加
        symbol_index = add_symbol(vm, &objModule->module_var_name, name, length);
        value_buffer_add(vm, &objModule->module_var_value, value);
    }
    else if(VALUE_IS_NUM(objModule->module_var_value.datas[symbol_index]))
    {
        //存在则判断模块名称是否是数字，是数字，则之前出现过引用该模块变量时，该模块变量尚未定义情况
        objModule->module_var_value.datas[symbol_index] = value;
    }
    else
    {
        symbol_index = -1;
    }

    return symbol_index;
}

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


static void compile_program(compile_unit * cu)
{

}

obj_fn * compile_module(VM* vm, obj_module * objModule, const char * module_code) {
    parser parser;
    parser.parent = vm->cur_parser;
    vm->cur_parser = &parser;

    if (objModule->name == NULL) {
        init_parser(vm, &parser, "core.script.inc", module_code, objModule);
    } else {
        init_parser(vm, &parser, (const char *) objModule->name->value.start, module_code, objModule);
    }

    compile_unit cu;
    init_compile_unit(&parser, &cu, NULL, false);

    uint32_t module_var_num_before = objModule->module_var_value.count;

    get_next_token(&parser);
    while (!match_token(&parser, TOKEN_EOF))
    {
        compile_program(&cu);
    }

    exit(0);
}

//添加常量并返回索引
static uint32_t add_constant(compile_unit * cu, value constant)
{
    value_buffer_add(cu->cur_parser->vm, &cu->fn->constants, constant);
    return cu->fn->constants.count - 1;
}

//生成加载常量的指令
static void emit_load_constant(compile_unit * cu, value value)
{
    int index = add_constant(cu, value);
    write_opcode_short_operand(cu, OPCODE_LOAD_CONSTANT, index);
}

//数字和字符串.nud() 编译字面量
static void literal(compile_unit * cu, bool can_assign UNUSED)
{
    emit_load_constant(cu, cu->cur_parser->pre_token.value);
}

//不关注左操作数的符号称作前缀符号
#define PREFIX_SYMBOL(nud)  {NULL, BP_NONE, nud, NULL, NULL}

//前缀运算符
#define PREFIX_OPERATOR(id) {id, BP_NONE, unary_operator, NULL, unary_method_signature}

//关注左操作数的符号称为中缀符号
#define INFIX_SYMBOL(lbp, led) {NULL , lbp , NULL , led, NULL)

//中缀运算符
#define INFIX_OPERATOR(id, lbp) {id, lbp, NULL, infix_operator, infix_method_signature)

//可做前缀又可做中缀的运算符，如－
#define MIX_OPERATOR(id) {id, BP_TERM, unary_operator, infix_operator, mix_method_signature)

//占位
#define UNUSED_RULE {NULL , BP_NONE , NULL , NULL, NULL}

//注册符号的语法和语义分析函数
symbol_bind_rule RULES[] = {
        UNUSED_RULE,
        PREFIX_SYMBOL(literal),
        PREFIX_SYMBOL(literal),
};