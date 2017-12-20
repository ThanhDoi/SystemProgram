obj-m   := printerDriver.o

KDIR    := /lib/modules/$(shell uname -r)/build
PWD     := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
	$(CC) -o testPrinterDriver testPrinterDriver.c

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
