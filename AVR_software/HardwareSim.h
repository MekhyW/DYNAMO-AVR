#ifndef HARDWARE_SIM_H
#define HARDWARE_SIM_H

#include "TestConfig.h"

#ifdef SIMULATE_HARDWARE

#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#define NEO_GRB ((1 << 6) | (1 << 4) | (0 << 2) | (2))
#define NEO_KHZ800 0x0000

class Adafruit_NeoPixel_Sim {
private:
    int num_pixels;
    int pin;
    uint16_t type;
    int brightness;
    std::vector<uint32_t> pixels;
    
public:
    Adafruit_NeoPixel_Sim(int n, int p, uint16_t t) : num_pixels(n), pin(p), type(t), brightness(50) {
        pixels.resize(n, 0);
    }
    void begin() { 
        std::cout << "NeoPixel strip initialized with " << num_pixels << " pixels on pin " << pin << std::endl; 
    }
    void show() { 
        std::cout << "NeoPixel update: " << pixels.size() << " pixels, brightness: " << brightness << std::endl; 
    }
    void setBrightness(int b) { 
        brightness = b; 
    }
    int getBrightness() { return brightness; }
    void setPixelColor(int i, uint32_t c) {
        if (i >= 0 && i < num_pixels) pixels[i] = c;
    }
    uint32_t getPixelColor(int i) {
        if (i >= 0 && i < num_pixels) return pixels[i];
        return 0;
    }
    int numPixels() { 
        return num_pixels; 
    }
    void clear() {
        for (int i = 0; i < num_pixels; i++) pixels[i] = 0;
        std::cout << "NeoPixel strip cleared" << std::endl;
    }
    void fill(uint32_t c, int first, int count) {
        for (int i = first; i < first + count && i < num_pixels; i++) pixels[i] = c;
    }
    uint32_t Color(uint8_t r, uint8_t g, uint8_t b) {
        return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
    }
    uint32_t gamma32(uint32_t color) { 
        return color; // Simplified
    }
    uint32_t ColorHSV(uint16_t hue, uint8_t sat = 255, uint8_t val = 255) {
        return Color(val, val, val); // Just return white for simplicity
    }
};

class Servo_Sim {
private:
    int pin;
    bool is_attached;
    int position;
    
public:
    Servo_Sim() : pin(-1), is_attached(false), position(90) {}
    void attach(int p) {
        pin = p;
        is_attached = true;
        std::cout << "Servo attached to pin " << pin << std::endl;
    }
    void detach() {
        is_attached = false;
        std::cout << "Servo on pin " << pin << " detached" << std::endl;
    }
    void write(int pos) {
        position = pos;
        if (is_attached) std::cout << "Servo on pin " << pin << " moved to position " << position << std::endl;
        else std::cout << "Warning: Trying to move detached servo on pin " << pin << std::endl;
    }
    int read() { 
        return position; 
    }
    bool attached() { 
        return is_attached; 
    }
};

#define Adafruit_NeoPixel Adafruit_NeoPixel_Sim
#define Servo Servo_Sim

#endif // SIMULATE_HARDWARE
#endif // HARDWARE_SIM_H