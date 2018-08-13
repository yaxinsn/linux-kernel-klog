
# Comment/uncomment the following line to enable/disable debugging
#DEBUG = y


ifeq ($(DEBUG),y)
  DEBFLAGS = -O -g -DSCULLP_DEBUG # "-O" is needed to expand inlines
else
  DEBFLAGS = -O2
endif

		
main-OBJECTS = klog.o

#CFLAGS += $(DEBFLAGS) -I$(LDDINC) -I"../misc/"
VPATH = .:../misc

# $(xscanMatchine_OBJECTS)

total-OBJECTS = $(main-OBJECTS)  
			
## EXT_HEADER=-I/usr/src/linux-headers-2.6.32-5-common-linx/include
EXTRA_CFLAGS += -I$(INC) -I$(SELF_PATH) $(EXT_HEADER)

TARGET = Klog

ifneq ($(KERNELRELEASE),)

Klogs-objs := $(total-OBJECTS)
obj-m	:= Klogs.o
# obj-m 制定最后模块的名字？
else

#添加相应的头文件目录
PRIVATE_INC = $(INC) 
__SELF = $(shell pwd)/
KERNELDIR ?= /lib/modules/$(shell uname -r)/build
#KERNELDIR ?= /lib/modules/3.7.10-1.32-desktop/build
PWD       := $(shell pwd)

EXTRA_CFLAGS += -I$(INC) -I$(SELF_PATH) #-I/usr/src/linux-headers-2.6.32-5-common-linx/include
modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) INC=$(PRIVATE_INC) SELF_PATH=$(__SELF) modules


# M=$(PWD) : 指定模块编译之后的路径,同时也指定了内核的MAKEFILE第二次再次调用本MAKEFILE;
# INC=$(PRIVATE_INC) ：通知内核的MAKEFILE定义一个INC变量，初始值为$(PRIVATE_INC)，
# PRIVATE_INC不能够直接传给内核MAKEFILE给第二次经过这个文件的时候用；
#

endif


install:
	install -d $(INSTALLDIR)
	install -c $(TARGET).o $(INSTALLDIR)

clean:
	rm -rf *.o $(total-OBJECTS) *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions  modules.order  Module.symvers


depend .depend dep:
	$(CC) $(CFLAGS) -M *.c > .depend

ifeq (.depend,$(wildcard .depend))
include .depend
endif
