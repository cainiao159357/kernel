//
// Created by 张鹏 on 2024/2/2.
//

#ifndef BOCHS_MEMORY_H
#define BOCHS_MEMORY_H
#include "stdint.h"
#include "bitmap.h"
#define	 PG_P_1	  1	 //页表项或页目录项存在属性位
#define	 PG_P_0	  0	 //页表项或页目录项存在属性位
#define	 PG_RW_R  0	 //R/W 属性位值, 读/执行
#define	 PG_RW_W  2	 //R/W 属性位值, 读/写/执行
#define	 PG_US_S  0  //U/S 属性位值, 系统级
#define	 PG_US_U  4	 //U/S 属性位值, 用户级

enum pool_flags{
    PF_KERNEL,
    PF_USER
};

struct pool_virtual_addr{
    struct bitmap addr_bitmap;
    uint32_t addr_start;
};
struct pool_phy_addr{
    struct bitmap addr_bitmap;
    uint32_t addr_start;
    uint32_t poll_size;
};
extern struct pool_phy_addr kernel_phy_pool,user_phy_pool;
void mem_init(void);
void* get_kernel_pages(uint32_t pg_cnt);
void* malloc_page(enum pool_flags pf, uint32_t pg_cnt);
void malloc_init(void);
uint32_t* pte_ptr(uint32_t vaddr);
uint32_t* pde_ptr(uint32_t vaddr);
#endif //BOCHS_MEMORY_H
