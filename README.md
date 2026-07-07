# ESP32-S3 FreeRTOS Task Synchronization and CPU Utilization Analysis

## Project Overview

This project demonstrates task synchronization and CPU utilization analysis using FreeRTOS on the ESP32-S3 microcontroller.

The application interfaces two sensors:

- DHT11 Temperature & Humidity Sensor
- MAX30100 Pulse Oximeter Sensor

Multiple FreeRTOS tasks are created to read sensor data. A Binary Semaphore is used to synchronize task execution and prevent resource conflicts. The project also compares CPU utilization in single-core and dual-core execution.

---

## Hardware Used

- ESP32-S3 Development Board
- DHT11 Temperature & Humidity Sensor
- MAX30100 Pulse Oximeter Sensor

---

## Software & Tools Used

- ESP-IDF
- FreeRTOS
- Embedded C
- Visual Studio Code

---

## Key Concepts

- FreeRTOS Task Creation
- Task Priorities
- Binary Semaphore
- CPU Utilization Analysis
- Single-Core Execution
- Dual-Core Execution
- I2C Communication

---

## Project Features

- Sensor interfacing using I2C
- FreeRTOS multitasking
- Binary Semaphore synchronization
- CPU utilization monitoring
- Task priority analysis
- Single-core vs Dual-core comparison

---

## Repository Structure

```text
ESP32_FreeRTOS_Task_Synchronization

├── code
│   ├── blink_led.c
│   ├── CPU_Utilization_using_Semaphore.c
│   └── sensor_interfacing.c
│
├── CODES.pdf
├── MTech_Thesis Documentation.pdf
└── README.md
```

---

## Future Improvements

- Add Queue implementation
- Add Mutex implementation
- Display sensor readings on OLED/LCD
- Send sensor data to cloud using Wi-Fi

---

## Author

Kulsum Shireen
