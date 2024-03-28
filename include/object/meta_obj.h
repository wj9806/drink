//
// Created by wj on 2024/3/28.
//

#ifndef DRINK_META_OBJ_H
#define DRINK_META_OBJ_H

#include "obj_string.h"

//ģ�����
typedef struct {
    obj_header obj_header;
    //ģ���е�ģ���������
    symbol_table module_var_name;
    //ģ���е�ģ�����ֵ
    value_buffer module_var_value;
    //ģ����
    obj_string * name;
} obj_module;

//ʵ������
typedef struct {
    obj_header obj_header;
    value fields[0];
} obj_instance;

obj_module * new_obj_module(VM* vm, const char * mod_name);

obj_instance * new_obj_instance(VM * vm, class* class);

#endif //DRINK_META_OBJ_H
