//
// Created by 张鹏 on 2024/2/2.
//

#include "memory.h"
#include "bitmap.h"
#include "stdint.h"
#include "global.h"
#include "debug.h"
#include "print.h"
#include "string.h"

#define PG_SIZE 0X1000

#define MEM_BITMAP_BASE 0xc009a000

#define PDE_IDX(addr) ((addr & 0xffc00000) >> 22)
#define PTE_IDX(addr) ((addr & 0x003ff000) >> 12)

#define K_HEAP_START 0xc0100000

struct pool_phy_addr kernel_phy_pool,user_phy_pool;
struct pool_virtual_addr kernel_vir_pool;

static void* vaddr_get(enum pool_flags pf, uint32_t pg_cnt){
    uint32_t vaddr=0;
    int bit_idx;
    if(pf==PF_KERNEL){
        bit_idx= bitmap_scan(&kernel_vir_pool.addr_bitmap,pg_cnt);
        if(bit_idx==-1){
            return NULL;
        }
        bitmap_set(&kernel_vir_pool.addr_bitmap,bit_idx,pg_cnt,1);
        vaddr=kernel_vir_pool.addr_start+PG_SIZE*bit_idx;
    }else{

    }
    return (void *)vaddr;
}
/* 得到虚拟地址vaddr对应的pte指针*/
uint32_t* pte_ptr(uint32_t vaddr){
    return (void *)(0xffc00000+(PDE_IDX(vaddr)<<12)+(PTE_IDX(vaddr)*4));
}
/* 得到虚拟地址vaddr对应的pde的指针 */
uint32_t* pde_ptr(uint32_t vaddr){
    return (void *)(0xfffff000+ PDE_IDX(vaddr)*4);
}
/* 在m_pool指向的物理内存池中分配1个物理页,
 * 成功则返回页框的物理地址,失败则返回NULL */
static void* palloc(struct pool_phy_addr* m_pool){
    int paddr;
    int bit_idx= bitmap_scan(&(m_pool->addr_bitmap),1);
    if(bit_idx==-1){
        return NULL;
    }
    bitmap_set(&(m_pool->addr_bitmap),bit_idx,1,1);
    paddr=m_pool->addr_start+bit_idx*PG_SIZE;
    return (void *)paddr;
}

/* 页表中添加虚拟地址_vaddr与物理地址_page_phyaddr的映射 */
static void page_table_add(void* _vaddr, void* _page_phyaddr){
    uint32_t *pte_addr= pte_ptr((uint32_t)_vaddr);
    uint32_t *pde_addr= pde_ptr((uint32_t)_vaddr);
    if((*pde_addr)&0x00000001){
        ASSERT(!((*pte_addr)&0x00000001));
        *pte_addr=(((uint32_t)_page_phyaddr)| PG_US_U | PG_RW_W | PG_P_1);
    }else{
        uint32_t paddr= (uint32_t)palloc(&kernel_phy_pool);
        *pde_addr=(paddr| PG_US_U | PG_RW_W | PG_P_1);
        memset((void *)(0xffc00000+ (PDE_IDX((uint32_t)_vaddr)<<12)),0,PG_SIZE);
        *pte_addr=(((uint32_t)_page_phyaddr)| PG_US_U | PG_RW_W | PG_P_1);
    }
}

/* 分配pg_cnt个页空间,成功则返回起始虚拟地址,失败时返回NULL */
void* malloc_page(enum pool_flags pf, uint32_t pg_cnt){
    ASSERT(pg_cnt>0 && pg_cnt<3840);
    void *vaddr= vaddr_get(pf,pg_cnt);
    void *paddr;
    struct pool_phy_addr *mem_pool;
    mem_pool=(pf==PF_KERNEL)?&kernel_phy_pool:&user_phy_pool;
    if(vaddr==NULL){
        return NULL;
    }
    for(uint32_t i=0;i<pg_cnt;i++){
        paddr= palloc(mem_pool);
        if(paddr==NULL){
            return NULL;
        }
        page_table_add(vaddr+i* PG_SIZE,paddr);
    }
    return vaddr;
}
/* 从内核物理内存池中申请pg_cnt页内存,成功则返回其虚拟地址,失败则返回NULL */
void* get_kernel_pages(uint32_t pg_cnt){
    void*vaddr= malloc_page(PF_KERNEL,pg_cnt);
    if(vaddr!=NULL){
        memset(vaddr,0,pg_cnt*PG_SIZE);
    }
    return vaddr;
}

/* 初始化内存池 */
static void mem_pool_init(uint32_t all_mem){
    put_str("mem_pool work start!!!\n");
    uint32_t page_table_used=256*PG_SIZE;
    uint32_t used_mem=0x100000+page_table_used;
    uint32_t free_mem=all_mem-used_mem;
    uint32_t free_pages=free_mem/PG_SIZE;

    uint32_t kernel_free_pages=free_pages/2;
    uint32_t user_free_pages=free_pages-kernel_free_pages;

    uint32_t kernel_bitmap_len=kernel_free_pages/8;
    uint32_t user_bitmap_len=user_free_pages/8;

    uint32_t kernel_phy_start=used_mem;
    uint32_t user_phy_start=used_mem+kernel_free_pages*PG_SIZE;
    kernel_phy_pool.addr_start=kernel_phy_start;
    user_phy_pool.addr_start=user_phy_start;

    kernel_phy_pool.poll_size=kernel_free_pages*PG_SIZE;
    user_phy_pool.poll_size=user_free_pages*PG_SIZE;

    kernel_phy_pool.addr_bitmap.btmp_bytes_len=kernel_bitmap_len;
    user_phy_pool.addr_bitmap.btmp_bytes_len=user_bitmap_len;

    kernel_phy_pool.addr_bitmap.bits=(uint8_t*)MEM_BITMAP_BASE;
    user_phy_pool.addr_bitmap.bits=(uint8_t *)(MEM_BITMAP_BASE+kernel_bitmap_len);

    /******************** 输出内存池信息 **********************/
    put_str("      kernel_pool_bitmap_start:");put_int((int)kernel_phy_pool.addr_bitmap.bits);
    put_str(" kernel_pool_phy_addr_start:");put_int(kernel_phy_pool.addr_start);
    put_str("\n");
    put_str("      user_pool_bitmap_start:");put_int((int)user_phy_pool.addr_bitmap.bits);
    put_str(" user_pool_phy_addr_start:");put_int(user_phy_pool.addr_start);
    put_str("\n");

    bitmap_init(&kernel_phy_pool.addr_bitmap);
    bitmap_init(&user_phy_pool.addr_bitmap);

    kernel_vir_pool.addr_bitmap.btmp_bytes_len=kernel_bitmap_len;
    kernel_vir_pool.addr_start=K_HEAP_START;
    kernel_vir_pool.addr_bitmap.bits=(uint8_t*)(MEM_BITMAP_BASE+kernel_bitmap_len+user_bitmap_len);

    bitmap_init(&kernel_vir_pool.addr_bitmap);

    put_str("mem_pool work down!!!\n");
}
void mem_init(){
    put_str("mem_init work start!!!\n");
    uint32_t mem_bytes_total = (*(uint32_t*)(0xb00));
    mem_pool_init(mem_bytes_total);
    put_str("mem_init work down!!!\n");
}