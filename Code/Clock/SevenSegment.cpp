
#include "SevenSegment.h"

// Constructors Only Init variables
SevenSegment::SevenSegment() : _i2cAddr(PCA9685_I2C_ADDRESS), _seg0DriverPos(0) {}
SevenSegment::SevenSegment(const uint8_t addr) : _i2cAddr(addr), _seg0DriverPos(0) {}
SevenSegment::SevenSegment(const uint8_t addr, const uint8_t offset) : _i2cAddr(addr), _seg0DriverPos(offset) {}

// begin routines start PCA9685 , ON/Off positions and default state of the servos
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

    // copy the position configuration to internal library variables

    memcpy(_servoPosOn, servoPosON, sizeof(_servoPosOn));
    memcpy(_servoPosOff, servoPosOff, sizeof(_servoPosOff));
    _servoTinyMove = servoTinyMove;

    // move  all the segments to show number 8 as default state
    setFull();

    return true;
}
// Set an specific  segment state On/off , true or false
void SevenSegment::setState(const uint8_t num, bool state)
{

    if (num == SEGMENT_7)
    {
        setSegmentSevenState(state);
    }
    else
    {
        setSegmentState(num, state);
    }
    delay(10);
}
// Set all the segment ON
void SevenSegment::setFull()
{
    for (int i = SEGMENT_7; i >= 0; i--)
    {
        setState(i, true);
    }
}
// Set all the segment Off
void SevenSegment::setEmpty()
{
    for (int i = SEGMENT_7; i >= 0; i--)
    {
        setState(i, false);
    }
}

// Show a number in the segments
void SevenSegment::setNum(uint8_t num)
{
    for (int i = SEGMENT_7; i >= 0; i--)
    {
        setState(i, numberSegmentState[num][i]);
    }
}
// Move side segments to allow move mid segment(Seven)
void SevenSegment::releaseSevenSegment()
{
    if (_displayState[SEGMENT_3] == true)
        setSegmentPos(SEGMENT_3, _servoPosOn[1] + _servoTinyMove);
    if (_displayState[SEGMENT_5] == true)
        setSegmentPos(SEGMENT_5, _servoPosOn[1] - _servoTinyMove);
}
// Initialize and configure PCA9685 driver
bool SevenSegment::init()
{

    if (_servo_driver)
        delete _servo_driver;

    _servo_driver = new Adafruit_PWMServoDriver(_i2cAddr);

    if (!_servo_driver->begin()) // Start driver
        return false;

    _servo_driver->setOscillatorFrequency(27000000); // Set the PWM oscillator frequency, used for fine calibration
    _servo_driver->setPWMFreq(50);                   // Set the servo operating frequency

    return true;
}
// set segment state On/Off .Only used at low level
void SevenSegment::setSegmentState(const uint8_t num, bool state)
{
    if (state)
        _servo_driver->setPWM(_seg0DriverPos + num, 0, _servoPosOn[num]);
    else
        _servo_driver->setPWM(_seg0DriverPos + num, 0, _servoPosOff[num]);

    _displayState[num] = state;
}
// set segment to an specific position (angle) .Only used at low level
void SevenSegment::setSegmentPos(const uint8_t num, uint16_t pos)
{
    _servo_driver->setPWM(_seg0DriverPos + num, 0, pos);
}
// set segment #7 state , this include moving side segments .Only used at low level
void SevenSegment::setSegmentSevenState(bool state)
{
    if (_displayState[SEGMENT_7] != state)
    {
        releaseSevenSegment();
        delay(100); // wait till server move

        setSegmentState(SEGMENT_7, state);
        _displayState[SEGMENT_7] = state;
        delay(100); // wait till server move

        setSegmentState(SEGMENT_3, _displayState[SEGMENT_3]);
        setSegmentState(SEGMENT_5, _displayState[SEGMENT_5]);
    }
}
