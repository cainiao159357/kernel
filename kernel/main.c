#include "print.h"
#include "init.h"
#include "debug.h"
int main(void) {
    put_str("I am kernel!\n");
    init_all();
    //asm volatile("sti;");
    ASSERT(1==2);
    return 0;
}