#define START 0
#define GAME 1
#define GAMERESET 2
#define LOSE 3
uint8_t state = START;
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif
#define PIN       5
#define NUMPIXELS 8
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
#define RED   6
#define GREEN 7
#define BLUE  8
#define ISPRESSED 1
#define SELECTED 1
#define NOTSELECTED 0
#define STARTED 1
#define ENDED 0
#define ON 100
#define OFF 0
uint16_t flashcount;
uint8_t buttons[3];
uint8_t colors[3];
uint8_t buttonpins[3] = {RED, GREEN, BLUE};
uint32_t timeThis;
uint32_t timeLast;
uint8_t pixelcount;
uint8_t alreadyselected;
uint8_t start;
uint16_t timebtwnpixels = 300;
uint8_t flashes;
void setup() {
  for (int x; x < 3; x++) {
    colors[x] = 0;
    buttons[x] = 0;
    alreadyselected = 0;
    pixelcount = 0;
  }

  pixels.begin();
  Serial.begin(9600);
  pinMode(4, INPUT);
  pinMode(3, INPUT);
  pinMode(2, INPUT);

}

void loop() {
  switch (state) {
    case START:
      for (int x = 0; x < 3; x++) {
        if (digitalRead(buttonpins[x]) == ISPRESSED) {
          state = GAME;
          for (int x = -1; x < 3; x++) {
            colors[x] = 0;
          }
          colors[1] = 100;
        }
      }

      pixelcount = 0;
      flashcount = 0;
      timebtwnpixels = 300;
      pixels.clear();
      delay(200);
      break;

    case GAME:
      if (pixelcount >= NUMPIXELS - 1) {
        state = GAMERESET;
      }
      // Checks which button is being pressed and sets button to a color
      for (int x = 0; x < 3; x++) {
        if ((digitalRead(buttonpins[x]) == ISPRESSED) && (alreadyselected == NOTSELECTED)) {
          buttons[x] = ON;
          alreadyselected = SELECTED;
          break;
        }
      }
      pixels.setPixelColor(7, pixels.Color(buttons[0], buttons[1], buttons[2]));

      // Move pixels upward
      timeThis = millis();
      if (timeThis - timeLast >= timebtwnpixels + 100) {
        pixels.setPixelColor(pixelcount++, pixels.Color(colors[0], colors[1], colors[2]));
        pixels.show();
        timeLast = timeThis;
      }
      break;

    case GAMERESET:
      state = GAME;
      timebtwnpixels *= .95;
      for (int x; x < 3; x++) {
        if (buttons[x] != colors[x]) {
          state = LOSE;
        }
      }
      flashcount ++;
      for (int x; x < 3; x++) {
        colors[x] = 0;
        buttons[x] = 0;
        alreadyselected = 0;
        pixelcount = 0;
      }
      pixels.clear();
      colors[random(0, 3)] = 100;
      break;
    case LOSE:
      pixels.clear();
      for (flashes = 0; flashes <= flashcount; flashes++) {
        for (pixelcount = 0; pixelcount <= NUMPIXELS; pixelcount ++) {
          pixels.setPixelColor(pixelcount, pixels.Color(ON, ON, ON));
          pixels.show();
        }
        delay(50);
        for (pixelcount = 0; pixelcount <= NUMPIXELS; pixelcount ++) {
          pixels.setPixelColor(pixelcount, pixels.Color(OFF, OFF, OFF));
          pixels.show();
        }
        delay(50);
      }
      flashcount -= 1;
      pixels.clear();
      for (int x = 0; x <= NUMPIXELS; x++) {
        int y = flashcount;
        y &= 1;
        flashcount >>= 1;
        if (y == 1) {
          pixels.setPixelColor(x, pixels.Color(ON, ON, ON));
          pixels.show();
        }

      }
      state = START;
      break;

  }
}
