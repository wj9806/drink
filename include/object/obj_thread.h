//
// Created by wj on 2024/4/6.
//

#ifndef DRINK_OBJ_THREAD_H
#define DRINK_OBJ_THREAD_H

#include "obj_fn.h"

typedef struct obj_thread {
    obj_header obj_header;
    //运行时栈底
    value * stack;
    //运行时栈顶
    value * esp;
    //栈容量
    uint32_t stack_capacity;
    //调用框架
    frame * frames;
    //已使用的frame数量
    uint32_t used_frame_num;
    //frame容量
    uint32_t frame_capacity;
    //打开的upvalue的链表首节点
    obj_upvalue * open_upvalues;
    //thread的调用线程
    struct obj_thread * caller;
    //导致运行时错误的对象
    value error_obj;
} obj_thread;

void prepare_frame(obj_thread * obj_thread, obj_closure * obj_closure, value * stack_start);

obj_thread * new_obj_thread(VM* vm, obj_closure * obj_closure);

void reset_thread(obj_thread * obj_thread, obj_closure * obj_closure);

#endif //DRINK_OBJ_THREAD_H
