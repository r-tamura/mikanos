.PHONY: run
run:
	./build.sh run

.PHONY: debug
debug:
	QEMU_OPTS='-gdb tcp::12345 -S' ./build.sh run

.PHONY: clean
clean:
	rm -f kernel/*.o
	rm -f kernel/kernel.elf