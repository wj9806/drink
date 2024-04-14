//
// Created by wj on 2024/4/8.
//

#ifndef DRINK_COMPILE_H
#define DRINK_COMPILE_H

#include "obj_fn.h"

#define MAX_LOCAL_VAR_NUM           128
#define MAX_UPVALUE_NUM             128
#define MAX_ID_LEN                  128
#define MAX_FIELD_NUM               128

#define MAX_METHOD_NAME_LEN         MAX_ID_LEN
#define MAX_ARG_NUM                 16
#define MAX_SIGN_LEN                (MAX_METHOD_NAME_LEN + MAX_ARG_NUM * 2 + 1)

typedef struct {
    //如果此upValue是直接外层函数的局部变量就设置为true
    bool is_enclosing_local_var;

    //外层函数中局部变量的索引或者外层函数中up_value的索引
    uint32_t index;
} up_value;

typedef struct {
    const char * name;
    uint32_t length;
    int scope_depth;
    //表示本函数中的局部变量是否是其内层函数所引用的upvalue
    bool is_upvalue;
} local_var;

//方法签名
typedef enum {
   SIGN_CONSTRUCT,
   SIGN_METHOD,
   SIGN_SETTER,
   SIGN_GETTER,
   SIGN_SUBSCRIPT,
   SIGN_SUBSCRIPT_SETTER,
} signature_type;

typedef struct {
    signature_type type;
    const char * name;
    uint32_t length;
    uint32_t arg_num;
} signature;

typedef struct loop {
    //循环中条件的地址
    int cond_start_index;
    //循环体的起始地址
    int body_start_index;
    ///循环中若有break，告诉它需要退出的作用域深度
    int scope_depth;
    //循环条件不满足时跳出循环体的目标地址
    int exit_index;
    //外层循环
    struct loop * enclosing_loop;
} loop;

//用于记录类编译时的信息
typedef struct {
    //类名
    obj_string * name;
    //类属性符号表
    symbol_table fields;
    //如果当前编译静态方法就为真
    bool in_static;
    //实例方法
    int_buffer instant_methods;
    //静态方法
    int_buffer static_methods;
    //当前正在编译的签名
    signature * signature;
} class_book_keep;

struct compile_unit {
    //所编译的函数
    obj_fn * fn;
    local_var local_vars[MAX_LOCAL_VAR_NUM];
    //已分配的局部变量个数
    uint32_t local_var_num;
    //记录本层函数所引用的up_value
    up_value up_values[MAX_UPVALUE_NUM];
    //当前代码所处的作用域
    int scope_depth;
    //当前使用的slot个数
    uint32_t stack_slot_num;
    //当前正在编译的循环曾
    loop * cur_loop;
    //当前正编译的类的编译信息
    class_book_keep * enclosing_class_bk;
    //包含此编译单元的编译单元c
    struct compile_unit* enclosing_unit;
    parser * cur_parser;
};

typedef struct compile_unit compile_unit;

int define_module_var(VM * vm, obj_module* objModule, const char * name, uint32_t length, value value);

obj_fn * compile_module(VM* vm, obj_module * objModule, const char * module_core);

#endif //DRINK_COMPILE_H
