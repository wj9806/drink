//
// Created by wj on 2024/4/3.
//

#ifndef DRINK_OBJ_RANGE_H
#define DRINK_OBJ_RANGE_H

#include "class.h"

/**
 * range�����ʾһ�β���Ϊ1���ַ�Χ��������..��ʽ����������1..8����ʾ1��8������
 * range�������������б����
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
