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

/*
 * new_model.c: simple test application
 *
 * This application tests the newly added XOR-TEST module.
 * XOR-TEST module got two registers.
 * ------------------------------------------------
 * | Register    Offset                           |
 * ------------------------------------------------
 *   Xdata      0x0
 *   Matcher    0x4
 */
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

#define XOR_TEST_ADDR           0xA0001000
#define REG_XDATA_OFFSET        0x0
#define REG_MATCHER_OFFSET      0x4

/*
 * Reads a 32 bit value out of a 32 bit memory mapped register
 */
uint32_t readReg(uintptr_t addr){
    return *(volatile uint32_t*)addr;
}

/*
 * Write a 32 bit value in to a 32 bit memory mapped register
 */
void writeReg(uintptr_t addr, uint32_t val){
    volatile uint32_t* tmp = (volatile uint32_t*)addr;
    *tmp = val;
}

/* UART definitions. */
#define PSU_UART0_ADDR              0xFF000000
#define PSU_UART0_IDR               0x000CU
#define PSU_UART0_CR                0x0000U
#define PSU_UART0_MR                0x0004U         // Mode Register [9:0]
#define PSU_UART0_RXWM              0x0020U         // RX FIFO Trigger Level [5:0]
#define PSU_UART0_TXWM              0x0044U         // TX FIFO Trigger Level [5:0]
#define PSU_UART0_RXTOUT            0x001CU         // RX Timeout [7:0]
#define PSU_UART0_BAUDGEN           0x0018U         // Baud Rate Generator [15:0]
#define PSU_UART0_BAUDDIV           0x0034U         // Baud Rate Divider [7:0]
#define PSU_UART0_SR                0x002CU         // Channel Status [14:0]
#define PSU_UART0_FIFO              0x0030U         // FIFO [7:0]
#define PSU_UART0_ISR               0x0014U         // Interrupt Status [12:0]
#define XUARTPS_IXR_MASK            0x00003FFFU
#define XUARTPS_CR_TXRST            0x00000002U    // TX logic reset
#define XUARTPS_CR_RXRST            0x00000001U    // RX logic reset
#define XUARTPS_MR_CHMODE_NORM      0x00000000U    // Normal mode
#define XUARTPS_RXWM_RESET_VAL      0x00000020U    // Reset value
#define XUARTPS_TXWM_RESET_VAL      0x00000020U    // Reset value
#define XUARTPS_RXTOUT_DISABLE      0x00000000U    // Disable time out
#define XUARTPS_BAUDGEN_RESET_VAL   0x0000028BU    // Reset value
#define XUARTPS_BAUDDIV_RESET_VAL   0x0000000FU    // Reset value
#define XUARTPS_CR_RX_DIS           0x00000008U    // RX disabled
#define XUARTPS_CR_TX_DIS           0x00000020U    // TX disabled
#define XUARTPS_CR_STOPBRK          0x00000100U    // Stop transmission of break
#define XUARTPS_SR_TXFULL           0x00000010U    // TX FIFO full

void outByte(uint32_t byte){
    while(readReg(PSU_UART0_ADDR + PSU_UART0_SR) ==
        (uint32_t)XUARTPS_SR_TXFULL) {
    }

    writeReg(PSU_UART0_ADDR + PSU_UART0_FIFO, (uint32_t)byte);
}

void outString(char* string){

    if(NULL == string)
        return;

    while(0 != *(string++)){
        outByte(*string);
    }
}

/*
 * Sets Up PS UART 0.
 */
void SetUpPsUart0(){
    /* Disable Interrupts. */
    writeReg(PSU_UART0_ADDR + PSU_UART0_IDR, (uint32_t)XUARTPS_IXR_MASK);

    /* Disable Receive & Transmit. */
    writeReg(PSU_UART0_ADDR + PSU_UART0_IDR,
        (uint32_t)XUARTPS_CR_RXRST | (uint32_t)XUARTPS_CR_TXRST);

    /* Clear status flags - SW reset wont clear sticky flags. */
    writeReg(PSU_UART0_ADDR + PSU_UART0_ISR, XUARTPS_IXR_MASK);

    /*
     * Mode register reset value : All zeroes
     * Normal mode, even parity, 1 stop bit
     */
    writeReg(PSU_UART0_ADDR + PSU_UART0_MR, XUARTPS_MR_CHMODE_NORM);

    /* Rx and TX trigger register reset values. */
    writeReg(PSU_UART0_ADDR + PSU_UART0_RXWM, XUARTPS_RXWM_RESET_VAL);

    writeReg(PSU_UART0_ADDR + PSU_UART0_TXWM, XUARTPS_TXWM_RESET_VAL);

    /* Rx timeout disabled by default. */
    writeReg(PSU_UART0_ADDR + PSU_UART0_RXTOUT,XUARTPS_RXTOUT_DISABLE);

    /* Baud rate generator and dividor reset values. */
    writeReg(PSU_UART0_ADDR + PSU_UART0_BAUDGEN, XUARTPS_BAUDGEN_RESET_VAL);
    writeReg(PSU_UART0_ADDR + PSU_UART0_BAUDDIV, XUARTPS_BAUDDIV_RESET_VAL);
}

int main(int argc, char* argv[]){

    SetUpPsUart0();
    outString(" Hello World on Xilinx's QEMU for ZCU102\n");

    writeReg(XOR_TEST_ADDR + REG_XDATA_OFFSET, 0xFFFF0105);
    writeReg(XOR_TEST_ADDR + REG_MATCHER_OFFSET, 0x00FF020F);
    writeReg(XOR_TEST_ADDR + REG_XDATA_OFFSET, 0xFF00030A);
    readReg(XOR_TEST_ADDR + REG_XDATA_OFFSET);

    return 0;
}
