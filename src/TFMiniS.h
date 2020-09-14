#include "Arduino.h"

#define TFMINIS_BAUDRATE 115200
#define TFMINIS_DEBUGMODE 1
#define TFMINIS_MAX_WAIT_BYTES 30

struct Measurement {
    uint16_t distance;
    uint16_t strength;
    uint16_t temperature;
};

class TFMiniS {
private:
    Stream* streamPtr;
    bool readCommandFrame(uint8_t* frame, uint8_t length);
    void writeFrame(uint8_t* frame, uint8_t length);
    void printFrame(uint8_t* frame, uint8_t length);

public:
    bool begin(Stream* _streamPtr);
    void getSoftwareVersion(char* version_out, uint8_t length);
    bool setFrameRate(uint16_t frame_rate);
    bool systemReset();
    bool saveSettings();
    Measurement getMeasurement();
    Measurement triggerMeasurement();
};
