#include <sbi.h>
#include <csrs.h>

#define CSR_STIMECMP 0x14D

void timer_enable()
{
}

uint64_t timer_get()
{
    uint64_t time;
    asm volatile("rdtime %0" : "=r"(time)); 
    return time;
}

void timer_set(uint64_t n)
{
    uint64_t time_val = 0;
    time_val = timer_get();
#if CVA6_SSTC_EXTENSION_ENBL == 1
    CSRW(CSR_STIMECMP, time_val + n);
#else
    sbi_set_timer(time_val + n);
#endif
}
