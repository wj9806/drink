//
// Created by xsy on 2024/3/21.
//

#include "vm.h"
#include "utils.h"

void initVm(VM* vm)
{
    vm->allocatedBytes = 0;
    vm->curParser = NULL;
}

VM * newVM(void)
{
    VM* vm = (VM*) malloc(sizeof (VM));
    if (vm == NULL)
        MEM_ERROR("allocate VM failed");

    initVm(vm);
    return vm;
}
