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
            RUN_ERROR("unsupported type hashed");
    }
    return 0;
}

//增加新的key返回true
//否则返回false
static bool add_entry(entry * entries, uint32_t capacity, value key, value value)
{
    uint32_t index = hash_value(key) % capacity;
    while (true)
    {
        if (entries[index].key.type == VT_UNDEFINED)
        {
            entries[index].key = key;
            entries[index].value = value;
            return true;
        }
        else if (value_is_equal(entries[index].key, key))
        {
            entries[index].value = value;
            return false;
        }
        index = (index + 1) % capacity;
    }
}

static void resize_map(VM * vm, obj_map* objMap, uint32_t new_capacity)
{
    entry * new_entries = ALLOCATE_ARRAY(vm, entry, new_capacity);
    uint32_t idx = 0;
    while (idx < new_capacity)
    {
        new_entries[idx].key = VT_TO_VALUE(VT_UNDEFINED);
        new_entries[idx].value = VT_TO_VALUE(VT_FALSE);
        idx++;
    }
    if (objMap->capacity > 0)
    {
        entry * entry_arr = objMap->entries;
        idx = 0;
        while (idx < objMap->capacity)
        {
            if (entry_arr[idx].key.type != VT_UNDEFINED)
            {
                add_entry(new_entries, new_capacity, entry_arr[idx].key, entry_arr[idx].value);
            }
            idx++;
        }
    }

    //回收
    DEALLOCATE_ARRAY(vm, objMap->entries, objMap->count);
    //更新
    objMap->entries = new_entries;
    objMap->capacity = new_capacity;
}

static entry* find_entry(obj_map* objMap, value key)
{
    if (objMap->capacity == 0) return NULL;

    uint32_t index = hash_value(key) % objMap->capacity;
    entry * entry;
    while (true)
    {
        entry = &objMap->entries[index];
        if (value_is_equal(entry->key, key))
        {
            return entry;
        }

        //探测链断了
        if (VALUE_IS_UNDEFINED(entry->key) && VALUE_IS_FALSE(entry->value))
        {
            return NULL;
        }

        //探测链未断，继续探测
        index = (index + 1) % objMap->capacity;
    }
}

void map_put(VM* vm, obj_map* obj_map, value key, value value)
{
    if (obj_map->count + 1 > obj_map->capacity* MAP_LOAD_PERCENT)
    {
        uint32_t new_capacity = obj_map->capacity * CAPACITY_GROW_FACTOR;
        if (new_capacity < MIN_CAPACITY)
        {
            new_capacity = MIN_CAPACITY;
        }
        resize_map(vm, obj_map, new_capacity);
    }
    if (add_entry(obj_map->entries, obj_map->capacity, key, value))
    {
        obj_map->count++;
    }
}

value map_get(obj_map* obj_map, value key)
{
    entry * entry = find_entry(obj_map, key);
    if(entry == NULL)
    {
        return VT_TO_VALUE(VT_UNDEFINED);
    }
    return entry->value;
}

void clear_map(VM * vm, obj_map * obj_map)
{
    DEALLOCATE_ARRAY(vm, obj_map->entries, obj_map->count);
    obj_map->entries = NULL;
    obj_map->capacity = obj_map->count = 0;
}

value remove_key(VM* vm, obj_map* obj_map, value key)
{
    entry * entry = find_entry(obj_map, key);
    if (entry == NULL)
    {
        return VT_TO_VALUE(VT_NULL);
    }
    value v = entry->value;
    entry->key = VT_TO_VALUE(VT_UNDEFINED);
    entry->value = VT_TO_VALUE(VT_TRUE);

    obj_map->count--;
    if (obj_map->count == 0)
    {
        clear_map(vm, obj_map);
    }
    else if(obj_map->capacity / (CAPACITY_GROW_FACTOR) * MAP_LOAD_PERCENT > obj_map->count)
    {
        if (obj_map->count > MIN_CAPACITY)
        {
            uint32_t new_capacity = obj_map->capacity / CAPACITY_GROW_FACTOR;
            if (new_capacity < MIN_CAPACITY)
            {
                new_capacity = MIN_CAPACITY;
            }
            resize_map(vm, obj_map, new_capacity);
        }
    }

    return v;
}

