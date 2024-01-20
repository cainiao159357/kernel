//
// Created by 张鹏 on 2024/1/19.
//

#ifndef BOCHS_INTERRUPT_H
#define BOCHS_INTERRUPT_H
#include "stdint.h"
typedef void* intr_handler;
void idt_init(void);

enum intr_status{
    intr_ON,
    intr_OFF
};
enum intr_status get_intr_status(void);
enum intr_status set_intr_status(enum intr_status);
enum intr_status intr_enable(void);
enum intr_status intr_disable(void);
#endif //BOCHS_INTERRUPT_H
