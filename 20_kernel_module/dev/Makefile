obj-m += dev_module.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

# all:
# 	make -C /lib/modules/6.8.0-83-generic/build/ M=$(PWD) modules

# clean:
# 	make -C/lib/modules/6.8.0-83-generic/build/ M=$(PWD) clean