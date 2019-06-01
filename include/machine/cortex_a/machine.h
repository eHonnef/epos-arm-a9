// EPOS ARM Cortex-A Mediator Declarations

#ifndef __cortex_a_h
#define __cortex_a_h

#include <architecture.h>
#include <machine/main.h>
#include <machine/rtc.h>
#include __MODEL_H
#include "info.h"
#include "memory_map.h"

__BEGIN_SYS

class Machine: private Machine_Common, private Machine_Model
{
    friend class Init_System;
    friend class First_Object;

public:
    Machine() {}

    static void delay(const RTC::Microsecond & time) { Machine_Model::delay(time); }// @TODO: so coloquei o retorno pra funcionar
    
    static void panic();
    static void reboot();
    static void poweroff() { reboot(); }// @TODO: so coloquei o retorno pra funcionar

    static unsigned int n_cpus() { return 1; } // @TODO: so coloquei o retorno pra funcionar
    static unsigned int cpu_id() {return 0;} // @TODO: so coloquei o retorno pra funcionar

    static void smp_barrier();
    static void smp_init(unsigned int);

    static const UUID & uuid() { return Machine_Model::uuid(); }

private:
    static void pre_init(System_Info * si);
    static void init();
};

__END_SYS

#endif
