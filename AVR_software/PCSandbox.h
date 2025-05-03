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

typedef struct TaskHandle {
    std::thread* thread;
    bool running;
} *TaskHandle_t;

typedef struct QueueHandle {
    std::queue<void*> queue;
    std::mutex mutex;
    std::condition_variable cv;
    size_t maxSize;
} *QueueHandle_t;

#define portTICK_PERIOD_MS 1
#define pdTRUE 1
#define pdFALSE 0
#define portMAX_DELAY ULONG_MAX

inline void xTaskCreate(void (*function)(void*), const char* name, int stackSize, void* parameters, int priority, TaskHandle_t* handle) {
    TaskHandle_t taskHandle = new TaskHandle;
    taskHandle->running = true;
    taskHandle->thread = new std::thread([function, parameters, taskHandle]() {
        while (taskHandle->running) {
            function(parameters);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    });
    if (handle != NULL) *handle = taskHandle;
}

inline void vTaskDelay(const unsigned long ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

inline QueueHandle_t xQueueCreate(int size, unsigned long long itemSize) {
    QueueHandle_t queueHandle = new QueueHandle;
    queueHandle->maxSize = size;
    return queueHandle;
}

inline int xQueueSendToBack(QueueHandle_t queue, void* item, unsigned long timeout) {
    std::unique_lock<std::mutex> lock(queue->mutex);
    if (queue->queue.size() >= queue->maxSize) return pdFALSE;
    void* itemCopy = malloc(sizeof(item));
    memcpy(itemCopy, item, sizeof(item));
    queue->queue.push(itemCopy);
    queue->cv.notify_one();
    return pdTRUE;
}

inline int xQueueReceive(QueueHandle_t queue, void* item, unsigned long timeout) {
    std::unique_lock<std::mutex> lock(queue->mutex);
    if (timeout == portMAX_DELAY) {
        queue->cv.wait(lock, [queue] { return !queue->queue.empty(); });
    } 
    else if (timeout > 0) {
        queue->cv.wait_for(lock, std::chrono::milliseconds(timeout), [queue] { return !queue->queue.empty(); });
    }
    if (queue->queue.empty()) return pdFALSE;
    void* queueItem = queue->queue.front();
    memcpy(item, queueItem, sizeof(item));
    free(queueItem);
    queue->queue.pop();
    return pdTRUE;
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

class SerialClass {
public:
    void begin(long speed) {
        std::cout << "Serial initialized at " << speed << " baud" << std::endl;
    }
    bool available() {
        return std::cin.rdbuf()->in_avail() > 0 || std::cin.peek() != EOF;
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

void pinMode(uint8_t pin, uint8_t mode) {
    std::cout << "Pin " << (int)pin << " set to mode " << (int)mode << std::endl;
}

void digitalWrite(uint8_t pin, uint8_t val) {
    std::cout << "Pin " << (int)pin << " set to " << (val ? "HIGH" : "LOW") << std::endl;
}

int digitalRead(uint8_t pin) {
    return LOW;
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