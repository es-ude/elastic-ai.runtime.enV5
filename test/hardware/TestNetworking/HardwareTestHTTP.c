#define SOURCE_FILE "HTTP-TEST"

/*!
 * Connect to Wi-Fi and requests HTTP response every 3 seconds!
 */

#include "CException.h"

#include "HardwareTestHelper.h"
#include "eai/Common.h"
#include "eai/network/HTTP.h"

void _Noreturn runTest(void) {
    PRINT("=== STARTING TEST ===");
    CEXCEPTION_T exception;
    while (1) {
        Try {
            HttpResponse_t *response = NULL;
            HTTPGet("http://127.0.0.1:5000/check", &response);
            PRINT("HTPPGet Success!\n\tResponse Length: %li\n\tResponse: %s", response->length,
                  response->response);
            HTTPCleanResponseBuffer(response);
        }
        Catch(exception) {
            PRINT("HTTPGet failed!");
        }
    }
}

int main() {
    initHardwareTest();
    connectToNetwork();
    runTest();
}
