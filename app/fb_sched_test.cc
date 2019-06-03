/*
    This program is written to test the Feedback Scheduler implemented.
    It creates two threads while the first one is supposed to be IO Bound.
    The IO Bound thread only sets a delay and leaves the processor. By doing
    so, it doesn't uses it's full quantum, so the context switch doensn't
    penalize this thread and it's priority remains the same.
    On the other hand, the second thread created is a CPU Bound Thread, that
    executes operations such as sum, multiplication and attribution. It does
    it inside a loop, so it never asks for a context switch. Therefore, it uses
    the processor during a full quantum and it is penalized when it doesn't
   finish before the quantum time expires. The behavior descripted above is
   shown by this program because the priorities are the same when the threads
   are instantiated, but the CPU Bound Thread priority is demoted during it's
   execution while the IO Bound thread priority remains the same.

*/

#include <process.h>
#include <time.h>

using namespace EPOS;

OStream cout;

Thread *thread_io;
Thread *thread_cpu;

int io_bound() {
  for (int i = 0; i < 100; i++) {
    Alarm::delay(100000);
  }

  return 0;
}

int cpu_bound() {
  int result = 1;
  for (int i = 0; i < 10000000; i++) {
    result *= (123 + 321);
  }

  return 0;
}

int main() {
  thread_io = new Thread(&io_bound);
  thread_cpu = new Thread(&cpu_bound);

  volatile int cpu = thread_cpu->priority();
  volatile int io = thread_io->priority();
  
  cout << "IO Bound Thread BEFORE execution has priority "
       << io << endl;
  cout << "CPU Bound Thread BEFORE execution has priority "
       << cpu << endl;
  
  
  cout << "\nExecuting..." << endl;
  thread_io->join();
  cout << "IO Bound Thread finished executing." << endl;
  int result = thread_cpu->join();
  cout << "CPU Bound Thread finished executing." << endl;
  cout << "Both threads have finished.\n" << endl;

  io = thread_io->priority();
  cpu = thread_cpu->priority();

  cout << "AFTER execution, IO Bound Thread has priority "
       << io << endl;
  cout << "AFTER exectuion, CPU Bound Thread has priority "
       << cpu << endl;

  
  delete thread_io;
  delete thread_cpu;
  return 0;
}