/*
Example code for Benewake TFMiniS time-of-flight distance sensor.
by Brennon Loveless (September 13/2020)

This example code is in the public domain.

This example disables the automatic measurements and instead allows the user
to control how quickly the measurements are taken by manually triggering
measurements.

You may have to switch out the hardware Serial2 for a SoftwareSerial if
your device doesn't have multiple Serial devices onboard.

THIS SOFTWARE IS PROVIDED ''AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE AUTHOR(S) BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "Arduino.h"
#include "TFMiniS.h"

TFMiniS tfminis;

void setup() {
  // Step 1: Initialize hardware serial port (serial debug port)
  Serial.begin(115200);
  // wait for serial port to connect. Needed for native USB port only
  while (!Serial);

  Serial.println ("Initializing...");

  // Step 2: Initialize the data rate for the Serial port
  Serial2.begin(TFMINIS_BAUDRATE);

  tfminis.begin(&Serial2);

  // Step 3: Disable automatic measurement by setting the frame rate to 0Hz
  tfminis.setFrameRate(0);

  // Step 4: Save setting. This is not necessary since we are setting the frame rate on every boot.
  tfminis.saveSettings();
}


void loop() {
  Measurement measurement = tfminis.triggerMeasurement();

  Serial.print("Dist: ");
  Serial.println(measurement.distance);
  Serial.print("Strength: ");
  Serial.println(measurement.strength);
  Serial.print("Temperature: ");
  Serial.println(measurement.temperature);

  // Wait some time before taking the next measurement
  // without delay, measurement is super fast
  delay(250);
}
