#include "TFMiniS.h"

bool TFMiniS::begin(Stream* _streamPtr) {
    streamPtr = _streamPtr;
}

Measurement TFMiniS::triggerMeasurement() {
    uint8_t command[] = {0x5A, 0x04, 0x04, 0x62};

    writeFrame(command, sizeof(command));

    return getMeasurement();
}

Measurement TFMiniS::getMeasurement() {
    uint8_t frame[9] = {0x59, 0x59, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t bytes_read = 0;
    uint8_t last_byte = 0;

    while (bytes_read < TFMINIS_MAX_WAIT_BYTES) {
        while (!streamPtr->available());

        uint8_t current_byte = streamPtr->read();

        if (current_byte == 0x59 && last_byte == 0x59) {
            break;
        } else {
            last_byte = current_byte;
            bytes_read += 1;
        }
    }

    for (uint8_t i = 2; i < sizeof(frame); i += 1) {
        frame[i] = streamPtr->read();
    }

    struct Measurement response;
    response.distance = (((uint16_t) frame[3] << 8) + frame[2]);
    response.strength = (((uint16_t) frame[5] << 8) + frame[4]);

    uint16_t raw_temperature = (((uint16_t) frame[7] << 8) + frame[6]);

    // Convert raw temperature to deg C
    response.temperature = raw_temperature / 8 - 256;

    return response;
}

void TFMiniS::getSoftwareVersion(char* version_out, uint8_t length) {
    uint8_t frame[7];
    uint8_t command[] = {0x5A, 0x04, 0x01, 0x5F};
    char version[17];

    writeFrame(command, sizeof(command));
    readCommandFrame(frame, sizeof(frame));

    sprintf(version, "Version: %u.%u.%u", frame[5], frame[4], frame[3]);

    strncpy(version_out, version, length);
}

bool TFMiniS::systemReset() {
    uint8_t frame[5];
    uint8_t command[] = {0x5A, 0x04, 0x02, 0x60};

    writeFrame(command, sizeof(command));
    readCommandFrame(frame, sizeof(frame));

    for (uint8_t i = 0; i < sizeof(frame); i += 1) {
        Serial.println(frame[i]);
    }

    // TODO: This command is returning 0x00 0x61 for some reason which is neither a success or a failure
    bool return_value = false;
    if (frame[3] == 0x00 && frame[4] == 0x60) {
        Serial.println("System reset was a success");
        return_value = true;
    } else if (frame[3] == 0x01 && frame[4] == 0x61) {
        Serial.println("System reset failed");
    } else {
        Serial.println("Command didn't return a valid return");
    }

    delay(1000);

    return return_value;
}

/**
 * setFrameRate will set the number of measurements taken per second
 * 
 * @param frame_rate Is the new frame rate in Hz
 **/
bool TFMiniS::setFrameRate(uint16_t frame_rate) {
    uint8_t response[6];
    uint8_t command[] = {0x5A, 0x06, 0x03, 0x00, 0x00, 0x00};
    // low byte
    command[3] = (uint8_t) (frame_rate & 0x00ff);
    // high byte
    command[4] = (uint8_t) (frame_rate >> 8);
    // checksum
    for (uint8_t i = 0; i < sizeof(command) - 1; i += 1) {
        command[sizeof(command) - 1] += command[i];
    }

    writeFrame(command, sizeof(command));
    readCommandFrame(response, sizeof(response));

    for (uint8_t i = 0; i < sizeof(command); i += 1) {
        if (response[i] != command[i]) {
            return false;
        }
    }

    return true;
}

bool TFMiniS::saveSettings() {
    uint8_t command[] = {0x5A, 0x04, 0x11, 0x6F};
    uint8_t frame[5];
    bool return_value = false;

    writeFrame(command, sizeof(command));
    readCommandFrame(frame, sizeof(frame));

    if (frame[3] == 0x00 && frame[4] == 0x70) {
        if (TFMINIS_DEBUGMODE == 1) {
            Serial.println("Settings saved");
        }
        return_value = true;
    } else if (frame[3] == 0x01 && frame[4] == 0x71) {
        if (TFMINIS_DEBUGMODE == 1) {
            Serial.println("Failed to save settings");
        }
    } else if (TFMINIS_DEBUGMODE == 1) {
        Serial.println("Unknown response from saving settings");
    }

    return return_value;
}

void TFMiniS::printFrame(uint8_t* frame, uint8_t length) {
    Serial.println("-- Begin Frame ---------------------");
    for(uint8_t i = 0; i < length; i += 1) {
        Serial.print(frame[i]);
        Serial.print(": ");
        Serial.println(frame[i], HEX);
    }
    Serial.println("-- End Frame -----------------------");
}

void TFMiniS::writeFrame(uint8_t* frame, uint8_t length) {
    for (uint8_t i = 0; i < length; i += 1) {
        streamPtr->write((uint8_t) frame[i]);
    }
}

bool TFMiniS::readCommandFrame(uint8_t* frame, uint8_t length) {
    uint8_t bytes_read = 0;

    while (bytes_read < TFMINIS_MAX_WAIT_BYTES) {
        if (streamPtr->available()) {
            uint8_t current_byte = streamPtr->read();
            if (current_byte == 0x5A) {
                // Found header
                frame[0] = current_byte;
                break;
            }
        }
    }

    if (bytes_read == TFMINIS_MAX_WAIT_BYTES) {
        Serial.println("Timed out reading command frame");
        return false;
    }

    uint8_t checksum = 0x5A;
    for (uint8_t i = 0; i < length - 1; i += 1) {
        frame[i+1] = streamPtr->read();

        // Add everything except the checksum byte
        if (i != length - 2) {
            checksum += frame[i+1];
        }
    }

    if (checksum != frame[length-1]) {
        Serial.println("Unable to verify checksum");
        Serial.print(checksum);
        Serial.print(" != ");
        Serial.println(frame[length-1]);

        return false;
    }

    return true;
}
