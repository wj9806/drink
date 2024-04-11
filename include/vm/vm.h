//
// Created by xsy on 2024/3/21.
//

#ifndef DRINK_VM_H
#define DRINK_VM_H

#include "common.h"
#include "header_obj.h"
#include "obj_map.h"
#include "obj_thread.h"

typedef enum {
    VM_RESULT_SUCCESS,
    VM_RESULT_ERROR,
} vm_result;

struct vm {
    class * class_of_class;
    class * object_class;
    class * string_class;
    class * fn_class;
    class * list_class;
    class * range_class;
    class * map_class;
    class * null_class;
    class * bool_class;
    class * num_class;
    class * thread_class;

    //累计已分配的内存量
    uint32_t allocated_bytes; //累计分配内存量
    obj_header * all_objects; //所有已分配对象链表
    symbol_table all_method_names;      //所有类的方法名
    obj_map * all_modules;
    obj_thread * cur_thread;        //当前线程
    parser * cur_parser;
};

void init_vm(VM* vm);

VM * new_vm(void);

#endif //DRINK_VM_H
