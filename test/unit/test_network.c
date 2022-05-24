#define TEST_BUILD

#include "Network.h"
#include "tcp.h"
#include "hardware/uart.h"
#include "unity.h"
#include "stdlib.h"

void setUp(void) {
    Network_DisconnectFromNetwork();
}

void tearDown(void) {}

void GetGoodTCPConnection() {
    uartMessage = "OK\0OK\0OK\0OK";
    Network_init();
    NetworkCredentials credentials = {"123", "abc"};
    uartMessage = "WIFI GOT IP";
    Network_ConnectToNetwork(credentials);
    uartMessage = "CONNECT";
    TCP_Open("123", 80);
}

void InvokeIrqHandler();

void TEST_ORDERING_ALL_SUCCESSFUL(void) {
    NetworkStatus_t flags = NetworkStatus;
    TEST_ASSERT_FALSE(flags.WIFIStatus || flags.TCPStatus);

    uartMessage = "OK\0OK\0OK\0OK";
    TEST_ASSERT_TRUE(Network_init());
    flags = NetworkStatus;
    TEST_ASSERT_FALSE(flags.WIFIStatus || flags.TCPStatus);

    uartMessage = "WIFI GOT IP";
    NetworkCredentials credentials = {"123", "abc"};
    TEST_ASSERT_TRUE(Network_ConnectToNetwork(credentials));
    flags = NetworkStatus;
    TEST_ASSERT_FALSE(!flags.WIFIStatus || flags.TCPStatus);

    uartMessage = "CONNECT";
    TEST_ASSERT_TRUE(TCP_Open("123", 80));
    flags = NetworkStatus;
    TEST_ASSERT_FALSE(!flags.WIFIStatus || !flags.TCPStatus);
}

void TEST_ORDERING_CONNECT_TO_NETWORK_FAIL(void) {
    NetworkStatus_t flags = NetworkStatus;
    TEST_ASSERT_FALSE(flags.WIFIStatus || flags.TCPStatus);

    uartMessage = "OK\0OK\0OK\0OK";
    TEST_ASSERT_TRUE(Network_init());
    flags = NetworkStatus;
    TEST_ASSERT_FALSE(flags.WIFIStatus || flags.TCPStatus);

    uartMessage = "Error";
    NetworkCredentials credentials = {"123", "abc"};
    TEST_ASSERT_FALSE(Network_ConnectToNetwork(credentials));
    flags = NetworkStatus;
    TEST_ASSERT_FALSE(flags.WIFIStatus || flags.TCPStatus);

    uartMessage = "Error";
    TEST_ASSERT_FALSE(TCP_Open("123", 80));
    flags = NetworkStatus;
    TEST_ASSERT_FALSE(flags.WIFIStatus || flags.TCPStatus);
}

void TEST_ORDERING_CONNECT_TO_TCP_FAIL(void) {
    NetworkStatus_t flags = NetworkStatus;
    TEST_ASSERT_FALSE(flags.WIFIStatus || flags.TCPStatus);

    uartMessage = "OK\0OK\0OK\0OK";
    TEST_ASSERT_TRUE(Network_init());
    flags = NetworkStatus;
    TEST_ASSERT_FALSE(flags.WIFIStatus || flags.TCPStatus);

    uartMessage = "WIFI GOT IP";
    NetworkCredentials credentials = {"123", "abc"};
    TEST_ASSERT_TRUE(Network_ConnectToNetwork(credentials));
    flags = NetworkStatus;
    TEST_ASSERT_FALSE(!flags.WIFIStatus || flags.TCPStatus);

    uartMessage = "Error";
    TEST_ASSERT_FALSE(TCP_Open("123", 80));
    flags = NetworkStatus;
    TEST_ASSERT_FALSE(!flags.WIFIStatus || flags.TCPStatus);
}

void TEST_Stay_OPEN_ON_GOOD_RESPONSE(void) {
    GetGoodTCPConnection();
    uartMessage = "+IPD,5:Hallo";
    InvokeIrqHandler();
    char *buf;

    buf = TCP_GetResponse();
    TEST_ASSERT_EQUAL_STRING("Hallo", buf);
    free(buf);
}

void TEST_CLOSE_ON_CLOSE_RESPONSE(void) {
    GetGoodTCPConnection();
    uartMessage = "+IPD,5:Hallo\r\nCLOSED";
    InvokeIrqHandler();
    char *buf;

    NetworkStatus_t flags = NetworkStatus;
    TEST_ASSERT_FALSE( !flags.TCPStatus);
    buf = TCP_GetResponse();
    TEST_ASSERT_EQUAL_STRING("Hallo", buf);
    free(buf);

    flags = NetworkStatus;
    TEST_ASSERT_FALSE(flags.TCPStatus);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(TEST_ORDERING_ALL_SUCCESSFUL);
    RUN_TEST(TEST_ORDERING_CONNECT_TO_NETWORK_FAIL);
    RUN_TEST(TEST_ORDERING_CONNECT_TO_TCP_FAIL);

    RUN_TEST(TEST_Stay_OPEN_ON_GOOD_RESPONSE);
    RUN_TEST(TEST_CLOSE_ON_CLOSE_RESPONSE);

    return UNITY_END();
}
