// EPOS Realview_PBX (ARM Cortex-A9) MCU Metainfo and Configuration

#ifndef __machine_traits_h
#define __machine_traits_h

#include <system/config.h>

__BEGIN_SYS

class Machine_Common;
template<> struct Traits<Machine_Common>: public Traits<void>
{
    static const bool debugged = Traits<void>::debugged;
};

template<> struct Traits<Machine>: public Traits<Machine_Common>
{
    static const unsigned int CPUS = Traits<Build>::CPUS;

    // Physical Memory 
    static const unsigned int MEM_BASE    = 0x00000000;
    static const unsigned int MEM_TOP     = 0x0FFFFFFF;
    static const unsigned int BOOT_STACK  = 0x0FFFFFF0; // MEM_TOP - sizeof(int)

    // Logical Memory Map
    static const unsigned int APP_LOW   = 0x00000000;
    static const unsigned int APP_CODE  = 0x00000000;
    static const unsigned int APP_DATA  = 0x03100000;
    static const unsigned int APP_HIGH  = 0x06100000;

    static const unsigned int PHY_MEM   = 0x00000000;
    static const unsigned int IO_BASE   = 0x00000000;
    static const unsigned int IO_TOP    = 0x00000000;

    static const unsigned int SYS       = 0x06100000;
    static const unsigned int SYS_CODE  = 0x06100000;
    static const unsigned int SYS_DATA  = 0x07100000;

    // Default Sizes and Quantities
    static const unsigned int STACK_SIZE = 16 * 1024;
    static const unsigned int HEAP_SIZE = 16 * 1024 * 1024;
    static const unsigned int MAX_THREADS = 16;
};

template<> struct Traits<IC>: public Traits<Machine_Common>
{
    static const bool debugged = hysterically_debugged;
};

template<> struct Traits<Timer>: public Traits<Machine_Common>
{
    static const bool debugged = hysterically_debugged;

    // Meaningful values for the timer frequency range from 100 to
    // 10000 Hz. The choice must respect the scheduler time-slice, i. e.,
    // it must be higher than the scheduler invocation frequency.
    static const int FREQUENCY = 1000; // Hz
};

template<> struct Traits<UART>: public Traits<Machine_Common>
{
    static const unsigned int UNITS = 4;

    static const unsigned int CLOCK = Traits<CPU>::CLOCK;

    static const unsigned int DEF_UNIT = 0;
    static const unsigned int DEF_BAUD_RATE = 38400;
    static const unsigned int DEF_DATA_BITS = 8;
    static const unsigned int DEF_PARITY = 0; // none
    static const unsigned int DEF_STOP_BITS = 1;
};

template<> struct Traits<USB>: public Traits<Machine_Common>
{
    static const bool enabled = false;
    static const unsigned int UNITS = 0;
    static const bool blocking = true;
};

template<> struct Traits<Scratchpad>: public Traits<Machine_Common>
{
    static const bool enabled = false;
};

__END_SYS

#endif
