#define TEST_BUILD

#include "network/Network.h"
#include "hardware/uart.h"
#include "../../extern/unity/src/unity.h"
#include "stdlib.h"


void setUp(void) {
    Network_DisconnectFromNetwork();
}

void tearDown(void) {}

void GetGoodTCPConnection() {
    uartMessage = "OK\0OK\0OK\0OK";
    Network_Init(false);
    NetworkCredentials credentials = {"123", "abc"};
    uartMessage = "WIFI GOT IP";
    Network_ConnectToNetwork(credentials);
    uartMessage = "CONNECT";
    Network_TCP_Open("123", 80);
}

void InvokeIrqHandler();

void ESPLIB_TEST_ORDERING_ALL_SUCCESSFULL(void) {
    NetworkStatusFlags flags = Network_GetStatusFlags();
    TEST_ASSERT_FALSE(flags.WIFIStatus || flags.TCPStatus || Network_TCP_IsResponseAvailable());

    uartMessage = "OK\0OK\0OK\0OK";
    TEST_ASSERT_TRUE(Network_Init(false));
    flags = Network_GetStatusFlags();
    TEST_ASSERT_FALSE(flags.WIFIStatus || flags.TCPStatus || Network_TCP_IsResponseAvailable());

    uartMessage = "WIFI GOT IP";
    NetworkCredentials credentials = {"123", "abc"};
    TEST_ASSERT_TRUE(Network_ConnectToNetwork(credentials));
    flags = Network_GetStatusFlags();
    TEST_ASSERT_FALSE(!flags.WIFIStatus || flags.TCPStatus || Network_TCP_IsResponseAvailable());

    uartMessage = "CONNECT";
    TEST_ASSERT_TRUE(Network_TCP_Open("123", 80));
    flags = Network_GetStatusFlags();
    TEST_ASSERT_FALSE(!flags.WIFIStatus || !flags.TCPStatus || Network_TCP_IsResponseAvailable());
}

void ESPLIB_TEST_ORDERING_CONNECT_TO_NETWORK_FAIL(void) {
    NetworkStatusFlags flags = Network_GetStatusFlags();
    TEST_ASSERT_FALSE(flags.WIFIStatus || flags.TCPStatus || Network_TCP_IsResponseAvailable());

    uartMessage = "OK\0OK\0OK\0OK";
    TEST_ASSERT_TRUE(Network_Init(false));
    flags = Network_GetStatusFlags();
    TEST_ASSERT_FALSE(flags.WIFIStatus || flags.TCPStatus || Network_TCP_IsResponseAvailable());

    uartMessage = "Error";
    NetworkCredentials credentials = {"123", "abc"};
    TEST_ASSERT_FALSE(Network_ConnectToNetwork(credentials));
    flags = Network_GetStatusFlags();
    TEST_ASSERT_FALSE(flags.WIFIStatus || flags.TCPStatus || Network_TCP_IsResponseAvailable());

    uartMessage = "Error";
    TEST_ASSERT_FALSE(Network_TCP_Open("123", 80));
    flags = Network_GetStatusFlags();
    TEST_ASSERT_FALSE(flags.WIFIStatus || flags.TCPStatus || Network_TCP_IsResponseAvailable());
}

void ESPLIB_TEST_ORDERING_CONNECT_TO_TCP_FAIL(void) {
    NetworkStatusFlags flags = Network_GetStatusFlags();
    TEST_ASSERT_FALSE(flags.WIFIStatus || flags.TCPStatus || Network_TCP_IsResponseAvailable());

    uartMessage = "OK\0OK\0OK\0OK";
    TEST_ASSERT_TRUE(Network_Init(false));
    flags = Network_GetStatusFlags();
    TEST_ASSERT_FALSE(flags.WIFIStatus || flags.TCPStatus || Network_TCP_IsResponseAvailable());

    uartMessage = "WIFI GOT IP";
    NetworkCredentials credentials = {"123", "abc"};
    TEST_ASSERT_TRUE(Network_ConnectToNetwork(credentials));
    flags = Network_GetStatusFlags();
    TEST_ASSERT_FALSE(!flags.WIFIStatus || flags.TCPStatus || Network_TCP_IsResponseAvailable());

    uartMessage = "Error";
    TEST_ASSERT_FALSE(Network_TCP_Open("123", 80));
    flags = Network_GetStatusFlags();
    TEST_ASSERT_FALSE(!flags.WIFIStatus || flags.TCPStatus || Network_TCP_IsResponseAvailable());
}

void ESPLIB_TEST_Stay_OPEN_ON_GOOD_RESPONSE(void) {
    GetGoodTCPConnection();
    uartMessage = "+IPD,5:Hallo";
    InvokeIrqHandler();
    char *buf;

    TEST_ASSERT_TRUE(Network_TCP_IsResponseAvailable());
    buf = Network_TCP_GetResponse();
    TEST_ASSERT_EQUAL_STRING("Hallo", buf);
    free(buf);
}

void ESPLIB_TEST_CLOSE_ON_CLOSE_RESPONSE(void) {
    GetGoodTCPConnection();
    uartMessage = "+IPD,5:Hallo\r\nCLOSED";
    InvokeIrqHandler();
    char *buf;

    TEST_ASSERT_TRUE(Network_TCP_IsResponseAvailable());
    NetworkStatusFlags flags = Network_GetStatusFlags();
    TEST_ASSERT_FALSE(!Network_TCP_IsResponseAvailable() || !flags.TCPStatus);
    buf = Network_TCP_GetResponse();
    TEST_ASSERT_EQUAL_STRING("Hallo", buf);
    free(buf);

    TEST_ASSERT_FALSE(Network_TCP_IsResponseAvailable());
    flags = Network_GetStatusFlags();
    TEST_ASSERT_FALSE(flags.TCPStatus);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(ESPLIB_TEST_ORDERING_ALL_SUCCESSFULL);
    RUN_TEST(ESPLIB_TEST_ORDERING_CONNECT_TO_NETWORK_FAIL);
    RUN_TEST(ESPLIB_TEST_ORDERING_CONNECT_TO_TCP_FAIL);

    RUN_TEST(ESPLIB_TEST_Stay_OPEN_ON_GOOD_RESPONSE);
    RUN_TEST(ESPLIB_TEST_CLOSE_ON_CLOSE_RESPONSE);

    return UNITY_END();
}
