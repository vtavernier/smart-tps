#include "mbed_INA219.h"

mbed_INA219::mbed_INA219(mbed::I2C &i2c, char addr)
    : i2c_(i2c), addr_(addr), last_reg_(-1) {}

void mbed_INA219::begin() {
    write(INA219_REG_CALIBRATION, 4096);
    write(INA219_REG_CONFIG, INA219_CONFIG_BVOLTAGERANGE_32V |
				 INA219_CONFIG_GAIN_8_320MV |
				 INA219_CONFIG_BADCRES_12BIT |
				 INA219_CONFIG_SADCRES_12BIT_2S_1060US |
				 INA219_CONFIG_MODE_SVOLT_CONTINUOUS);
}

void mbed_INA219::write(char addr, uint16_t val) {
	char cmd[3];

	cmd[0] = addr;
	// MSBFIRST
	cmd[1] = (val & 0xFF00) >> 8;
	cmd[2] = val & 0xFF;

	i2c_.write(addr_ << 1, cmd, sizeof(cmd));
}

int16_t mbed_INA219::read(char addr) {
	if (last_reg_ != addr) {
		// Initiate read
		i2c_.write(addr_ << 1, &addr, sizeof(addr));
		last_reg_ = addr;
	}

	// Read 2 bytes
	char val[2] = {0};
	if (i2c_.read(addr_ << 1, val, sizeof(val)) == 0)
		return (val[0] << 8) | val[1];
	else
		return INT16_MAX;
}

float mbed_INA219::get_current() {
	auto val = read(INA219_REG_CURRENT);

	// Check for failed reads
	if (val == INT16_MAX)
		return MAXFLOAT;

	return val / 10.f;
}
