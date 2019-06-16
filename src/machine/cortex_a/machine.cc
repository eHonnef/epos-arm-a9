// EPOS Cortex-A Mediator Implementation

#include <machine/cortex_a/machine.h>
#include <machine/display.h>

__BEGIN_SYS

// Class attributes
volatile unsigned int Machine::_n_cpus;

void Machine::panic()
{
    CPU::int_disable();
    if(Traits<Display>::enabled)
        Display::puts("PANIC!\n");
    if(Traits<System>::reboot)
        reboot();
    else
        CPU::halt();
}

void Machine::reboot()
{
    db<Machine>(WRN) << "Machine::reboot()" << endl;
    Machine_Model::reboot();
}

void Machine::smp_barrier(unsigned long n_cpus)
{
    static volatile unsigned long ready[2];
    static volatile unsigned long i;

    if(smp) {
        int j = i;

        CPU::finc(ready[j]);
        if(cpu_id() == 0) {
            while(ready[j] < n_cpus); // wait for all CPUs to be ready
            i = !i;                   // toggle ready
            ready[j] = 0;             // signalizes waiting CPUs
        } else {
            while(ready[j]);          // wait for CPU[0] signal
        }
    }
}

__END_SYS
