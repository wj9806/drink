//
// Created by wj on 2024/3/25.
//

#ifndef DRINK_OBJ_FN_H
#define DRINK_OBJ_FN_H
#include "utils.h"
#include "header_obj.h"
#include "meta_obj.h"

typedef struct {
    char* fn_name;         //函数名
    int_buffer line_no;    //行号
} fn_debug;

//函数对象
typedef struct {
    obj_header obj_header;
    //函数编译后的指令流
    byte_buffer instr_stream;
    //函数常量表
    value_buffer constants;
    //函数的模块
    obj_module * module;
    //函数最多需要的栈空间
    uint32_t max_stack_slot_used_num;
    //upvalue数量
    uint32_t upvalue_num;
    //形参个数
    uint8_t arg_num;
#if DEBUG
    fn_debug * debug;
#endif
} obj_fn;

//upvalue对象
//是位于本函数直接或间接外层函数中的局部变量
typedef struct upvalue{
    obj_header obj_header;
    //local_var_ptr指向upvalue所关联的局部变量
    value * local_var_ptr;
    //已经被关闭的upvalue
    value closed_upvalue;
    struct upvalue* next;
} obj_upvalue;


//闭包对象
typedef struct {
    obj_header obj_header;
    //闭包中所要引用的函数
    obj_fn * fn;

    obj_upvalue * upvalues[0];
} obj_closure;

typedef struct {
    //程序计数器。指向下一个被指定的指令
    uint8_t * ip;
    obj_closure * closure;
    //指向本frame所在thread运行时栈的起始地址
    value * stack_start;
} frame;

#define INITIAL_FRAME_NUM       4

obj_upvalue * new_obj_upvalue(VM* vm, value* local_var_ptr);

obj_closure * new_obj_closure(VM* vm, obj_fn * fn);

obj_fn * new_obj_fn(VM* vm, obj_module* obj_module, uint32_t max_stack_slot_used_num);

#endif //DRINK_OBJ_FN_H
