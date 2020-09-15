 #
 # Copyright 2020, Xilinx Inc
 #
 # Permission is hereby granted, free of charge, to any person obtaining a copy
 # of this software and associated documentation files (the "Software"), to deal
 # in the Software without restriction, including without limitation the rights
 # to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 # copies of the Software, and to permit persons to whom the Software is
 # furnished to do so, subject to the following conditions:
 #    * The above copyright notice and this permission notice shall be included
 #      in all copies or substantial portions of the Software.
 #
 # THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 # IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 # FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 # AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 # LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 # OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 # SOFTWARE.

if [[ ! $# -eq 2 ]] ; then
    echo 'Please provide qemu-system-aarch64 path and zcu102-arm.dtb path.'
    exit 1
fi

if [ ! -f $1/qemu-system-aarch64 ]; then
    echo "qemu executable not fount at location: $1/"
    exit 1
fi

if [ ! -f $2/zcu102-arm.dtb ]; then
    echo "zcu102 device tree binary not found at: $2/"
    exit 1
fi

$1/qemu-system-aarch64 \
-M arm-generic-fdt \
-serial mon:stdio \
-device loader,file=freertos_a53_hello_world.elf,cpu-num=0 \
-device loader,addr=0xfd1a0104,data=0x8000000e,data-len=4 \
-hw-dtb $2/zcu102-arm.dtb \
 -display none
