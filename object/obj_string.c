//
// Created by wj on 2024/3/28.
//
#include "obj_string.h"
#include "utils.h"
#include "vm.h"
#include <string.h>

//fnv-la�㷨
uint32_t hash_string(const char * str, uint32_t length)
{
    uint32_t hash_code = 2166136261, idx = 0;
    while (idx < length)
    {
        hash_code ^= str[idx];
        hash_code *= 16777619;
        idx++;
    }

    return hash_code;
}

//����string hashֵ
void hash_obj_string(obj_string* obj_string)
{
    obj_string->hash_code = hash_string(obj_string->value.start, obj_string->value.length);
}

//�����ַ�������
obj_string * new_obj_string(VM* vm, const char * str, uint32_t length)
{
    ASSERT(length == 0 || str != NULL, "str length don't match str");
    // + 1 �����Ͻ�β�� '\0'
    obj_string * ss = ALLOCATE_EXTRA(vm, obj_string, length + 1);
    if (ss != NULL)
    {
        init_obj_header(vm, &ss->obj_header, OT_STRING, vm->string_class);
        ss->value.length = length;
        //�����ַ�������
        if (length > 0)
        {
            memcpy(ss->value.start, str, length);
        }
        ss->value.start[length] = '\0';
        hash_obj_string(ss);
    }
    else
    {
        MEM_ERROR("Allocating obj_string failed");
    }
    return ss;
}
