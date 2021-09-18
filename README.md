# mikanos

MikanOS: An Educational Operating System

## Files

- MikanLoaderPkg

  - The MikanOS loader as a UEFI Application

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
~/osbook/devenv/run_qemu.sh ~/edk2/Build/MikanLoaderX64/DEBUG_CLANG38/X64/Loader.efi ~/workspace/mikanos/kernel/kernel.elf
```