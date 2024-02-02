//
// Created by 张鹏 on 2024/1/18.
//

#ifndef BOCHS_PRINT_H
#define BOCHS_PRINT_H
#include "stdint.h"
void put_char(uint8_t char_asci);
void put_str(const char *str);
void put_int(uint32_t num);
#endif //BOCHS_PRINT_H
