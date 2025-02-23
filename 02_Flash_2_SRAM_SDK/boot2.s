;@ Copyright (c) 2023 CarlosFTM
;@ This code is licensed under MIT license (see LICENSE.txt for details)

.cpu cortex-m0plus
.thumb

.section .boot2, "ax"
    ldr r0, =XIP_SSI_SSIENR ;@ Set up SSIENR Register
    ldr r1, =0x00000000 ;@ Disable the XIP SSI, Execute-In-Place Serial Flash Interface
    str r1, [r0]

    ldr r0, =XIP_SSI_CTRLR0 ;@ Set up SSI CTRLR0 Register
    ldr r1, =0x001F0300 ;@ Set SPI_FRF as DUAL, DFS_32 as 15, TMOD as EEPROM_READ
    str r1, [r0]

    ldr r0, =XIP_SSI_BAUDR ;@ Set up BAUDR Register
    ldr r1, =0x00000008 ;@ Set SSI Clock Divider as 8
    str r1, [r0]

    ldr r0, =XIP_SSI_SPI_CTRLR0 ;@ Set up SPI_CTRLR0 Register
    ldr r1, =0x03000218 ;@ Set ADDR_L as 24b
    str r1, [r0]

    ldr r0, =XIP_SSI_CTRLR1 ;@ Set up CTRLR1 Register
    ldr r1, =0x00000000 ;@ Set number of data frames as 0
    str r1, [r0]

    ldr r0, =XIP_SSI_SSIENR ;@ Set up SSIENR Register
    ldr r1, =0x00000001 ;@ Enable XIP SSI
    str r1, [r0]

    ldr r4, =0x10000100  ;@ Source address (FLASH)
    ldr r5, =0x20000100  ;@ Destination (SRAM)
    ldr r6, =0x1000      ;@ Size of code

_copyToRam:
    ;@ load 16 bytes from FLASH to RAM at a time
    ldmia r4!, {r0-r3}
    stmia r5!, {r0-r3}    
    sub   r6, #16
    bne   _copyToRam

    ;@ Jump to the main function
    ldr r0, =0x20000101
    bx  r0

.set XIP_SSI_BASE,       0x18000000
.set XIP_SSI_CTRLR0,     XIP_SSI_BASE + 0x00
.set XIP_SSI_CTRLR1,     XIP_SSI_BASE + 0x04
.set XIP_SSI_SSIENR,     XIP_SSI_BASE + 0x08
.set XIP_SSI_BAUDR,      XIP_SSI_BASE + 0x14
.set XIP_SSI_SPI_CTRLR0, XIP_SSI_BASE + 0xF4

.end
