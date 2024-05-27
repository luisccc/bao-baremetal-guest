/** 
 * Bao, a Lightweight Static Partitioning Hypervisor 
 *
 * Copyright (c) Bao Project (www.bao-project.org), 2019-
 *
 * Authors:
 *      Jose Martins <jose.martins@bao-project.org>
 *      Sandro Pinto <sandro.pinto@bao-project.org>
 *
 * Bao is free software; you can redistribute it and/or modify it under the
 * terms of the GNU General Public License version 2 as published by the Free
 * Software Foundation, with a special exception exempting guest code from such
 * license. See the COPYING file in the top-level directory for details. 
 *
 */

#include <core.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <cpu.h>
#include <wfi.h>
#include <spinlock.h>
#include <plat.h>
#include <irq.h>
#include <uart.h>
#include <timer.h>
#include <idma.h>

#define TIMER_INTERVAL (TIME_S(1))

spinlock_t print_lock = SPINLOCK_INITVAL;

// DMA parameters
#define DMA_TRANSFER_SIZE  (8) // 4KB, i.e. page size

#define DMA_CONF_DECOUPLE 0
#define DMA_CONF_DEBURST 0
#define DMA_CONF_SERIALIZE 0

#define DMA_SRC_ATTACK          (0x83200000ULL)
#define DMA_DST_ATTACK          (0x83200100ULL)
#define DMA_OPENSBI_BASE_ATTACK (0x80002700ULL)     // 80038000
#define DMA_BAO_BASE_ATTACK     (0x80200000ULL)
#define DMA_ATTACK_LOOPS        (1)

/*
* DMA configuration registers
*/
volatile uint64_t *dma_src       = (volatile uint64_t *) DMA_SRC_ADDR(0);
volatile uint64_t *dma_dst       = (volatile uint64_t *) DMA_DST_ADDR(0);
volatile uint64_t *dma_num_bytes = (volatile uint64_t *) DMA_NUMBYTES_ADDR(0);
volatile uint64_t *dma_conf      = (volatile uint64_t *) DMA_CONF_ADDR(0);
volatile uint64_t *dma_nextid    = (volatile uint64_t *) DMA_NEXTID_ADDR(0);
volatile uint64_t *dma_done      = (volatile uint64_t *) DMA_DONE_ADDR(0);
volatile uint64_t *dma_intf      = (volatile uint64_t *) DMA_INTF_ADDR(0);

// OpenSBI as default target
uint64_t fixed_dst = (uint64_t) DMA_DST_ATTACK;
uint64_t dst = (uint64_t) DMA_DST_ATTACK;
uint64_t src = (uint64_t) DMA_SRC_ATTACK;

static inline void fence_i() {
    asm volatile("fence.i" ::: "memory");
}

void uart_rx_handler(){
    printf("cpu%d: %s\n",get_cpuid(), __func__);
    uart_clear_rxirq();
}

void ipi_handler(){
    printf("cpu%d: %s\n", get_cpuid(), __func__);
    irq_send_ipi(1ull << (get_cpuid() + 1));
}

void timer_handler(){
    //printf("cpu%d: %s\n", get_cpuid(), __func__);
    timer_set(TIMER_INTERVAL);
    irq_send_ipi(1ull << (get_cpuid() + 1));
}

void state_attack(void)
{
    uint64_t intf = 0;
    uint64_t *tmps = (uint64_t *) src;
    uint64_t *tmpd = (uint64_t *) dst;
    
    *tmps = 23;
    *tmpd = 0;

    fence_i();
    printf("Src: %p\n", tmps);
    printf("Dst: %p\n", tmpd);
    printf("Src: %d\n", *tmps);
    printf("Dst: %d\n", *tmpd);

    uint64_t transfer_id = *dma_nextid;
    printf("transfer_id: %d\n", transfer_id);

    for(int i = 0; i < 1000000; i++)
        ;

    /*** Attack! ***/
    printf("\t We here!\r\n");

    /*** Setup DMA transfer ***/
    *dma_src        = (uint64_t) src;
    *dma_dst        = (uint64_t) dst;
    *dma_num_bytes  = (uint64_t) DMA_TRANSFER_SIZE;
    *dma_conf       = (DMA_CONF_DECOUPLE  << DMA_FRONTEND_CONF_DECOUPLE_BIT   ) |
                      (DMA_CONF_DEBURST   << DMA_FRONTEND_CONF_DEBURST_BIT    ) |
                      (DMA_CONF_SERIALIZE << DMA_FRONTEND_CONF_SERIALIZE_BIT  );

    //dst = fixed_dst;

    for (int i = 0; i < DMA_ATTACK_LOOPS; i++)
    {
        fence_i();
        
        printf("%x\n", *dma_dst);

        // launch transfer: read id
        uint64_t transfer_id = *dma_nextid;
        printf("transfer_id: %d\n", transfer_id);

        // poll done register
        while (*dma_done != transfer_id);

        // increment for next attack
        //dst = dst + DMA_TRANSFER_SIZE;
    }

    for(int i = 0; i < 1000000; i++)
        ;

    fence_i();
    tmps = (uint64_t *) src;
    tmpd = (uint64_t *) dst;

    // for (int i = 0; i < DMA_TRANSFER_SIZE; i++)
    // {
        printf("Src: %d\n", *tmps);
        printf("Dst: %d\n", *tmpd);

        //tmps++;
        //tmpd++;
    //}
}

void main(void){

    static volatile bool master_done = false;

    if(cpu_is_master()){
        spin_lock(&print_lock);
        printf("Bao IOPMP bare-metal test guest\n");
        spin_unlock(&print_lock);

        irq_set_handler(UART_IRQ_ID, uart_rx_handler);
        irq_set_handler(TIMER_IRQ_ID, timer_handler);
        irq_set_handler(IPI_IRQ_ID, ipi_handler);

        uart_enable_rxirq();

        timer_set(TIMER_INTERVAL);
        irq_enable(TIMER_IRQ_ID);
        irq_set_prio(TIMER_IRQ_ID, IRQ_MAX_PRIO);

        master_done = true;
    }

    irq_enable(UART_IRQ_ID);
    irq_set_prio(UART_IRQ_ID, IRQ_MAX_PRIO);
    irq_enable(IPI_IRQ_ID);
    irq_set_prio(IPI_IRQ_ID, IRQ_MAX_PRIO);

    while(!master_done);
    spin_lock(&print_lock);
    printf("cpu %d up\n", get_cpuid());
    spin_unlock(&print_lock);

    state_attack();
    while(1)
        wfi();
}
