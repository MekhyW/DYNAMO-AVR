#include "TestConfig.h"
#ifdef SIMULATE_HARDWARE

#ifndef Arduino_FreeRTOS_h
#define Arduino_FreeRTOS_h

#include <thread>
#include <chrono>
#include <functional>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>

typedef struct {
    std::thread* thread;
    bool running;
} TaskHandle_t;

template <typename T>
class QueueHandle {
private:
    std::queue<T> queue;
    std::mutex mutex;
    std::condition_variable cv;
    size_t maxSize;
public:
    QueueHandle(size_t size) : maxSize(size) {}
    bool send(T item, unsigned long timeout) {
        std::unique_lock<std::mutex> lock(mutex);
        if (queue.size() >= maxSize) {
            return false;
        }
        queue.push(item);
        cv.notify_one();
        return true;
    }
    bool receive(T* item, unsigned long timeout) {
        std::unique_lock<std::mutex> lock(mutex);
        if (timeout > 0) {
            cv.wait_for(lock, std::chrono::milliseconds(timeout), [this]{ return !queue.empty(); });
        }
        if (queue.empty()) {
            return false;
        }
        *item = queue.front();
        queue.pop();
        return true;
    }
};

#define portTICK_PERIOD_MS 1
#define pdTRUE 1
#define pdFALSE 0
#define portMAX_DELAY ULONG_MAX

inline void xTaskCreate(void (*function)(void*), const char* name, 
                 int stackSize, void* parameters, int priority, TaskHandle_t* handle) {
    TaskHandle_t newTask = new TaskHandle_t;
    newTask->running = true;
    newTask->thread = new std::thread([function, parameters, newTask]() {
        while (newTask->running) {
            function(parameters);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    });
    
    if (handle != NULL) {
        *handle = newTask;
    }
}

inline void vTaskDelay(const unsigned long ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

template <typename T>
inline QueueHandle<T>* xQueueCreate(int size, int itemSize) {
    return new QueueHandle<T>(size);
}

template <typename T>
inline int xQueueSendToBack(QueueHandle<T>* queue, T* item, unsigned long timeout) {
    return queue->send(*item, timeout) ? pdTRUE : pdFALSE;
}

template <typename T>
inline int xQueueReceive(QueueHandle<T>* queue, T* item, unsigned long timeout) {
    return queue->receive(item, timeout) ? pdTRUE : pdFALSE;
}

#endif // Arduino_FreeRTOS_h

#ifndef Arduino_h
#define Arduino_h

#include <iostream>
#include <string>
#include <cstdint>
#include <chrono>
#include <thread>

#define HIGH 1
#define LOW 0
#define INPUT 0
#define OUTPUT 1
#define INPUT_PULLUP 2

class SerialClass {
public:
    void begin(long speed) {
        std::cout << "Serial initialized at " << speed << " baud" << std::endl;
    }
    bool available() {
        return std::cin.rdbuf()->in_avail() > 0;
    }
    char read() {
        char c;
        std::cin.get(c);
        return c;
    }
    void print(const char* str) {
        std::cout << str;
    }
    void print(int value) {
        std::cout << value;
    }
    void print(float value) {
        std::cout << value;
    }
    void println(const char* str) {
        std::cout << str << std::endl;
    }
    void println(int value) {
        std::cout << value << std::endl;
    }
    void println(float value) {
        std::cout << value << std::endl;
    }
    void println() {
        std::cout << std::endl;
    }
    String readStringUntil(char terminator) {
        std::string input;
        std::getline(std::cin, input);
        return String(input.c_str());
    }
    operator bool() const { return true; }
};

class String {
private:
    std::string str;
public:
    String() : str("") {}
    String(const char* s) : str(s) {}
    String(const std::string& s) : str(s) {}
    const char* c_str() const { return str.c_str(); }
    int length() const { return str.length(); }
    int indexOf(char c, int start = 0) const {
        size_t pos = str.find(c, start);
        return (pos == std::string::npos) ? -1 : pos;
    }
    String substring(int start, int end) const {
        return String(str.substr(start, end - start).c_str());
    }
    int toInt() const {
        return std::stoi(str);
    }
};

void pinMode(uint8_t pin, uint8_t mode) {
    std::cout << "Pin " << (int)pin << " set to mode " << (int)mode << std::endl;
}

void digitalWrite(uint8_t pin, uint8_t val) {
    std::cout << "Pin " << (int)pin << " set to " << (val ? "HIGH" : "LOW") << std::endl;
}

int digitalRead(uint8_t pin) {
    return HIGH; // Default to HIGH for simulation
}

unsigned long millis() {
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    return duration.count();
}

void delay(unsigned long ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

SerialClass Serial;

#endif // Arduino_h

#endif // SIMULATE_HARDWARE