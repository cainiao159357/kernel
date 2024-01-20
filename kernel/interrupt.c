//
// Created by 张鹏 on 2024/1/19.
//
#include "global.h"
#include "interrupt.h"
#include "io.h"
#include "stdint.h"
#include "print.h"

#define PIC_M_CTRL 0x20
#define PIC_M_DATA 0x21
#define PIC_S_CTRL 0Xa0
#define PIC_S_DATA 0xa1

#define IDT_DESC_NUM 0x21
#define eflags_IF 0x00000200
#define get_eflags(eflags) asm volatile ("pushfl; popl %0":"=g"(eflags))


//中断门
struct inter_gate_desc{
    uint16_t function_low_off;
    uint16_t sector;
    uint8_t dcount;
    uint8_t attribute;
    uint16_t function_high_off;
};

static struct inter_gate_desc idt[IDT_DESC_NUM];
static void build_gata_desc(struct inter_gate_desc *gate_desc,uint8_t attribute,intr_handler function_addr);
extern intr_handler  intr_entry_table[IDT_DESC_NUM];
intr_handler idt_table[IDT_DESC_NUM];
char *intr_name[IDT_DESC_NUM];

enum intr_status get_intr_status(){
    uint32_t eflags=0;
    get_eflags(eflags);
    return (eflags_IF & eflags) ? intr_ON:intr_OFF;
}
enum intr_status intr_disable(){
    enum intr_status old_status;
    if(intr_ON==get_intr_status()){
        old_status=intr_ON;
        asm volatile(
                "cli;"
                :::"memory"
                );
    }else{
        old_status=intr_OFF;
    }
    return old_status;
}
enum intr_status intr_enable(){
    enum intr_status old_status;
    if(intr_OFF==get_intr_status()){
        old_status=intr_OFF;
        asm volatile("sti;");
    }else{
        old_status=intr_ON;
    }
    return old_status;
}

enum intr_status set_intr_status(enum intr_status i_status){
    if(i_status==intr_ON){
        return intr_enable();
    }else{
        return intr_disable();
    }
}

//初始化中断代理
static void pic_init(void){
    //初始化主片
    outb(PIC_M_CTRL,0x11); //ICW1:级联，需要ICW4,边沿触发
    outb(PIC_M_DATA,0x20); //ICW2：初始中断向量号是0x20
    outb(PIC_M_DATA,0x04); //ICW3:IR2接从片
    outb(PIC_M_DATA,0x01); //ICW4:EOI模式，手动退出

    //初始化从片
    outb(PIC_S_CTRL,0x11); //ICW1:级联，需要ICW4,边沿触发
    outb(PIC_S_DATA,0x28); //ICW2：初始中断向量号是0x28
    outb(PIC_S_DATA,0x02); //ICW3:从片连接到主片的IR2
    outb(PIC_S_DATA,0x01); //ICW4:EOI模式，手动退出

    //打开时钟中断
    outb(PIC_M_DATA,0xfe);
    outb(PIC_S_DATA,0xff);
    put_str("pic_init done\n");
}
static void build_gata_desc(struct inter_gate_desc *gate_desc,uint8_t attribute,intr_handler function_addr){
    gate_desc->function_low_off=(uint32_t)function_addr &0xffff;
    gate_desc->sector=SELECTOR_K_CODE;
    gate_desc->dcount=0;
    gate_desc->attribute=attribute;
    gate_desc->function_high_off=((uint32_t)function_addr&0xffff0000)>>16;
}
static void idt_desc_init(void){
    for(int i=0;i<IDT_DESC_NUM;i++){
        build_gata_desc(&idt[i],IDT_DESC_ATTR_DPL0,intr_entry_table[i]);
    }
    put_str("idt_desc_init done\n");
}
static void general_intr_handler(uint8_t intr_vec){
    if(intr_vec==0x27|| intr_vec==0x2f){
        return;
    }
    put_str("intr vector: 0x");
    put_int(intr_vec);
    put_char('\n');
}
static void exception_init(void){
    for(int i=0;i<IDT_DESC_NUM;i++){
        idt_table[i]= general_intr_handler;
        intr_name[i]="unknown";
    }
    intr_name[0] = "#DE Divide Error";
    intr_name[1] = "#DB Debug Exception";
    intr_name[2] = "NMI Interrupt";
    intr_name[3] = "#BP Breakpoint Exception";
    intr_name[4] = "#OF Overflow Exception";
    intr_name[5] = "#BR BOUND Range Exceeded Exception";
    intr_name[6] = "#UD Invalid Opcode Exception";
    intr_name[7] = "#NM Device Not Available Exception";
    intr_name[8] = "#DF Double Fault Exception";
    intr_name[9] = "Coprocessor Segment Overrun";
    intr_name[10] = "#TS Invalid TSS Exception";
    intr_name[11] = "#NP Segment Not Present";
    intr_name[12] = "#SS Stack Fault Exception";
    intr_name[13] = "#GP General Protection Exception";
    intr_name[14] = "#PF Page-Fault Exception";
    intr_name[16] = "#MF x87 FPU Floating-Point Error";
    intr_name[17] = "#AC Alignment Check Exception";
    intr_name[18] = "#MC Machine-Check Exception";
    intr_name[19] = "#XF SIMD Floating-Point Exception";
}

void idt_init(){
    pic_init();
    idt_desc_init();
    exception_init();
    uint64_t idt_operand=((((uint64_t)(uint32_t)idt)<<16)|(sizeof(idt)-1));
    asm volatile(
            "lidt %0"
            ::"m"(idt_operand)
            );
    put_str("idt_init done\n");
}