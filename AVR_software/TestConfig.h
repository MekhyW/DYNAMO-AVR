#ifndef TEST_CONFIG_H
#define TEST_CONFIG_H

#ifdef TEST_MODE
#undef TEST_MODE
#endif

//#define TEST_MODE // Uncomment this to enable test mode
//#define SIMULATE_HARDWARE // Uncomment to simulate hardware instead of using real hardware

#define MAX_COMMAND_LENGTH 64
#define MAX_ARGS 10

#endif // TEST_CONFIG_H
