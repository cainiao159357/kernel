//
// Created by 张鹏 on 2024/1/19.
//

#ifndef BOCHS_IO_H
#define BOCHS_IO_H
#include "stdint.h"
static inline void outb(uint16_t port,uint8_t data){
    asm volatile(
            "outb %b0,%w1"
            ::"a"(data),"Nd"(port)
            );
}
static inline uint8_t intb(uint16_t port){
    uint8_t data;
    asm volatile(
            "inb %w1,%b0"
            :"=a"(data)
            :"Nd"(port)
            );
    return data;
}
static inline void outsw(uint16_t port,uint32_t word_cnt,void* addr){
    asm volatile(
            "cld;"
            "rep outsw;"
            :"+S"(addr),"+c"(word_cnt)
            :"Nd"(port)
            );
}
static inline void intsw(uint16_t port,uint32_t word_cnt,void* addr){
    asm volatile(
            "cld;"
            "rep insw;"
            :"+c"(word_cnt),"+D"(addr)
            :"Nd"(port)
            :"memory"
            );
}
#endif //BOCHS_IO_H
