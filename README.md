# QEMU-docs

This repository contains basic QEMU usages examples.
To run test.sh please provide QEMU executable directory path and device tree binary path as argument.

Example: ./test.sh /home/qemu_xilinx/build/aarch64-softmmu /home/dts_xilinx/LATEST/SINGLE_ARCH

#Device trees:
When user compiles Xilinx Device trees. It will create two folders under LATEST directory named MULTI_ARCH and SINGLE_ARCH.
 * SINGLE_ARCH contains device tree binaries suitable for running ARM architecture only.
 * MULTI_ARCH containts device tree binaries for Arm arch and MicroBlaze Arch.
