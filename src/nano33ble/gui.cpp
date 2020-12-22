#include <Adafruit_SSD1306.h>
#include <Fonts/Picopixel.h>

#include "gui.hpp"
#include "nano33ble/project_pins.h"

extern const uint8_t PROGMEM splash1_data[];

class OledGuiImpl : public GuiImpl {
	Adafruit_SSD1306 oled_;
	char buf_[64];

      public:
	OledGuiImpl() : oled_(64, 48, &SPI, TPS_PIN_OLED_DC, TPS_PIN_OLED_RESET, TPS_PIN_OLED_CS) {}

	void begin() override {
		oled_.begin(SSD1306_SWITCHCAPVCC, 0, true, true);
		oled_.display();
	}

	void clear() override { oled_.clearDisplay(); }

	void display() override { oled_.display(); }

	void show_splash(const SplashData &data) override {
		oled_.drawBitmap(0, 0, splash1_data, oled_.width(), oled_.height(), WHITE);
	}

	void show_stats(const StatsData &data) override {
		oled_.setTextColor(WHITE);
		oled_.setFont();
		oled_.setCursor(0, 0);

		// Print current measure
		sprintf(buf_, "%3.1fHz", data.frequency);
		oled_.println(buf_);

		sprintf(buf_, "%2.0f%%", data.duty_cycle);
		oled_.println(buf_);

		// Blank line
		oled_.println();

		// Print expected values if in test mode
		if (data.expected_enabled) {
			sprintf(buf_, "%s: %2.0f%%", data.expected_load_type == LoadType::Liner ? "L" : "S",
				data.expected_duty_cycle);
			oled_.println(buf_);
		}

		// Draw status bar
		oled_.drawFastHLine(0, oled_.height() - 8, oled_.width(), WHITE);
		oled_.setCursor(0, oled_.height() - 7);
		oled_.setFont(&Picopixel);
		oled_.print(stateToShortString(data.current_state));
		oled_.print(" ");
		oled_.print(data.screen_lasts);
		oled_.println("s");

		// Right part of status bar: measure type
		oled_.setCursor(oled_.width() - 6, oled_.height() - 4);
		oled_.print(data.type == LoadType::Liner ? "L" : "S");
	}
};

GuiImpl *new_gui_impl() { return new OledGuiImpl(); }
