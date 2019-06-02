// EPOS Cortex-A SETUP

#include <system/config.h>

extern "C" {
void _vector_table() __attribute__((used, naked, section(".init")));
}

// Interrupt Vector Table
void _vector_table() {
    ASM("\t\n\
        // EPOS Cortex-A SETUP                          \t\n\
                                                        \t\n\
        //.file cortex_a_setup.S                        \t\n\
                                                        \t\n\
        // Interrupt Vector Table                       \t\n\
        .section .init                                  \t\n\
        .type _vector_table, object                     \t\n\
                                                        \t\n\
        _vector_table:                                  \t\n\
        ldr pc, _start_addr                             \t\n\
        ldr pc, _undefined_instruction_addr             \t\n\
        ldr pc, _swi_addr                               \t\n\
        ldr pc, _prefetch_abort_addr                    \t\n\
        ldr pc, _data_abort_addr                        \t\n\
        ldr pc, _reserved_addr                          \t\n\
        ldr pc, _irq_addr                               \t\n\
        ldr pc, _fiq_addr                               \t\n\
                                                        \t\n\
        _start_addr:                                    \t\n\
                .word _start                            \t\n\
        _undefined_instruction_addr:                    \t\n\
                .word _undefined_instruction            \t\n\
        _swi_addr:                                      \t\n\
                .word _software_interrupt               \t\n\
        _prefetch_abort_addr:                           \t\n\
                .word _prefetch_abort                   \t\n\
        _data_abort_addr:                               \t\n\
                .word _data_abort                       \t\n\
        _reserved_addr:                                 \t\n\
                .word _reserved                         \t\n\
        _irq_addr:                                      \t\n\
                .word _int_entry                        \t\n\
        _fiq_addr:                                      \t\n\
                .word _fiq                              \t\n\
    ");
}
