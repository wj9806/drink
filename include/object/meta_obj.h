//
// Created by wj on 2024/3/28.
//

#ifndef DRINK_META_OBJ_H
#define DRINK_META_OBJ_H

#include "obj_string.h"

//模块对象
typedef struct {
    obj_header obj_header;
    //模块中的模块变量名称
    symbol_table module_var_name;
    //模块中的模块变量值
    value_buffer module_var_value;
    //模块名
    obj_string * name;
} obj_module;

//实例对象
typedef struct {
    obj_header obj_header;
    value fields[0];
} obj_instance;

obj_module * new_obj_module(VM* vm, const char * mod_name);

obj_instance * new_obj_instance(VM * vm, class* class);

#endif //DRINK_META_OBJ_H
