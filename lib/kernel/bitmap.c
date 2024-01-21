//
// Created by 张鹏 on 2024/1/21.
//

#include "bitmap.h"
#include "debug.h"
#include "string.h"
void bitmap_init(struct bitmap* btmp){
    ASSERT(btmp!=NULL);
    memset(btmp->bits,0,btmp->btmp_bytes_len);
}

/* 判断bit_idx位是否为1,若为1则返回true，否则返回false */
bool bitmap_scan_test(struct bitmap* btmp, uint32_t bit_idx){
    ASSERT(btmp!=NULL);
    uint32_t btmp_bits_idx=bit_idx/8;
    uint32_t btmp_bit_idx=bit_idx%8;
    if(btmp->bits[btmp_bits_idx]&SHL(BITMAP_MASK,btmp_bit_idx)){
        return true;
    }else{
        return false;
    }
}
/* 在位图中申请连续cnt个位,成功则返回其起始位下标，失败返回-1 */
int bitmap_scan(struct bitmap* btmp, uint32_t cnt){
    ASSERT(btmp!=NULL && cnt>=1);
    uint32_t bit_idx;
    for(bit_idx=0;bit_idx/8<btmp->btmp_bytes_len;bit_idx++){
        if(bitmap_scan_test(btmp,bit_idx)){
            continue;
        }
        uint32_t scan_idx;
        for(scan_idx=bit_idx+1;(scan_idx-bit_idx)< cnt;scan_idx++){
            if(bitmap_scan_test(btmp,scan_idx)){
                break;
            }
        }
        if((scan_idx-bit_idx)==cnt){
            return bit_idx;
        }else{
            bit_idx=scan_idx+1;
        }
    }
    return -1;
}

/* 将位图btmp的bit_idx位设置为value */
void bitmap_set(struct bitmap* btmp, uint32_t bit_idx, int8_t value){
    ASSERT(btmp!=NULL && (value==0||value==1));
    uint32_t btmp_bits_idx=bit_idx/8;
    uint32_t btmp_bit_idx=bit_idx%8;
    if(value==1){
        btmp->bits[btmp_bits_idx]|=SHL(BITMAP_MASK,btmp_bit_idx);
    }else{
        btmp->bits[btmp_bits_idx]&=~SHL(BITMAP_MASK,btmp_bit_idx);
    }
}