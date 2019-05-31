// EPOS ARM Cortex-A IC Mediator Declarations

#ifndef __cortex_a_ic_h
#define __cortex_a_ic_h

#include <architecture/cpu.h>
#include <machine/ic.h>
#include __MODEL_H

__BEGIN_SYS
/*
    typedef Interrupt_Id IRQ;
    enum {
        IRQ_GPIOA       = 0,
        IRQ_GPIOB       = 1,
        IRQ_GPIOC       = 2,
        IRQ_GPIOD       = 3,
        IRQ_GPIOE       = 4,
        IRQ_UART0       = 5,
        IRQ_UART1       = 6,
        IRQ_SSI0        = 7,
        IRQ_I2C         = 8,
        IRQ_ADC         = 14,
        IRQ_WATCHDOG    = 18,
        IRQ_GPT0A       = 19,
        IRQ_GPT0B       = 20,
        IRQ_GPT1A       = 21,
        IRQ_GPT1B       = 22,
        IRQ_GPT2A       = 23,
        IRQ_GPT2B       = 24,
        IRQ_AC          = 25,
        IRQ_RFTXRX      = 26,
        IRQ_RFERR       = 27,
        IRQ_SC          = 28,
        IRQ_FC          = 29,
        IRQ_AES         = 30,
        IRQ_PKA         = 31,
        IRQ_SMT         = 32,
        IRQ_MACTIMER    = 33,
        IRQ_SSI1        = 34,
        IRQ_GPT3A       = 35,
        TIMER01         = 36,
        TIMER23         = 37,
        IRQ_USB         = 44, // Using alternate interrupt mapping
        IRQ_UDMASW      = 46,
        IRQ_UDMAERR     = 47,
        TIMER45         = 73,
        TIMER67         = 74,
        IRQ_LAST        = IRQ_UDMAERR
    };
*/

class IC: private Engine
{
    friend class Machine;

private:

public:
    using IC_Common::Interrupt_Id;
    using IC_Common::Interrupt_Handler;
    using Engine::INT_TIMER;
    using Engine::INT_USER_TIMER0;
    using Engine::INT_USER_TIMER1;
    using Engine::INT_USER_TIMER2;
    using Engine::INT_USER_TIMER3;
    using Engine::INT_GPIOA;
    using Engine::INT_GPIOB;
    using Engine::INT_GPIOC;
    using Engine::INT_GPIOD;
    using Engine::INT_USB0;
    using Engine::INT_NIC0_RX;
    using Engine::INT_NIC0_TX;
    using Engine::INT_NIC0_ERR;
    using Engine::INT_NIC0_TIMER;
    using Engine::INT_RESCHEDULER;

public:
    IC() {}

    static Interrupt_Handler int_vector(const Interrupt_Id & i) {
        assert(i < INTS);
        return _int_vector[i];
    }

    static void int_vector(const Interrupt_Id & i, const Interrupt_Handler & h) {
        db<IC>(TRC) << "IC::int_vector(int=" << i << ",h=" << reinterpret_cast<void *>(h) <<")" << endl;
        assert(i < INTS);
        _int_vector[i] = h;
    }

    static void enable() {
        db<IC>(TRC) << "IC::enable()" << endl;
        Engine::enable();
    }

    static void enable(const Interrupt_Id & i) {
        db<IC>(TRC) << "IC::enable(int=" << i << ")" << endl;
        assert(i < INTS);
        Engine::enable(i);
    }

    static void disable() {
        db<IC>(TRC) << "IC::disable()" << endl;
        Engine::disable();
    }

    static void disable(const Interrupt_Id & i) {
        db<IC>(TRC) << "IC::disable(int=" << i << ")" << endl;
        assert(i < INTS);
        Engine::disable(i);
    }

    using Engine::irq2int;

    static void ipi_send(unsigned int cpu, Interrupt_Id int_id) {}

    void undefined_instruction();
    void software_interrupt();
    void prefetch_abort();
    void data_abort();
    void reserved();
    void fiq();

private:
    static void dispatch(unsigned int i);
    static void eoi(unsigned int i);

    // Logical handlers
    static void int_not(const Interrupt_Id & i);
    static void hard_fault(const Interrupt_Id & i);

    // Physical handler
    static void entry();

    static void init();

private:
    static Interrupt_Handler _int_vector[INTS];
    static Interrupt_Handler _eoi_vector[INTS];
};

__END_SYS

#endif
