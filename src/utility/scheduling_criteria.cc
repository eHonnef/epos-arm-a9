// EPOS CPU Affinity Scheduler Component Implementation

#include <utility/scheduler.h>
#include <time.h>

__BEGIN_SYS

// The following Scheduling Criteria depend on Alarm, which is not available at scheduler.h
volatile unsigned int Scheduling_Criteria::Variable_Queue::_next_queue;

namespace Scheduling_Criteria {
    FCFS::FCFS(int p): Priority((p == IDLE) ? IDLE : Alarm::_elapsed) {}
};

__END_SYS
