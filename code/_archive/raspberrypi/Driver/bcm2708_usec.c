#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/io.h>
 
MODULE_LICENSE("GPL");
 
/*
 * Major 240 is the base of the "experimental" devices.
 * If this driver enters the kernel tree, these should change.
 */
#define MAJORDEV 240
#define MINORDEV 0
 
/* prototype for the core function */
ssize_t timer_read(struct file *, char *, size_t, loff_t *);
 
/* other local functions */
static int bcm2708_usec_init(void);
static void bcm2708_usec_cleanup(void);
static int do_bcm2708_usec_init(void);
static void do_bcm2708_usec_deinit(void);
 
#define TIMER_PAGE_BASE 0x20003000
#define TIMER_OFFSET 4
 
/* some driver-local storage */
static u8 *timer_pf;        /* the page frame containing the timer */
static u8 *timer;       /* byte ptr to the timer itself */
 
/* NOTE: up to 8 bytes of timer available, enforced in code */
 
static struct cdev cdev;
 
static struct file_operations usec_fops = {
    .owner = THIS_MODULE,
    .read = timer_read,
    .write = NULL,
};
 
/**
 * bcm2708_usec_init - initialize this module's private info
 *
 * Description:
 *   Registers the necessary kernel structures, and
 *   maps the bcm2708's monotonic 1MHz timer into a
 *   kernel page.
 */
 
static int __init bcm2708_usec_init(void)
{
    dev_t devnum = MKDEV(MAJORDEV, MINORDEV);
    int err;
 
    /* register the character major/minor */
    err = register_chrdev_region(devnum, 1, "bcm2708_usec");
    if (err)
        goto bail;
 
    /* set up the chardev control structure */
    cdev_init(&cdev, &usec_fops);
    <span class="skimlinks-unlinked">cdev.owner</span> = THIS_MODULE;
    <span class="skimlinks-unlinked">cdev.ops</span> = &usec_fops;
    err = cdev_add(&cdev, devnum, 1);
    if (err)
        goto bail;
 
    /* get the mapping for the page frame containing the timer */
    timer_pf = ioremap(TIMER_PAGE_BASE, SZ_4K);
    /* and set the pointer to the timer itself */
    timer = timer_pf + TIMER_OFFSET;
    pr_info("bcm2708_usec initialized; timer @ %pK\n", timer);
    return 0;
 
bail:
    /* undo the developer's brain damage (hopefully) */
    bcm2708_usec_cleanup();
    pr_err("bcm2708_usec failed to initialize, err = %d\n", err);
    return -ENODEV;
}
 
/**
 * bcm2708_usec_cleanup - release module's allocated resources
 *
 * Description:
 *   Unmaps the module's global page containing the pointer,
 *   then unregisters the driver's kernel structures.
 */
static void __exit bcm2708_usec_cleanup(void)
{
    dev_t devnum = MKDEV(MAJORDEV, MINORDEV);
 
    /* if the timer was mapped (final step of successful module init) */
    if (timer_pf)
        /* release the mapping */
        iounmap(timer_pf);
    /* and release the device major/minor allocation */
    unregister_chrdev_region(devnum, 1);
}
 
/**
 * timer_read - fetch bytes from the BCM2708's 1MHz timer
 * @flip:       The file pointer (guaranteed to be correct after open())
 * @buff:       The buffer receiving the lowest N bytes of the 1MHz timer
 * @count:      How many bytes to transfer (maximum 8 bytes)
 * @offset:     Ignored; every read transfers only the lowest N bytes
 *
 * Description:
 *   This is the core method of the source code here presented. A user program
 *   requests N bytes (N<=8) from the BCM2708's timer, and this routine copies
 *   them to the supplied user-space pointer. Positioning is ignored. All calls
 *   to this routine will copy the lowest N bytes, guaranteeing 1MHz granularity.
 *   There is no syncrhonization within this scope. Functions called from here
 *   may do their own synchronizations. Otherwise, a simple bounds check is the
 *   only necessary step before calling copy_to_user.
 *
 * Returns:
 *   The number of bytes copied.
 */
 
ssize_t timer_read(struct file *filp, char __user * buff, size_t count,
           loff_t * offset)
{
    u64 cur_timer;  /* thread-local storage for the current timer */
 
    /* The null case, return early. Unlikely, but we won't argue. */
    if (count < 1)       return 0;   /* grab the timer (in a single ARM operation, hopefully) */     cur_timer = *timer;     /* Transfer maximum of 8 bytes. Note that copy_to_user returns the       * number of bytes NOT copied, so we need to invert that result.     */     return count - copy_to_user(buff, timer, count > 8 ? 8 : count);
 
    /*
     * Yes, that's how brain-dead this operation is. Just copying bytes
     * into a user buffer, and returning the count of bytes copied.
     */
}
 
/*
 * The following two functions may seem redundant, but this seems to be a
 * Linux kernel code idiom. The users have expectations about how the code
 * behaves; the devs have expectations about how the code looks.
 */
 
static int __init do_bcm2708_usec_init(void)
{
    return bcm2708_usec_init();
}
 
static void __exit do_bcm2708_usec_deinit(void)
{
    pr_info("Unloading bcm2708_usec module.\n");
    /*
     * This routine is the "sane" case, for unloading a successfully-loaded
     * driver. Others may call bcm2708_usec_cleanup() for their own,
     * pathological reasons.
     */
    bcm2708_usec_cleanup();
}
 
/* End of the code road. Hope you enjoyed the journey. */
 
module_init(do_bcm2708_usec_init);
module_exit(do_bcm2708_usec_deinit);