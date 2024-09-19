// https://docs.espressif.com/projects/arduino-esp32/en/latest/api/rmt.html


// Copyright 2024 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/**
 * @brief This example demonstrates usage of RGB LED driven by RMT
 *
 * The output is a visual WS2812 RGB LED color moving in a 8 x 4 LED matrix
 * Parameters can be changed by the user. In a single LED circuit, it will just blink.
 */
#define PIN_LED_RGB 8

#define NR_OF_LEDS     12
#define NR_OF_ALL_BITS 24 * NR_OF_LEDS

//
// Note: This example uses a board with 32 WS2812b LEDs chained one
//      after another, each RGB LED has its 24 bit value
//      for color configuration (8b for each color)
//
//      Bits encoded as pulses as follows:
//
//      "0":
//         +-------+              +--
//         |       |              |
//         |       |              |
//         |       |              |
//      ---|       |--------------|
//         +       +              +
//         | 0.4us |   0.85 0us   |
//
//      "1":
//         +-------------+       +--
//         |             |       |
//         |             |       |
//         |             |       |
//         |             |       |
//      ---+             +-------+
//         |    0.8us    | 0.4us |

rmt_data_t led_data[NR_OF_ALL_BITS];

void setup() {
  Serial.begin(115200);
  if (!rmtInit(PIN_LED_RGB, RMT_TX_MODE, RMT_MEM_NUM_BLOCKS_1, 10000000)) {
    Serial.println("init sender failed\n");
  }
  Serial.println("real tick set to: 100ns");
}

int color[] = {0x11, 0x99, 0x11};  // Green Red Blue values
int led_index = 0;

void loop() 
{
  // Init data with only one led ON
  int led, col, bit;
  int i = 0;
  for (led = 0; led < NR_OF_LEDS; led++) {
    for (col = 0; col < 3; col++) {
      for (bit = 0; bit < 8; bit++) {
        if ((color[col] & (1 << (7 - bit))) && (led == led_index)) {
          led_data[i].level0 = 1;
          led_data[i].duration0 = 8;
          led_data[i].level1 = 0;
          led_data[i].duration1 = 4;
        } else {
          led_data[i].level0 = 1;
          led_data[i].duration0 = 4;
          led_data[i].level1 = 0;
          led_data[i].duration1 = 8;
        }
        i++;
      }
    }
  }
  // make the led travel in the panel
  if ((++led_index) >= NR_OF_LEDS) {
    led_index = 0;
  }
  // Send the data and wait until it is done
  rmtWrite(PIN_LED_RGB, led_data, NR_OF_ALL_BITS, RMT_WAIT_FOR_EVER);
//  delay(100);
}
