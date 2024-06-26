//
// Created by wj on 2024/3/25.
//

#ifndef DRINK_HEADER_OBJ_H
#define DRINK_HEADER_OBJ_H

#include "utils.h"

typedef enum {
    OT_CLASS,
    OT_LIST,
    OT_MAP,
    OT_MODULE,
    OT_RANGE,
    OT_STRING,
    OT_UPVALUE,
    OT_FUNCTION,
    OT_CLOSURE,
    OT_INSTANCE,
    OT_THREAD,
} obj_type;

//对象头
typedef struct obj_header {
    obj_type type;
    bool is_dark; //是否可达
    class * class; //对象所属的类
    struct obj_header* next;
} obj_header;

typedef enum {
    VT_UNDEFINED,
    VT_NULL,
    VT_FALSE,
    VT_TRUE,
    VT_NUM,
    VT_OBJ
} value_type;  //value类型

//通用的值结构
typedef struct {
    //值类型
    value_type type;
    //数值
    union {
        //数字或者对象
        double num;
        obj_header * obj_header;
    };
} value;

DECLARE_BUFFER_TYPE(value)

void init_obj_header(VM* vm, obj_header * obj_header, obj_type obj_type, class* class);

#endif //DRINK_HEADER_OBJ_H
