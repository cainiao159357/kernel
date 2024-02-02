//
// Created by 张鹏 on 2024/1/21.
//
#include "string.h"
#include "global.h"
#include "debug.h"
void memset(void* dst_, uint8_t value, uint32_t size){
    ASSERT(dst_!=NULL);
    uint8_t *dst=(uint8_t *)dst_;
    while(size--){
       *dst=value;
       dst++;
    }
}
void memcpy(void* dst_, const void* src_, uint32_t size){
    ASSERT(dst_!=NULL && src_!=NULL);
    uint8_t *dst=(uint8_t *)dst_;
    uint8_t *src=(uint8_t *)src_;
    while(size--){
        *dst=*src;
        dst++;
        src++;
    }
}
int memcmp(const void* a_, const void* b_, uint32_t size){
    ASSERT(a_!=NULL && b_!=NULL);
    uint8_t *a=(uint8_t *)a_;
    uint8_t *b=(uint8_t *)b_;
    for(uint32_t i=0;i<size;i++){
        if(a[i]!=b[i]){
            return a[i]>b[i]?1:-1;
        }
    }
    return 0;
}
char* strcpy(char* dst_, const char* src_){
    ASSERT(dst_!=NULL && src_!=NULL);
    uint32_t i;
    for(i=0;src_[i]!=0;i++){
        dst_[i]=src_[i];
    }
    dst_[i]=0;
    return dst_;
}
uint32_t strlen(const char* str){
    ASSERT(str!=NULL);
    uint32_t length=0;
    while (str[length]!=0){
        length++;
    }
    return length;
}
int8_t strcmp (const char *a, const char *b){
    ASSERT(a!=NULL && b!=NULL);
    for(int i=0;;i++){
        if(a[i]==b[i] && a[i]==0){
            return 0;
        }
        if(a[i]!=b[i]){
            return a[i]>b[i]?1:-1;
        }
    }
}
char* strrchr(const char* string, uint8_t ch){
    ASSERT(string!=NULL);
    char *res_addr=NULL;
    for(int i=0;string[i]!=0;i++){
        if(string[i]==ch){
            res_addr=(char *)&string[i];
        }
    }
    return res_addr;
}
char* strchr(const char* string, uint8_t ch){
    ASSERT(string!=NULL);
    const char *res_addr=NULL;
    for(int i=0;string[i]!=0;i++){
        if(string[i]==ch){
            res_addr=&string[i];
            break;
        }
    }
    return (char *)res_addr;
}
char* strcat(char* dst_, const char* src_){
    ASSERT(dst_!=NULL && src_!=NULL);
    int idx=0;
    while(dst_[idx]!=0){
        idx++;
    }
    for(int i=0;src_[i]!=0;i++){
        dst_[idx+i]=src_[i];
    }
    return dst_;
}
uint32_t strchrs(const char* filename, uint8_t ch){
    uint32_t  cnt=0;
    for(int i=0;filename[i]!=0;i++){
        if(filename[i]==ch){
            cnt++;
        }
    }
    return cnt;
}

