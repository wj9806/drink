//
// Created by wj on 2024/4/6.
//

#include "obj_thread.h"
#include "vm.h"
#include "class.h"

void prepare_frame(obj_thread * obj_thread, obj_closure * obj_closure, value * stack_start)
{
    ASSERT(obj_thread->frame_capacity > obj_thread->used_frame_num, "frame not enough");

    frame * frame = &(obj_thread->frames[obj_thread->used_frame_num++]);
    frame->stack_start = stack_start;
    frame->closure = obj_closure;
    //指令起始地址即闭包函数中指令流的起始地址
    frame->ip = obj_closure->fn->instr_stream.datas;
}

/**
 *  线程中运行的是闭包obj_closure中的函数
 */
obj_thread * new_obj_thread(VM* vm, obj_closure * obj_closure)
{
    ASSERT(obj_closure != NULL, "obj_closure is NULL");

    frame * frames = ALLOCATE_ARRAY(vm, frame, INITIAL_FRAME_NUM);

    uint32_t stack_capacity = ceilToPowerOf2(obj_closure->fn->max_stack_slot_used_num + 1);
    value * new_stack = ALLOCATE_ARRAY(vm, value, stack_capacity);

    obj_thread * thread = ALLOCATE(vm, obj_thread);
    init_obj_header(vm, &thread->obj_header, OT_THREAD, vm->thread_class);
    thread->frames = frames;
    thread->frame_capacity = INITIAL_FRAME_NUM;
    thread->stack = new_stack;
    thread->stack_capacity = stack_capacity;
    return thread;
}

void reset_thread(obj_thread * obj_thread, obj_closure * obj_closure)
{
    obj_thread->esp = obj_thread->stack;
    obj_thread->open_upvalues = NULL;
    obj_thread->caller = NULL;
    obj_thread->error_obj = VT_TO_VALUE(VT_NULL);
    obj_thread->used_frame_num = 0;

    ASSERT(obj_closure != NULL, "obj_closure is null in function reset thread");
    prepare_frame(obj_thread, obj_closure, obj_thread->stack);
}