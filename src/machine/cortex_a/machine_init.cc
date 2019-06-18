// EPOS Cortex-A Mediator Initialization

#include <machine.h>

__BEGIN_SYS

void Machine::pre_init(System_Info * si)
{
    Display::init();

    if(Traits<IC>::enabled)
        IC::init();

    Machine_Model::pre_init();

    if(Traits<System>::multicore)
        smp_init(Traits<Build>::CPUS);
}

void Machine::init()
{
    db<Init, Machine>(TRC) << "Machine::init()" << endl;

    Machine_Model::init();

    // if(Traits<IC>::enabled)
    //     IC::init();
    if(Traits<Timer>::enabled)
        Timer::init();


#ifdef __USB_H
    if(Traits<USB>::enabled)
        USB::init();
#endif
}

__END_SYS
