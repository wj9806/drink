//
// Created by wj on 2024/3/25.
//

#ifndef DRINK_OBJ_STRING_H
#define DRINK_OBJ_STRING_H

#include "header_obj.h"

typedef struct {
    obj_header objHeader;
    uint32_t hash_code;
    char_value value;
} obj_string;


#endif //DRINK_OBJ_STRING_H
