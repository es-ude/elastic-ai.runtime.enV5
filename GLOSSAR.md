# Glossary

This file aims to provide a broad overview of commonly used terms in this project.

|                   Term |        Scope        | Description                                                                                         |
|-----------------------:|:-------------------:|:----------------------------------------------------------------------------------------------------|
|                   enV5 |         --          | Hardware with the MCU, Flash, FPGA, ...                                                             |
|                  Flash |         --          | Flash storage on the board                                                                          | 
|                    HAL |         --          | Hardware Abstraction Layer, Libraries to bundle direct hardware dependencies (uart, spi, gpio, ...) |
| Config / Configuration |  FPGA, Middleware   | bin file used to configure the FPGA                                                                 |
|               FreeRTOS |       Network       | Open Source Real Time Operating System, Used to emulate Threads                                     |
|            ESP32 / ESP |       Network       | WiFi Module from espressif                                                                          |
|            AT Commands | Network, MQTT, HTTP | String based command set to control the ESP32 WiFi module                                           |
|                 Broker |    MQTT, Network    | MQTT User implementation, used to publish and subscribe to topics                                   |
