#include <process.h>
#include <time.h>

using namespace EPOS;

OStream cout;

int main() {
    while (true) {
        Alarm::delay(10000000);
        cout << "Teste de delay" << endl;
    }
    return 0;
}
