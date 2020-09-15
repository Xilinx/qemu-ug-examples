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

PATH?=/usr/bin
CROSS_PREFIX?=$(PATH)/aarch64-none-elf-

all: hello_world.elf

hello_world.o: hello_world.c
	$(CROSS_PREFIX)gcc -Wall -O0 -g3 -c -fmessage-length=0 -mcpu=cortex-a72 -Iinclude -MMD -MP -MF"helloworld.d" -MT$@ -o $@ $<

platform.o: platform.c
	$(CROSS_PREFIX)gcc -Wall -O0 -g3 -c -fmessage-length=0 -mcpu=cortex-a72 -Iinclude -MMD -MP -MF"platform.d" -MT$@ -o $@ $<

hello_world.elf: hello_world.o platform.o
	$(CROSS_PREFIX)gcc -mcpu=cortex-a72 -Wl,-T -Wl,lscript.ld -Llib -o $@ $^   -Wl,--start-group,-lxil,-lgcc,-lc,--end-group

hello_world.bin: hello_world.elf
	$(CROSS_PREFIX)objcopy -O binary $< $@

clean:
	rm -f hello_world.bin hello_world.elf platform.o hello_world.o

