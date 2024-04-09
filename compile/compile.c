//
// Created by wj on 2024/4/8.
//

#include "compile.h"
#include "obj_fn.h"
#include <string.h>
#include "vm.h"
#include "core.h"
#include "class.h"

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
    class_book_keep enclosing_class_bk;
    //包含此编译单元的编译单元c
    struct compile_unit* enclosing_unit;
    parser * cur_parser;
};

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
