//
// Created by wj on 24-3-21.
//

#ifndef DRINK_CORE_H
#define DRINK_CORE_H

#include "vm.h"

extern char * root_dir;

char * read_file(const char * source_file);

vm_result execute_module(VM * vm, value module_name, const char * module_code);

void build_core(VM * vm);

int get_index_from_symbol_table(symbol_table * table, const char * symbol, uint32_t length);

int add_symbol(VM * vm, symbol_table * table, const char * symbol, uint32_t length);

#endif //DRINK_CORE_H
