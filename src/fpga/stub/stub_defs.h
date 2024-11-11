#ifndef ENV5_STUB_DEVS_H
#define ENV5_STUB_DEVS_H

#ifndef MIDDLEWARE_STUB_VERSION
#error "You have to specify the stub version with -DMIDDLEWARE_STUB_VERSION=<version_number>"
#else
#ifdef MIDDLEWARE_STUB_VERSION
#if MIDDLEWARE_STUB_VERSION == 1
#include "stub_defs_v1.h"
#elif MIDDLEWARE_STUB_VERSION == 2
#include "stub_defs_v2.h"
#else
#error "Unsupported MIDDLEWARE_STUB_VERSION"
#endif
#endif
#endif

#endif // ENV5_STUB_DEVS_H
