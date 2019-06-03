// EPOS Cortex-A SETUP

#include <system/config.h>

extern "C" {
void _vector_table() __attribute__((used, naked, section(".init")));
}

// Interrupt Vector Table
void _vector_table() {
    
    ASM(
        // Interrupt Vector Table
        "ldr pc, _start_addr                             \n"
        "ldr pc, _undefined_instruction_addr             \n"
        "ldr pc, _swi_addr                               \n"
        "ldr pc, _prefetch_abort_addr                    \n"
        "ldr pc, _data_abort_addr                        \n"
        "ldr pc, _reserved_addr                          \n"
        "ldr pc, _irq_addr                               \n"
        "ldr pc, _fiq_addr                               \n"
        "_start_addr:                                    \n"
        ".word _start                                    \n"
        "_undefined_instruction_addr:                    \n"
        ".word _undefined_instruction                    \n"
        "_swi_addr:                                      \n"
        ".word _software_interrupt                       \n"
        "_prefetch_abort_addr:                           \n"
        ".word _prefetch_abort                           \n"
        "_data_abort_addr:                               \n"
        ".word _data_abort                               \n"
        "_reserved_addr:                                 \n"
        ".word _reserved                                 \n"
        "_irq_addr:                                      \n"
        ".word _int_entry                                \n"
        "_fiq_addr:                                      \n"
        ".word _fiq                                      \n" 
    );
}
