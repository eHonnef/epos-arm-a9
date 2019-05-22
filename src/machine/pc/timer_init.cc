// EPOS PC Timer Mediator Initialization

#include <machine/timer.h>
#include <machine/ic.h>

__BEGIN_SYS

void Timer::init()
{
    db<Init, Timer>(TRC) << "Timer::init()" << endl;

    CPU::int_disable();

    IC::int_vector(IC::INT_TIMER, int_handler);
    Engine::config(0, Engine::clock() / FREQUENCY);
    IC::enable(IC::INT_TIMER);

    CPU::int_enable();
}

__END_SYS
