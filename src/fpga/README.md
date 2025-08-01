# Debugging

The [Vivado-Debugger-Guide](Vivado_Debugger.adoc) provides some information for
debugging the Field-Programmable-Gate-Array (FPGA) with help of Vivado.

## Libraries

### FPGA configuration handler

The [FPGA Config Handler library](fpgaConfigurationHandler/) provides functions
to easily download FPGA configuration files from the internet or
from USB in the local Flash storage.

### Middleware

[The middleware folder](middleware) has the middleware implementation.

Further information about the middleware can is in the
[elastic-ai.creator repository](https://github.com/es-ude/elastic-ai.creator)

### Stub

The stub implementation is in the [stub folder](stub).
It provides the counter part for the skeleton defined for the FPGA.

It is possible to define your own custom stub implementation for AI accelerator
by defining your memory configuration by providing your own `stub_defs.h` file
for the `stub_lib`:

```C title="stub.h"
#ifndef STUB_DEFINES
#define STUB_DEFINES

  #ifndef ADDR_COMPUTATION_ENABLE
  #define ADDR_COMPUTATION_ENABLE 16
  #endif

  #ifndef ADDR_SKELETON_INPUTS
  #define ADDR_SKELETON_INPUTS 17
  #endif

  #ifndef ADDR_MODEL_ID
  #define ADDR_MODEL_ID 0
  #endif

  #ifndef BYTES_MODEL_ID
  #define BYTES_MODEL_ID 16
  #endif

#endif
```

> [!IMPORTANT]
> We currently offer support for version 1 and version 2 of the AI skeleton.

Further information about the stub is in the
[elastic-ai.creator repository](https://github.com/es-ude/elastic-ai.creator)
