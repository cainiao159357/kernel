//
// Created by 张鹏 on 2024/1/20.
//
#include "debug.h"
#include "print.h"
#include "interrupt.h"

void panic_spin(char *filename,int line,const char *func,const char* condition){
    intr_disable();
    put_str("kernel error!!!!\n");
    put_str("filename:");put_str(filename);put_str("\n");
    put_str("line: 0x");put_int(line);put_str("\n");
    put_str("function:");put_str(func);put_str("\n");
    put_str("condition:");put_str(condition);put_str("\n");
    while(1);
}