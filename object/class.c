//
// Created by wj on 2024/3/25.
//

#include "class.h"
#include "string.h"
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