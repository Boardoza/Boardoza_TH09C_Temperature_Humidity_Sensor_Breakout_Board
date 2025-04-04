/*!
 *  @file TH09C.cpp
 *
 *  @mainpage TH09C is a low cost temperature/humidity sensor.
 *
 *  @section intro_sec Introduction
 *
 *  This is a library for Boardoza TH09C low cost temperature/humidity sensors.
 *
 *  @section author Author
 *
 *  Written by Boardoza.
 *
 *  @section license License
 *
 *  MIT license, all text above must be included in any redistribution
 */


#include "TH09C.h"
#include <Arduino.h>

/** @brief begin
*          Starts the I2C connection and setup sensor pins.
* @param frequency I2C speed in KHz.
*/ 

TH09C::TH09C() {

}

void TH09C::begin() {
  Wire.begin();  // Setting I2C speed.
  delay(100);

  uint8_t TH09_Ctrl = readByteOneReg(TH09_Address, SYS_CTRL);
  if (!TH09_Ctrl) {
    uint8_t TH09_Low_Power = 0;  // Failed to read control register, low power mode inactive
  } else {
    while (writeByteOneReg(TH09_Address, SYS_CTRL, 0x00) != 0) {
      // Activating low power mode, waiting until the process is complete...
    }
    uint8_t TH09_Low_Power = 1;  // Low power mode successfully activated
  }

  uint8_t TH09_Status = readByteOneReg(TH09_Address, SYS_STAT);

  uint8_t TH09_SensRun = readByteOneReg(TH09_Address, SENS_RUN);
  if (!TH09_SensRun) {
    while (writeByteOneReg(TH09_Address, SENS_RUN, 0x03) != 0) {
      // Activating the sensor, waiting until the activation process is complete...
    }
  }
  
  uint8_t TH09_SensStart = readByteOneReg(TH09_Address, SENS_START);
  if (!TH09_SensStart) {
    while (writeByteOneReg(TH09_Address, SENS_START, 0x03) != 0) {
      // Initializing the sensor, waiting until initialization is complete...
    }
  }
}

/**
 * @brief Reads the temperature and humidity values from the TH09 sensor.
 *
 * This function reads the temperature and humidity values from the TH09 sensor
 * using the `readMultiBytesOneReg` function. It converts the raw temperature value
 * from Fahrenheit to Celsius and calculates the relative humidity as a percentage.
 * The results are stored in the `temperature` and `humidity` variables passed by reference.
 *
 * @param temperature Reference to a float variable to store the temperature in Celsius.
 * @param humidity Reference to a float variable to store the relative humidity in percentage.
 */


void TH09C::readTempHum(float& temperature, float& humidity) {
  uint8_t TH09_Temp_Array[2];
  readMultiBytesOneReg(TH09_Address, T_VAL, TH09_Temp_Array, 2);

  float TH09_Temp =  TH09_Temp_Array[1] << 8 | TH09_Temp_Array[0];
  TH09_Temp = (TH09_Temp) / 64;            // Fahrenheit to Kelvin   
  temperature = TH09_Temp - 273.15;        // Kelvin to Celsius

  uint8_t TH09_Hum_Array[2];
  readMultiBytesOneReg(TH09_Address, H_VAL, TH09_Hum_Array, 2);
  uint16_t TH09_Hum = TH09_Hum_Array[1] << 8 | TH09_Hum_Array[0];
  humidity = (float)TH09_Hum / 512;        // Relative humidity (in %)
}

/**
 * @brief Reads the temperature value from the TH09 sensor.
 *
 * This function reads the temperature value from the TH09 sensor
 * using the `readMultiBytesOneReg` function. It converts the raw temperature value
 * from Fahrenheit to Celsius.
 * The results are stored in the `temperature` and variables passed by reference.
 *
 * @param temperature Reference to a float variable to store the temperature in Celsius.
 */

void TH09C::readTemp(float& temperature) {
  uint8_t TH09_Temp_Array[2];
  readMultiBytesOneReg(TH09_Address, T_VAL, TH09_Temp_Array, 2);

  float TH09_Temp =  TH09_Temp_Array[1] << 8 | TH09_Temp_Array[0];
  TH09_Temp = (TH09_Temp) / 64;            // Fahrenheit to Kelvin   
  temperature = TH09_Temp - 273.15;        // Kelvin to Celsius
}

/**
 * @brief Reads the humidity value from the TH09 sensor.
 *
 * This function reads the humidity value from the TH09 sensor
 * using the `readMultiBytesOneReg` function. It calculates the relative humidity as a percentage.
 * The results are stored in the `temperature` and `humidity` variables passed by reference.
 *
 * @param temperature Reference to a float variable to store the temperature in Celsius.
 */

void TH09C::readHum(float& humidity) {
  uint8_t TH09_Hum_Array[2];
  readMultiBytesOneReg(TH09_Address, H_VAL, TH09_Hum_Array, 2);
  uint16_t TH09_Hum = TH09_Hum_Array[1] << 8 | TH09_Hum_Array[0];
  humidity = (float)TH09_Hum / 512;        // Relative humidity (in %)
}


/**
 * @brief Converts a temperature value to different units.
 *
 * This function converts a temperature value to different units based on the specified unit parameter.
 * The supported unit values are "K" (Kelvin) and "F" (Fahrenheit). If an invalid unit is specified,
 * the function returns the original temperature value without any conversion.
 *
 * @param temperature The temperature value to be converted.
 * @param unit The unit to which the temperature should be converted. Valid values are "K" and "F".
 *
 * @return The converted temperature value.
 * 
 * @note This function assumes that the input temperature is in Celsius.
 *       If the temperature is in a different unit, convert it to Celsius before calling this function.
 */

float TH09C::celciusConverter(float& temperature, char unit) {
  if (unit == 'K') {           // Kelvin
    return temperature + 273.15;
  } else if (unit == 'F') {    // Fahrenheit
    return (temperature * 9/5) + 32;
  } else {
    // Returns the original temperature value if an invalid unit is specified.
    return temperature;
  }
}

/**
 * @brief Converts a temperature value from Kelvin to different units.
 *
 * This function converts a temperature value from Kelvin to different units based on the specified unit parameter.
 * The supported unit values are "C" (Celsius) and "F" (Fahrenheit). If an invalid unit is specified,
 * the function returns the original temperature value without any conversion.
 *
 * @param temperature The temperature value in Kelvin to be converted.
 * @param unit The unit to which the temperature should be converted. Valid values are "C" and "F".
 *
 * @return The converted temperature value.
 *
 * @note This function assumes that the input temperature is in Kelvin.
 *       If the temperature is in a different unit, convert it to Kelvin before calling this function.
 */

float TH09C::kelvinConverter(float& temperature, char unit) {
  if(unit == 'C') {
    return temperature - 273.15;
  } else if (unit == 'F') {
    return (temperature * 9/5) - 459.67;
  } else {
    // Returns the original temperature value if an invalid unit is specified.
    return temperature;
  }
}

/**
 * @brief Converts a temperature value from Fahrenheit to different units.
 *
 * This function converts a temperature value from Fahrenheit to different units based on the specified unit parameter.
 * The supported unit values are "C" (Celsius) and "K" (Kelvin). If an invalid unit is specified,
 * the function returns the original temperature value without any conversion.
 *
 * @param temperature The temperature value in Fahrenheit to be converted.
 * @param unit The unit to which the temperature should be converted. Valid values are "C" and "K".
 *
 * @return The converted temperature value.
 * 
 * @note This function assumes that the input temperature is in Fahrenheit.
 *       If the temperature is in a different unit, convert it to Fahrenheit before calling this function.
 */

float TH09C::fahrenheitConverter(float& temperature, char unit) {
  if(unit == 'C') {
    return (temperature - 32) * 5/9; 
  } else if (unit == 'K') {
    return (temperature + 459.67) * 5/9;
  } else  {
    // Returns the original temperature value if an invalid unit is specified.
    return temperature;
  }
}

/**
 * @brief Reads multiple bytes of data from a single register of a device.
 *
 * This function reads multiple bytes of data from a specified register address of a device using the I2C protocol.
 * The data is stored in the provided data buffer.
 *
 * @param devAddr The I2C address of the device from which the data should be read.
 * @param regAddr The register address from which the data should be read.
 * @param data A pointer to the buffer where the read data will be stored.
 * @param count The number of bytes to be read from the register.
 *
 * @note Before calling this function, ensure that the Wire library has been initialized using Wire.begin().
 * @note The function assumes that the device is connected and accessible via the I2C bus.
 */

void TH09C::readMultiBytesOneReg(uint8_t devAddr, uint8_t regAddr, uint8_t* data, uint8_t count) {
  Wire.beginTransmission(devAddr);
  Wire.write(regAddr);
  Wire.endTransmission(true);

  Wire.requestFrom(devAddr, count);
  for (uint8_t i = 0; i < count; i++) {
    data[i] = Wire.read();
  }
}

/**
 * @brief Reads a single byte of data from a register of a device.
 *
 * This function reads a single byte of data from a specified register address of a device using the I2C protocol.
 * The read data is returned as an unsigned 8-bit integer.
 *
 * @param devAddr The I2C address of the device from which the data should be read.
 * @param regAddr The register address from which the data should be read.
 *
 * @return The read data as an unsigned 8-bit integer.
 *         If no data is available or an error occurs during the read operation, 0 is returned.
 *
 * @note Before calling this function, ensure that the Wire library has been initialized using Wire.begin().
 * @note The function assumes that the device is connected and accessible via the I2C bus.
 */

uint8_t TH09C::readByteOneReg(uint8_t devAddr, uint8_t regAddr) {
  Wire.beginTransmission(devAddr);
  Wire.write(regAddr);
  Wire.endTransmission(true);

  Wire.requestFrom(devAddr, (uint8_t)1);
  if (Wire.available()) {
    return Wire.read();
  }
  return 0;
}

/**
 * @brief Writes a single byte of data to a register of a device.
 *
 * This function writes a single byte of data to a specified register address of a device using the I2C protocol.
 *
 * @param devAddr The I2C address of the device to which the data should be written.
 * @param regAddr The register address to which the data should be written.
 * @param data The data byte to be written.
 *
 * @return Always returns 0 to indicate a successful write operation.
 *
 * @note Before calling this function, ensure that the Wire library has been initialized using TH09.begin().
 * @note The function assumes that the device is connected and accessible via the I2C bus.
 */

uint8_t TH09C::writeByteOneReg(uint8_t devAddr, uint8_t regAddr, uint8_t data) {
  Wire.beginTransmission(devAddr);
  Wire.write(regAddr);
  Wire.write(data);
  Wire.endTransmission(true);

  return 0;
}

void TH09C::calibration() {
  uint8_t TH09_Ctrl = readByteOneReg(TH09_Address, SYS_CTRL);
  if (!TH09_Ctrl) {
    uint8_t TH09_Low_Power = 0;  // Failed to read control register, low power mode inactive
  } else {
    while (writeByteOneReg(TH09_Address, SYS_CTRL, 0x00) != 0) {
      // Activating low power mode, waiting until the process is complete...
    }
    uint8_t TH09_Low_Power = 1;  // Low power mode successfully activated
  }

  uint8_t TH09_Status = readByteOneReg(TH09_Address, SYS_STAT);

  uint8_t TH09_SensRun = readByteOneReg(TH09_Address, SENS_RUN);
  if (!TH09_SensRun) {
    while (writeByteOneReg(TH09_Address, SENS_RUN, 0x03) != 0) {
      // Activating the sensor, waiting until the activation process is complete...
    }
  }
  
  uint8_t TH09_SensStart = readByteOneReg(TH09_Address, SENS_START);
  if (!TH09_SensStart) {
    while (writeByteOneReg(TH09_Address, SENS_START, 0x03) != 0) {
      // Initializing the sensor, waiting until initialization is complete...
    }
  }
}