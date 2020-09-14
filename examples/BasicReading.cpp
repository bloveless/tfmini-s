/*
Example code for Benewake TFMiniS time-of-flight distance sensor.
by Brennon Loveless (September 13/2020)

This example code is in the public domain.

This example shows how to adjust the frame rate for automatic readings.
The sensor frame rate is set to 4Hz and the loop delays for 250ms. So
the sensor matches up with the speed with which we are reading on UART.
If your sensor frame rate is faster than you are reading from it then
you'll get hit by a lot of UART buffering and it will seem as though
the sensor is very slow. Matching the frame rate with the speed that
I'm reading is how I made the sensor feel performant.

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

  // Step 3: Set the frame rate to how quickly you are planning on reading from the device
  // Picking your frame rate is the most imporant part because UART will buffer readings and
  // if you aren't reading quickly enough then the sensor values will change VERY slowly.
  // You'll notice that I've set the frame rate to 4Hz and in the loop I'm delaying by 250ms
  // So I will be keeping up with the sensor readings. This is what makes the sensor seem
  // performant.
  tfminis.setFrameRate(4);

  // Step 4: Save setting. This is not necessary since we are setting the frame rate on every boot.
  tfminis.saveSettings();
}


void loop() {
  Measurement measurement = tfminis.getMeasurement();

  Serial.print("Dist: ");
  Serial.println(measurement.distance);
  Serial.print("Strength: ");
  Serial.println(measurement.strength);
  Serial.print("Temperature: ");
  Serial.println(measurement.temperature);

  // Wait some time before taking the next measurement. Match this with your frame rate.
  delay(250);
}
