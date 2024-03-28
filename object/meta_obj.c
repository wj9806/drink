//
// Created by wj on 2024/3/28.
//
#include "meta_obj.h"
#include "class.h"
#include "vm.h"
#include <string.h>

obj_module * new_obj_module(VM* vm, const char * mod_name){
    obj_module * module = ALLOCATE(vm, obj_module);
    if (module == NULL)
    {
        MEM_ERROR("allocate obj_module failed");
        return (obj_module *)0;
    }

    init_obj_header(vm, &module->obj_header, OT_MODULE, NULL);
    string_buffer_init(&module->module_var_name);
    value_buffer_init(&module->module_var_value);

    module->name = NULL;

    if (mod_name != NULL)
    {
        module->name = new_obj_string(vm, mod_name, strlen(mod_name));
    }
    return module;
}

obj_instance * new_obj_instance(VM * vm, class* clazz){
    obj_instance * instance = ALLOCATE_EXTRA(vm, obj_instance, sizeof(value) * clazz->field_num);
    init_obj_header(vm, &instance->obj_header, OT_INSTANCE, clazz);

    uint32_t idx = 0;
    while (idx < clazz->field_num)
    {
        instance->fields[idx++] = VT_TO_VALUE(VT_NULL);
    }
    return instance;
}
