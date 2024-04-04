//
// Created by wj on 2024/4/3.
//

#ifndef DRINK_OBJ_MAP_H
#define DRINK_OBJ_MAP_H
#include "header_obj.h"

//负载因子
#define MAP_LOAD_PERCENT    0.75

typedef struct {
    value key;
    value value;
} entry;

typedef struct {
    obj_header obj_header;
    uint32_t capacity;  //map容量
    uint32_t count; //map中使用的entry数量
    entry * entries;
} obj_map;

obj_map * new_obj_map(VM* vm);

void map_put(VM* vm, obj_map* obj_map, value key, value value);

value map_get(obj_map* obj_map, value key);

void clear_map(VM * vm, obj_map * obj_map);

value remove_key(VM* vm, obj_map* obj_map, value key);

#endif //DRINK_OBJ_MAP_H
