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
    //������ĺ���
    obj_fn * fn;
    local_var local_vars[MAX_LOCAL_VAR_NUM];
    //�ѷ���ľֲ���������
    uint32_t local_var_num;
    //��¼���㺯�������õ�up_value
    up_value up_values[MAX_UPVALUE_NUM];
    //��ǰ����������������
    int scope_depth;
    //��ǰʹ�õ�slot����
    uint32_t stack_slot_num;
    //��ǰ���ڱ����ѭ����
    loop * cur_loop;
    //��ǰ���������ı�����Ϣ
    class_book_keep enclosing_class_bk;
    //�����˱��뵥Ԫ�ı��뵥Ԫc
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
