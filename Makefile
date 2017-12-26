obj-m += part2.o
obj-m += part1.o
kdir=/lib/modules/$(shell uname -r)/build
all: test
	make -C ${kdir} M=$$PWD
test: test.o
	gcc test.o -o test
test.o: test.c
	gcc -c test.c
clean:
	$(MAKE) -C $(kdir) M=$(PWD) clean
	rm -f test
	rm -f *.o
