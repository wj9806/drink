//
// Created by xsy on 2024/3/21.
//

#include "vm.h"
#include "utils.h"

void init_vm(VM* vm)
{
    vm->allocated_bytes = 0;
    vm->cur_parser = NULL;
    vm->all_objects = NULL;
}

VM * new_vm(void)
{
    VM* vm = (VM*) malloc(sizeof (VM));
    if (vm == NULL)
        MEM_ERROR("allocate VM failed");

    init_vm(vm);
    return vm;
}
