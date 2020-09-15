/*
 * Copyright 2020, Xilinx Inc
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *    * The above copyright notice and this permission notice shall be included
 *      in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define UART0_BASE              0xFF000000

#define UART_OFFSET_SR          0x002C
#define UART_OFFSET_FIFO        0x0030

#define UART_MASK_SR_TXFULL     0x00000010

#define MENU_STR \
    "\n***************UART TX MENU***************\n" \
    "g: Generate new data to transmit\n" \
    "t: Transmit the UART data\n" \
    "<RETURN>: Exit\n"

static inline uint32_t REG_R32(const uint32_t *reg)
{
    return *(volatile uint32_t *)reg;
}

static inline void REG_W32(uint32_t *reg, uint32_t val)
{
    volatile uint32_t *p;
    p = (volatile uint32_t *)reg;
    *p = val;
}

static void uart_tx_byte(uint32_t *uart, char byte)
{
    // Spin until TX isn't full
    while (REG_R32((const uint32_t *)(uart + UART_OFFSET_SR)) &
           UART_MASK_SR_TXFULL);
            
    printf("TX->%.2x\n", byte);
    REG_W32((uint32_t *)(uart + UART_OFFSET_FIFO), byte);
}

static void uart_tx(uint32_t *uart, const char *data, size_t size)
{
    size_t i;

    for (i=0; i<size; ++i) {
        uart_tx_byte(uart, data[i]);
    }
}

static void *map_peripheral(size_t base_addr)
{
    void *addr;
    int fd;

    fd = open("/dev/mem", O_RDWR);
    addr = mmap(NULL, 0x1000, PROT_READ | PROT_WRITE,
                MAP_SHARED, fd, base_addr);
    close(fd);

    if (addr == MAP_FAILED) {
        printf("mmap failed: %s\n", strerror(errno));
        addr = NULL;
    }

    return addr;
}

static void prog_loop(void *uart)
{
    size_t i;
    char buf[16];
    char cmd[16];
    bool done;
    
    while (!done) {
        fgets(cmd, sizeof(cmd), stdin);

        switch(cmd[0]) {
            case 'g':
                printf("Gen:");
                for (i=0; i<sizeof(buf); ++i) {
                    buf[i] = rand() & 0xFF;
                    printf(" %.2x", buf[i]);
                }
                puts("");
            case 't':
                uart_tx(uart, buf, sizeof(buf));
                break;
            case '\n':
                done = true;
                break;
            default:
                printf("Unknown command %s", cmd);
                break;
        }
    }
}

int main(void)
{
    void *addr;

    puts(MENU_STR);
    
    addr = map_peripheral(UART0_BASE);
    if (!addr) {
        return 1;
    }

    srand(time(NULL));
    prog_loop(addr);

    return 0;
}
