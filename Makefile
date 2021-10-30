.PHONY: run
run:
	./build.sh run

.PHONY: debug
debug:
	QEMU_OPTS='-gdb tcp::12345 -S' ./build.sh run
