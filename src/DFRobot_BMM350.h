/**
 * @file  DFRobot_BMM350.h
 * @brief Defines the infrastructure of the DFRobot_BMM350 class
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license     The MIT License (MIT)
 * @author      [GDuang](yonglei.ren@dfrobot.com)
 * @version     V1.0.0
 * @date        2024-05-06
 * @url         https://github.com/DFRobot/DFRobot_BMM350
 */
#ifndef __DFRobot_BMM350_H__
#define __DFRobot_BMM350_H__

#include "bmm350_defs.h"
#include "bmm350.h"

#include "Arduino.h"
#include <stdlib.h>
#include <Wire.h>
#include <math.h>


//#define ENABLE_DBG                //< Open this macro to see the program running in detail

#ifdef ENABLE_DBG
#define DBG(...) {Serial.print("[");Serial.print(__FUNCTION__); Serial.print("(): "); Serial.print(__LINE__); Serial.print(" ] "); Serial.println(__VA_ARGS__);}
#else
#define DBG(...)
#endif

#define BMM350_INTERFACE_I2C            UINT8_C(0x00)
#define BMM350_INTERFACE_I3C            UINT8_C(0x01)
#define BMM350_SELF_TEST_NORMAL         UINT8_C(0x00)
#define BMM350_SELF_TEST_ADVANCED       UINT8_C(0x01)

enum eBmm350Interface_t {
  eBmm350InterfaceI2C = BMM350_INTERFACE_I2C,
  eBmm350InterfaceI3C = BMM350_INTERFACE_I3C
};

enum eBmm350SelfTest_t {
  eBmm350SelfTestNormal = BMM350_SELF_TEST_NORMAL
};

void bmm350DelayUs(uint32_t period);

class DFRobot_BMM350{
public:
  DFRobot_BMM350(pBmm350ReadFptr_t bmm350ReadReg, pBmm350WriteFptr_t bmm350WriteReg, pBmm350DelayUsFptr_t bmm350DelayUs, uint8_t pBmm350Addr);

  ~DFRobot_BMM350();

  /**
   * @fn softReset
   * @brief Soft reset, restore to suspended mode after soft reset. (You need to manually enter the normal mode)
   */
  void softReset(void);

  /**
   * @fn setOperationMode
   * @brief Set sensor operation mode
   * @param powermode
   * @n eBmm350SuspendMode      suspend mode: Suspend mode is the default power mode of BMM350 after the chip is powered, Current consumption in suspend mode is minimal, 
   *                                           so, this mode is useful for periods when data conversion is not needed. Read and write of all registers is possible.
   * @n eBmm350NormalMode       normal mode: Get geomagnetic data normally.      
   * @n eBmm350ForcedMode       forced mode: Single measurement, the sensor restores to suspend mode when the measurement is done.
   * @n eBmm350ForcedModeFast  To reach ODR = 200Hz is only possible by using FM_ FAST.
   */
  void setOperationMode(enum eBmm350PowerModes_t powermode);

  /**
   * @fn getOperationMode
   * @brief Get sensor operation mode
   * @return result Return sensor operation mode as a character string
   */
  String getOperationMode(void);

  /**
   * @fn setPresetMode
   * @brief Set preset mode, make it easier for users to configure sensor to get geomagnetic data (The default collection rate is 12.5Hz)
   * @param presetMode
   * @n BMM350_PRESETMODE_LOWPOWER       Low power mode, get a fraction of data and take the mean value.
   * @n BMM350_PRESETMODE_REGULAR        Regular mode, get a number of data and take the mean value.
   * @n BMM350_PRESETMODE_ENHANCED       Enhanced mode, get a plenty of data and take the mean value.
   * @n BMM350_PRESETMODE_HIGHACCURACY   High accuracy mode, get a huge number of data and take the mean value.
   * @param rate
   * @n BMM350_DATA_RATE_1_5625HZ
   * @n BMM350_DATA_RATE_3_125HZ
   * @n BMM350_DATA_RATE_6_25HZ
   * @n BMM350_DATA_RATE_12_5HZ
   * @n BMM350_DATA_RATE_25HZ
   * @n BMM350_DATA_RATE_50HZ
   * @n BMM350_DATA_RATE_100HZ
   * @n BMM350_DATA_RATE_200HZ
   * @n BMM350_DATA_RATE_400HZ
   */
  void setPresetMode(uint8_t presetMode, enum eBmm350DataRates_t rate=BMM350_DATA_RATE_12_5HZ);
  /**
   * @fn setRate
   * @brief Set the rate of obtaining geomagnetic data, the higher, the faster (without delay function)
   * @param rate
   * @n BMM350_DATA_RATE_1_5625HZ
   * @n BMM350_DATA_RATE_3_125HZ
   * @n BMM350_DATA_RATE_6_25HZ
   * @n BMM350_DATA_RATE_12_5HZ  (default rate)
   * @n BMM350_DATA_RATE_25HZ
   * @n BMM350_DATA_RATE_50HZ
   * @n BMM350_DATA_RATE_100HZ
   * @n BMM350_DATA_RATE_200HZ
   * @n BMM350_DATA_RATE_400HZ
   */
  void setRate(uint8_t rate);

  /**
   * @fn getRate
   * @brief Get the config data rate, unit: HZ
   * @return rate
   */
  float getRate(void);

  /**
   * @fn selfTest
   * @brief The sensor self test, the returned value indicate the self test result.
   * @param testMode:
   * @n     eBmm350SelfTestNormal               Normal self test, test whether x-axis, y-axis and z-axis are connected or short-circuited
   * @return result The returned character string is the self test result
   */
  String selfTest(eBmm350SelfTest_t testMode = eBmm350SelfTestNormal);
  
  /**
   * @fn setMeasurementXYZ
   * @brief Enable the measurement at x-axis, y-axis and z-axis, default to be enabled. After disabling, the geomagnetic data at x, y, and z axis are wrong.
   * @param en_x
   * @n   BMM350_X_EN        Enable the measurement at x-axis
   * @n   BMM350_X_DIS       Disable the measurement at x-axis
   * @param en_y
   * @n   BMM350_Y_EN        Enable the measurement at y-axis
   * @n   BMM350_Y_DIS       Disable the measurement at y-axis
   * @param en_z
   * @n   BMM350_Z_EN        Enable the measurement at z-axis
   * @n   BMM350_Z_DIS       Disable the measurement at z-axis
   */
  void setMeasurementXYZ(enum eBmm350XAxisEnDis_t enX = BMM350_X_EN, enum eBmm350YAxisEnDis_t enY = BMM350_Y_EN, enum eBmm350ZAxisEnDis_t enZ = BMM350_Z_EN);

  /**
   * @fn getMeasurementStateXYZ
   * @brief Get the enabling status at x-axis, y-axis and z-axis
   * @return result Return enabling status as a character string
   */
  String getMeasurementStateXYZ(void);

  /**
   * @fn getGeomagneticData
   * @brief Get the geomagnetic data of 3 axis (x, y, z)
   * @return Geomagnetic data structure, unit: (uT)
   */
  sBmm350MagData_t getGeomagneticData(void);


  /**
   * @fn getCompassDegree
   * @brief Get compass degree
   * @return Compass degree (0° - 360°)
   * @n      0° = North, 90° = East, 180° = South, 270° = West.
   */
  float getCompassDegree(void);

  /**
   * @fn setDataReadyPin
   * @brief Enable or disable data ready interrupt pin
   * @n After enabling, the DRDY pin jump when there's data coming.
   * @n After disabling, the DRDY pin will not jump when there's data coming.
   * @n High polarity: active on high, the default is low level, which turns to high level when the interrupt is triggered.
   * @n Low polarity: active on low, default is high level, which turns to low level when the interrupt is triggered.
   * @param modes
   * @n     BMM350_ENABLE_INTERRUPT        Enable DRDY
   * @n     BMM350_DISABLE_INTERRUPT       Disable DRDY
   * @param polarity
   * @n     BMM350_ACTIVE_HIGH              High polarity
   * @n     BMM350_ACTIVE_LOW               Low polarity
   */
  void setDataReadyPin(enum eBmm350InterruptEnableDisable_t modes, enum eBmm350IntrPolarity_t polarity=BMM350_ACTIVE_HIGH);

  /**
   * @fn getDataReadyState
   * @brief Get the data ready status, determine whether the data is ready
   * @return status
   * @retval true  Data ready
   * @retval false Data is not ready
   */
  bool getDataReadyState(void);

  /**
   * @fn setThresholdInterrupt(uint8_t modes, int8_t threshold, enum eBmm350IntrPolarity_t polarity)
   * @brief Set threshold interrupt, an interrupt is triggered when the geomagnetic value of a channel is beyond/below the threshold
   * @n      High polarity: active on high level, the default is low level, which turns to high level when the interrupt is triggered.
   * @n      Low polarity: active on low level, the default is high level, which turns to low level when the interrupt is triggered.
   * @param modes
   * @n     LOW_THRESHOLD_INTERRUPT       Low threshold interrupt mode
   * @n     HIGH_THRESHOLD_INTERRUPT      High threshold interrupt mode
   * @param  threshold
   * @n     Threshold, default to expand 16 times, for example: under low threshold mode, if the threshold is set to be 1, actually the geomagnetic data below 16 will trigger an interrupt
   * @param polarity
   * @n     POLARITY_HIGH      High polarity
   * @n     POLARITY_LOW       Low polarity
   */
  void setThresholdInterrupt(uint8_t modes, int8_t threshold, enum eBmm350IntrPolarity_t polarity);

  /**
   * @fn getThresholdData
   * @brief Get the data with threshold interrupt occurred
   * @return Returns the structure for storing geomagnetic data, the structure stores the data of 3 axis and interrupt status,
   * @n The interrupt is not triggered when the data at x-axis, y-axis and z-axis are NO_DATA
   * @n mag_x、mag_y、mag_z store geomagnetic data
   * @n interrupt_x、interrupt_y、interrupt_z store the xyz axis interrupt state
   */
  sBmm350ThresholdData_t getThresholdData(void);

protected:
  /**
   * @fn sensorInit
   * @brief Init bmm350 check whether the chip id is right
   * @return  state
   * @retval  true  Chip id is right init succeeds
   * @retval  false Chip id is wrong init failed
   */
  bool sensorInit(void);

  /**
   * @fn getChipID
   * @brief get bmm350 chip id
   * @return chip id
   */
  uint8_t getChipID(void);

private:
  uint8_t __thresholdMode = 3;
  int8_t threshold = 0;
  sBmm350ThresholdData_t thresholdData;
};

class DFRobot_BMM350_I2C:public DFRobot_BMM350
{
  public:
    DFRobot_BMM350_I2C(TwoWire *pWire, uint8_t addr = 0x14);
    uint8_t begin(void);
};


#endif
