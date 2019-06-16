// EPOS RealView PBX (ARM Cortex) MCU Initialization

#include <machine/main.h>

#ifdef __mmod_realview_pbx__

void _startup_cache();

__BEGIN_SYS

void RealView_PBX::pre_init() {
  db<Init>(INF) << "RealView pre-init" << endl;

  ASM("mcr p15, 0, %0, c12, c0, 0" : : "p"(Traits<Machine>::VECTOR_TABLE));

  if (Machine::cpu_id() == 0 && Traits<System>::multicore) {
    ASM("\t\n\
        //Enable SCU                                                                                            \t\n\
        MRC     p15, 4, r0, c15, c0, 0  // Read periph base address                                             \t\n\
        LDR     r1, [r0, #0x0]          // Read the SCU Control Register                                        \t\n\
        ORR     r1, r1, #0x1            // Set bit 0 (The Enable bit)                                           \t\n\
        STR     r1, [r0, #0x0]          // Write back modifed value                                             \t\n\
                                                                                                                \t\n\
        //Join SMP                                                                                              \t\n\
        MOV     r0, #0x0                  // Move CPU ID into r0                                                \t\n\
        MOV     r1, #0xF                  // Move 0xF (represents all four ways) into r1                        \t\n\
                                                                                                                \t\n\
        //Secure SCU invalidate                                                                                 \t\n\
        AND     r0, r0, #0x03           // Mask off unused bits of CPU ID                                       \t\n\
        MOV     r0, r0, LSL #2          // Convert into bit offset (four bits per core)                         \t\n\
        AND     r1, r1, #0x0F           // Mask off unused bits of ways                                         \t\n\
        MOV     r1, r1, LSL r0          // Shift ways into the correct CPU field                                \t\n\
        MRC     p15, 4, r2, c15, c0, 0  // Read periph base address                                             \t\n\
        STR     r1, [r2, #0x0C]         // Write to SCU Invalidate All in Secure State                          \t\n\
                                                                                                                \t\n\
        //Now is joining                                                                                        \t\n\
        MRC     p15, 0, r0, c1, c0, 1   // Read ACTLR                                                           \t\n\
        ORR     r0, r0, #0x040          // Set bit 6                                                            \t\n\
        MCR     p15, 0, r0, c1, c0, 1   // Write ACTLR                                                          \t\n\
                                                                                                                \t\n\
        //Maintenance broadcast                                                                                 \t\n\
        MRC     p15, 0, r0, c1, c0, 1   // Read Aux Ctrl register                                               \t\n\
        ORR     r0, r0, #0x01           // Set the FW bit (bit 0)                                               \t\n\
        MCR     p15, 0, r0, c1, c0, 1   // Write Aux Ctrl register                                              \t\n\
                                                                                                                \t\n\
        //GIC INIT                                                                                              \t\n\
        //Enable GIC                                                                                            \t\n\
        MRC     p15, 4, r0, c15, c0, 0  // Read periph base address                                             \t\n\
        ADD     r0, r0, #0x1000         // Add the GIC offset                                                   \t\n\
        LDR     r1, [r0]                // Read the GIC's Enable Register  (ICDDCR)                             \t\n\
        ORR     r1, r1, #0x01           // Set bit 0, the enable bit                                            \t\n\
        STR     r1, [r0]                // Write the GIC's Enable Register  (ICDDCR)                            \t\n\
                                                                                                                \t\n\
        //GIC processor interface enable                                                                        \t\n\
        MRC     p15, 4, r0, c15, c0, 0  // Read periph base address                                             \t\n\
        LDR     r1, [r0, #0x100]        // Read the Processor Interface Control register   (ICCICR/ICPICR)      \t\n\
        ORR     r1, r1, #0x03           // Bit 0: Enables secure interrupts, Bit 1: Enables Non-Secure interrupts \t\n\
        STR     r1, [r0, #0x100]        // Write the Processor Interface Control register  (ICCICR/ICPICR)      \t\n\
                                                                                                                \t\n\
        //Set location for other CPUs                                                                           \t\n\
        ldr r0, =0x00010000                                                                                     \t\n\
        ldr r1, =0x10000030                                                                                     \t\n\
        str r0, [r1] 																							\t\n\
	");

    // Acordando as outras CPUS
    ASM(" \t\n\
        LDR r0, =0x0 \t\n\
        LDR r1, =0x0F \t\n\
        LDR r2, =0x01 \t\n\
        AND     r3, r0, #0x0F         // Mask off unused bits of ID, and move to r3 \t\n\
        AND     r1, r1, #0x0F        // Mask off unused bits of target_filter \t\n\
        AND     r2, r2, #0x0F        // Mask off unused bits of filter_list \t\n\
                                                                            \t\n\
        ORR     r3, r3, r1, LSL #16     // Combine ID and target_filter \t\n\
        ORR     r3, r3, r2, LSL #24     // and now the filter list \t\n\
                                                                            \t\n\
        // Get the address of the GIC                                                           \t\n\
        MRC     p15, 4, r0, c15, c0, 0  // Read periph base address     \t\n\
        ADD     r0, r0, #0x1F00         // Add offset of the sgi_trigger reg    \t\n\
                                                                                        \t\n\
        STR     r3, [r0]                // Write to the Software Generated Interrupt Register  (ICDSGIR) \t\n\
    ");

  } else if (Traits<System>::multicore) {
    ASM("\t\n\
        //Join SMP                                                                                              \t\n\
        MOV     r0, #0x0                  // Move CPU ID into r0                                                \t\n\
        MOV     r1, #0xF                  // Move 0xF (represents all four ways) into r1                        \t\n\
                                                                                                                \t\n\
        //Secure SCU invalidate                                                                                 \t\n\
        AND     r0, r0, #0x03           // Mask off unused bits of CPU ID                                       \t\n\
        MOV     r0, r0, LSL #2          // Convert into bit offset (four bits per core)                         \t\n\
        AND     r1, r1, #0x0F           // Mask off unused bits of ways                                         \t\n\
        MOV     r1, r1, LSL r0          // Shift ways into the correct CPU field                                \t\n\
        MRC     p15, 4, r2, c15, c0, 0  // Read periph base address                                             \t\n\
        STR     r1, [r2, #0x0C]         // Write to SCU Invalidate All in Secure State                          \t\n\
                                                                                                                \t\n\
        //Now is joining                                                                                        \t\n\
        MRC     p15, 0, r0, c1, c0, 1   // Read ACTLR                                                           \t\n\
        ORR     r0, r0, #0x040          // Set bit 6                                                            \t\n\
        MCR     p15, 0, r0, c1, c0, 1   // Write ACTLR                                                          \t\n\
                                                                                                                \t\n\
        //Maintenance broadcast                                                                                 \t\n\
        MRC     p15, 0, r0, c1, c0, 1   // Read Aux Ctrl register                                               \t\n\
        ORR     r0, r0, #0x01           // Set the FW bit (bit 0)                                               \t\n\
        MCR     p15, 0, r0, c1, c0, 1   // Write Aux Ctrl register                                              \t\n\
    ");
  }
}

void RealView_PBX::init() { db<Init>(INF) << "RealView Init" << endl; }

__END_SYS

#endif
