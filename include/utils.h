//
// Created by xsy on 2024/3/20.
//

#ifndef MINIATURE_GIGGLE_UTILS_H
#define MINIATURE_GIGGLE_UTILS_H

#include "common.h"

typedef uint8_t byte;

void* memManager(VM* vm, void* ptr, uint32_t old_size, uint32_t new_size);

#define ALLOCATE(vmPtr, type) \
   (type*)memManager(vmPtr, NULL, 0, sizeof(type))

#define ALLOCATE_EXTRA(vmPtr, mainType, extraSize) \
   (mainType*)memManager(vmPtr, NULL, 0, sizeof(mainType) + extraSize)

#define ALLOCATE_ARRAY(vmPtr, type, count) \
   (type*)memManager(vmPtr, NULL, 0, sizeof(type) * count)

#define DEALLOCATE_ARRAY(vmPtr, arrayPtr, count) \
   memManager(vmPtr, arrayPtr, sizeof(arrayPtr[0]) * count, 0)

#define DEALLOCATE(vmPtr, memPtr) memManager(vmPtr, memPtr, 0, 0)

uint32_t ceilToPowerOf2(uint32_t v);

typedef struct {
    char* str;
    uint32_t length;
} string;

typedef struct {
    uint32_t length; //除结束'\0'之外的字符个数
    char start[0];  //类似c99中的柔性数组
} char_value;  //字符串缓冲区

//声明buffer类型
#define DECLARE_BUFFER_TYPE(type)\
   typedef struct {\
      /* 数据缓冲区 */    \
      type* datas;\
      /*缓冲区中已使用的元素个数*/\
      uint32_t count;\
      /*缓冲区容量用*/\
      uint32_t capacity;\
   } type##_buffer;\
   void type##_buffer_init(type##_buffer* buf);\
   void type##_buffer_fill_write(VM* vm, \
	 type##_buffer* buf, type data, uint32_t fill_count);\
   void type##_buffer_add(VM* vm, type##_buffer* buf, type data);\
   void type##_buffer_clear(VM* vm, type##_buffer* buf);

//定义buffer方法
#define DEFINE_BUFFER_METHOD(type)\
   void type##_buffer_init(type##_buffer* buf) {\
      buf->datas = NULL;\
      buf->count = buf->capacity = 0;\
   }\
\
   void type##_buffer_fill_write(VM* vm, \
	 type##_buffer* buf, type data, uint32_t fill_count) {\
      uint32_t new_counts = buf->count + fill_count;\
      if (new_counts > buf->capacity) {\
	 size_t old_size = buf->capacity * sizeof(type);\
	 buf->capacity = ceilToPowerOf2(new_counts);\
	 size_t new_size = buf->capacity * sizeof(type);\
	 ASSERT(new_size > old_size, "faint...memory allocate!");\
	 buf->datas = (type*)memManager(vm, buf->datas, old_size, new_size);\
      }\
      uint32_t cnt = 0;\
      while (cnt < fill_count) {\
	 buf->datas[buf->count++] = data;\
	 cnt++;\
      }\
   }\
\
   void type##_buffer_add(VM* vm, type##_buffer* buf, type data) {\
      type##_buffer_fill_write(vm, buf, data, 1);\
   }\
\
   void type##_buffer_clear(VM* vm, type##_buffer* buf) {\
      size_t old_size = buf->capacity * sizeof(buf->datas[0]);\
      memManager(vm, buf->datas, old_size, 0);\
      type##_buffer_init(buf);\
   }

DECLARE_BUFFER_TYPE(string)
DECLARE_BUFFER_TYPE(int)
DECLARE_BUFFER_TYPE(char)
DECLARE_BUFFER_TYPE(byte)

#define SymbolTable string_buffer

typedef enum {
    ERROR_IO,
    ERROR_MEM,
    ERROR_LEX,
    ERROR_COMPILE,
    ERROR_RUNTIME
} ErrorType;

void errorReport(void* parser,
                 ErrorType errorType, const char* fmt, ...);

void symbolTableClear(VM*, SymbolTable* buffer);

#define IO_ERROR(...)\
   errorReport(NULL, ERROR_IO, __VA_ARGS__)

#define MEM_ERROR(...)\
   errorReport(NULL, ERROR_MEM, __VA_ARGS__)

#define LEX_ERROR(parser, ...)\
   errorReport(parser, ERROR_LEX, __VA_ARGS__)

#define COMPILE_ERROR(parser, ...)\
   errorReport(parser, ERROR_COMPILE, __VA_ARGS__)

#define RUN_ERROR(...)\
   errorReport(NULL, ERROR_RUNTIME, __VA_ARGS__)

#define DEFAULT_BUfFER_SIZE 512

#endif //MINIATURE_GIGGLE_UTILS_H
