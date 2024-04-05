//
// Created by wj on 2024/3/25.
//

#ifndef DRINK_CLASS_H
#define DRINK_CLASS_H

#include "common.h"
#include "utils.h"
#include "header_obj.h"
#include "obj_string.h"
#include "obj_fn.h"

typedef enum {
    MT_NONE,       //空方法
    MT_PRIMITIVE,  //用c实现的原生方法
    MT_SCRIPT,     //脚本中定义的方法
    MT_FN_CALL     //有关函数对象的调用方法
} method_type;

//把值类型转换成值结构
#define VT_TO_VALUE(vt)  ((value) {vt, {0}})

#define BOOL_TO_VALUE(boolean) (boolean ? VT_TO_VALUE(VT_TRUE) : VT_TO_VALUE(VT_FALSE))
#define VALUE_TO_BOOL(value) ((value).type == VT_TRUE ? true: false)
#define NUM_TO_VALUE(num) ((value) {VT_NUM, {num}})
#define VALUE_TO_NUM(value) value.num

#define OBJ_TO_VALUE(obj_ptr) ({\
            value value;\
            value.type = VT_OBJ;\
            value.obj_header = (obj_header *) (obj_ptr); \
            value;   \
})

#define VALUE_TO_OBJ(value) (value.obj_header)
#define VALUE_TO_OBJSTR(value) ((obj_string*)VALUE_TO_OBJ(value))
#define VALUE_TO_OBJFN(value) ((obj_fn*)VALUE_TO_OBJ(value))
#define VALUE_TO_OBJRANGE(value) ((obj_range*)VALUE_TO_OBJ(value))
#define VALUE_TO_OBJINSTANCE(value) ((obj_instance *)VALUE_TO_OBJ(value))
#define VALUE_TO_OBJLIST(value) ((obj_list *)VALUE_TO_OBJ(value))
#define VALUE_TO_OBJMAP(value) ((obj_map *)VALUE_TO_OBJ(value))
#define VALUE_TO_OBJCLOSURE(value) ((obj_closure*)VALUE_TO_OBJ(value))
#define VALUE_TO_OBJTHREAD(value) ((obj_thread*)VALUE_TO_OBJ(value))
#define VALUE_TO_OBJMODULE(value) ((obj_module*)VALUE_TO_OBJ(value))
#define VALUE_TO_CLASS(value) ((class*)VALUE_TO_OBJ(value))

#define VALUE_IS_UNDEFINED(value) ((value).type == VT_UNDEFINED)
#define VALUE_IS_NULL(value) ((value).type == VT_NULL)
#define VALUE_IS_TRUE(value) ((value).type == VT_TRUE)
#define VALUE_IS_FALSE(value) ((value).type == VT_FALSE)
#define VALUE_IS_NUM(value) ((value).type == VT_NUM)
#define VALUE_IS_OBJ(value) ((value).type == VT_OBJ)
#define VALUE_IS_CERTAIN_OBJ(value, obj_type) (VALUE_IS_OBJ(value) && VALUE_TO_OBJ(value)->type == obj_type)
#define VALUE_IS_OBJSTR(value) (VALUE_IS_CERTAIN_OBJ(value, OT_STRING))
#define VALUE_IS_OBJINSTANCE(value) (VALUE_IS_CERTAIN_OBJ(value, OT_INSTANCE))
#define VALUE_IS_OBJCLOSURE(value) (VALUE_IS_CERTAIN_OBJ(value, OT_CLOSURE))
#define VALUE_IS_OBJRANGE(value) (VALUE_IS_CERTAIN_OBJ(value, OT_RANGE)
#define VALUE_IS_OBJCLASS(value) (VALUE_IS_CERTAIN_OBJ(value, OT_CLASS)
#define VALUE_IS_0(value) (VALUE_IS_NUM(value) && (value).num == 0)

//原生方法指针
typedef bool (*primitive) (VM* vm, value* args);

typedef struct {
    method_type type;
    union {
        //指向脚本方法关联的c实现
        primitive prim_fn;
        //指向脚本代码编译后的obj_closure或obj_fn
        obj_closure * obj;
    };
} method;

DECLARE_BUFFER_TYPE(method)

struct class {
    obj_header obj_header;
    struct class * super_class; //父类
    uint32_t field_num;         //本类的字段数
    method_buffer methods;      //方法
    obj_string * name;          //类名
};

//bits64 用来存储64位数据
typedef union {
    uint64_t bit64;
    uint32_t bit32[2];
    double num;
}bits64;

#define CAPACITY_GROW_FACTOR    4
#define MIN_CAPACITY            64

bool value_is_equal(value a, value b);

#endif //DRINK_CLASS_H
