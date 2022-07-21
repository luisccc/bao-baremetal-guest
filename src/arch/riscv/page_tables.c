#include <stdint.h>
#include <stdio.h>
#include <page_tables.h> 
#include <csrs.h>
#include <plat.h>

pte_t pt[6][PAGE_SIZE/sizeof(pte_t)] __attribute__((aligned(PAGE_SIZE)));

/* void pt_init(){

    uintptr_t addr = 0x0000000;
    for(int i = 0; i < 4; i++){
        pt[i] = PTE_V | PTE_AD | PTE_RWX | (addr >> 2);  
        addr +=  SUPERPAGE_SIZE(0);
    }

    uintptr_t satp = (((uintptr_t)pt) >> 12) | (0x8ULL << 60);
    CSRW(satp, satp);
} */
//pte_t vspt[6][PAGE_SIZE/sizeof(pte_t)] __attribute__((aligned(PAGE_SIZE)));

void pt_init(){

    uintptr_t addr;

    addr = 0x00000000;
    for(int i = 0; i < 4; i++){
        pt[0][i] = 
            PTE_V | PTE_AD | PTE_RWX | (addr >> 2);  
        addr +=  SUPERPAGE_SIZE(0);
    }

    pt[0][0x80000000/SUPERPAGE_SIZE(0)] = 
        PTE_V | (((uintptr_t)&pt[1][0]) >> 2);

    addr = 0x80000000;
    for(int i = 0; i < 512; i++) pt[1][i] = 0;
    for(int i = 0; i <  PLAT_MEM_SIZE/SUPERPAGE_SIZE(1)/2; i++){
        pt[1][i] = 
           PTE_V | PTE_AD | PTE_RWX | (addr >> 2);  
        addr +=  SUPERPAGE_SIZE(1);
    }

    uintptr_t satp = (((uintptr_t)pt) >> 12) | (0x8ULL << 60);
    CSRW(satp, satp);  
}