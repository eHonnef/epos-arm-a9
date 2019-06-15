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

    static void delay(const RTC::Microsecond & time) { Machine_Model::delay(time); };
    
    static void panic();
    static void reboot();
    static void poweroff();

    static unsigned int n_cpus() { return Traits<Build>::CPUS; }
    static unsigned int cpu_id() {
        int id;
        ASM("mrc p15, 0, %0, c0, c0, 5" : "=r"(id) : : );
        return id & 0x3;
    }

    static void smp_barrier() {
        db<Init>(INF) << "SMP barrier" << endl;
        
        static volatile unsigned int ready[2];
        static volatile unsigned int i;

        if(smp) {
            int j = i;

            CPU::finc(ready[j]);

            if(cpu_id() == 0) {
        	    while(ready[j] < n_cpus());     // wait for all CPUs to be ready
                i = !i;                         // toggle ready
                ready[j] = 0;                   // signalizes waiting CPUs
            } else
        	    while(ready[j]);            // wait for CPU[0] signal
        }
    }

    static void smp_init(unsigned int n_cpus) {}

    static const UUID & uuid() { return Machine_Model::uuid(); }

private:
    static const bool smp = Traits<System>::multicore;
    
    static void pre_init(System_Info * si);
    static void init();
};

__END_SYS

#endif
