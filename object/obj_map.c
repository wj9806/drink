//
// Created by wj on 2024/4/3.
//

#include "obj_map.h"
#include "class.h"
#include "vm.h"
#include "obj_range.h"

obj_map * new_obj_map(VM* vm)
{
    obj_map * objMap = ALLOCATE(vm, obj_map);
    init_obj_header(vm, &objMap->obj_header, OT_MAP, vm->map_class);
    objMap->capacity = objMap->count = 0;
    objMap->entries = NULL;
    return objMap;
}

static uint32_t hash_num(double num)
{
    bits64 bits64;
    bits64.num = num;
    return bits64.bit32[0] ^ bits64.bit32[1];
}
static uint32_t hash_obj(obj_header * objHeader)
{
    switch (objHeader->type) {
        case OT_CLASS:
            return hash_string(((class *)objHeader)->name->value.start, ((class *)objHeader)->name->value.length);
        case OT_RANGE:
            obj_range * objRange = (obj_range*)objHeader;
            return hash_num(objRange->from) ^ hash_num(objRange->to);
        case OT_STRING:
            return ((obj_string *)objHeader)->hash_code;
        default:
            RUN_ERROR("the type error");
    }
    return 0;
}

static uint32_t hash_value(value value)
{
    switch (value.type) {
        case VT_FALSE:
            return 0;
        case VT_NULL:
            return 1;
        case VT_NUM:
            return hash_num(value.num);
        case VT_TRUE:
            return 2;
        case VT_OBJ:
            return hash_obj(value.obj_header);
        default:
            RUN_ERROR("unsupport type hashed");
    }
    return 0;
}

void map_put(VM* vm, obj_map* obj_map, value key, value value)
{

}

value map_get(obj_map* obj_map, value key)
{

}

void clear_map(VM * vm, obj_map * obj_map)
{

}

value remove_key(VM* vm, obj_map* obj_map, value key)
{

}

