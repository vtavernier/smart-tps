#ifndef _NANO33BLE_PROJECT_PINS_H_
#define _NANO33BLE_PROJECT_PINS_H_

/// SPI SCK
#define TPS_PIN_SCK 13
/// I2C SDA
#define TPS_PIN_SDA PIN_WIRE_SDA
/// I2C SCL
#define TPS_PIN_SCL PIN_WIRE_SCL
/// SPI MISO
#define TPS_PIN_MISO 12
/// SPI MOSI
#define TPS_PIN_MOSI 11
/// OLED screen chip select
#define TPS_PIN_OLED_CS 10
/// OLED screen reset
#define TPS_PIN_OLED_RESET 9
/// OLED screen D/C
#define TPS_PIN_OLED_DC 8
/// MAX16054 clear pin
#define TPS_PIN_AOFF 7
/// Load injector shader driver pin
#define TPS_PIN_LTS 4
/// Load injector liner driver pin
#define TPS_PIN_LTL 3
/// Load injector base driver pin
#define TPS_PIN_LTB 2

/// INA219 for liner I2C address
#define TPS_INA219_LINER_ADDR 0x40
/// INA219 for shader I2C address
#define TPS_INA219_SHADER_ADDR 0x41

#endif /* _NANO33BLE_PROJECT_PINS_H_ */
