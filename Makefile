.PHONY: debug
debug:
	QEMU_OPTS='-gdb tcp::12345 -S' ./build.sh run
