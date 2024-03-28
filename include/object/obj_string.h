//
// Created by wj on 2024/3/25.
//

#ifndef DRINK_OBJ_STRING_H
#define DRINK_OBJ_STRING_H

#include "header_obj.h"

//�ַ�������
typedef struct {
    obj_header obj_header;
    //�ַ���hashֵ
    uint32_t hash_code;
    //�ַ�������
    char_value value;
} obj_string;

uint32_t hash_string(const char * str, uint32_t length);

void hash_obj_string(obj_string* obj_string);

obj_string * new_obj_string(VM* vm, const char * str, uint32_t length);


#endif //DRINK_OBJ_STRING_H
