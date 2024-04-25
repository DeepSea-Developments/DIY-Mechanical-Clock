
#include "SevenSegment.h"

SevenSegment::SevenSegment() : _i2cAddr(PCA9685_I2C_ADDRESS), _seg0DriverPos(0) {}
SevenSegment::SevenSegment(const uint8_t addr) : _i2cAddr(addr), _seg0DriverPos(0) {}
SevenSegment::SevenSegment(const uint8_t addr, const uint8_t offset) : _i2cAddr(addr), _seg0DriverPos(offset) {}

bool SevenSegment::begin()
{
    return begin(_defaultServoPosON, _defaultServoPosOff, SERVO_TINY_MOVE_DEFAULT);
}

bool SevenSegment::begin(const uint16_t servoPosON[SEGMENT_NUMS], const uint16_t servoPosOff[SEGMENT_NUMS])
{
    return begin(servoPosON, servoPosOff, SERVO_TINY_MOVE_DEFAULT);
}
bool SevenSegment::begin(const uint16_t servoPosON[SEGMENT_NUMS], const uint16_t servoPosOff[SEGMENT_NUMS], const uint8_t servoTinyMove)
{

    if (!init())
        return false;

    memcpy(_servoPosOn, servoPosON, sizeof(_servoPosOn));
    memcpy(_servoPosOff, servoPosOff, sizeof(_servoPosOff));

    _servoTinyMove = servoTinyMove;

    setFull();

    return true;
}
void SevenSegment::setState(const uint8_t num, bool state)
{

    if (num == SEGMENT_7)
    {
        setSegmentSevenPos(state);
    }
    else
    {
        setSegmentState(num, state);
    }
    delay(10);
}
void SevenSegment::setFull()
{
    for (int i = SEGMENT_7; i >= 0; i--)
    {
        setState(i, true);
    }
}
void SevenSegment::setEmpty()
{
    for (int i = SEGMENT_7; i >= 0; i--)
    {
        setState(i, false);
    }
}
void SevenSegment::setNum(uint8_t num)
{
    for (int i = SEGMENT_7; i >= 0; i--)
    {
        setState(i, numberSegmentState[num][i]);
    }
}
void SevenSegment::releaseSevenSegment()
{
    if (_displayState[SEGMENT_2] == true)
        setSegmentPos(SEGMENT_2, _servoPosOn[1] - _servoTinyMove);
    if (_displayState[SEGMENT_6] == true)
        setSegmentPos(SEGMENT_6, _servoPosOn[1] + _servoTinyMove);
}
bool SevenSegment::init()
{

    if (_servo_driver)
        delete _servo_driver;

    _servo_driver = new Adafruit_PWMServoDriver(_i2cAddr);

    if (!_servo_driver->begin()) // Start each board
        return false;

    _servo_driver->setOscillatorFrequency(27000000); // Set the PWM oscillator frequency, used for fine calibration
    _servo_driver->setPWMFreq(50);                   // Set the servo operating frequency

    return true;
}
void SevenSegment::setSegmentState(const uint8_t num, bool state)
{
    if (state)
        _servo_driver->setPWM(_seg0DriverPos + num, 0, _servoPosOn[num]);
    else
        _servo_driver->setPWM(_seg0DriverPos + num, 0, _servoPosOff[num]);

    _displayState[num] = state;
}
void SevenSegment::setSegmentPos(const uint8_t num, uint16_t pos)
{
    _servo_driver->setPWM(_seg0DriverPos + num, 0, pos);
}
void SevenSegment::setSegmentSevenPos(bool state)
{
    if (_displayState[SEGMENT_7] != state)
    {
        releaseSevenSegment();
    }
    delay(100);
    setSegmentState(SEGMENT_7, state);

    _displayState[SEGMENT_7] = state;
}
