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
