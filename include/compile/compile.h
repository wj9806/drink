//
// Created by wj on 2024/4/8.
//

#ifndef DRINK_COMPILE_H
#define DRINK_COMPILE_H

#include "obj_fn.h"

#define MAX_LOCAL_VAR_NUM           128
#define MAX_UPVALUE_NUM             128
#define MAX_ID_LEN                  128
#define MAX_FIELD_NUM               128

#define MAX_METHOD_NAME_LEN         MAX_ID_LEN
#define MAX_ARG_NUM                 16
#define MAX_SIGN_LEN                (MAX_METHOD_NAME_LEN + MAX_ARG_NUM * 2 + 1)

typedef struct {
    //�����upValue��ֱ����㺯���ľֲ�����������Ϊtrue
    bool is_enclosing_local_var;

    //��㺯���оֲ�����������������㺯����up_value������
    uint32_t index;
} up_value;

typedef struct {
    const char * name;
    uint32_t length;
    int scope_depth;
    //��ʾ�������еľֲ������Ƿ������ڲ㺯�������õ�upvalue
    bool is_upvalue;
} local_var;

//����ǩ��
typedef enum {
   SIGN_CONSTRUCT,
   SIGN_METHOD,
   SIGN_SETTER,
   SIGN_GETTER,
   SIGN_SUBSCRIPT,
   SIGN_SUBSCRIPT_SETTER,
} signature_type;

typedef struct {
    signature_type type;
    const char * name;
    uint32_t length;
    uint32_t arg_num;
} signature;

typedef struct loop {
    //ѭ���������ĵ�ַ
    int cond_start_index;
    //ѭ�������ʼ��ַ
    int body_start_index;
    ///ѭ��������break����������Ҫ�˳������������
    int scope_depth;
    //ѭ������������ʱ����ѭ�����Ŀ���ַ
    int exit_index;
    //���ѭ��
    struct loop * enclosing_loop;
} loop;

//���ڼ�¼�����ʱ����Ϣ
typedef struct {
    //����
    obj_string * name;
    //�����Է��ű�
    symbol_table fields;
    //�����ǰ���뾲̬������Ϊ��
    bool in_static;
    //ʵ������
    int_buffer instant_methods;
    //��̬����
    int_buffer static_methods;
    //��ǰ���ڱ����ǩ��
    signature * signature;
} class_book_keep;

struct compile_unit {
    //������ĺ���
    obj_fn * fn;
    local_var local_vars[MAX_LOCAL_VAR_NUM];
    //�ѷ���ľֲ���������
    uint32_t local_var_num;
    //��¼���㺯�������õ�up_value
    up_value up_values[MAX_UPVALUE_NUM];
    //��ǰ����������������
    int scope_depth;
    //��ǰʹ�õ�slot����
    uint32_t stack_slot_num;
    //��ǰ���ڱ����ѭ����
    loop * cur_loop;
    //��ǰ���������ı�����Ϣ
    class_book_keep * enclosing_class_bk;
    //�����˱��뵥Ԫ�ı��뵥Ԫc
    struct compile_unit* enclosing_unit;
    parser * cur_parser;
};

typedef struct compile_unit compile_unit;

int define_module_var(VM * vm, obj_module* objModule, const char * name, uint32_t length, value value);

obj_fn * compile_module(VM* vm, obj_module * objModule, const char * module_core);

#endif //DRINK_COMPILE_H
