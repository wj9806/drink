//
// Created by wj on 2024/4/3.
//

#ifndef DRINK_OBJ_RANGE_H
#define DRINK_OBJ_RANGE_H

#include "class.h"

/**
 * range对象表示一段步进为1数字范围，他是用..隐式创建，例如1..8，表示1到8的排列
 * range对象用于索引列表对象
 * [1,2,3,4,5,6] [3..1] = [4,3,2]
 * [1,2,3,4,5,6] [1..3] = [2,3,4]
 * [1,2,3,4,5,6] [3..3] = [4]
 */

typedef struct {
    obj_header obj_header;
    int from;
    int to;
} obj_range;

obj_range * new_obj_range(VM* vm, int from, int to);

#endif //DRINK_OBJ_RANGE_H
