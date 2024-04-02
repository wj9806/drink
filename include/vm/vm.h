//
// Created by xsy on 2024/3/21.
//

#ifndef DRINK_VM_H
#define DRINK_VM_H

#include "common.h"
#include "header_obj.h"

struct vm {
    class * string_class;
    class * fn_class;
    class * list_class;

    //累计已分配的内存量
    uint32_t allocated_bytes; //累计分配内存量
    obj_header * all_objects; //所有已分配对象链表
    parser * cur_parser;
};

void init_vm(VM* vm);

VM * new_vm(void);

#endif //DRINK_VM_H
