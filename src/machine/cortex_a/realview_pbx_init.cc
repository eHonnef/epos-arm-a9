// EPOS RealView PBX (ARM Cortex) MCU Initialization

#include <machine/main.h>

#ifdef __mmod_realview_pbx__

void _startup_cache();

__BEGIN_SYS

void Realview_PBX::pre_init() {
	db<Init, Machine>(TRC) << "Pre inicializando a realview" << endl;

	if(Machine::cpu_id() == 0) {
		//primary CPU
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

	} else if (Traits<System>::multicore) {
		//secondary CPU

		ASM(" \t\n\
		//GIC processor interface enable                                                                        \t\n\
        //MRC     p15, 4, r0, c15, c0, 0  // Read periph base address                                             \t\n\
        //LDR     r1, [r0, #0x100]        // Read the Processor Interface Control register   (ICCICR/ICPICR)      \t\n\
        //ORR     r1, r1, #0x03           // Bit 0: Enables secure interrupts, Bit 1: Enables Non-Secure interrupts \t\n\
        //STR     r1, [r0, #0x100]        // Write the Processor Interface Control register  (ICCICR/ICPICR)      \t\n\
                                                                                                                \t\n\
        //Priority mask                                                                                         \t\n\
        //MOV     r0, #0x1F               // Priority                                                             \t\n\
        //MOV     r1, r0                  // Back up passed in ID value                                           \t\n\
        //MRC     p15, 4, r0, c15, c0, 0  // Read periph base address                                             \t\n\
        //STR     r1, [r0, #0x0104]       // Write the Priority Mask register (ICCPMR/ICCIPMR)                    \t\n\
                                                                                                                \t\n\
        //MOV     r0, #0x0                  // ID                                                                 \t\n\
        //Get base address of private perpherial space                                                          \t\n\
        //MOV     r1, r0                  // Back up passed in ID value                                           \t\n\
        //MRC     p15, 4, r0, c15, c0, 0  // Read periph base address                                             \t\n\
        // Each interrupt source has an enable bit in the GIC.  These                                           \t\n\
        // are grouped into registers, with 32 sources per register                                             \t\n\
        // First, we need to identify which 32 bit block the interrupt lives in                                 \t\n\
        //MOV     r2, r1                  // Make working copy of ID in r2                                        \t\n\
        //MOV     r2, r2, LSR #5          // LSR by 5 places, affective divide by 32                              \t\n\
                                        // r2 now contains the 32 bit block this ID lives in                    \t\n\
        //MOV     r2, r2, LSL #2          // Now multiply by 4, to covert offset into an address offset (four bytes per reg) \t\n\
        // Now work out which bit within the 32 bit block the ID is                                             \t\n\
        //AND     r1, r1, #0x1F           // Mask off to give offset within 32bit block                           \t\n\
        //MOV     r3, #1                  // Move enable value into r3                                            \t\n\
        //MOV     r3, r3, LSL r1          // Shift it left to position of ID                                      \t\n\
        //ADD     r2, r2, #0x1100         // Add the base offset of the Enable Set registers to the offset for the ID \t\n\
        //STR     r3, [r0, r2]            // Store out  (ICDISER)                                                 \t\n\
                                                                                                                \t\n\
        //MOV     r0, #0x0                  // ID                                                                 \t\n\
        //MOV     r1, #0x0                  // Priority                                                           \t\n\
        // Get base address of private perpherial space                                                         \t\n\
        //MOV     r2, r0                  // Back up passed in ID value                                           \t\n\
        //MRC     p15, 4, r0, c15, c0, 0  // Read periph base address                                             \t\n\
        // Make sure that priority value is only 5 bits, and convert to expected format                         \t\n\
        //AND     r1, r1, #0x1F                                                                                   \t\n\
        //MOV     r1, r1, LSL #3                                                                                  \t\n\
        // Find which priority register this ID lives in                                                        \t\n\
        //BIC     r3, r2, #0x03           // Make a copy of the ID, clearing off the bottom two bits              \t\n\
        // There are four IDs per reg, by clearing the bottom two bits we get an address offset                 \t\n\
        //ADD     r3, r3, #0x1400         // Now add the offset of the Priority Level registers from the base of the private peripheral space \t\n\
        //ADD     r0, r0, r3              // Now add in the base address of the private peripheral space, giving us the absolute address \t\n\
        // Now work out which ID in the register it is                                                          \t\n\
        //AND     r2, r2, #0x03           // Clear all but the bottom four bits, leaves which ID in the reg it is (which byte) \t\n\
        //MOV     r2, r2, LSL #3          // Multiply by 8, this gives a bit offset                               \t\n\
        // Read -> Modify -> Write                                                                              \t\n\
        //MOV     r12, #0xFF              // Mask (8 bits)                                                        \t\n\
        //MOV     r12, r12, LSL r2        // Move mask into correct bit position                                  \t\n\
        //MOV     r1, r1, LSL r2          // Also, move passed in priority value into correct bit position        \t\n\
        //LDR     r3, [r0]                // Read current value of the Priority Level register (ICDIPR)           \t\n\
        //BIC     r3, r3, r12             // Clear appropiate field                                               \t\n\
        //ORR     r3, r3, r1              // Now OR in the priority value                                         \t\n\
        //STR     r3, [r0]                // And store it back again  (ICDIPR)                                    \t\n\
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
        //HOLDING                                                                                               \t\n\
        //MOV     r2, #0x00                 // Clear r2                                                           \t\n\
        //CPSIE   i                         // Enable interrupts                                                  \t\n\
    //holding_pen:                                                                                                \t\n\
        //CMP     r2, #0x0                  // r2 will be set to 0x1 by IRQ handler on receiving SGI              \t\n\
        //WFIEQ                                                                                                   \t\n\
        //BEQ     holding_pen                                                                                     \t\n\
        //CPSID   i                         // IRQs not used in reset of example, so mask out interrupts          \t\n\
		");
	}
}

void Realview_PBX::init() {
	db<Init, Machine>(TRC) << "Inicializando a realview" << endl;
}

__END_SYS

#endif