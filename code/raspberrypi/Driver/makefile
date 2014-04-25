# remove any special parameters to Make
MAKE = make
 
ifneq ($(KERNELRELEASE),)
    obj-m := bcm2708_usec.o
else
    KERNELDIR ?= /lib/modules/$(shell uname -r)/build
    PWD := $(shell pwd)
 
default:
    $(MAKE) -C $(KERNELDIR) M=$(PWD) modules
 
endif
 
clean:
    rm -fv *.ko *.o *.mod.? Module.symvers <span class="skimlinks-unlinked">modules.order</span>