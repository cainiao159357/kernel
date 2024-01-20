//
// Created by 张鹏 on 2024/1/19.
//

#include "timer.h"
#include "io.h"
#include "print.h"

#define IRQ0_FREQUENCY	   1
#define INPUT_FREQUENCY	   1193180
#define COUNTER0_VALUE	   ((uint16_t)INPUT_FREQUENCY / IRQ0_FREQUENCY)
#define CONTRER0_PORT	   0x40
#define COUNTER0_IDX	   0
#define COUNTER_MODE	   2
#define READ_WRITE_LATCH   3
#define PIT_CONTROL_PORT   0x43

static void timer_intr_set(uint16_t port,uint8_t counter_idx,uint8_t rw,uint8_t counter_mode,uint16_t value){
    //选择计数器，设置mode,选择rw方式
    outb(PIT_CONTROL_PORT,(uint8_t)((counter_idx<<6)|(rw<<4)|(counter_mode<<1)));
    //写选择的计数器的低八位
    outb(port,(uint8_t)value);
    //高八位
    outb(port,(uint8_t)(value>>8));
}

void timer_init(){
    timer_intr_set(CONTRER0_PORT,COUNTER0_IDX,READ_WRITE_LATCH,\
    COUNTER_MODE,COUNTER0_VALUE);
    put_str("timer_init done\n");
}