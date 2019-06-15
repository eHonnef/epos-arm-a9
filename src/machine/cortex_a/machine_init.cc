// EPOS Cortex-A Mediator Initialization

#include <machine.h>

__BEGIN_SYS

void Machine::pre_init(System_Info * si)
{
    db<Init, Machine>(TRC) << "Machine::init()" << endl;

    Machine_Model::pre_init();

    Display::init();
}

void Machine::init()
{
    db<Init, Machine>(TRC) << "Machine::init()" << endl;

    Machine_Model::init();

    if(Machine::cpu_id() == 0 && Traits<System>::multicore) {
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
    }

    //Verificar se é a CPU 0
        //send SGI

    // se multicore ativo -> configurar SMP

    if(Traits<IC>::enabled)
        IC::init();
    if(Traits<Timer>::enabled)
        Timer::init();

    
#ifdef __USB_H
    if(Traits<USB>::enabled)
        USB::init();
#endif
}

__END_SYS
