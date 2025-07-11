#ifndef CLI_H
#define CLI_H

#include "TestConfig.h"
#ifdef SIMULATE_HARDWARE
#include "PCSandbox.h"
#include "HardwareSim.h"
#else
#include <Arduino_FreeRTOS.h>
#endif
#include "LEDs.h"
#include "Servos.h"

#ifdef TEST_MODE

#include <string.h>

static QueueHandle_t* g_queue_leds = NULL;
static QueueHandle_t* g_queue_servos = NULL;

typedef struct {
    const char* command;
    const char* description;
    void (*function)(int argc, char* argv[]);
} Command_t;

void cmd_help(int argc, char* argv[]);
void cmd_leds_test(int argc, char* argv[]);
void cmd_servo_test(int argc, char* argv[]);
void cmd_emotion_test(int argc, char* argv[]);
void cmd_set_inputs(int argc, char* argv[]);

const Command_t commands[] = {
    {"help", "Display available commands", cmd_help},
    {"leds", "Test LED functionality: leds <effect> <color_r> <color_g> <color_b> <brightness>", cmd_leds_test},
    {"servo", "Test servo functionality: servo <index> <position>", cmd_servo_test},
    {"emotion", "Test emotion blending: emotion <angry> <disgusted> <happy> <neutral> <sad> <surprised>", cmd_emotion_test},
    {"inputs", "Set all inputs directly: inputs <14 comma-separated values>", cmd_set_inputs},
    {NULL, NULL, NULL}
};

void init_cli_queues(QueueHandle_t* leds_queue, QueueHandle_t* servos_queue) {
    g_queue_leds = leds_queue;
    g_queue_servos = servos_queue;
}

void process_command(char* input) {
    char* argv[MAX_ARGS];
    int argc = 0;
    char* token = strtok(input, " ");
    while (token != NULL && argc < MAX_ARGS) {
        argv[argc++] = token;
        token = strtok(NULL, " ");
    }
    if (argc == 0) return;
    for (int i = 0; commands[i].command != NULL; i++) {
        if (strcmp(argv[0], commands[i].command) == 0) {
            commands[i].function(argc, argv);
            return;
        }
    }
    Serial.print("Unknown command: ");
    Serial.println(argv[0]);
    Serial.println("Type 'help' for available commands");
}

void cmd_help(int argc, char* argv[]) {
    Serial.println("Available commands:");
    for (int i = 0; commands[i].command != NULL; i++) {
        Serial.print("  ");
        Serial.print(commands[i].command);
        Serial.print(" - ");
        Serial.println(commands[i].description);
    }
}

void cmd_leds_test(int argc, char* argv[]) {
    if (argc < 4) {
        Serial.println("Usage: leds <effect> <color_r> <color_g> <color_b> <brightness>");
        return;
    }
    if (g_queue_leds == NULL) {
        Serial.println("Error: LED queue not initialized");
        return;
    }
    int effect = atoi(argv[1]);
    int color_r = atoi(argv[2]);
    int color_g = atoi(argv[3]);
    int color_b = atoi(argv[4]);
    int brightness = atoi(argv[5]);
    LEDsTaskInput leds_input = {1, brightness, color_r, color_g, color_b, effect, 100};
    if (xQueueSendToBack(*g_queue_leds, &leds_input, 0) != pdTRUE) Serial.println("Error: Queue full");
    else Serial.println("LED test command sent"); 
}

void cmd_servo_test(int argc, char* argv[]) {
    if (argc < 3) {
        Serial.println("Usage: servo <index> <position>");
        return;
    }
    int servo_index = atoi(argv[1]);
    int position = atoi(argv[2]);
    if (servo_index < 0 || servo_index >= NUM_SERVOS) {
        Serial.println("Invalid servo index");
        return;
    }
    servos[servo_index].write(position);
}

void cmd_emotion_test(int argc, char* argv[]) {
    if (argc < 7) {
        Serial.println("Usage: emotion <angry> <disgusted> <happy> <neutral> <sad> <surprised>");
        return;
    }
    if (g_queue_servos == NULL) {
        Serial.println("Error: Servos queue not initialized");
        return;
    }
    ServosTaskInput servos_input = {
        1, // animatronics_on
        atoi(argv[1]), // angry
        atoi(argv[2]), // disgusted
        atoi(argv[3]), // happy
        atoi(argv[4]), // neutral
        atoi(argv[5]), // sad
        atoi(argv[6])  // surprised
    };
    if (xQueueSendToBack(*g_queue_servos, &servos_input, 0) != pdTRUE) Serial.println("Error: Queue full");    
    else Serial.println("Emotion test command sent");
}

void cmd_set_inputs(int argc, char* argv[]) {
    if (argc < 2) {
        Serial.println("Usage: inputs <14 comma-separated values>");
        return;
    }
    if (g_queue_leds == NULL || g_queue_servos == NULL) {
        Serial.println("Error: Queues not initialized");
        return;
    }
    char* input_str = argv[1];
    char* token;
    int input_values[14];
    int count = 0;
    token = strtok(input_str, ",");
    while (token != NULL && count < 14) {
        input_values[count++] = atoi(token);
        token = strtok(NULL, ",");
    }
    if (count != 14) {
        Serial.print("Error: Expected 14 values, got ");
        Serial.println(count);
        return;
    }
    LEDsTaskInput leds_input = {
        input_values[1],  // leds_on
        input_values[2],  // leds_brightness
        input_values[3],  // leds_color_r
        input_values[4],  // leds_color_g
        input_values[5],  // leds_color_b
        input_values[6],  // leds_effect
        input_values[7]   // leds_level
    };
    ServosTaskInput servos_input = {
        input_values[0],  // animatronics_on
        input_values[8],  // emotion_angry
        input_values[9],  // emotion_disgusted
        input_values[10],  // emotion_happy
        input_values[11],  // emotion_neutral
        input_values[12], // emotion_sad
        input_values[13]  // emotion_surprised
    };
    if (xQueueSendToBack(*g_queue_leds, &leds_input, 0) != pdTRUE) Serial.println("Error: LED queue full");
    if (xQueueSendToBack(*g_queue_servos, &servos_input, 0) != pdTRUE) Serial.println("Error: Servo queue full");
    Serial.println("Inputs updated successfully");
}

#endif // TEST_MODE
#endif // CLI_H