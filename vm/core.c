//
// Created by wj on 24-3-21.
//

#include "core.h"
#include "utils.h"
#include <sys/stat.h>
#include "class.h"
#include "obj_map.h"
#include <string.h>
#include "compile.h"

//根目录
char * root_dir = NULL;

#define CORE_MODULE  VT_TO_VALUE(VT_NULL)

#define RET_VALUE(value) \
    do{                  \
        args[0] = value;           \
        return true;              \
    } while(0);

#define RET_OBJ(obj_ptr)     RET_VALUE(OBJ_TO_VALUE(obj_ptr))
#define RET_BOOL(boolean)    RET_VALUE(BOOL_TO_VALUE(boolean))
#define RET_NUM(num)         RET_VALUE(NUM_TO_VALUE(num))
#define RET_NULL             RET_VALUE(VT_TO_VALUE(VT_NULL))
#define RET_TRUE             RET_VALUE(VT_TO_VALUE(VT_TRUE))
#define RET_FALSE            RET_VALUE(VT_TO_VALUE(VT_FALSE))

#define SET_ERROR_FALSE(vm_ptr, err_msg) \
    do{                                  \
      vm_ptr->cur_thread->error_obj = OBJ_TO_VALUE(new_obj_string(vm_ptr, err_msg, strlen(err_msg))); \
      return false;                                   \
    }while(0);

//绑定方法func到class_ptr指向的类
#define PRIM_METHOD_BIND(class_ptr, method_name, func) \
{ \
    uint32_t length = strlen(method_name);               \
    int global_index = get_index_from_symbol_table(&vm->all_method_names, method_name, length); \
    if (global_index == -1)                              \
    {                                                    \
           global_index = add_symbol(vm, &vm->all_method_names, method_name, length);                                               \
    }                                                  \
    method me;                                         \
    me.type = MT_PRIMITIVE;                            \
    me.prim_fn = func;                                 \
    bind_method(vm, class_ptr, (uint32_t)global_index, me);\
}

// !object object取反，返回false
static bool prim_object_not(VM *vm UNUSED, value * args)
{
    RET_VALUE(VT_TO_VALUE(VT_FALSE));
}

//判断两个对象是否相等
static bool prim_object_equal(VM *vm UNUSED, value * args)
{
    value bool_value = BOOL_TO_VALUE(value_is_equal(args[0], args[1]));
    RET_VALUE(bool_value);
}

static bool prim_object_not_equal(VM *vm UNUSED, value * args)
{
    value bool_value = BOOL_TO_VALUE(!value_is_equal(args[0], args[1]));
    RET_VALUE(bool_value);
}

//类arg[0]是否为arg[1]的子类
static bool prim_object_is(VM * vm, value * args)
{
    if (!VALUE_IS_CLASS(args[1]))
    {
        RUN_ERROR("arg must be class");
    }

    class * this_class = get_class_obj(vm, args[0]);
    class * base_class = (class *)(args[1].obj_header);
    while (base_class != NULL)
    {
        if (this_class == base_class)
        {
            RET_VALUE(VT_TO_VALUE(VT_TRUE));
        }
        base_class = base_class->super_class;
    }
    RET_VALUE(VT_TO_VALUE(VT_FALSE));
}

//args[0].to_string
static bool prim_object_to_string(VM * vm UNUSED, value * args)
{
    class * class = args[0].obj_header->class;
    value name_value = OBJ_TO_VALUE(class->name);
    RET_VALUE(name_value);
}

//args[0].type
static bool prim_object_type(VM * vm UNUSED, value * args)
{
    class * class = get_class_obj(vm, args[0]);
    RET_OBJ(class);
}

//args[0].name
static bool prim_class_name(VM * vm UNUSED, value * args)
{
    RET_OBJ(VALUE_TO_CLASS(args[0])->name);
}

//args[0].supertype
static bool prim_class_supertype(VM * vm UNUSED, value * args)
{
    class * clz = VALUE_TO_CLASS(args[0]);
    if (clz->super_class != NULL)
    {
        RET_OBJ(clz->super_class);
    }
    RET_VALUE(VT_TO_VALUE(VT_NULL));
}

static bool prim_class_to_string(VM * vm UNUSED, value * args)
{
    RET_OBJ(VALUE_TO_CLASS(args[0])->name);
}

//args[O].sarne (args[l], args[2 ］）：返回 args[l ］和 args[2 ］是否相等
static bool prim_objectmata_same(VM * vm UNUSED, value * args)
{
    value bool_value = BOOL_TO_VALUE(value_is_equal(args[1], args[2]));
    RET_VALUE(bool_value);
}

static class * define_class(VM * vm, obj_module* objModule, const char * name)
{
    class * clz = new_raw_class(vm, name, 0);

    define_module_var(vm, objModule, name, strlen(name), OBJ_TO_VALUE(clz));
    return clz;
}

vm_result execute_module(VM * vm, value module_name, const char * module_code)
{
    return VM_RESULT_ERROR;
}

/**
 * 编译核心模块
 * 对象调用实例方法，运行时系统会在对象所属的类的方法集合中查找方法
 * 类调用发发，运行时系统会在类的meta-class的方法集合中查找
 */
void build_core(VM * vm)
{
    obj_module * core_module = new_obj_module(vm, NULL);
    map_put(vm, vm->all_modules, CORE_MODULE, OBJ_TO_VALUE(core_module));

    //对象的基类（所有类的父类）
    vm->object_class = define_class(vm, core_module, "object");
    PRIM_METHOD_BIND(vm->object_class, "!", prim_object_not);
    PRIM_METHOD_BIND(vm->object_class, "==(_)", prim_object_equal);
    PRIM_METHOD_BIND(vm->object_class, "!=(_)", prim_object_not_equal);
    PRIM_METHOD_BIND(vm->object_class, "is(_)", prim_object_is);
    PRIM_METHOD_BIND(vm->object_class, "to_string", prim_object_to_string);
    PRIM_METHOD_BIND(vm->object_class, "type", prim_object_type);

    //元信息类，它是所有meta-class的类
    vm->class_of_class = define_class(vm, core_module, "class");
    bind_super_class(vm, vm->class_of_class, vm->object_class);

    PRIM_METHOD_BIND(vm->class_of_class, "name", prim_class_name);
    PRIM_METHOD_BIND(vm->class_of_class, "super_type", prim_class_supertype);
    PRIM_METHOD_BIND(vm->class_of_class, "to_string", prim_class_to_string);

    //元信息，可以通过类调用方法
    class * object_meta_class = define_class(vm, core_module, "object_meta");

    bind_super_class(vm, object_meta_class, vm->class_of_class);
    PRIM_METHOD_BIND(object_meta_class, "same(_,_)", prim_objectmata_same);

    vm->object_class->obj_header.class = object_meta_class;
    object_meta_class->obj_header.class = vm->class_of_class;
    //让元信息类对象头指向自己
    vm->class_of_class->obj_header.class = vm->class_of_class;

}


/**
 * 读取源码文件
 * @param path 源码文件路径
 * @return 源码文件内容
 */
char * read_file(const char * path)
{
    FILE * file = fopen(path, "r");
    if (file == NULL)
    {
        IO_ERROR("Couldn't open file %s .\n", path);
    }

    struct stat file_stat;
    stat(path, &file_stat);
    //通过stat获取文件大小
    size_t file_size = file_stat.st_size;
    char * file_content = (char *) malloc(file_size + 1);
    if (file_content == NULL)
    {
        MEM_ERROR("Couldn't alloc mem for read file: %s .\n", path);
        return (char *)0;
    }

    size_t read_size = fread(file_content, sizeof(char), file_size, file);
    if (read_size < file_size)
    {
        IO_ERROR("Read file: %s failed. \n", path);
    }
    file_content[file_size] = '\0';
    fclose(file);
    return file_content;
}

//从符号表中查询符号，返回索引
int get_index_from_symbol_table(symbol_table * table, const char * symbol, uint32_t length)
{
    ASSERT(length != 0, "length of symbol is 0");
    int index = 0;
    while (index < table->count)
    {
        if (length == table->datas[index].length && memcmp(table->datas[index].str, symbol, length) == 0)
        {
            return index;
        }
        index++;
    }
    return -1;
}

//往table中添加符号symbol，返回其索引
int add_symbol(VM * vm, symbol_table * table, const char * symbol, uint32_t length)
{
    ASSERT(length != 0, "length of symbol is 0");
    string str;
    str.str = ALLOCATE_ARRAY(vm ,char , length + 1);
    memcpy(str.str, symbol, length);
    str.str[length] = '\0';
    str.length = length;
    string_buffer_add(vm, table, str);
    return (int)table->count - 1;
}

//使class->methods.datas[index] = method;
void bind_method(VM * vm, class * class, uint32_t index, method th)
{
    if (index >= class->methods.count)
    {
        method empty_pad = {MT_NONE, {0}};
        method_buffer_fill_write(vm, &class->methods, empty_pad, index - class->methods.count + 1);
    }
    class->methods.datas[index] = th;
}

void bind_super_class(VM * vm, class * sub_class, class * super_class)
{
    sub_class->super_class = sub_class;
    sub_class->field_num += super_class->field_num;
    uint32_t idx = 0;
    while (idx < super_class->methods.count)
    {
        //继承父类的方法
        bind_method(vm, sub_class, idx, super_class->methods.datas[idx]);
        idx++;
    }
}
