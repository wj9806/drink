//
// Created by wj on 2024/4/6.
//

#ifndef DRINK_OBJ_THREAD_H
#define DRINK_OBJ_THREAD_H

#include "obj_fn.h"

typedef struct obj_thread {
    obj_header obj_header;
    //����ʱջ��
    value * stack;
    //����ʱջ��
    value * esp;
    //ջ����
    uint32_t stack_capacity;
    //���ÿ��
    frame * frames;
    //��ʹ�õ�frame����
    uint32_t used_frame_num;
    //frame����
    uint32_t frame_capacity;
    //�򿪵�upvalue�������׽ڵ�
    obj_upvalue * open_upvalues;
    //thread�ĵ����߳�
    struct obj_thread * caller;
    //��������ʱ����Ķ���
    value error_obj;
} obj_thread;

void prepare_frame(obj_thread * obj_thread, obj_closure * obj_closure, value * stack_start);

obj_thread * new_obj_thread(VM* vm, obj_closure * obj_closure);

void reset_thread(obj_thread * obj_thread, obj_closure * obj_closure);

#endif //DRINK_OBJ_THREAD_H
