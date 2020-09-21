#ifndef __SBI_H__
#define __SBI_H__

#include <stdint.h>

/**
 * From https://github.com/riscv/riscv-sbi-doc
 */

#define SBI_SUCCESS (0)
#define SBI_ERR_FAILURE (-1)
#define SBI_ERR_NOT_SUPPORTED (-2)
#define SBI_ERR_INVALID_PARAM (-3)
#define SBI_ERR_DENIED (-4)
#define SBI_ERR_INVALID_ADDRESS (-5)
#define SBI_ERR_ALREADY_AVAILABLE (-6)

struct sbiret {
    long error;
    long value;
};

void sbi_console_putchar(int ch);

struct sbiret sbi_get_spec_version(void);
struct sbiret sbi_get_impl_id(void);
struct sbiret sbi_get_impl_version(void);
struct sbiret sbi_probe_extension(long extension_id);
struct sbiret sbi_get_mvendorid(void);
struct sbiret sbi_get_marchid(void);
struct sbiret sbi_get_mimpid(void);

struct sbiret sbi_send_ipi(const unsigned long hart_mask,
                           unsigned long hart_mask_base);

struct sbiret sbi_set_timer(uint64_t stime_value);

struct sbiret sbi_remote_fence_i(const unsigned long hart_mask,
                                 unsigned long hart_mask_base);

struct sbiret sbi_remote_sfence_vma(const unsigned long hart_mask,
                                    unsigned long hart_mask_base,
                                    unsigned long start_addr,
                                    unsigned long size);

struct sbiret sbi_remote_sfence_vma_asid(const unsigned long hart_mask,
                                         unsigned long hart_mask_base,
                                         unsigned long start_addr,
                                         unsigned long size,
                                         unsigned long asid);

struct sbiret sbi_remote_hfence_gvma_vmid(const unsigned long hart_mask,
                                          unsigned long hart_mask_base,
                                          unsigned long start_addr,
                                          unsigned long size,
                                          unsigned long vmid);

struct sbiret sbi_remote_hfence_gvma(const unsigned long hart_mask,
                                     unsigned long hart_mask_base,
                                     unsigned long start_addr,
                                     unsigned long size);

struct sbiret sbi_remote_hfence_vvma_asid(const unsigned long hart_mask,
                                          unsigned long hart_mask_base,
                                          unsigned long start_addr,
                                          unsigned long size,
                                          unsigned long asid);

struct sbiret sbi_remote_hfence_vvma(const unsigned long hart_mask,
                                     unsigned long hart_mask_base,
                                     unsigned long start_addr,
                                     unsigned long size);

struct sbiret sbi_hart_start(unsigned long hartid, unsigned long start_addr,
                             unsigned long priv);
struct sbiret sbi_hart_stop();
struct sbiret sbi_hart_status(unsigned long hartid);

void sbi_shutdown(void);

#endif /* __SBI_H__ */
