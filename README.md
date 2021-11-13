# mikanos

MikanOS: An Educational Operating System

## Files

- MikanLoaderPkg
  - The MikanOS loader as a UEFI Application

- kernel
  - The MikanOS kernel

- resource/nihongo.ttf
  - IPA gothic font file
  
- IPA_Font_License_Agreement_v1.0.txt
  - License agreement for IPA fonts

## Build

- Build the kernel

```shell
# Add the include path to be required to build
source ~/osbook/devenv/buildenv.sh
cd mikanos/kernel
make
```

- Build the Bootloader

```shell
cd ~/edk2
source edksetup.sh
build
```

- Run on QEMU

```shell
make run
```

- Run QEMU with debug mode enabled

```shell
make debug
```
Then, run debugger '(gdb) Attach to QEMU' on VSCode.