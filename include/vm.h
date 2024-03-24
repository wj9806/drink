//
// Created by xsy on 2024/3/21.
//

#ifndef DRINK_VM_H
#define DRINK_VM_H

#include "common.h"

struct vm {
    //累计已分配的内存量
    uint32_t allocated_bytes;
    parser * cur_parser;
};

void init_vm(VM* vm);

VM * new_vm(void);

#endif //DRINK_VM_H
