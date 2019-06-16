// EPOS Cortex-A SETUP

#include <system/config.h>

extern "C" { void _vector_table() __attribute__ ((used, naked, section(".init"))); }

// Interrupt Vector Table
void _vector_table()
{
    ASM("\t\n\
    _vector_table: \t\n\
        b   _reset                                                           \t\n\
        b   _undefined_instruction                                           \t\n\
        b   _software_interrupt                                              \t\n\
        b   _prefetch_abort                                                  \t\n\
        b   _data_abort                                                      \t\n\
        nop                           // Reserved                            \t\n\
        b   _int_entry                                                       \t\n\
        b   _fiq                                                             \t\n\
                                                                             \t\n\
    _reset:                                                                  \t\n\
        //-----------------------------------------------                                             \t\n\
        // 1.MMU, L1$ disable                                                                         \t\n\
        //-----------------------------------------------                                             \t\n\
        MRC p15, 0, r1, c1, c0, 0   // Read System Control Register (SCTLR)                           \t\n\
        BIC r1, r1, #1              // mmu off                                                        \t\n\
        BIC r1, r1, #(1 << 12)      // i-cache off                                                    \t\n\
        BIC r1, r1, #(1 << 2)       // d-cache & L2-$ off                                             \t\n\
        MCR p15, 0, r1, c1, c0, 0   // Write System Control Register (SCTLR)                          \t\n\
        //-----------------------------------------------                                             \t\n\
        // 2. invalidate: L1$, TLB, branch predictor                                                  \t\n\
        //-----------------------------------------------                                             \t\n\
        MOV r0, #0                                                                                    \t\n\
        MCR p15, 0, r0, c7, c5, 0   // Invalidate Instruction Cache                                   \t\n\
        MCR p15, 0, r0, c7, c5, 6   // Invalidate branch prediction array                             \t\n\
        MCR p15, 0, r0, c8, c7, 0   // Invalidate entire Unified Main TLB                             \t\n\
        ISB                         // instr sync barrier                                             \t\n\
        //-----------------------------------------------                                             \t\n\
        // 2.a. Enable I cache + branch prediction                                                    \t\n\
        //-----------------------------------------------                                             \t\n\
        MRC p15, 0, r0, c1, c0, 0   // System control register                                        \t\n\
        ORR r0, r0, #1 << 12        // Instruction cache enable                                       \t\n\
        ORR r0, r0, #1 << 11        // Program flow prediction                                        \t\n\
        MCR p15, 0, r0, c1, c0, 0   // System control register                                        \t\n\
        //-----------------------------------------------                                             \t\n\
        // Setup Stacks                                                                               \t\n\
        //-----------------------------------------------                                             \t\n\
        //MSR     CPSR_c, #0xD2                                                                         \t\n\
        //LDR     sp, =0x001fe000                                                                       \t\n\
                                                                                                      \t\n\
        //MSR     CPSR_c, #0xDF       // No interrupts                                                  \t\n\
        //LDR     sp, =0x001fc000                                                                       \t\n\
                                                                                                      \t\n\
                                                                                                      \t\n\
        MRC p15, 1, R0, c0, c0, 1   // Read CLIDR into R0                                             \t\n\
        ANDS R3, R0, #0x07000000                                                                      \t\n\
        MOV R3, R3, LSR #23         // Cache level value (naturally aligned)                          \t\n\
        BEQ Finished                                                                                  \t\n\
        MOV R10, #0                                                                                   \t\n\
    Loop1:                                                                                            \t\n\
        ADD R2, R10, R10, LSR #1    // Work out 3 x cachelevel                                        \t\n\
        MOV R1, R0, LSR R2          // bottom 3 bits are the Cache type for this level                \t\n\
        AND R1, R1, #7              // get those 3 bits alone                                         \t\n\
        CMP R1, #2                                                                                    \t\n\
        BLT Skip                    // no cache or only instruction cache at this                     \t\n\
        MCR p15, 2, R10, c0, c0, 0  // write CSSELR from R10                                          \t\n\
        ISB                         // ISB to sync the change to the CCSIDR                           \t\n\
        MRC p15, 1, R1, c0, c0, 0   // read current CCSIDR to R1                                      \t\n\
        AND R2, R1, #7              // extract the line length field                                  \t\n\
        ADD R2, R2, #4              // add 4 for the line length offset (log2 16 bytes)               \t\n\
        LDR R4, =0x3FF                                                                                \t\n\
        ANDS R4, R4, R1, LSR #3     // R4 is the max number on the way size (right aligned)           \t\n\
        CLZ R5, R4                  // R5 is the bit position of the way size increment               \t\n\
        MOV R9, R4                  // R9 working copy of the max way size (right aligned)            \t\n\
    Loop2:                                                                                            \t\n\
        LDR R7, =0x00007FFF                                                                           \t\n\
        ANDS R7, R7, R1, LSR #13    // R7 is the max number of the index size (right aligned)         \t\n\
    Loop3:                                                                                            \t\n\
        ORR R11, R10, R9, LSL R5    // factor in the way number and cache number into R11             \t\n\
        ORR R11, R11, R7, LSL R2    // factor in the index number                                     \t\n\
        MCR p15, 0, R11, c7, c10, 2 // DCCSW, clean by set/way                                        \t\n\
        SUBS R7, R7, #1             // decrement the index                                            \t\n\
        BGE Loop3                                                                                     \t\n\
        SUBS R9, R9, #1             // decrement the way number                                       \t\n\
        BGE Loop2                                                                                     \t\n\
                                                                                                      \t\n\
    Skip:                                                                                             \t\n\
        ADD R10, R10, #2            // increment the cache number                                     \t\n\
        CMP R3, R10                                                                                   \t\n\
        BGT Loop1                                                                                     \t\n\
        DSB                                                                                           \t\n\
    Finished:                                                                                         \t\n\
        ADD R4, #0                                                                                    \t\n\
        ADD R4, R4, #10                                                                               \t\n\
                                                                                                      \t\n\
    mmu_start:                                                                                \t\n\
        // Disable MMU                                                                        \t\n\
        MRC p15, 0, r1, c1, c0, 0	// Atribui-se ao R1 o valor do registrador 1 do           \t\n\
                                    // coprocessor 15                                         \t\n\
        BIC r1, r1, #0x1           	// Atribui-se ao bit 0 em R1 o valor 0, para              \t\n\
                                    // desligar a MMU                                         \t\n\
        MCR p15, 0, r1, c1, c0, 0	// Escreve-se no reg 1 do coprocessor 15                  \t\n\
                                    // o que ha em R1, desabilitando a MMU                    \t\n\
                                                                                              \t\n\
        // Disable L1 Caches                                                                  \t\n\
        MRC p15, 0, r1, c1, c0, 0	// Read Control Register configuration data               \t\n\
        BIC r1, r1, #(0x1 << 12)	// Disable I Cache                                        \t\n\
        BIC r1, r1, #(0x1 << 2)		// Disable D Cache                                        \t\n\
        MCR p15, 0, r1, c1, c0, 0	// Write Control Register configuration data              \t\n\
                                                                                              \t\n\
        // Invalidate L1 Caches                                                               \t\n\
        // Invalidate Instruction cache                                                       \t\n\
        MOV r1, #0                                                                            \t\n\
        MCR p15, 0, r1, c7, c5, 0                                                             \t\n\
                                                                                              \t\n\
        // Invalidate Data cache                                                              \t\n\
        // to make the code general purpose, we calculate the                                 \t\n\
        // cache size first and loop through each set + way                                   \t\n\
        MRC p15, 1, r0, c0, c0, 0  	// Read Cache Size ID                                     \t\n\
        LDR r3, =#0x1ff                                                                       \t\n\
        AND r0, r3, r0, LSR #13		// r0 = no. of sets - 1                                   \t\n\
        MOV r1, #0			        // r1 = way counter way_loop                              \t\n\
        way_loop:                                                                             \t\n\
        MOV r3, #0			        // r3 = set counter set_loop                              \t\n\
        set_loop:                                                                             \t\n\
        MOV r2, r1, LSL #30                                                                   \t\n\
        ORR r2, r3, LSL #5 		    // r2 = set/way cache operation format                    \t\n\
        MCR p15, 0, r2, c7, c6, 2 	// Invalidate line described by r2                        \t\n\
        ADD r3, r3, #1 			    // Increment set counter                                  \t\n\
        CMP r0, r3 			        // Last set reached yet?                                  \t\n\
        BGT set_loop 			    // if not, iterate set_loop                               \t\n\
        ADD r1, r1, #1 			    // else, next                                             \t\n\
        CMP r1, #4 			        // Last way reached yet?                                  \t\n\
        BNE way_loop 			    // if not, iterate way_loop                               \t\n\
                                                                                              \t\n\
        // Invalidate TLB                                                                     \t\n\
        MCR p15, 0, r1, c8, c7, 0                                                             \t\n\
                                                                                              \t\n\
        // Aqui é criada uma L1 translation table na RAM que divide                           \t\n\
        // todo o espaço de endereçamento de 4GB em seções de 1 MB,                           \t\n\
        // todas com Full Access e Strongly Ordered                                           \t\n\
        LDR r0, =0xDE2			        // Atribui-se ao R0 parte do descriptor               \t\n\
        LDR r1, =0xFA0000       	    // Atribui-se ao R1 endereço base                     \t\n\
                                        // da L1 tranlastion table                            \t\n\
        LDR r3, = 4095         		    // R3 se torna o contador para o loop                 \t\n\
                                                                                              \t\n\
        write_pte:                  	// Label do loop para escrita das                     \t\n\
                                        // page table entry (PTE) da translation table        \t\n\
            ORR r2, r0, r3, LSL #20     // Atribui-se ao R2 OR entre o endereço               \t\n\
                                        // e os bits padrão da PTE                            \t\n\
        STR r2, [r1, r3, LSL #2]    	// Escreve-se a PTE na translation table              \t\n\
                                        // (endereço de escrita é o ttb_address somado        \t\n\
                                        // com contador e multiplicado por 4)                 \t\n\
        SUB r3, r3, #1              	// Decrementa-se contador do loop                     \t\n\
        CMP r3, #-1                 	// Faz-se a comparação para verificar                 \t\n\
                                        // se loop acabou                                     \t\n\
        BNE write_pte               	// Caso o loop não tenha acabado,                     \t\n\
                                        // escreve mais uma pte                               \t\n\
                                                                                              \t\n\
        // Faz-se a primeira entrada da translation table                                     \t\n\
        // cacheable, normal, write-back, write allocate                                      \t\n\
        BIC r0, r0, #0xC		    // Limpa-se CB bits                                       \t\n\
        ORR r0, r0, #0X4 		    // Write-back, write allocate                             \t\n\
        BIC r0, r0, #0x7000 		// Limpa-se TEX bits                                      \t\n\
        ORR r0, r0, #0x5000 		// Faz-se TEX write-back e write allocate                 \t\n\
        ORR r0, r0, #0x10000 		// Torna compartilhável                                   \t\n\
        STR r0, [r1]			    // Escreve-se na primeira entrada                         \t\n\
                                                                                              \t\n\
        // Inicializa a MMU                                                                   \t\n\
        MOV r1,#0x0                                                                           \t\n\
            MCR p15, 0, r1, c2, c0, 2   // Escrita do Translation Table Base Control Register \t\n\
        LDR r1, =0xFA0000   		    // Atribui-se ao R1 endereço base                     \t\n\
                                        // da L1 tranlastion table                            \t\n\
        MCR p15, 0, r1, c2, c0, 0	    // Escreve-se no reg 1 do coprocessor 15 o que ha     \t\n\
                                        // em r1 (endereco base da tranlastion table)         \t\n\
                                                                                              \t\n\
        // In this simple example, we don't use TRE or Normal Memory Remap Register.          \t\n\
        // Set all Domains to Client                                                          \t\n\
        LDR r1, =0x55555555                                                                   \t\n\
        MCR p15, 0, r1, c3, c0, 0       // Write Domain Access Control Register               \t\n\
                                                                                              \t\n\
        // Enable MMU                                                                         \t\n\
        MRC p15, 0, r1, c1, c0, 0	    // Atribui-se ao R1 o valor do registrador 1 do       \t\n\
                                        // coprocessor 15                                     \t\n\
        ORR r1, r1, #0x1           	    // Atribui-se ao bit 0 em R1 o valor 1, para          \t\n\
                                        // ligar a MMU                                        \t\n\
        MCR p15, 0, r1, c1, c0, 0	    // Escreve-se no reg 1 do coprocessor 15              \t\n\
                                        // o que há em R1, habilitando a MMU                  \t\n\
                                                                                              \t\n\
        //BRANCH PREDICTION INIT                                                              \t\n\
        MRC     p15, 0, r0, c1, c0, 0     // Read SCTLR                                       \t\n\
        ORR     r0, r0, #(1 << 11)        // Set the Z bit (bit 11)                           \t\n\
        MCR     p15, 0,r0, c1, c0, 0      // Write SCTLR                                      \t\n\
                                                                                              \t\n\
        b _mcu_start                                                                          \t\n\
    ");
}