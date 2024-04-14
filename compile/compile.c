//
// Created by wj on 2024/4/8.
//

#include "compile.h"
#include "obj_fn.h"
#include <string.h>
#include "vm.h"
#include "core.h"
#include "class.h"

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
        //�����ڣ������
        symbol_index = add_symbol(vm, &objModule->module_var_name, name, length);
        value_buffer_add(vm, &objModule->module_var_value, value);
    }
    else if(VALUE_IS_NUM(objModule->module_var_value.datas[symbol_index]))
    {
        //�������ж�ģ�������Ƿ������֣������֣���֮ǰ���ֹ����ø�ģ�����ʱ����ģ�������δ�������
        objModule->module_var_value.datas[symbol_index] = value;
    }
    else
    {
        symbol_index = -1;
    }

    return symbol_index;
}

obj_fn * compile_module(VM* vm, obj_module * objModule, const char * module_core)
{
    ;
}