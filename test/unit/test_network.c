#define TEST_BUILD

#include "hardware/uart.h"
#include "Network.h"
#include "unity.h"

void setUp(void) {
    Network_DisconnectFromNetwork();
}

void tearDown(void) {}

void GetGoodNetworkConnection() {
    uartMessage = "OK\0OK\0OK\0OK";
    Network_init();
    NetworkCredentials credentials = {"123", "abc"};
    uartMessage = "WIFI GOT IP";
    Network_ConnectToNetwork(credentials);
    uartMessage = "CONNECT";
}

void InvokeIrqHandler();

void TEST_ORDERING_ALL_SUCCESSFUL(void) {
    NetworkStatus_t flags = NetworkStatus;
    TEST_ASSERT_FALSE(flags.WIFIStatus);

    uartMessage = "OK\0OK\0OK\0OK";
    TEST_ASSERT_TRUE(Network_init());
    flags = NetworkStatus;
    TEST_ASSERT_FALSE(flags.WIFIStatus);

    uartMessage = "WIFI GOT IP";
    NetworkCredentials credentials = {"123", "abc"};
    TEST_ASSERT_TRUE(Network_ConnectToNetwork(credentials));
    flags = NetworkStatus;
    TEST_ASSERT_FALSE(!flags.WIFIStatus);
}

void TEST_ORDERING_CONNECT_TO_NETWORK_FAIL(void) {
    NetworkStatus_t flags = NetworkStatus;
    TEST_ASSERT_FALSE(flags.WIFIStatus);

    uartMessage = "OK\0OK\0OK\0OK";
    TEST_ASSERT_TRUE(Network_init());
    flags = NetworkStatus;
    TEST_ASSERT_FALSE(flags.WIFIStatus);

    uartMessage = "Error";
    NetworkCredentials credentials = {"123", "abc"};
    TEST_ASSERT_FALSE(Network_ConnectToNetwork(credentials));
    flags = NetworkStatus;
    TEST_ASSERT_FALSE(flags.WIFIStatus);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(GetGoodNetworkConnection);
    RUN_TEST(TEST_ORDERING_ALL_SUCCESSFUL);
    RUN_TEST(TEST_ORDERING_CONNECT_TO_NETWORK_FAIL);

    return UNITY_END();
}
