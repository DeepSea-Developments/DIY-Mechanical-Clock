#ifndef _SEVEN_SEGMENT_
#define _SEVEN_SEGMENT_

#include <Adafruit_PWMServoDriver.h>
#include <Arduino.h>

#define PCA9685_I2C_ADDRESS 0x40
#define SEGMENT_NUMS 7
#define DIGIT_NUMS 10
#define SERVO_TINY_MOVE_DEFAULT 100

enum
{
    SEGMENT_1,
    SEGMENT_2,
    SEGMENT_3,
    SEGMENT_4,
    SEGMENT_5,
    SEGMENT_6,
    SEGMENT_7,
};

// enum
// {
//     SEGMENT_1,
//     SEGMENT_2,
//     SEGMENT_3,
//     SEGMENT_4,
//     SEGMENT_5,
//     SEGMENT_6,
//     SEGMENT_7,
//     SEGMENT_8,
// };

class SevenSegment
{
public:
    SevenSegment();
    SevenSegment(const uint8_t addr);
    SevenSegment(const uint8_t addr, const uint8_t pwm_num);
    bool begin();
    bool begin(const uint16_t servoPosON[SEGMENT_NUMS], const uint16_t servoPosOff[SEGMENT_NUMS]);
    bool begin(const uint16_t servoPosON[SEGMENT_NUMS], const uint16_t servoPosOff[SEGMENT_NUMS], const uint8_t servoTinyMove);
    void setState(const uint8_t num, bool state);
    void setFull();
    void setEmpty();
    void setNum(uint8_t num);
    void releaseSevenSegment();

private:
    Adafruit_PWMServoDriver *_servo_driver = NULL;
    uint8_t _i2cAddr;
    uint8_t _seg0DriverPos;
    bool _displayState[SEGMENT_NUMS] = {true, true, true, true, true, true, true};
    uint16_t _servoPosOn[SEGMENT_NUMS];
    uint16_t _servoPosOff[SEGMENT_NUMS];
    uint16_t _servoTinyMove;

    const uint16_t _defaultServoPosON[SEGMENT_NUMS] = {375, 375, 390, 350, 375, 390, 340};
    const uint16_t _defaultServoPosOff[SEGMENT_NUMS] = {200, 500, 490, 480, 250, 250, 200};
    const uint8_t _defaultDisplayState[SEGMENT_NUMS] = {0, 0, 0, 0, 0, 0, 0};

    const bool numberSegmentState[DIGIT_NUMS][SEGMENT_NUMS] = {
        {true, true, true, true, true, true, false},
        {false, true, true, false, false, false, false},
        {true, true, false, true, true, false, true},
        {true, true, true, true, false, false, true},
        {false, true, true, false, false, true, true},
        {true, false, true, true, false, true, true},
        {true, false, true, true, true, true, true},
        {true, true, true, false, false, false, false},
        {true, true, true, true, true, true, true},
        {true, true, true, true, false, true, true}};

    bool init();
    void setSegmentState(const uint8_t num, bool state);
    void setSegmentPos(const uint8_t num, uint16_t pos);
    void setSegmentSevenState(bool state);
};

#endif
