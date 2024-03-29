//
// Created by wj on 2024/3/25.
//

#ifndef DRINK_OBJ_FN_H
#define DRINK_OBJ_FN_H
#include "utils.h"
#include "header_obj.h"
#include "meta_obj.h"

typedef struct {
    char* fn_name;         //������
    int_buffer line_no;    //�к�
} fn_debug;

//��������
typedef struct {
    obj_header obj_header;
    //����������ָ����
    byte_buffer instr_stream;
    //����������
    value_buffer constants;
    //������ģ��
    obj_module * module;
    //���������Ҫ��ջ�ռ�
    uint32_t max_stack_slot_used_num;
    //upvalue����
    uint32_t upvalue_num;
    //�βθ���
    uint8_t arg_num;
#if DEBUG
    fn_debug * debug;
#endif
} obj_fn;

//upvalue����
//��λ�ڱ�����ֱ�ӻ�����㺯���еľֲ�����
typedef struct upvalue{
    obj_header obj_header;
    //local_var_ptrָ��upvalue�������ľֲ�����
    value * local_var_ptr;
    //�Ѿ����رյ�upvalue
    value closed_upvalue;
    struct upvalue* next;
} obj_upvalue;


//�հ�����
typedef struct {
    obj_header obj_header;
    //�հ�����Ҫ���õĺ���
    obj_fn * fn;

    obj_upvalue * upvalues[0];
} obj_closure;

typedef struct {
    //�����������ָ����һ����ָ����ָ��
    uint8_t * ip;
    obj_closure * closure;
    //ָ��frame����thread����ʱջ����ʼ��ַ
    value * stack_start;
} frame;

#define INITIAL_FRAME_NUM       4

obj_upvalue * new_obj_upvalue(VM* vm, value* local_var_ptr);

obj_closure * new_obj_closure(VM* vm, obj_fn * fn);

obj_fn * new_obj_fn(VM* vm, obj_module* obj_module, uint32_t max_stack_slot_used_num);

#endif //DRINK_OBJ_FN_H
