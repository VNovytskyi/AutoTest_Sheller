#include <QtTest>

#include "string.h"

extern "C" {
    #include "../../../Libs/sheller/Source/sheller.h"
}


static const uint16_t crc16_table[256] = {
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
    0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
    0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6,
    0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
    0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485,
    0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4,
    0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
    0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
    0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12,
    0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,
    0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41,
    0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
    0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70,
    0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,
    0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F,
    0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E,
    0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
    0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D,
    0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
    0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C,
    0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB,
    0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
    0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A,
    0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
    0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9,
    0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
    0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8,
    0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0
};

inline uint16_t test_get_crc(const uint8_t *data, const uint16_t length)
{
    uint16_t crc = 0xFFFF;
    for(uint8_t i = 0; i < length; ++i){
         crc = (crc << 8) ^ crc16_table[(crc >> 8) ^ *data++];
    }

    return crc;
}

inline void test_get_crc_by_byte(uint16_t *crc_value, const uint8_t byte)
{
    *crc_value = (*crc_value << 8) ^ crc16_table[(*crc_value >> 8) ^ byte];
}

#define TEST_START_BYTE 0x23
#define TEST_DATA_LENGTH 8
#define TEST_RX_BUFF_LENGTH 128


class base : public QObject
{
    Q_OBJECT

    sheller_t shell;

public:
    base();
    ~base();

private slots:
    void initSheller();
    void wrappingTest1();
    void wrappingTest2();
    void wrappingTest3();
    void readNormalMessage();
    void readDamagedMessage();
    void readNormalMessage2();
    void readSeparate();
    void readNormalMessage3();
    void trickySize();
};

base::base()
{

}

base::~base()
{

}

void base::initSheller()
{
    uint8_t init_result = sheller_init(&shell, TEST_START_BYTE, TEST_DATA_LENGTH, TEST_RX_BUFF_LENGTH);
    if (init_result == SHELLER_ERROR) {
        QFAIL("Sheller initialization failed");
    }
}

void base::wrappingTest1()
{
    uint8_t message[] = { 0, 1, 2, 3, 4, 5, 6, 7 }; //0x14
    uint8_t message_wrapered[128] = {0, };
    uint8_t wrap_result = sheller_wrap(&shell, message, sizeof(message), message_wrapered);
    if (wrap_result == SHELLER_ERROR) {
        QFAIL("Wrappering message failed: sheller_wrap return error");
    }

    QCOMPARE(message_wrapered[0], TEST_START_BYTE);

    uint16_t crc = 0xFFFF;
    for (uint8_t i = 0; i < sizeof(message); ++i) {
        if (message_wrapered[i + 1] != message[i]) {
            QFAIL("Wrappering message failed: wrote data not exists");
        }
        test_get_crc_by_byte(&crc, message[i]);
    }

    for (uint8_t i = 0; i < (TEST_DATA_LENGTH - sizeof(message)); ++i) {
        test_get_crc_by_byte(&crc, 0);
    }

    uint8_t crcL = crc & 0xFF;
    uint8_t crcH = (crc >> 8) & 0xFF;

    if (crcL != message_wrapered[sheller_get_package_length(&shell) - 2] || crcH != message_wrapered[sheller_get_package_length(&shell) - 1]) {
        QFAIL("Wrappering message failed: not equil crc");
    }
}

void base::wrappingTest2()
{
    uint8_t message[] = { 15, 1, 32, 34, 4, 64, 9, 12 }; //0xA0
    uint8_t message_wrapered[128] = {0, };
    uint8_t wrap_result = sheller_wrap(&shell, message, sizeof(message), message_wrapered);
    if (wrap_result == SHELLER_ERROR) {
        QFAIL("Wrappering message failed: sheller_wrap return error");
    }

    QCOMPARE(message_wrapered[0], TEST_START_BYTE);

    uint16_t crc = 0xFFFF;
    for (uint8_t i = 0; i < sizeof(message); ++i) {
        if (message_wrapered[i + 1] != message[i]) {
            QFAIL("Wrappering message failed: wrote data not exists");
        }
        test_get_crc_by_byte(&crc, message[i]);
    }

    for (uint8_t i = 0; i < (TEST_DATA_LENGTH - sizeof(message)); ++i) {
        test_get_crc_by_byte(&crc, 0);
    }

    uint8_t crcL = crc & 0xFF;
    uint8_t crcH = (crc >> 8) & 0xFF;

    if (crcL != message_wrapered[sheller_get_package_length(&shell) - 2] || crcH != message_wrapered[sheller_get_package_length(&shell) - 1]) {
        QFAIL("Wrappering message failed: not equil crc");
    }
}

void base::wrappingTest3()
{
    uint8_t message[] = { 5, 5, 2, 1, 5 };
    uint8_t message_wrapered[11] = {0, };
    uint8_t wrap_result = sheller_wrap(&shell, message, sizeof(message), message_wrapered);
    if (wrap_result == SHELLER_ERROR) {
        QFAIL("Wrappering message failed: sheller_wrap return error");
    }

    QCOMPARE(message_wrapered[0], TEST_START_BYTE);

    uint16_t crc = 0xFFFF;
    for (uint8_t i = 0; i < sizeof(message); ++i) {
        if (message_wrapered[i + 1] != message[i]) {
            QFAIL("Wrappering message failed: wrote data not exists");
        }
        test_get_crc_by_byte(&crc, message[i]);
    }

    for (uint8_t i = 0; i < (TEST_DATA_LENGTH - sizeof(message)); ++i) {
        test_get_crc_by_byte(&crc, 0);
    }

    uint8_t crcL = crc & 0xFF;
    uint8_t crcH = (crc >> 8) & 0xFF;

    if (crcL != message_wrapered[sheller_get_package_length(&shell) - 2] || crcH != message_wrapered[sheller_get_package_length(&shell) - 1]) {
        QFAIL("Wrappering message failed: not equil crc");
    }
}

void base::readNormalMessage()
{
    uint8_t message[] = { 0x23, 0x4c, 0x60, 0xf1, 0xca, 0x46, 0xd6, 0x80, 0x60, 0x46, 0x2d };
    for (uint8_t i = 0; i < sizeof(message); ++i) {
        sheller_push(&shell, message[i]);
    }

    uint8_t received_message[8] = {0};
    bool result = sheller_read(&shell, received_message);
    if (result == true) {
        if (strncmp((char*)(message + 1), (char*)received_message, 8)) {
            QFAIL("Reading message failed: not equil send and receive message");
        }
    } else {
        QFAIL("Reading message failed: no received data");
    }
}

void base::readDamagedMessage()
{
    uint8_t message[] = { 0x23, 0x00, 0x01, 0x02, 0x03, 0x04, 0x04, 0x06, 0x07, 0x14 };
    for (uint8_t i = 0; i < sizeof(message); ++i) {
        sheller_push(&shell, message[i]);
    }

    uint8_t received_message[8] = {0};
    bool result = sheller_read(&shell, received_message);
    if (result == true) {
        QFAIL("Reading message unexpected successfull: the message is deliberately damaged but it was accepted");
    }
}

void base::readNormalMessage2()
{
    uint8_t message[] = { 0x23, 0xac, 0x27, 0xbb, 0x63, 0xd7, 0xc2, 0xec, 0xfe, 0x65, 0xf4 };
    for (uint8_t i = 0; i < sizeof(message); ++i) {
        sheller_push(&shell, message[i]);
    }

    uint8_t received_message[8] = {0};
    sheller_read(&shell, received_message);
    bool result = sheller_read(&shell, received_message);
    if (result == true) {
        if (strncmp((char*)(message + 1), (char*)received_message, 8)) {
            QFAIL("Reading message failed: not equil send and receive message");
        }
    } else {
        QFAIL("Reading message failed: no received data");
    }
}

void base::readSeparate()
{
    uint8_t message1[] = { 0x23, 0x4c, 0x60, 0xf1, 0xca, 0x46, 0xd6, 0x80, 0x60, 0x46, 0x2d };
    uint8_t message2[] = { 0x23, 0xac, 0x27, 0xbb, 0x63, 0xd7, 0xc2, 0xec, 0xfe, 0x65, 0xf4 };

    for (uint8_t i = 0; i < sizeof(message1); ++i) {
        sheller_push(&shell, message1[i]);
    }

    sheller_push(&shell, message2[0]);
    sheller_push(&shell, message2[1]);
    sheller_push(&shell, message2[2]);
    sheller_push(&shell, message2[3]);
    sheller_push(&shell, message2[4]);
    sheller_push(&shell, message2[5]);
    sheller_push(&shell, message2[6]);
    sheller_push(&shell, message2[7]);
    sheller_push(&shell, message2[8]);
    sheller_push(&shell, message2[9]);

    uint8_t received_message[8] = {0};
    bool result = sheller_read(&shell, received_message);
    if (result == true) {
        if (strncmp((char*)(message1 + 1), (char*)received_message, 8)) {
            QFAIL("Reading message1 failed: not equil send and receive message");
        }
    } else {
        QFAIL("Reading message failed: no received data");
    }

    sheller_read(&shell, received_message);


    sheller_push(&shell, message2[10]);


    uint8_t received_message2[8] = {0};
    result = sheller_read(&shell, received_message2);
    if (result == true) {
        if (strncmp((char*)(message2 + 1), (char*)received_message2, 8)) {
            QFAIL("Reading message2 failed: not equil send and receive message");
        }
    } else {
        QFAIL("Reading message failed: no received data");
    }
}


void base::readNormalMessage3()
{
    uint8_t message[] = { 0x23, 0xac, 0x27, 0xbb, 0x63, 0xd7, 0xc2, 0xec, 0xfe, 0x65, 0xf4 };
    for (uint8_t i = 0; i < sizeof(message); ++i) {
        sheller_push(&shell, message[i]);
    }

    uint8_t received_message[8] = {0};
    //sheller_read(&shell, received_message);
    bool result = sheller_read(&shell, received_message);
    if (result == true) {
        if (strncmp((char*)(message + 1), (char*)received_message, 8)) {
            QFAIL("Reading message failed: not equil send and receive message");
        }
    } else {
        QFAIL("Reading message failed: no received data");
    }
}

void base::trickySize()
{
    sheller_push(&shell, 0x0C);
    uint8_t message[] = { 0x23, 0x4c, 0x60, 0xf1, 0xca, 0x46, 0xd6, 0x80, 0x60, 0x46, 0x2d };
    for (uint8_t i = 0; i < (sizeof(message) - 1); ++i) {
        sheller_push(&shell, message[i]);
    }

    bool result = false;
    uint8_t received_message[8] = {0};

    result = sheller_read(&shell, received_message); //Test case 5
    sheller_push(&shell, 0x2d);
    result = sheller_read(&shell, received_message);

    if (result == true) {
        if (strncmp((char*)(message + 1), (char*)received_message, 8)) {
            QFAIL("Reading message failed: not equil send and receive message");
        }
    } else {
        QFAIL("Reading message failed: no received data");
    }
}


QTEST_APPLESS_MAIN(base)

#include "tst_base.moc"
