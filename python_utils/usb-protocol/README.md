# enV5 Remote Control Protocol

In this file the communication protocol is explained.

The communication is driven by the external device to determine the elasticNodes behavior.
The communication protocol is as follows. For each communication an ack or nack is sent by the receiver.
As a default we recommend to throw an error after 5 failed attempts.

The byteorder for number conversion is big endian.

This protocol only works with skeleton V2.

## Transmission

**Transmission**

| command | payload-size |      payload |      checksum |
|--------:|-------------:|-------------:|--------------:|
| 1 bytes |      4 bytes | payload-size | Checksum-size |

## Checksum-function

1 byte XOR over all command + payload-size + payload bytes

## Commands

The first 128 are our commands.
The second 128 can be user specific commands.

**Commands**

|                  command | uint8 |
|-------------------------:|:------|
|                     nack | 0     |
|                      ack | 1     |
|         read skeleton id | 2     |
| get chunk-size for flash | 3     |
|       send data to flash | 4     |
|     read data from flash | 5     |
|               FPGA power | 6     |
|                FPGA LEDs | 7     |
|                 MCU LEDs | 8     |
|      inference with data | 9     |

### nack

The payload for the nack response is empty

```mermaid
sequenceDiagram
    Sender -->> Receiver: {command, payload}
    Receiver -->> Sender: {nack , NULL}
```

### ack

The payload for the ack response is empty

```mermaid
sequenceDiagram
    Sender -->> Receiver: {command, payload}
    Receiver -->> Sender: {ack , NULL}
```

### read skeleton id

**❗ IMPORTANT**\
Model needs to be deployed in advance.

The payload for the nack response is empty

```mermaid
sequenceDiagram
    PC -->> enV5: {read skeleton id, NULL}
    enV5 -->> PC: ack/nack
    enV5 -->> PC: {read skeleton id, skeleton id(16)}
    PC -->> enV5: ack/nack
```

### get chunk-size for flash

**Payload**

| chunk-size |
|------------|
| 31-0       |

```mermaid
sequenceDiagram
    PC -->> enV5: {get chunk-size for flash, NULL}
    enV5 -->> PC: ack/nack
    enV5 -->> PC: {get chunk-size for flash, payload}
    PC -->> enV5: ack/nack
```

### send data to flash

**❗ IMPORTANT**\
Chunk-size has to be requested in advance.

**❗ IMPORTANT**\
Turn off FPGA in advance.

```mermaid
sequenceDiagram
    PC -->> enV5: {send data to flash, flash sector address(4), size-bytes(4)}
    enV5 -->> PC: ack/nack
    loop for each chunk
        PC -->> enV5: {send data to flash, chunk}
        enV5 -->> PC: ack/nack
    end
```

### read data from flash

**❗ IMPORTANT**\
Chunk-size has to be requested in advance.

**❗ IMPORTANT**\
Turn off FPGA in advance.

```mermaid
sequenceDiagram
    PC -->> enV5: {read data from flash, flash start address(4), num-bytes(4)}
    enV5 -->> PC: ack/nack
    loop for each chunk
        enV5 -->> PC: {read data from flash, chunk}
        PC -->> enV5: ack/nack
    end
```

### FPGA power

```mermaid
sequenceDiagram
    alt Power on
        PC -->> enV5: {FPGA power, 0xFF}
        enV5 -->> PC: ack/nack
    else Power off
        PC -->> enV5: {FPGA power, 0x00}
        enV5 -->> PC: ack/nack
    end
```

### FPGA LEDs

**Payload**

| Bit | FPGA LED |
|----:|:---------|
| 7-4 | None     |
|   3 | 4        |
|   2 | 3        |
|   1 | 2        |
|   0 | 1        |

Mermaid Diagram

```mermaid
sequenceDiagram
    PC -->> enV5: {FPGA LEDs, payload}
    enV5 -->> PC: ack/nack
```

### MCU LEDs

**Payload**

| Bit | MCU LED |
|----:|:--------|
| 7-3 | None    |
|   2 | 3       |
|   1 | 2       |
|   0 | 1       |

Mermaid Diagram

```mermaid
sequenceDiagram
    PC -->> enV5: {MCU LEDs, payload}
    enV5 -->> PC: ack/nack
```

### inference with data

**❗ IMPORTANT**\
Chunk-size has to be requested in advance.

**❗ IMPORTANT**\
Deploy model in advance

```mermaid
sequenceDiagram
    PC -->> enV5: {inference with data, num_bytes_input(4), num_bytes_outputs(4), Bin-file address(4), Skeleton-ID(16)}
    enV5 -->> PC: ack/nack
    loop for each chunk
        PC -->> enV5: {inference with data, chunk}
        enV5 -->> PC: ack/nack
    end
    Note over enV5: Run inference
    loop for each chunk
        enV5 -->> PC: {inference with data, chunk}
        PC -->> enV5: ack/nack
    end
```

### deploy model

**❗ IMPORTANT**\
Chunk-size has to be requested in advance.

**❗ IMPORTANT**\
Model needs to be send to flash in advance.

**❗ IMPORTANT**\
FPGA needs to be turned on.

**payload**

| Bit | Payload    |
|----:|:-----------|
| 7-1 | None       |
|   0 | true/false |

Payload is true if models skeleton id is correct. Otherwise, false is returned

```mermaid
sequenceDiagram
    PC -->> enV5: {deploy model, Bin-file address(4), Skeleton-ID(16)}
    enV5 -->> PC: ack/nack
    Note over enV5: Call stub for model deployment
    Note over enV5: Get skeleton-id of deployed model with stub
    enV5 -->> PC: {deploy model, payload(1)}
    PC -->> enV5: ack/nack
```
