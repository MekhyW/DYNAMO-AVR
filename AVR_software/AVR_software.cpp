#include "TestConfig.h"
#ifdef SIMULATE_HARDWARE
#include "AVR_software.ino"

int main(int argc, char *argv[]) {
    #ifdef SIMULATE_HARDWARE
    setup();
    while (true) loop();
    #endif
    return 0;
}

#endif