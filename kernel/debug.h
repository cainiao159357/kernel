//
// Created by 张鹏 on 2024/1/20.
//

#ifndef BOCHS_DEBUG_H
#define BOCHS_DEBUG_H
void panic_spin(char *filename,int line,const char *func,const char* condition);

#define PANIC(...) panic_spin(__FILE__,__LINE__,__func__,__VA_ARGS__)

#ifdef NODEBUG
    #define ASSERT(CONDITION) ()
#else
    #define ASSERT(CONDITION)  if(CONDITION){ \
    }else{                                    \
    PANIC(#CONDITION);                        \
                                              \
    }

#endif //NODEBUG

#endif //BOCHS_DEBUG_H
