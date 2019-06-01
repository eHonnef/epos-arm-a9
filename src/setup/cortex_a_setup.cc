// EPOS Cortex-A SETUP

#include <system/config.h>

extern "C" {
void _vector_table() __attribute__((used, naked, section(".init")));
}

// Interrupt Vector Table
void _vector_table() {
    ASM("\t\n\
        .equ MODE_FIQ, 0x11 \t\n\
        .equ MODE_IRQ, 0x12 \t\n\
        .equ MODE_SVC, 0x13 \t\n\
\t\n\
        //.section .vector_table, 'x' \t\n\
        .global _Reset \t\n\
        .global _start \t\n\
\t\n\
        _Reset: \t\n\
        b _reset                                                           \t\n\
        b abort  //_undefined_instruction                                           \t\n\
        b .      //_software_interrupt                  \t\n\
        b abort  //_prefetch_abort                                                  \t\n\
        b abort  //_data_abort                                                      \t\n\
        nop      // Reserved                            \t\n\
        b   _irq                                                             \t\n\
        b   .    //_fiq                                                            \t\n\
\t\n\
        _reset: \t\n\
        // Change the vector table base address \t\n\
        ldr r0, =0x60000000 \t\n\
        mcr p15, #0, r0, c12, c0, #0 \t\n\
\t\n\
        // FIQ stack\t\n\
        //msr cpsr_c, MODE_FIQ \t\n\
        //ldr r1, =_fiq_stack_start \t\n\
        //ldr sp, =_fiq_stack_end \t\n\
        //movw r0, #0xFEFE \t\n\
        //movt r0, #0xFEFE \t\n\
\t\n\
        abort: \t\n\
        swi 0xFF \t\n\
\t\n\
        .global _irq \t\n\
        _irq: \t\n\
        ldr r0, =0x10009000 \t\n\
        ldr r1, [r0] \t\n\
        b . \t\n\
    ");
}
