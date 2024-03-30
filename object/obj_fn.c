//
// Created by wj on 2024/3/30.
//
#include "obj_fn.h"
#include "class.h"
#include "vm.h"

obj_upvalue * new_obj_upvalue(VM* vm, value* local_var_ptr)
{
    obj_upvalue * objvalue = ALLOCATE(vm, obj_upvalue);
    init_obj_header(vm, &objvalue->obj_header, OT_UPVALUE, NULL);
    objvalue->local_var_ptr = local_var_ptr;
    objvalue->closed_upvalue = VT_TO_VALUE(VT_NULL);
    objvalue->next = NULL;
    return objvalue;
}

obj_closure * new_obj_closure(VM* vm, obj_fn * fn)
{
    obj_closure * objClosure = ALLOCATE_EXTRA(vm, obj_closure, sizeof(obj_closure *) * fn->upvalue_num);
    init_obj_header(vm, &objClosure->obj_header, OT_CLOSURE, vm->fn_class);
    objClosure->fn = fn;

    uint32_t idx = 0;
    while (idx < fn->upvalue_num)
    {
        objClosure->upvalues[idx] = NULL;
        idx++;
    }

    return objClosure;
}

obj_fn * new_obj_fn(VM* vm, obj_module* obj_module, uint32_t max_stack_slot_used_num)
{
    obj_fn * objFn = ALLOCATE(vm, obj_fn);
    if (objFn == NULL)
    {
        MEM_ERROR("allocate obj_fn failed");
        return (obj_fn *) 0;
    }
    init_obj_header(vm, &objFn->obj_header, OT_FUNCTION, vm->fn_class);
    byte_buffer_init(&objFn->instr_stream);
    value_buffer_init(&objFn->constants);
    objFn->module = obj_module;
    objFn->max_stack_slot_used_num = max_stack_slot_used_num;
    objFn->upvalue_num = objFn->arg_num = 0;

#ifdef DEBUG
    objFn->debug = ALLOCATE(vm, fn_debug);
    objFn->debug->fn_name = NULL;
    int_buffer_init(&objFn->debug->line_no);
#endif
    return objFn;
}

