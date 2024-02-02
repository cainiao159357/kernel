//
// Created by 张鹏 on 2024/1/19.
//
#include "init.h"
#include "print.h"
#include "interrupt.h"
#include "timer.h"
#include "memory.h"
void init_all(){
    put_str("init_all start\n");
    timer_init();
    idt_init();
    mem_init();
}