//
// Created by wj on 2024/3/25.
//
#include "header_obj.h"
#include "vm.h"

void init_obj_header(VM* vm, obj_header * obj_header, obj_type obj_type, class* class)
{
    obj_header->type = obj_type;
    obj_header->is_dark = false;
    obj_header->class = class;
    //ͷ�巨
    obj_header->next = vm->all_objects;
    vm->all_objects = obj_header;
}