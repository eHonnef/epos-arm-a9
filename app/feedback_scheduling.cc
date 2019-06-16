#include <process.h>
#include <time.h>

using namespace EPOS;

OStream cout;

Thread * io;
Thread * cpu;

int io_thread() {
    for(int i = 0; i < 100; i++) {
        Alarm::delay(100000);
    }
    return 0;
}

int cpu_tread() {
    int result = 0;
    for(int i = 0; i < 100000000; i++) {
        result = i;
    }
    return result;
}


int main() {
    io = new Thread(&io_thread);
    cpu = new Thread(&cpu_tread);
    cout << "IO Thread Priority: " << io->priority() << endl;
    cout << "CPU Thread Priority: " << cpu->priority() << endl;
    io->join();
    int result = cpu->join();
    cout << "IO Thread Priority: " << io->priority() << endl;
    cout << "CPU Thread Priority: " << cpu->priority() << endl;
    cout << "Result CPU Thread: " << result << endl;
    delete io;
    delete cpu;
    return 0;
}
