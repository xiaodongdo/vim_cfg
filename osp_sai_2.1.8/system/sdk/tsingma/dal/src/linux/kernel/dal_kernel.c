/**
 @file dal_kernal.c

 @date 2012-10-18

 @version v2.0


*/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/types.h>
#include <asm/io.h>
#include <linux/pci.h>
#include <linux/sched.h>
#include <asm/irq.h>
#include <linux/poll.h>
#include <linux/wait.h>
#include <linux/interrupt.h>
#include <linux/version.h>
#include <linux/dma-mapping.h>
#ifdef SOC_ACTIVE
#include <linux/platform_device.h>
#endif
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 0, 0))
#include <linux/irqdomain.h>
#endif
#include "dal_kernel.h"
#include "dal_common.h"
#include "dal_mpool.h"
#include <linux/slab.h>
MODULE_AUTHOR("Centec Networks Inc.");
MODULE_DESCRIPTION("DAL kernel module");
MODULE_LICENSE("GPL");

/* DMA memory pool size */
static char* dma_pool_size;
module_param(dma_pool_size, charp, 0);
MODULE_PARM_DESC(dma_pool_size,
                 "Specify DMA memory pool size (default 4MB)");

/*****************************************************************************
 * defines
 *****************************************************************************/
#define MB_SIZE 0x100000
#define CTC_MAX_INTR_NUM 8

#define MEM_MAP_RESERVE SetPageReserved
#define MEM_MAP_UNRESERVE ClearPageReserved

#define CTC_VENDOR_VID 0xc001
#define CTC_HUMBER_DEVICE_ID 0x6048
#define CTC_GOLDENGATE_DEVICE_ID 0xc010
#define CTC_PCIE_VENDOR_ID   0xcb10
#define CTC_DUET2_DEVICE_ID  0x7148
#define CTC_TSINGMA_DEVICE_ID  0x5236

#define MEM_MAP_RESERVE SetPageReserved
#define MEM_MAP_UNRESERVE ClearPageReserved

#define CTC_GREATBELT_DEVICE_ID 0x03e8  /* TBD */
#define DAL_MAX_CHIP_NUM   8
#define VIRT_TO_PAGE(p)     virt_to_page((p))
#define DAL_UNTAG_BLOCK         0
#define DAL_DISCARD_BLOCK      1
#define DAL_MATCHED_BLOCK     2
#define DAL_CUR_MATCH_BLOCk 3
/*****************************************************************************
 * typedef
 *****************************************************************************/
/* Control Data */
typedef struct dal_isr_s
{
    int irq;
    void (* isr)(void*);
    void (* isr_knet)(void*);
    void* isr_data;
    void* isr_knet_data;
    int trigger;
    int count;
    wait_queue_head_t wqh;
} dal_isr_t;

#ifdef SOC_ACTIVE
typedef struct dal_kernel_dev_s
{
    struct list_head list;
    struct platform_device* pci_dev;

    /* PCI I/O mapped base address */
    void __iomem * logic_address;

    /* Physical address */
    uintptr phys_address;
} dal_kern_dev_t;
#else
typedef struct dal_kernel_dev_s
{
    struct list_head list;
    struct pci_dev* pci_dev;

    /* PCI I/O mapped base address */
    uintptr logic_address;

    /* Physical address */
    unsigned long long phys_address;
} dal_kern_dev_t;
#endif

typedef struct _dma_segment
{
    struct list_head list;
    unsigned long req_size;     /* Requested DMA segment size */
    unsigned long blk_size;     /* DMA block size */
    unsigned long blk_order;    /* DMA block size in alternate format */
    unsigned long seg_size;     /* Current DMA segment size */
    unsigned long seg_begin;    /* Logical address of segment */
    unsigned long seg_end;      /* Logical end address of segment */
    unsigned long* blk_ptr;     /* Array of logical DMA block addresses */
    int blk_cnt_max;            /* Maximum number of block to allocate */
    int blk_cnt;                /* Current number of blocks allocated */
} dma_segment_t;

typedef irqreturn_t (*p_func) (int irq, void* dev_id);

/***************************************************************************
 *declared
 ***************************************************************************/
static unsigned int linux_dal_poll0(struct file* filp, struct poll_table_struct* p);
static unsigned int linux_dal_poll1(struct file* filp, struct poll_table_struct* p);
static unsigned int linux_dal_poll2(struct file* filp, struct poll_table_struct* p);
static unsigned int linux_dal_poll3(struct file* filp, struct poll_table_struct* p);
static unsigned int linux_dal_poll4(struct file* filp, struct poll_table_struct* p);
static unsigned int linux_dal_poll5(struct file* filp, struct poll_table_struct* p);
static unsigned int linux_dal_poll6(struct file* filp, struct poll_table_struct* p);
static unsigned int linux_dal_poll7(struct file* filp, struct poll_table_struct* p);

/*****************************************************************************
 * global variables
 *****************************************************************************/
static dal_kern_dev_t dal_dev[DAL_MAX_CHIP_NUM];
static dal_isr_t dal_isr[CTC_MAX_INTR_NUM];
#ifdef SOC_ACTIVE
static dal_isr_t dal_int[CTC_MAX_INTR_NUM] = {0};
#endif
static int dal_chip_num = 0;
static int dal_version = 0;
static int dal_intr_num = 0;
static int use_high_memory = 0;
static unsigned int* dma_virt_base[DAL_MAX_CHIP_NUM];
static unsigned long long dma_phy_base[DAL_MAX_CHIP_NUM];
static unsigned int dma_mem_size = 0xc00000;
static unsigned int msi_irq_base[DAL_MAX_CHIP_NUM];
static unsigned int msi_irq_num[DAL_MAX_CHIP_NUM];
static unsigned int msi_used = 0;
static struct class *dal_class;

static LIST_HEAD(_dma_seg);
static int dal_debug = 0;
module_param(dal_debug, int, 0);
MODULE_PARM_DESC(dal_debug, "Set debug level (default 0)");

#ifndef SOC_ACTIVE
static struct pci_device_id dal_id_table[] =
{
    {PCI_DEVICE(CTC_VENDOR_VID, CTC_GREATBELT_DEVICE_ID)},
    {PCI_DEVICE(CTC_PCIE_VENDOR_ID, CTC_GOLDENGATE_DEVICE_ID)},
    {PCI_DEVICE((CTC_PCIE_VENDOR_ID+1), (CTC_GOLDENGATE_DEVICE_ID+1))},
    {PCI_DEVICE(CTC_PCIE_VENDOR_ID, CTC_DUET2_DEVICE_ID)},
    {PCI_DEVICE(CTC_PCIE_VENDOR_ID, CTC_TSINGMA_DEVICE_ID)},
    {0, },
};
#else
static const struct of_device_id linux_dal_of_match[] = {
    { .compatible = "centec,dal-localbus",},
    {},
};
MODULE_DEVICE_TABLE(of, linux_dal_of_match);
#endif

static wait_queue_head_t poll_intr[CTC_MAX_INTR_NUM];

p_func intr_handler_fun[CTC_MAX_INTR_NUM];

static int poll_intr_trigger[CTC_MAX_INTR_NUM];

static struct file_operations dal_intr_fops[CTC_MAX_INTR_NUM] =
{
    {
        .owner = THIS_MODULE,
        .poll = linux_dal_poll0,
    },
    {
        .owner = THIS_MODULE,
        .poll = linux_dal_poll1,
    },
    {
        .owner = THIS_MODULE,
        .poll = linux_dal_poll2,
    },
    {
        .owner = THIS_MODULE,
        .poll = linux_dal_poll3,
    },
    {
        .owner = THIS_MODULE,
        .poll = linux_dal_poll4,
    },
    {
        .owner = THIS_MODULE,
        .poll = linux_dal_poll5,
    },
    {
        .owner = THIS_MODULE,
        .poll = linux_dal_poll6,
    },
    {
        .owner = THIS_MODULE,
        .poll = linux_dal_poll7,
    },
};
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,0))
#include <linux/slab.h>
#define virt_to_bus virt_to_phys
#define bus_to_virt phys_to_virt
#endif
/*****************************************************************************
 * macros
 *****************************************************************************/
#define VERIFY_CHIP_INDEX(n) (n < dal_chip_num)

#define _KERNEL_INTERUPT_PROCESS
static irqreturn_t
intr0_handler(int irq, void* dev_id)
{
    dal_isr_t* p_dal_isr = (dal_isr_t*)dev_id;

    if(poll_intr_trigger[0])
    {
        return IRQ_HANDLED;
    }

    disable_irq_nosync(irq);

    if (p_dal_isr)
    {
        if (p_dal_isr->isr)
        {
            /* kernel mode interrupt handler */
            p_dal_isr->isr(p_dal_isr->isr_data);
        }
        else if ((NULL == p_dal_isr->isr) && (NULL == p_dal_isr->isr_data))
        {
            /* user mode interrupt handler */
           poll_intr_trigger[0] = 1;
           wake_up(&poll_intr[0]);
        }

        if (p_dal_isr->isr_knet)
        {
            /* kernel mode interrupt handler */
            p_dal_isr->isr_knet(p_dal_isr->isr_knet_data);
        }
    }

    return IRQ_HANDLED;
}

static irqreturn_t
intr1_handler(int irq, void* dev_id)
{
    dal_isr_t* p_dal_isr = (dal_isr_t*)dev_id;

    if(poll_intr_trigger[1])
    {
        return IRQ_HANDLED;
    }

    disable_irq_nosync(irq);

    if (p_dal_isr)
    {
        if (p_dal_isr->isr)
        {
            /* kernel mode interrupt handler */
            p_dal_isr->isr(p_dal_isr->isr_data);
        }
        else if ((NULL == p_dal_isr->isr) && (NULL == p_dal_isr->isr_data))
        {
            /* user mode interrupt handler */
           poll_intr_trigger[1] = 1;
           wake_up(&poll_intr[1]);
        }
    }

    return IRQ_HANDLED;
}

static irqreturn_t
intr2_handler(int irq, void* dev_id)
{
    dal_isr_t* p_dal_isr = (dal_isr_t*)dev_id;
    if(poll_intr_trigger[2])
    {
        return IRQ_HANDLED;
    }
    disable_irq_nosync(irq);

    if (p_dal_isr)
    {
        if (p_dal_isr->isr)
        {
            /* kernel mode interrupt handler */
            p_dal_isr->isr(p_dal_isr->isr_data);
        }
        else if ((NULL == p_dal_isr->isr) && (NULL == p_dal_isr->isr_data))
        {
            /* user mode interrupt handler */
           poll_intr_trigger[2] = 1;
           wake_up(&poll_intr[2]);
        }
    }

    return IRQ_HANDLED;
}

static irqreturn_t
intr3_handler(int irq, void* dev_id)
{
    dal_isr_t* p_dal_isr = (dal_isr_t*)dev_id;
    if(poll_intr_trigger[3])
    {
        return IRQ_HANDLED;
    }
    disable_irq_nosync(irq);

    if (p_dal_isr)
    {
        if (p_dal_isr->isr)
        {
            /* kernel mode interrupt handler */
            p_dal_isr->isr(p_dal_isr->isr_data);
        }
        else if ((NULL == p_dal_isr->isr) && (NULL == p_dal_isr->isr_data))
        {
            /* user mode interrupt handler */
           poll_intr_trigger[3] = 1;
           wake_up(&poll_intr[3]);
        }
    }

    return IRQ_HANDLED;
}

static irqreturn_t
intr4_handler(int irq, void* dev_id)
{
    dal_isr_t* p_dal_isr = (dal_isr_t*)dev_id;
    if(poll_intr_trigger[4])
    {
        return IRQ_HANDLED;
    }
    disable_irq_nosync(irq);

    if (p_dal_isr)
    {
        if (p_dal_isr->isr)
        {
            /* kernel mode interrupt handler */
            p_dal_isr->isr(p_dal_isr->isr_data);
        }
        else if ((NULL == p_dal_isr->isr) && (NULL == p_dal_isr->isr_data))
        {
            /* user mode interrupt handler */
           poll_intr_trigger[4] = 1;
           wake_up(&poll_intr[4]);
        }
    }

    return IRQ_HANDLED;
}

static irqreturn_t
intr5_handler(int irq, void* dev_id)
{
    dal_isr_t* p_dal_isr = (dal_isr_t*)dev_id;
    if(poll_intr_trigger[5])
    {
        return IRQ_HANDLED;
    }
    disable_irq_nosync(irq);

    if (p_dal_isr)
    {
        if (p_dal_isr->isr)
        {
            /* kernel mode interrupt handler */
            p_dal_isr->isr(p_dal_isr->isr_data);
        }
        else if ((NULL == p_dal_isr->isr) && (NULL == p_dal_isr->isr_data))
        {
            /* user mode interrupt handler */
           poll_intr_trigger[5] = 1;
           wake_up(&poll_intr[5]);
        }
    }

    return IRQ_HANDLED;
}

static irqreturn_t
intr6_handler(int irq, void* dev_id)
{
    dal_isr_t* p_dal_isr = (dal_isr_t*)dev_id;
    if(poll_intr_trigger[6])
    {
        return IRQ_HANDLED;
    }
    disable_irq_nosync(irq);

    if (p_dal_isr)
    {
        if (p_dal_isr->isr)
        {
            /* kernel mode interrupt handler */
            p_dal_isr->isr(p_dal_isr->isr_data);
        }
        else if ((NULL == p_dal_isr->isr) && (NULL == p_dal_isr->isr_data))
        {
            /* user mode interrupt handler */
           poll_intr_trigger[6] = 1;
           wake_up(&poll_intr[6]);
        }
    }

    return IRQ_HANDLED;
}

static irqreturn_t
intr7_handler(int irq, void* dev_id)
{
    dal_isr_t* p_dal_isr = (dal_isr_t*)dev_id;
    if(poll_intr_trigger[7])
    {
        return IRQ_HANDLED;
    }
    disable_irq_nosync(irq);

    if (p_dal_isr)
    {
        if (p_dal_isr->isr)
        {
            /* kernel mode interrupt handler */
            p_dal_isr->isr(p_dal_isr->isr_data);
        }
        else if ((NULL == p_dal_isr->isr) && (NULL == p_dal_isr->isr_data))
        {
            /* user mode interrupt handler */
           poll_intr_trigger[7] = 1;
           wake_up(&poll_intr[7]);
        }
    }

    return IRQ_HANDLED;
}

int
dal_interrupt_register(unsigned int irq, int prio, void (* isr)(void*), void* data)
{
    int ret;
    unsigned char str[16];
    unsigned char* int_name = NULL;
    unsigned int intr_num_tmp = 0;
    unsigned int intr_num = CTC_MAX_INTR_NUM;
    unsigned long irq_flags = 0;

    if (dal_intr_num >= CTC_MAX_INTR_NUM)
    {
        printk("Interrupt numbers exceeds max.\n");
        return -1;
    }

    if (msi_used)
    {
        int_name = "dal_msi";
    }
    else
    {
        int_name = "dal_intr";
    }


    for (intr_num_tmp=0;intr_num_tmp < CTC_MAX_INTR_NUM; intr_num_tmp++)
    {
        if (irq == dal_isr[intr_num_tmp].irq)
        {
            if (0 == msi_used)
            {
                dal_isr[intr_num_tmp].count++;
                printk("Interrupt irq %d register count %d.\n", irq, dal_isr[intr_num_tmp].count);
            }
            return 0;
        }
        if ((0 == dal_isr[intr_num_tmp].irq) && (CTC_MAX_INTR_NUM == intr_num))
        {
            intr_num = intr_num_tmp;
            dal_isr[intr_num].count = 0;
        }
    }
    dal_isr[intr_num].irq = irq;
    dal_isr[intr_num].isr = isr;
    dal_isr[intr_num].isr_data = data;
    dal_isr[intr_num].count++;

    init_waitqueue_head(&poll_intr[intr_num]);

    /* only user mode */
    if ((NULL == isr) && (NULL == data))
    {
        snprintf(str, 16, "%s%d", "dal_intr", intr_num);
        ret = register_chrdev(DAL_DEV_INTR_MAJOR_BASE + intr_num,
                              str, &dal_intr_fops[intr_num]);
        if (ret < 0)
        {
            printk("Register character device for irq %d failed, ret= %d", irq, ret);
            return ret;
        }
    }
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 1, 0))
    irq_flags = 0;
#else
    irq_flags = IRQF_DISABLED;
#endif
    if ((ret = request_irq(irq,
                           intr_handler_fun[intr_num],
                           irq_flags,
                           int_name,
                           &dal_isr[intr_num])) < 0)
    {
        printk("Cannot request irq %d, ret %d.\n", irq, ret);
        unregister_chrdev(DAL_DEV_INTR_MAJOR_BASE + intr_num, str);
    }

    if (0 == ret)
    {
        dal_intr_num++;
    }

    return ret;
}

int
dal_interrupt_unregister(unsigned int irq)
{
    unsigned char str[16];
    int intr_idx = 0;
    int find_flag = 0;

    /* get intr device index */
    for (intr_idx = 0; intr_idx < CTC_MAX_INTR_NUM; intr_idx++)
    {
        if (dal_isr[intr_idx].irq == irq)
        {
            find_flag = 1;
            break;
        }
    }

    if (find_flag == 0)
    {
        printk ("irq%d is not registered! unregister failed \n", irq);
        return -1;
    }

    dal_isr[intr_idx].count--;
    if (0 != dal_isr[intr_idx].count)
    {
        printk("Interrupt irq %d unregister count %d.\n", irq, dal_isr[intr_idx].count);
        return -1;
    }
    snprintf(str, 16, "%s%d", "dal_intr", intr_idx);

    unregister_chrdev(DAL_DEV_INTR_MAJOR_BASE + intr_idx, str);

    free_irq(irq, &dal_isr[intr_idx]);

    dal_isr[intr_idx].irq = 0;

    dal_intr_num--;

    return 0;
}

int
dal_interrupt_connect(unsigned int irq, int prio, void (* isr)(void*), void* data)
{
    int irq_idx = 0;

    for (irq_idx = 0; irq_idx < dal_intr_num; irq_idx++)
    {
        if (dal_isr[irq_idx].irq == irq)
        {
            dal_isr[irq_idx].isr_knet = isr;
            dal_isr[irq_idx].isr_knet_data = data;

            return 0;
        }
    }

    return -1;
}

int
dal_interrupt_disconnect(unsigned int irq)
{
    int irq_idx = 0;

    for (irq_idx = 0; irq_idx < dal_intr_num; irq_idx++)
    {
        if (dal_isr[irq_idx].irq == irq)
        {
            dal_isr[irq_idx].isr_knet = NULL;
            dal_isr[irq_idx].isr_knet_data = NULL;

            return 0;
        }
    }

    return -1;
}

static dal_ops_t g_dal_ops =
{
    interrupt_connect:dal_interrupt_connect,
    interrupt_disconnect:dal_interrupt_disconnect,
};

int
dal_get_dal_ops(dal_ops_t **dal_ops)
{
    *dal_ops = &g_dal_ops;

    return 0;
}

int
dal_interrupt_set_en(unsigned int irq, unsigned int enable)
{
    enable ? enable_irq(irq) : disable_irq_nosync(irq);
    return 0;
}

static int
_dal_set_msi_enabe(unsigned int lchip, unsigned int irq_num)
{
    int ret = 0;

#ifndef SOC_ACTIVE
    if (irq_num == 1)
    {
        ret = pci_enable_msi(dal_dev[lchip].pci_dev);
        if (ret)
        {
            printk ("msi enable failed!!! lchip = %d, irq_num = %d\n", lchip, irq_num);
            pci_disable_msi(dal_dev[lchip].pci_dev);
            msi_used = 0;
        }

        msi_irq_base[lchip] = dal_dev[lchip].pci_dev->irq;
        msi_irq_num[lchip] = 1;
    }
    else
    {
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 14, 79))
        ret = pci_enable_msi_exact(dal_dev[lchip].pci_dev, irq_num);
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 26, 32))
        ret = pci_enable_msi_block(dal_dev[lchip].pci_dev, irq_num);
#else
        ret = -1;
#endif
        if (ret)
        {
            printk ("msi enable failed!!! lchip = %d, irq_num = %d\n", lchip, irq_num);
            pci_disable_msi(dal_dev[lchip].pci_dev);
            msi_used = 0;
        }

        msi_irq_base[lchip] = dal_dev[lchip].pci_dev->irq;
        msi_irq_num[lchip] = irq_num;
    }
#endif

    return ret;
}

static int
_dal_set_msi_disable(unsigned int lchip)
{
#ifndef SOC_ACTIVE
    pci_disable_msi(dal_dev[lchip].pci_dev);

    msi_irq_base[lchip] = 0;
    msi_irq_num[lchip] = 0;
#endif
    return 0;
}

int
dal_set_msi_cap(unsigned long arg)
{
    int ret = 0;
#ifndef SOC_ACTIVE
    int index = 0;
    dal_msi_info_t msi_info;

    if (copy_from_user(&msi_info, (void*)arg, sizeof(dal_msi_info_t)))
    {
        return -EFAULT;
    }

    printk("####dal_set_msi_cap lchip %d base %d num:%d\n", msi_info.lchip, msi_info.irq_base, msi_info.irq_num);
    if (msi_info.irq_num > 0)
    {
        if (0 == msi_used)
        {
            msi_used = 1;
            ret = _dal_set_msi_enabe(msi_info.lchip, msi_info.irq_num);
        }
        else if ((1 == msi_used) && (msi_info.irq_num != msi_irq_num[msi_info.lchip]))
        {
            for (index = 0; index < msi_irq_num[msi_info.lchip]; index++)
            {
                dal_interrupt_unregister(msi_irq_base[msi_info.lchip]+index);
            }
            _dal_set_msi_disable(msi_info.lchip);
            msi_used = 1;
            ret = _dal_set_msi_enabe(msi_info.lchip, msi_info.irq_num);
        }
    }
    else
    {
        msi_used = 0;
        ret = _dal_set_msi_disable(msi_info.lchip);
    }
#endif

    return ret;
}

int
dal_user_interrupt_register(unsigned long arg)
{
    int irq = 0;
    if (copy_from_user(&irq, (void*)arg, sizeof(int)))
    {
        return -EFAULT;
    }
    printk("####register interrupt irq:%d\n", irq);
    return dal_interrupt_register(irq, 0, NULL, NULL);
}

int
dal_user_interrupt_unregister(unsigned long arg)
{
    int irq = 0;
    if (copy_from_user(&irq, (void*)arg, sizeof(int)))
    {
        return -EFAULT;
    }
    printk("####unregister interrupt irq:%d\n", irq);
    return dal_interrupt_unregister(irq);
}

int
dal_user_interrupt_set_en(unsigned long arg)
{
    dal_intr_parm_t dal_intr_parm;

    if (copy_from_user(&dal_intr_parm, (void*)arg, sizeof(dal_intr_parm_t)))
    {
        return -EFAULT;
    }

    return dal_interrupt_set_en(dal_intr_parm.irq, dal_intr_parm.enable);
}

/*
 * Function: _dal_dma_segment_free
 */

/*
 * Function: _find_largest_segment
 *
 * Purpose:
 *    Find largest contiguous segment from a pool of DMA blocks.
 * Parameters:
 *    dseg - DMA segment descriptor
 * Returns:
 *    0 on success, < 0 on error.
 * Notes:
 *    Assembly stops if a segment of the requested segment size
 *    has been obtained.
 *
 *    Lower address bits of the DMA blocks are used as follows:
 *       0: Untagged
 *       1: Discarded block
 *       2: Part of largest contiguous segment
 *       3: Part of current contiguous segment
 */
#ifndef DMA_MEM_MODE_PLATFORM
static int
_dal_find_largest_segment(dma_segment_t* dseg)
{
    int i, j, blks, found;
    unsigned long seg_begin;
    unsigned long seg_end;
    unsigned long seg_tmp;

    blks = dseg->blk_cnt;

    /* Clear all block tags */
    for (i = 0; i < blks; i++)
    {
        dseg->blk_ptr[i] &= ~3;
    }

    for (i = 0; i < blks && dseg->seg_size < dseg->req_size; i++)
    {
        /* First block must be an untagged block */
        if ((dseg->blk_ptr[i] & 3) == DAL_UNTAG_BLOCK)
        {
            /* Initial segment size is the block size */
            seg_begin = dseg->blk_ptr[i];
            seg_end = seg_begin + dseg->blk_size;
            dseg->blk_ptr[i] |= DAL_CUR_MATCH_BLOCk;

            /* Loop looking for adjacent blocks */
            do
            {
                found = 0;

                for (j = i + 1; j < blks && (seg_end - seg_begin) < dseg->req_size; j++)
                {
                    seg_tmp = dseg->blk_ptr[j];
                    /* Check untagged blocks only */
                    if ((seg_tmp & 3) == DAL_UNTAG_BLOCK)
                    {
                        if (seg_tmp == (seg_begin - dseg->blk_size))
                        {
                            /* Found adjacent block below current segment */
                            dseg->blk_ptr[j] |= DAL_CUR_MATCH_BLOCk;
                            seg_begin = seg_tmp;
                            found = 1;
                        }
                        else if (seg_tmp == seg_end)
                        {
                            /* Found adjacent block above current segment */
                            dseg->blk_ptr[j] |= DAL_CUR_MATCH_BLOCk;
                            seg_end += dseg->blk_size;
                            found = 1;
                        }
                    }
                }
            }
            while (found);

            if ((seg_end - seg_begin) > dseg->seg_size)
            {
                /* The current block is largest so far */
                dseg->seg_begin = seg_begin;
                dseg->seg_end = seg_end;
                dseg->seg_size = seg_end - seg_begin;

                /* Re-tag current and previous largest segment */
                for (j = 0; j < blks; j++)
                {
                    if ((dseg->blk_ptr[j] & 3) == DAL_CUR_MATCH_BLOCk)
                    {
                        /* Tag current segment as the largest */
                        dseg->blk_ptr[j] &= ~1;
                    }
                    else if ((dseg->blk_ptr[j] & 3) == DAL_MATCHED_BLOCK)
                    {
                        /* Discard previous largest segment */
                        dseg->blk_ptr[j] ^= 3;
                    }
                }
            }
            else
            {
                /* Discard all blocks in current segment */
                for (j = 0; j < blks; j++)
                {
                    if ((dseg->blk_ptr[j] & 3) == DAL_CUR_MATCH_BLOCk)
                    {
                        dseg->blk_ptr[j] &= ~2;
                    }
                }
            }
        }
    }

    return 0;
}

/*
 * Function: _alloc_dma_blocks
 */
static int
_dal_alloc_dma_blocks(dma_segment_t* dseg, int blks)
{
    int i, start;
    unsigned long addr;

    if (dseg->blk_cnt + blks > dseg->blk_cnt_max)
    {
        printk("No more DMA blocks\n");
        return -1;
    }

    start = dseg->blk_cnt;
    dseg->blk_cnt += blks;

    for (i = start; i < dseg->blk_cnt; i++)
    {
        addr = __get_free_pages(GFP_ATOMIC, dseg->blk_order);
        if (addr)
        {
            dseg->blk_ptr[i] = addr;
        }
        else
        {
            printk("DMA allocation failed\n");
            return -1;
        }
    }

    return 0;
}

/*
 * Function: _dal_dma_segment_alloc
 */
static dma_segment_t*
_dal_dma_segment_alloc(unsigned int size, unsigned int blk_size)
{
    dma_segment_t* dseg;
    int i, blk_ptr_size;
    unsigned long page_addr;
    struct sysinfo si;

    /* Sanity check */
    if (size == 0 || blk_size == 0)
    {
        return NULL;
    }

    /* Allocate an initialize DMA segment descriptor */
    if ((dseg = kmalloc(sizeof(dma_segment_t), GFP_ATOMIC)) == NULL)
    {
        return NULL;
    }

    memset(dseg, 0, sizeof(dma_segment_t));
    dseg->req_size = size;
    dseg->blk_size = PAGE_ALIGN(blk_size);

    while ((PAGE_SIZE << dseg->blk_order) < dseg->blk_size)
    {
        dseg->blk_order++;
    }

    si_meminfo(&si);
    dseg->blk_cnt_max = (si.totalram << PAGE_SHIFT) / dseg->blk_size;
    blk_ptr_size = dseg->blk_cnt_max * sizeof(unsigned long);
    /* Allocate an initialize DMA block pool */
    dseg->blk_ptr = kmalloc(blk_ptr_size, GFP_KERNEL);
    if (dseg->blk_ptr == NULL)
    {
        kfree(dseg);
        return NULL;
    }

    memset(dseg->blk_ptr, 0, blk_ptr_size);
    /* Allocate minimum number of blocks */
    _dal_alloc_dma_blocks(dseg, dseg->req_size / dseg->blk_size);

    /* Allocate more blocks until we have a complete segment */
    do
    {
        _dal_find_largest_segment(dseg);
        if (dseg->seg_size >= dseg->req_size)
        {
            break;
        }
    }
    while (_dal_alloc_dma_blocks(dseg, 8) == 0);

    /* Reserve all pages in the DMA segment and free unused blocks */
    for (i = 0; i < dseg->blk_cnt; i++)
    {
        if ((dseg->blk_ptr[i] & 3) == 2)
        {
            dseg->blk_ptr[i] &= ~3;

            for (page_addr = dseg->blk_ptr[i];
                 page_addr < dseg->blk_ptr[i] + dseg->blk_size;
                 page_addr += PAGE_SIZE)
            {
                MEM_MAP_RESERVE(VIRT_TO_PAGE((void*)page_addr));
            }
        }
        else if (dseg->blk_ptr[i])
        {
            dseg->blk_ptr[i] &= ~3;
            free_pages(dseg->blk_ptr[i], dseg->blk_order);
            dseg->blk_ptr[i] = 0;
        }
    }

    return dseg;
}

/*
 * Function: _dal_dma_segment_free
 */
static void
_dal_dma_segment_free(dma_segment_t* dseg)
{
    int i;
    unsigned long page_addr;

    if (dseg->blk_ptr)
    {
        for (i = 0; i < dseg->blk_cnt; i++)
        {
            if (dseg->blk_ptr[i])
            {
                for (page_addr = dseg->blk_ptr[i];
                     page_addr < dseg->blk_ptr[i] + dseg->blk_size;
                     page_addr += PAGE_SIZE)
                {
                    MEM_MAP_UNRESERVE(VIRT_TO_PAGE(page_addr));
                }

                free_pages(dseg->blk_ptr[i], dseg->blk_order);
            }
        }

        kfree(dseg->blk_ptr);
        kfree(dseg);
    }
}

/*
 * Function: -dal_pgalloc
 */
static void*
_dal_pgalloc(unsigned int size)
{
    dma_segment_t* dseg;
    unsigned int blk_size;

    blk_size = (size < DMA_BLOCK_SIZE) ? size : DMA_BLOCK_SIZE;
    if ((dseg = _dal_dma_segment_alloc(size, blk_size)) == NULL)
    {
        return NULL;
    }

    if (dseg->seg_size < size)
    {
        /* If we didn't get the full size then forget it */
        printk("Notice: Can not get enough memory for requset!!\n");
        printk("actual size:0x%lx, request size:0x%x\n", dseg->seg_size, size);
         /*-_dal_dma_segment_free(dseg);*/
         /*-return NULL;*/
    }

    list_add(&dseg->list, &_dma_seg);
    return (void*)dseg->seg_begin;
}

/*
 * Function: _dal_pgfree
 */
static int
_dal_pgfree(void* ptr)
{
    struct list_head* pos;

    list_for_each(pos, &_dma_seg)
    {
        dma_segment_t* dseg = list_entry(pos, dma_segment_t, list);
        if (ptr == (void*)dseg->seg_begin)
        {
            list_del(&dseg->list);
            _dal_dma_segment_free(dseg);
            return 0;
        }
    }
    return -1;
}
#endif
static void
dal_alloc_dma_pool(int lchip, int size)
{
    if (use_high_memory)
    {
        dma_phy_base[lchip] = virt_to_bus(high_memory);
        dma_virt_base[lchip] = ioremap_nocache(dma_phy_base[lchip], size);
    }
    else
    {
#if defined(DMA_MEM_MODE_PLATFORM) || defined(SOC_ACTIVE)
    dma_virt_base[lchip] = dma_alloc_coherent(&(dal_dev[lchip].pci_dev->dev), dma_mem_size,
                                                  &dma_phy_base[lchip], GFP_KERNEL);

    printk(KERN_WARNING "########Using DMA_MEM_MODE_PLATFORM \n");
#else
    /* Get DMA memory from kernel */
    dma_virt_base[lchip] = _dal_pgalloc(size);
    dma_phy_base[lchip] = virt_to_bus(dma_virt_base[lchip]);
    //dma_virt_base [lchip]= ioremap_nocache(dma_phy_base[lchip], size);
#endif
    }
}

static void
dal_free_dma_pool(int lchip)
{
    int ret = 0;
    ret = ret;
    if (use_high_memory)
    {
        iounmap(dma_virt_base[lchip]);
    }
    else
    {
#if defined(DMA_MEM_MODE_PLATFORM) || defined(SOC_ACTIVE)
    dma_free_coherent(&(dal_dev[lchip].pci_dev->dev), dma_mem_size,
                                                  dma_virt_base[lchip], dma_phy_base[lchip]);
#else
    iounmap(dma_virt_base[lchip]);
    ret = _dal_pgfree(dma_virt_base[lchip]);
    if(ret<0)
    {
        printk("Dma free memory fail !!!!!! \n");
    }
#endif
    }
}

#define _KERNEL_DAL_IO
static int
_dal_pci_read(unsigned char lchip, unsigned int offset, unsigned int* value)
{
    if (!VERIFY_CHIP_INDEX(lchip))
    {
        return -1;
    }

    *value = *(volatile unsigned int*)(dal_dev[lchip].logic_address + offset);
    return 0;
}

int
dal_create_irq_mapping(unsigned long arg)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 0, 0))

#ifndef NO_IRQ
#define NO_IRQ (-1)
#endif
    dal_irq_mapping_t irq_map;

    if (copy_from_user(&irq_map, (void*)arg, sizeof(dal_irq_mapping_t)))
    {
        return -EFAULT;
    }

    irq_map.sw_irq = irq_create_mapping(NULL, irq_map.hw_irq);
    if (irq_map.sw_irq == NO_IRQ)
    {
        printk("IRQ mapping fail !!!!!! \n");
        return -1;
    }

    if (copy_to_user((dal_irq_mapping_t*)arg, (void*)&irq_map, sizeof(dal_irq_mapping_t)))
    {
        return -EFAULT;
    }
#endif
    return 0;
}

int
dal_pci_read(unsigned long arg)
{
    dal_chip_parm_t cmdpara_chip;

    if (copy_from_user(&cmdpara_chip, (void*)arg, sizeof(dal_chip_parm_t)))
    {
        return -EFAULT;
    }

    _dal_pci_read((unsigned char)cmdpara_chip.lchip, (unsigned int)cmdpara_chip.reg_addr,
                                                       (unsigned int*)(&(cmdpara_chip.value)));

    if (copy_to_user((dal_chip_parm_t*)arg, (void*)&cmdpara_chip, sizeof(dal_chip_parm_t)))
    {
        return -EFAULT;
    }

    return 0;
}

static int
_dal_pci_write(unsigned char lchip, unsigned int offset, unsigned int value)
{
    if (!VERIFY_CHIP_INDEX(lchip))
    {
        return -1;
    }

    *(volatile unsigned int*)(dal_dev[lchip].logic_address + offset) = value;
    return 0;
}

int
dal_pci_write(unsigned long arg)
{
    dal_chip_parm_t cmdpara_chip;

    if (copy_from_user(&cmdpara_chip, (void*)arg, sizeof(dal_chip_parm_t)))
    {
        return -EFAULT;
    }

    _dal_pci_write((unsigned char)cmdpara_chip.lchip, (unsigned int)cmdpara_chip.reg_addr,
                                                         (unsigned int)cmdpara_chip.value);

    return 0;
}

int
dal_pci_conf_read(unsigned char lchip, unsigned int offset, unsigned int* value)
{
    if (!VERIFY_CHIP_INDEX(lchip))
    {
        return -1;
    }

#ifndef SOC_ACTIVE
    pci_read_config_dword(dal_dev[lchip].pci_dev, offset, value);
#endif

    return 0;
}

int
dal_pci_conf_write(unsigned char lchip, unsigned int offset, unsigned int value)
{
    if (!VERIFY_CHIP_INDEX(lchip))
    {
        return -1;
    }

#ifndef SOC_ACTIVE
    pci_write_config_dword(dal_dev[lchip].pci_dev, offset, value);
#endif

    return 0;
}
int
dal_user_read_pci_conf(unsigned long arg)
{
    dal_pci_cfg_ioctl_t dal_cfg;

    if (copy_from_user(&dal_cfg, (void*)arg, sizeof(dal_pci_cfg_ioctl_t)))
    {
        return -EFAULT;
    }

    if (dal_pci_conf_read(dal_cfg.lchip, dal_cfg.offset, &dal_cfg.value))
    {
        printk("dal_pci_conf_read failed.\n");
        return -EFAULT;
    }

    if (copy_to_user((dal_pci_cfg_ioctl_t*)arg, (void*)&dal_cfg, sizeof(dal_pci_cfg_ioctl_t)))
    {
        return -EFAULT;
    }

    return 0;
}

int
dal_user_write_pci_conf(unsigned long arg)
{
    dal_pci_cfg_ioctl_t dal_cfg;

    if (copy_from_user(&dal_cfg, (void*)arg, sizeof(dal_pci_cfg_ioctl_t)))
    {
        return -EFAULT;
    }

    return dal_pci_conf_write(dal_cfg.lchip, dal_cfg.offset, dal_cfg.value);
}

static int
linux_get_device(unsigned long arg)
{
    dal_user_dev_t user_dev;
    int lchip = 0;

    if (copy_from_user(&user_dev, (void*)arg, sizeof(user_dev)))
    {
        return -EFAULT;
    }

    user_dev.chip_num = dal_chip_num;
    lchip = user_dev.lchip;

    if (lchip < dal_chip_num)
    {
        user_dev.phy_base0 = (unsigned int)dal_dev[lchip].phys_address;
        user_dev.phy_base1 = (unsigned int)(dal_dev[lchip].phys_address >> 32);

#ifndef SOC_ACTIVE
        user_dev.bus_no = dal_dev[lchip].pci_dev->bus->number;
        user_dev.dev_no = dal_dev[lchip].pci_dev->device;
        user_dev.fun_no = dal_dev[lchip].pci_dev->devfn;
        user_dev.soc_active = 0;
#else
        user_dev.bus_no = 0;
        user_dev.dev_no = CTC_TSINGMA_DEVICE_ID;
        user_dev.fun_no = 0;
        user_dev.soc_active = 1;
#endif
    }

    if (copy_to_user((dal_user_dev_t*)arg, (void*)&user_dev, sizeof(user_dev)))
    {
        return -EFAULT;
    }

    return 0;
}

/* set dal version, copy to user */
static int
linux_get_dal_version(unsigned long arg)
{
    int dal_ver = VERSION_1DOT2;    /* set dal version */

    if (copy_to_user((int*)arg, (void*)&dal_ver, sizeof(dal_ver)))
    {
        return -EFAULT;
    }

    dal_version = dal_ver;         /* up sw */

    return 0;
}

static int
linux_get_dma_info(unsigned long arg)
{
    dal_dma_info_t dma_para;

    if (copy_from_user(&dma_para, (void*)arg, sizeof(dal_dma_info_t)))
    {
        return -EFAULT;
    }

    dma_para.phy_base = (unsigned int)dma_phy_base[dma_para.lchip];
    dma_para.phy_base_hi = dma_phy_base[dma_para.lchip] >> 32;
    dma_para.virt_base = dma_virt_base[dma_para.lchip];
    dma_para.size = dma_mem_size;

    printk("dal dma phy addr: 0x%llx, virt addr: %p.\n", dma_phy_base[dma_para.lchip], dma_virt_base[dma_para.lchip]);

    if (copy_to_user((dal_dma_info_t*)arg, (void*)&dma_para, sizeof(dal_dma_info_t)))
    {
        return -EFAULT;
    }

    return 0;
}

static int
dal_get_msi_info(unsigned long arg)
{
    dal_msi_info_t msi_para;
    unsigned int lchip = 0;

    /* get lchip form user mode */
    if (copy_from_user(&msi_para, (void*)arg, sizeof(dal_msi_info_t)))
    {
        return -EFAULT;
    }
    lchip = msi_para.lchip;

#ifndef SOC_ACTIVE
    msi_para.irq_base = msi_irq_base[lchip];
    msi_para.irq_num = msi_irq_num[lchip];
#else
    msi_para.irq_base = dal_int[lchip].irq;
    msi_para.irq_num = CTC_MAX_INTR_NUM;
#endif

    /* send msi info to user mode */
    if (copy_to_user((dal_msi_info_t*)arg, (void*)&msi_para, sizeof(dal_msi_info_t)))
    {
        return -EFAULT;
    }

    return 0;
}


static int
dal_get_intr_info(unsigned long arg)
{
    dal_intr_info_t intr_para;
    unsigned int intr_num = 0;

    /* get lchip form user mode */
    if (copy_from_user(&intr_para, (void*)arg, sizeof(dal_intr_info_t)))
    {
        return -EFAULT;
    }

    intr_para.irq_idx = CTC_MAX_INTR_NUM;
    for (intr_num=0; intr_num< CTC_MAX_INTR_NUM; intr_num++)
    {
        if (intr_para.irq == dal_isr[intr_num].irq)
        {
            intr_para.irq_idx = intr_num;
            break;
        }
    }

    if (CTC_MAX_INTR_NUM == intr_para.irq_idx)
    {
        printk("Interrupt %d cann't find.\n", intr_para.irq);
    }
    /* send msi info to user mode */
    if (copy_to_user((dal_intr_info_t*)arg, (void*)&intr_para, sizeof(dal_intr_info_t)))
    {
        return -EFAULT;
    }

    return 0;
}

int
dal_cache_inval(unsigned long ptr, unsigned int length)
{
#ifdef DMA_CACHE_COHERENCE_EN
    /*dma_cache_wback_inv((unsigned long)intr_para.ptr, intr_para.length);*/

    dma_sync_single_for_cpu(NULL, ptr, length, DMA_FROM_DEVICE);

    /*dma_cache_sync(NULL, (void*)bus_to_virt(intr_para.ptr), intr_para.length, DMA_FROM_DEVICE);*/
#endif
    return 0;
}

int
dal_cache_flush(unsigned long ptr, unsigned int length)
{
#ifdef DMA_CACHE_COHERENCE_EN
    /*dma_cache_wback_inv(intr_para.ptr, intr_para.length);*/

    dma_sync_single_for_device(NULL, ptr, length, DMA_TO_DEVICE);

    /*dma_cache_sync(NULL, (void*)bus_to_virt(intr_para.ptr), intr_para.length, DMA_TO_DEVICE);*/
#endif
    return 0;
}

static int
dal_user_cache_inval(unsigned long arg)
{
#ifndef DMA_MEM_MODE_PLATFORM
    dal_dma_cache_info_t intr_para;

    if (copy_from_user(&intr_para, (void*)arg, sizeof(dal_dma_cache_info_t)))
    {
        return -EFAULT;
    }

    dal_cache_inval(intr_para.ptr, intr_para.length);
#endif
    return 0;
}

static int
dal_user_cache_flush(unsigned long arg)
{
#ifndef DMA_MEM_MODE_PLATFORM
    dal_dma_cache_info_t intr_para;

    if (copy_from_user(&intr_para, (void*)arg, sizeof(dal_dma_cache_info_t)))
    {
        return -EFAULT;
    }

    dal_cache_flush(intr_para.ptr, intr_para.length);
#endif
    return 0;
}

#ifndef SOC_ACTIVE
int linux_dal_probe(struct pci_dev* pdev, const struct pci_device_id* id)
#else
static int linux_dal_probe(struct platform_device *pdev)
#endif
{
    dal_kern_dev_t* dev = NULL;
    unsigned int temp = 0;
    unsigned int lchip = 0;
#ifndef SOC_ACTIVE
    int bar = 0;
    int ret = 0;
    /*unsigned int devid = 0;*/
#else
    int i = 0;
    int irq = 0;
    struct resource * res = NULL;
#endif

    printk(KERN_WARNING "********found dal device*****\n");

    for (lchip = 0; lchip < DAL_MAX_CHIP_NUM; lchip ++)
    {
        if (NULL == dal_dev[lchip].pci_dev)
        {
            break;
        }
    }

    if (lchip >= DAL_MAX_CHIP_NUM)
    {
        printk("Exceed max local chip num\n");
        return -1;
    }

    dev = &dal_dev[lchip];
    if (NULL == dev)
    {
        printk("Cannot obtain PCI resources\n");
    }

    lchip = lchip;
    dal_chip_num += 1;

    dev->pci_dev = pdev;

#ifndef SOC_ACTIVE
    if (pdev->device == 0x5236)
    {
        printk("use bar2 to config memory space\n");
        bar = 2;
    }

    if (pci_enable_device(pdev) < 0)
    {
        printk("Cannot enable PCI device: vendor id = %x, device id = %x\n",
               pdev->vendor, pdev->device);
    }

    ret = pci_set_dma_mask(pdev, DMA_BIT_MASK(64));
    if (ret)
    {
        ret = pci_set_dma_mask(pdev, DMA_BIT_MASK(32));
        if (ret)
        {
            printk("Could not set PCI DMA Mask\n");
            return ret;
        }
    }

    if (pci_request_regions(pdev, DAL_NAME) < 0)
    {
        printk("Cannot obtain PCI resources\n");
    }

    dev->phys_address = pci_resource_start(pdev, bar);
    dev->logic_address = (uintptr)ioremap_nocache(dev->phys_address,
                                                pci_resource_len(dev->pci_dev, bar));
#else
    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    dev->phys_address = res->start;
    dev->logic_address = devm_ioremap_resource(&pdev->dev, res);
    if (IS_ERR(dev->logic_address))
    {
        return PTR_ERR(dev->logic_address);
    }

    for (i = 0; i < CTC_MAX_INTR_NUM; i++)
    {
        irq = platform_get_irq(pdev, i);
        if (irq < 0)
        {
            printk( "can't get irq number\n");
            return irq;
        }
        dal_int[i].irq = irq;
        printk( "irq %d vector %d\n", i, irq);
    }
#endif
    _dal_pci_read(lchip, 0x48, &temp);
    if (((temp >> 8) & 0xffff) == 0x3412)
    {
        printk("Little endian Cpu detected!!! \n");
        _dal_pci_write(lchip, 0x48, 0xFFFFFFFF);
    }

#ifndef SOC_ACTIVE
    pci_set_master(pdev);
#endif

    /* alloc dma_mem_size for every chip */
    if (dma_mem_size)
    {
        dal_alloc_dma_pool(lchip,  dma_mem_size);

        /*add check Dma memory pool cannot cross 4G space*/
        if ((0==(dma_phy_base[lchip]>>32)) && (0!=((dma_phy_base[lchip]+dma_mem_size)>>32)))
        {
            printk("Dma malloc memory cross 4G space!!!!!! \n");
            return -1;
        }
    }

    printk(KERN_WARNING "linux_dal_probe end*****\n");

    return 0;
}

#ifndef SOC_ACTIVE
void
linux_dal_remove(struct pci_dev* pdev)
#else
static int
linux_dal_remove(struct platform_device *pdev)
#endif
{
    unsigned int lchip = 0;
    unsigned int flag = 0;

    for (lchip = 0; lchip < DAL_MAX_CHIP_NUM; lchip ++)
    {
        if (pdev == dal_dev[lchip].pci_dev)
        {
            flag = 1;
            break;
        }
    }

    if (1 == flag)
    {
        dal_free_dma_pool(lchip);
#ifndef SOC_ACTIVE
        pci_release_regions(pdev);
        pci_disable_device(pdev);
#endif
        dal_dev[lchip].pci_dev = NULL;
        dal_chip_num--;
    }

#ifdef SOC_ACTIVE
    return 0;
#endif

}

#ifdef CONFIG_COMPAT
static long
linux_dal_ioctl(struct file* file,
                unsigned int cmd, unsigned long arg)
#else

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 36))
static long
linux_dal_ioctl(struct file* file,
                unsigned int cmd, unsigned long arg)
#else
static int
linux_dal_ioctl(struct inode* inode, struct file* file,
                unsigned int cmd, unsigned long arg)
#endif

#endif
{
    switch (cmd)
    {

    case CMD_READ_CHIP:
        return dal_pci_read(arg);

    case CMD_WRITE_CHIP:
        return dal_pci_write(arg);

    case CMD_GET_DEVICES:
        return linux_get_device(arg);

    case CMD_GET_DAL_VERSION:
        return linux_get_dal_version(arg);

    case CMD_GET_DMA_INFO:
        return linux_get_dma_info(arg);

    case CMD_PCI_CONFIG_READ:
        return dal_user_read_pci_conf(arg);

    case CMD_PCI_CONFIG_WRITE:
        return dal_user_write_pci_conf(arg);

    case CMD_REG_INTERRUPTS:
        return dal_user_interrupt_register(arg);

    case CMD_UNREG_INTERRUPTS:
        return dal_user_interrupt_unregister(arg);

    case CMD_EN_INTERRUPTS:
        return dal_user_interrupt_set_en(arg);

    case CMD_SET_MSI_CAP:
        return dal_set_msi_cap(arg);

    case CMD_GET_MSI_INFO:
        return dal_get_msi_info(arg);

    case CMD_IRQ_MAPPING:
        return dal_create_irq_mapping(arg);

    case CMD_GET_INTR_INFO:
        return dal_get_intr_info(arg);

    case CMD_CACHE_INVAL:
        return dal_user_cache_inval(arg);

    case CMD_CACHE_FLUSH:
        return dal_user_cache_flush(arg);

    default:
        break;
    }

    return 0;
}

static unsigned int
linux_dal_poll0(struct file* filp, struct poll_table_struct* p)
{
    unsigned int mask = 0;
    unsigned long flags;

    poll_wait(filp, &poll_intr[0], p);
    local_irq_save(flags);
    if (poll_intr_trigger[0])
    {
        poll_intr_trigger[0] = 0;
        mask |= POLLIN | POLLRDNORM;
    }

    local_irq_restore(flags);

    return mask;
}

static unsigned int
linux_dal_poll1(struct file* filp, struct poll_table_struct* p)
{
    unsigned int mask = 0;
    unsigned long flags;

    poll_wait(filp, &poll_intr[1], p);
    local_irq_save(flags);
    if (poll_intr_trigger[1])
    {
        poll_intr_trigger[1] = 0;
        mask |= POLLIN | POLLRDNORM;
    }

    local_irq_restore(flags);

    return mask;
}

static unsigned int
linux_dal_poll2(struct file* filp, struct poll_table_struct* p)
{
    unsigned int mask = 0;
    unsigned long flags;

    poll_wait(filp, &poll_intr[2], p);
    local_irq_save(flags);
    if (poll_intr_trigger[2])
    {
        poll_intr_trigger[2] = 0;
        mask |= POLLIN | POLLRDNORM;
    }

    local_irq_restore(flags);

    return mask;
}

static unsigned int
linux_dal_poll3(struct file* filp, struct poll_table_struct* p)
{
    unsigned int mask = 0;
    unsigned long flags;

    poll_wait(filp, &poll_intr[3], p);
    local_irq_save(flags);
    if (poll_intr_trigger[3])
    {
        poll_intr_trigger[3] = 0;
        mask |= POLLIN | POLLRDNORM;
    }

    local_irq_restore(flags);

    return mask;
}

static unsigned int
linux_dal_poll4(struct file* filp, struct poll_table_struct* p)
{
    unsigned int mask = 0;
    unsigned long flags;

    poll_wait(filp, &poll_intr[4], p);
    local_irq_save(flags);
    if (poll_intr_trigger[4])
    {
        poll_intr_trigger[4] = 0;
        mask |= POLLIN | POLLRDNORM;
    }

    local_irq_restore(flags);

    return mask;
}

static unsigned int
linux_dal_poll5(struct file* filp, struct poll_table_struct* p)
{
    unsigned int mask = 0;
    unsigned long flags;

    poll_wait(filp, &poll_intr[5], p);
    local_irq_save(flags);
    if (poll_intr_trigger[5])
    {
        poll_intr_trigger[5] = 0;
        mask |= POLLIN | POLLRDNORM;
    }

    local_irq_restore(flags);

    return mask;
}

static unsigned int
linux_dal_poll6(struct file* filp, struct poll_table_struct* p)
{
    unsigned int mask = 0;
    unsigned long flags;

    poll_wait(filp, &poll_intr[6], p);
    local_irq_save(flags);
    if (poll_intr_trigger[6])
    {
        poll_intr_trigger[6] = 0;
        mask |= POLLIN | POLLRDNORM;
    }

    local_irq_restore(flags);

    return mask;
}

static unsigned int
linux_dal_poll7(struct file* filp, struct poll_table_struct* p)
{
    unsigned int mask = 0;
    unsigned long flags;

    poll_wait(filp, &poll_intr[7], p);
    local_irq_save(flags);
    if (poll_intr_trigger[7])
    {
        poll_intr_trigger[7] = 0;
        mask |= POLLIN | POLLRDNORM;
    }

    local_irq_restore(flags);

    return mask;
}

#ifndef SOC_ACTIVE
static struct pci_driver linux_dal_driver =
{
    .name = DAL_NAME,
    .id_table = dal_id_table,
    .probe = linux_dal_probe,
    .remove = linux_dal_remove,
};
#else
static struct platform_driver linux_dal_driver =
{
    .probe = linux_dal_probe,
    .remove = linux_dal_remove,
    .driver = {
        .name = DAL_NAME,
        .of_match_table = of_match_ptr(linux_dal_of_match),
    },
};
#endif

static struct file_operations fops =
{
    .owner = THIS_MODULE,
#ifdef CONFIG_COMPAT
    .compat_ioctl = linux_dal_ioctl,
    .unlocked_ioctl = linux_dal_ioctl,
#else
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 36))
    .unlocked_ioctl = linux_dal_ioctl,
#else
    .ioctl = linux_dal_ioctl,
#endif
#endif
};

static int __init
linux_dal_init(void)
{
    int ret = 0;

    /* Get DMA memory pool size form dal.ok input param, or use default dma_mem_size */
    if (dma_pool_size)
    {
        if ((dma_pool_size[strlen(dma_pool_size) - 1] & ~0x20) == 'M')
        {
            dma_mem_size = simple_strtoul(dma_pool_size, NULL, 0);
            printk("dma_mem_size: 0x%x \n", dma_mem_size);

            dma_mem_size *= MB_SIZE;
        }
        else
        {
            printk("DMA memory pool size must be specified as e.g. dma_pool_size=8M\n");
        }

        if (dma_mem_size & (dma_mem_size - 1))
        {
            printk("dma_mem_size must be a power of 2 (1M, 2M, 4M, 8M etc.)\n");
            dma_mem_size = 0;
        }
    }

    ret = register_chrdev(DAL_DEV_MAJOR, DAL_NAME, &fops);
    if (ret < 0)
    {
        printk(KERN_WARNING "Register linux_dal device, ret %d\n", ret);
        return ret;
    }

#ifndef SOC_ACTIVE
    ret = pci_register_driver(&linux_dal_driver);
    if (ret < 0)
    {
        printk(KERN_WARNING "Register ASIC PCI driver failed, ret %d\n", ret);
        return ret;
    }
#else
    ret = platform_driver_register(&linux_dal_driver);
    if (ret < 0)
    {
        printk(KERN_WARNING "Register ASIC LOCALBUS driver failed, ret %d\n", ret);
    }
#endif

    /* alloc /dev/linux_dal node */
    dal_class = class_create(THIS_MODULE, DAL_NAME);
    device_create(dal_class, NULL, MKDEV(DAL_DEV_MAJOR, 0), NULL, DAL_NAME);

    /* init interrupt function */
    intr_handler_fun[0] = intr0_handler;
    intr_handler_fun[1] = intr1_handler;
    intr_handler_fun[2] = intr2_handler;
    intr_handler_fun[3] = intr3_handler;
    intr_handler_fun[4] = intr4_handler;
    intr_handler_fun[5] = intr5_handler;
    intr_handler_fun[6] = intr6_handler;
    intr_handler_fun[7] = intr7_handler;

    return ret;
}

static void __exit
linux_dal_exit(void)
{
    device_destroy(dal_class, MKDEV(DAL_DEV_MAJOR, 0));
    class_destroy(dal_class);
    unregister_chrdev(DAL_DEV_MAJOR, "linux_dal");
#ifndef SOC_ACTIVE
    pci_unregister_driver(&linux_dal_driver);
#else
    platform_driver_unregister(&linux_dal_driver);
#endif
}

module_init(linux_dal_init);
module_exit(linux_dal_exit);
EXPORT_SYMBOL(dal_get_dal_ops);
EXPORT_SYMBOL(dal_cache_inval);
EXPORT_SYMBOL(dal_cache_flush);

