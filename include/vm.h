//
// Created by xsy on 2024/3/21.
//

#ifndef DRINK_VM_H
#define DRINK_VM_H

#include "common.h"

struct vm {
    //累计已分配的内存量
    uint32_t allocatedBytes;
    Parser * curParser;
};

void initVm(VM* vm);

VM * newVM(void);

#endif //DRINK_VM_H
