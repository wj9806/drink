//
// Created by wj on 2024/4/2.
//

#include "obj_list.h"
#include "class.h"

obj_list * new_obj_list(VM* vm, uint32_t element_num)
{
    value * element_array = NULL;
    if (element_num > 0)
    {
        element_array = ALLOCATE_ARRAY(vm, value, element_num);
    }

    obj_list * objList = ALLOCATE(vm, obj_list);

    objList->elements.datas = element_array;
    objList->elements.capacity = objList->elements.count = element_num;
    init_obj_header(vm, &objList->obj_header, OT_LIST, vm->list_class);
    return objList;
}

//调整list容量
static void shrink_list(VM* vm, obj_list* objList, uint32_t new_capacity)
{
    uint32_t old_size = objList->elements.capacity * sizeof(value);
    uint32_t new_size = new_capacity * sizeof(value);
    memManager(vm, objList->elements.datas, old_size, new_size);
    objList->elements.capacity = new_capacity;
}

value remove_element(VM* vm, obj_list * objList, uint32_t index)
{
    value value_removed = objList->elements.datas[index];

    //index后的元素向前移动一位
    uint32_t idx = index;
    while (idx < objList->elements.count)
    {
        objList->elements.datas[idx] = objList->elements.datas[idx + 1];
        idx++;
    }

    //容量过低就减小容量
    uint32_t cap = objList->elements.capacity / CAPACITY_GROW_FACTOR;
    if (cap > objList->elements.count)
    {
        shrink_list(vm, objList, cap);
    }

    objList->elements.count--;
    return value_removed;
}

void insert_element(VM* vm, obj_list * objList, uint32_t index, value v)
{
    if (index > objList->elements.count - 1)
    {
        RUN_ERROR("index out bounded!");
    }
    value_buffer_add(vm, &objList->elements, VT_TO_VALUE(VT_NULL));

    uint32_t idx = objList->elements.count - 1;
    //index后面元素后移一位
    while (idx > index)
    {
        objList->elements.datas[idx] = objList->elements.datas[idx - 1];
        idx--;
    }
    objList->elements.datas[index] = v;
}
