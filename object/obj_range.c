//
// Created by wj on 2024/4/3.
//

#include "obj_range.h"
#include "vm.h"

obj_range * new_obj_range(VM* vm, int from, int to)
{
    obj_range * range = ALLOCATE(vm, obj_range);
    init_obj_header(vm, &range->obj_header, OT_RANGE, vm->range_class);
    range->from = from;
    range->to = to;
    return range;
}