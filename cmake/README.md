# CMake scripts

## Encapsulated libraries

### Constraints

- Fine-grained control over dependencies for flexibility and to ease link-time substitution
- Prevent access to header files of transitive dependencies, i.e., if you call a function defined in `libB` you have to explicitly depend on it.
  There should not be a way to include a header provided by `B` without declaring that explicit dependency.
  This way, we avoid hidden source code dependencies.
  And each module only sees the header-files it actually needs.
- For convenience, we want to have a target combines implementation and headers.

We achieve these by (typically) defining four build targets for each of our modules.
Assuming we want to define a new library `A` that performs a direct call to library `B`.

### Target definitions

#### `A__hdrs`

Providing access to all public header files of our module `A`.

```CMake
add_library(A__hdrs INTERFACE)
target_include_directories(A__hdrs INTERFACE include)
```

#### `A__impl`

Containing the implementation of `A`. This target needs access to the headers of its dependent modules.

```CMake
add_library(A__impl A.c)
target_link_libraries(A__impl PRIVATE B__hdrs)
```

#### `A

Providing access to public headers for consuming targets (e.g., executable) and transitively providing dependent library implementations.

```CMake
add_library(A INTERFACE)
target_link_libraries(A INTERFACE A__impl A__hdrs)
```

Currently, the `elastic_ai_add_library` function provides most of this functionality.
However, using it will prevent the IDE from automatically discovering your targets for the sake of jump-to-definition and auto-completion.

## Building hardware dependent libraries

The build scripts will set a global variable `BUILDING_FOR_ELASTIC_NODE` to `true` if we are building for hardware (e.g, elastic-node, pico).
If you need to define a hardware dependent lib, e.g., because it calls into the _pico-SDK_, you should ask for that flag and define the `__impl` target as an empty interface library.

```cmake
if(BUILDING_FOR_ELASTIC_NODE)
  add_library(A__impl A.c)
  target_link_libraries(A__impl PRIVATE Pico::pico_runtime__hdrs)
else()
  add_library(A__impl INTERFACE)
endif ()
```

