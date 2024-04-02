//
// Created by wj on 2024/4/2.
//

#ifndef DRINK_OBJ_LIST_H
#define DRINK_OBJ_LIST_H

#include "class.h"
#include "vm.h"

typedef struct
{
    obj_header obj_header;
    value_buffer elements;
} obj_list;

obj_list * new_obj_list(VM* vm, uint32_t element_num);

value remove_element(VM* vm, obj_list * objList, uint32_t index);

void insert_element(VM* vm, obj_list * objList, uint32_t index, value value);

#endif //DRINK_OBJ_LIST_H
