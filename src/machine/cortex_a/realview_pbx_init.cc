// EPOS Realview_PBX (ARM Cortex) MCU Initialization

#include <machine/main.h>

#ifdef __mmod_realview_pbx__

__BEGIN_SYS

void Realview_PBX::pre_init() {}

void Realview_PBX::init()
{
    unlock_slcr();
    init_machine();
    db<Init, Machine>(TRC) << "Realview_PBX::init" << endl;
}

__END_SYS

#endif
