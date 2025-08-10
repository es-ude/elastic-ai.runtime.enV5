#include <malloc.h>
#include <stdio.h>

#include "Linear.h"
#include "pico/bootrom.h"
#include "pico/stdlib.h"

linearConfig_t *initLinearConfig() {
    size_t weightSize = 6;
    size_t biasSize = 2;
    float weight[] = {-1.f, 2.f, -3.f, 4.f, 5.f, -6.f};
    float bias[] = {-1.f, 3.f};
    return initLinearConfigWithWeightBias(weight, weightSize, bias, biasSize);
}

int main() {
    stdio_init_all();
    sleep_ms(2000);

    struct mallinfo before, after;

    before = mallinfo();

    linearConfig_t *linearConfig = initLinearConfig();

    float input[] = {0.f, 1.f, 2.f};
    float *output = linearForward(linearConfig, input);

    after = mallinfo();

    printf("\nMemory Usage:\n");
    printf("Before: total allocated = %d bytes\n", before.uordblks);
    printf("After : total allocated = %d bytes\n", after.uordblks);
    printf("Total bytes used = %d bytes\n", after.uordblks - before.uordblks);

    rom_reset_usb_boot(0, 0);
}
