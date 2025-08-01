# Network stack

## Hardware

Based on the `espressif ESP32-C3-WROOM-02` Wi-Fi module.
The extension board hosts the module and connects it to the MCU
by Serial Peripheral Interface (SPI).

The module currently operates based on the standard espressif binary,
that based the communication with the MCU on the string based
[espressif protocol (ESP)](#espressif-protocol).

> [!NOTE]  
> [data sheet](https://www.espressif.com/sites/default/files/documentation/esp32-c3-wroom-02_datasheet_en.pdf)

## Espressif protocol

The ESP protocol is a String-based protocol based on the
[AT command set](#at-commands) to send and receive data.

### AT-commands

The [AT-Commands](https://docs.espressif.com/projects/esp-at/en/release-v2.2.0.0_esp8266/AT_Command_Set/index.html)
are the core of the ESP protocols, and this network stack uses only a subset.
The [`AtCommands.h` file](atCommands/include/AtCommands.h) has the provided commands.

## Wi-Fi connection

The Wi-Fi library under [wifi/](wifi/) provides all required functions
to connect to a Wi-Fi network.

Pass the Service Set Identifier (SSID) of the network and the Password to
the library to connect to a network.
The CMake scripts will automatically create a `NetworkConfig.c` inside
the [root folder](../../NetworkConfig.c).
When you want to use Wi-Fi capabilities you than have to add
the `network_config` library to your source code.

### Wi-Fi example

```C title=main.c
#include "Network.h"

int main (void) {
  //...
  networkConnectToNetwork();
  //...
}
```

```C title=NetworkConfig.c
#include "Network.h"

// ...

networkCredentials_t networkCredentials = {
  .ssid = "SSID",
  .password = "PASSWORD"
};
```

```CMake title=CMakeLists.txt
add_executable(my-network-app
  main.c)
target_link_libraries(my-network-app
  ...
  network_config
  network_lib)

create_enV5_executable(my-network-app)
```

## HTTP

The HTTP library provided under [http/](http/) provides all required
functions to run HTTP-GET requests.

To properly handle the received answer, register a handle that processes
the received HTTP-GET messages from the ESP32-module.
This handle is automatically set when the esp connects to a Wi-Fi network with
the [Wi-Fi library](#wi-fi-connection).

## MQTT

The MQTT library provided under [broker/](broker/) provides all required
functions for a MQTT client that can connect to a Broker.

Register a handle to properly process the received messages from the ESP32-module.
This handler is automatically applied when the broker connection was successful.

Pass the required credentials to the library to connect to the broker.
The CMake scripts will automatically create a `NetworkConfig.c` inside
the [root folder](../../NetworkConfig.c).
When you want to use MQTT capabilities you have to add
the `network_config` library to your source code.

> [!CAUTION]  
> The `clientId` of every device has to be different

### MQTT example

```C title=main.c
#include "MQTTBroker.h"

char[] brokerDomain = "eip://uni-due.de/es";
char[] clientId = "enV5";

int main (void) {
  //...
  mqttBrokerConnectToBroker(brokerDomain, clientId);
  //...
}
```

```C title=NetworkConfig.c
#include "MQTTBroker.h"

// ...

mqttHost_t broker = {
  .ip = "127.0.0.1",
  .port = 1883,
  .userId = "USER",
  .password = "PASSWORD"
};
```

```CMake title=CMakeLists.txt
add_executable(my-network-app
  main.c)
target_link_libraries(my-network-app
  ...
  network_config
  espBroker_lib)
create_enV5_executable(my-network-app)
```

> [!IMPORTANT]
> If you send MQTT messages too fast to the device, it will drop some messages!
