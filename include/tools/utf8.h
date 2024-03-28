//
// Created by wj on 24-3-21.
//

#ifndef DRINK_UTF8_H
#define DRINK_UTF8_H

#include <stdint.h>

//返回value按照utf8编码后的字节数
uint32_t get_byte_num_of_encode_utf8(int value);

//返回解码utf8的字节数
uint32_t get_byte_num_of_decode_utf8(uint8_t byte);

//把value编码为utf8后写入缓冲区,返回写入的字节数
uint8_t encode_utf8(uint8_t * buf, int value);

//解码以byte为起始地址的utf8序列，其最大长度为length,如果不是utf8序列，则返回-1
int decode_utf8(const uint8_t * byte, uint32_t length);

#endif //DRINK_UTF8_H
