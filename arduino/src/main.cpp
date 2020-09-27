#include <Adafruit_INA219.h>
#include <Arduino.h>
#include <SFE_MicroOLED.h>

const int OLED_CS = 10;
const int OLED_RST = 9;
const int OLED_DC = 8;

MicroOLED oled(OLED_RST, OLED_DC, OLED_CS);

const int LED_PINR = LEDR;
const int LED_PING = LEDG;
const int LED_PINB = LEDB;

const int LOAD_SIM = 2;

Adafruit_INA219 ina219(0x41);

void setup() {
	// Serial
	Serial.begin(115200);

	// Initialize INA219
	if (!ina219.begin()) {
		Serial.println("Failed to find INA219");
		while (1) delay(100);
	}

	// Initialize load output
	pinMode(LOAD_SIM, OUTPUT);

	// Initialize RGB LED
	pinMode(LED_PINR, OUTPUT);
	pinMode(LED_PING, OUTPUT);
	pinMode(LED_PINB, OUTPUT);
	digitalWrite(LED_PINR, HIGH);
	digitalWrite(LED_PING, HIGH);
	digitalWrite(LED_PINB, HIGH);

	// Initialize, clear screen
	oled.begin();
	oled.setFontType(0);
	oled.clear(PAGE);
	oled.clear(ALL);
	oled.display();
}

uint32_t counter = 0;
float avgbuf[40];
size_t avgbufidx = 0;

void loop() {
	counter++;

	uint32_t phase = (counter / 4) % 512;

	// Hard blink
	if (phase > 255) {
		phase = 255;
	} else {
		phase = 0;
	}

	// Smooth blink
	//phase = phase > 255 ? 512 - phase : phase;
	//phase = (phase * phase) / 255;

	// Update load simulator and red LED for status
	analogWrite(LED_PINR, 255 - phase);
	analogWrite(LOAD_SIM, phase);

	delay(5);

	// Update current average
	avgbuf[avgbufidx] = ina219.getCurrent_mA();
	avgbufidx = (avgbufidx + 1) % (sizeof(avgbuf) / sizeof(float));

	if (counter % 20 == 0) {
		oled.clear(PAGE);
		float c = 0.0f;
		for (size_t i = 0; i < sizeof(avgbuf) / sizeof(float); ++i)
			c += avgbuf[i];
		c /= sizeof(avgbuf) / sizeof(float);

		oled.setCursor(0, 0);
		if (c >= 0.0f) {
			oled.print(" ");
		}
		oled.print(c);
		oled.println(" mA");
		oled.display();
	}
}
