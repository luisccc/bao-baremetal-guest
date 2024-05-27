#include <config.h>

VM_IMAGE(baremetal_image, XSTR(path/to/baremetal/build/cva6/baremetal.bin));

struct config config = {
    
    CONFIG_HEADER

    .vmlist_size = 1,
    .vmlist = {
        { 
            .image = {
                .base_addr = 0x80200000,
                .load_addr = VM_IMAGE_OFFSET(baremetal_image),
                .size = VM_IMAGE_SIZE(baremetal_image),
                .inplace = true
            },

            .entry = 0x80200000,

            .platform = {
                .cpu_num = 1,
                
                .region_num = 2,
                .regions =  (struct vm_mem_region[]) {
                    {
                        .base = 0x80200000,
                        .size = 0x2000000
                    },
                    {
                        .base = 0x83200000,
                        .size = 0x1000000,
                        .place_phys = true,
                        .phys = 0x83200000
                    }
                },

                .dev_num = 6,
                .devs =  (struct vm_dev_region[]) {
                    {
                        .pa = 0x10000000,   
                        .va = 0x10000000,  
                        .size = 0x00010000,  
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) {1}
                    },
                    {
                        .pa = 0x18000000,   
                        .va = 0x18000000,  
                        .size = 0x00001000,  
                        .interrupt_num = 4,
                        .interrupts = (irqid_t[]) {4,5,6,7}
                    },
                    {
                        .pa = 0x20000000,   
                        .va = 0x20000000,  
                        .size = 0x00001000,  
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) {2}
                    },
                    {
                        .pa = 0x30000000,   
                        .va = 0x30000000,  
                        .size = 0x00008000,  
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) {3}
                    },
                    {
                        .pa = 0x40000000,   
                        .va = 0x40000000,  
                        .size = 0x00010000,  
                        .interrupt_num = 0,
                        .interrupts = (irqid_t[]) {}
                    },
                    {
                        .pa = 0x50000000,   
                        .va = 0x50000000,  
                        .size = 0x00010000,  
                        .interrupt_num = 0,
                        .interrupts = (irqid_t[]) {}
                    },
                },

                .arch = {
                   .irqc.plic.base = 0xc000000,
                }
            },
        },
     }
};
