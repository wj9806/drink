//
// Created by wj on 2024/3/25.
//

#include "class.h"
#include <string.h>
#include "obj_range.h"


DEFINE_BUFFER_METHOD(method)

bool value_is_equal(value a, value b)
{
    if (a.type != b.type)
    {
        return false;
    }

    if (a.type == VT_NUM)
    {
        return a.num == b.num;
    }

    if (a.obj_header == b.obj_header)
    {
        return true;
    }

    if (a.obj_header->type != b.obj_header->type)
    {
        return false;
    }

    if (a.obj_header->type == OT_STRING)
    {
        obj_string * str_a = VALUE_TO_OBJSTR(a);
        obj_string * str_b = VALUE_TO_OBJSTR(b);

        return (str_a->value.length == str_b->value.length) && memcmp(str_a->value.start, str_b->value.start, str_a->value.length) == 0;
    }

    if (a.obj_header->type == OT_RANGE)
    {
        obj_range * rga = VALUE_TO_OBJRANGE(a);
        obj_range * rgb = VALUE_TO_OBJRANGE(a);
        return (rga->from == rgb->from && rga->to == rgb->to);
    }
    return false;
}

//新建一个类
class * new_raw_class(VM * vm, const char * name, uint32_t field_num)
{
    class * clazz = ALLOCATE(vm, class);
    init_obj_header(vm, &clazz->obj_header, OT_CLASS, NULL);
    clazz->name = new_obj_string(vm ,name, strlen(name));
    clazz->field_num = field_num;
    clazz->super_class = NULL;
    method_buffer_init(&clazz->methods);
    return clazz;
}

class * get_class_obj(VM * vm, value object)
{
    switch (object.type) {
        case VT_NULL:
            return vm->null_class;
        case VT_TRUE:
        case VT_FALSE:
            return vm->bool_class;
        case VT_NUM:
            return vm->num_class;
        case VT_OBJ:
            return VALUE_TO_OBJ(object)->class;
        default:
            NOT_REACHED();
    }
}