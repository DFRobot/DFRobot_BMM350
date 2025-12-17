
/**
 * @file  DFRobot_BMM350.cpp
 * @brief  Define the infrastructure of the DFRobot_BMM350 class and the implementation of the underlying methods
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license     The MIT License (MIT)
 * @author      [GDuang](yonglei.ren@dfrobot.com)
 * @version     V1.0.0
 * @date        2024-05-06
 * @url         https://github.com/DFRobot/DFRobot_BMM350
 */

#include "DFRobot_BMM350.h"

static struct bmm350_dev bmm350Sensor;
/*! Variable that holds the I2C device address selection */
static uint8_t devAddr;
TwoWire *_pWire = NULL;
uint8_t bmm350I2CAddr = 0;

void bmm350DelayUs(uint32_t period, void *intfPtr)
{
    UNUSED(intfPtr);
    if (period > 1000)
    {
        delay(period / 1000);
    }
    else
    {
        delayMicroseconds(period);
    }
}

DFRobot_BMM350::DFRobot_BMM350(pBmm350ReadFptr_t bmm350ReadReg, pBmm350WriteFptr_t bmm350WriteReg, pBmm350DelayUsFptr_t bmm350DelayUs, uint8_t pBmm350Addr)
{
    devAddr = pBmm350Addr;
    bmm350Sensor.intfPtr = &devAddr;
    bmm350Sensor.read = bmm350ReadReg;
    bmm350Sensor.write = bmm350WriteReg;
    bmm350Sensor.delayUs = bmm350DelayUs;
}

DFRobot_BMM350::~DFRobot_BMM350()
{
}

bool DFRobot_BMM350::sensorInit(void)
{
    return bmm350Init(&bmm350Sensor) == 0;
}

uint8_t DFRobot_BMM350::getChipID(void)
{
    return bmm350Sensor.chipId;
}

void DFRobot_BMM350::softReset(void)
{
    bmm350SoftReset(&bmm350Sensor);
    bmm350SetPowerMode(eBmm350SuspendMode, &bmm350Sensor);
}

void DFRobot_BMM350::setOperationMode(enum eBmm350PowerModes_t powermode)
{
    bmm350SetPowerMode(powermode, &bmm350Sensor);
}

String DFRobot_BMM350::getOperationMode(void)
{
    String result;
    switch (bmm350Sensor.powerMode)
    {
    case eBmm350SuspendMode:
        result = "bmm350 is suspend mode!";
        break;
    case eBmm350NormalMode:
        result = "bmm350 is normal mode!";
        break;
    case eBmm350ForcedMode:
        result = "bmm350 is forced mode!";
        break;
    case eBmm350ForcedModeFast:
        result = "bmm350 is forced_fast mode!";
        break;
    default:
        result = "error mode!";
        break;
    }
    return result;
}

void DFRobot_BMM350::setPresetMode(uint8_t presetMode, enum eBmm350DataRates_t rate)
{
    switch (presetMode)
    {
    case BMM350_PRESETMODE_LOWPOWER:
        bmm350SetOdrPerformance(rate, BMM350_NO_AVERAGING, &bmm350Sensor);
        break;
    case BMM350_PRESETMODE_REGULAR:
        bmm350SetOdrPerformance(rate, BMM350_AVERAGING_2, &bmm350Sensor);
        break;
    case BMM350_PRESETMODE_ENHANCED:
        bmm350SetOdrPerformance(rate, BMM350_AVERAGING_4, &bmm350Sensor);
        break;
    case BMM350_PRESETMODE_HIGHACCURACY:
        bmm350SetOdrPerformance(rate, BMM350_AVERAGING_8, &bmm350Sensor);
        break;
    default:
        break;
    }
}
void DFRobot_BMM350::setRate(uint8_t rate)
{
    /* Variable to store the function result */
    int8_t rslt;

    uint8_t avgOdrReg = 0;
    uint8_t avgReg = 0;
    uint8_t regData = 0;

    switch(rate){
        case BMM350_DATA_RATE_1_5625HZ:
        case BMM350_DATA_RATE_3_125HZ:
        case BMM350_DATA_RATE_6_25HZ:
        case BMM350_DATA_RATE_12_5HZ:
        case BMM350_DATA_RATE_25HZ:
        case BMM350_DATA_RATE_50HZ:
        case BMM350_DATA_RATE_100HZ:
        case BMM350_DATA_RATE_200HZ:
        case BMM350_DATA_RATE_400HZ:
            /* Get the configurations for ODR and performance */
            rslt = bmm350GetRegs(BMM350_REG_PMU_CMD_AGGR_SET, &avgOdrReg, 1, &bmm350Sensor);
            if (rslt == BMM350_OK){
                /* Read the performance status */
                avgReg = BMM350_GET_BITS(avgOdrReg, BMM350_AVG);
            }
            /* ODR is an enum taking the generated constants from the register map */
            regData = ((uint8_t)rate & BMM350_ODR_MSK);
            /* AVG / performance is an enum taking the generated constants from the register map */
            regData = BMM350_SET_BITS(regData, BMM350_AVG, (uint8_t)avgReg);
            /* Set PMU command configurations for ODR and performance */
            rslt = bmm350SetRegs(BMM350_REG_PMU_CMD_AGGR_SET, &regData, 1, &bmm350Sensor);
            if (rslt == BMM350_OK){
                /* Set PMU command configurations to update odr and average */
                regData = BMM350_PMU_CMD_UPD_OAE;
                /* Set PMU command configuration */
                rslt = bmm350SetRegs(BMM350_REG_PMU_CMD, &regData, 1, &bmm350Sensor);
                if (rslt == BMM350_OK){
                    rslt = bmm350DelayUs(BMM350_UPD_OAE_DELAY, &bmm350Sensor);
                }
            }
            break;
        default:
            break;
    }
}

float DFRobot_BMM350::getRate(void)
{
    /* Variable to store the function result */
    int8_t rslt;

    uint8_t avgOdrReg = 0;
    uint8_t odrReg = 0;
    float result = 0;

    /* Get the configurations for ODR and performance */
    rslt = bmm350GetRegs(BMM350_REG_PMU_CMD_AGGR_SET, &avgOdrReg, 1, &bmm350Sensor);
    if (rslt == BMM350_OK)
    {
        /* Read the performance status */
        odrReg = BMM350_GET_BITS(avgOdrReg, BMM350_ODR);
    }
    switch (odrReg)
    {
    case BMM350_DATA_RATE_1_5625HZ:
        result = 1.5625;
        break;
    case BMM350_DATA_RATE_3_125HZ:
        result = 3.125;
        break;
    case BMM350_DATA_RATE_6_25HZ:
        result = 6.25;
        break;
    case BMM350_DATA_RATE_12_5HZ:
        result = 12.5;
        break;
    case BMM350_DATA_RATE_25HZ:
        result = 25;
        break;
    case BMM350_DATA_RATE_50HZ:
        result = 50;
        break;
    case BMM350_DATA_RATE_100HZ:
        result = 100;
        break;
    case BMM350_DATA_RATE_200HZ:
        result = 200;
        break;
    case BMM350_DATA_RATE_400HZ:
        result = 400;
        break;
    default:
        break;
    }
    return result;
}

String DFRobot_BMM350::selfTest(eBmm350SelfTest_t testMode)
{
    String result;
    /* Structure instance of self-test data */
    struct sBmm350SelfTest_t stData;
    memset(&stData, 0, sizeof(stData));
    switch (testMode)
    {
    case eBmm350SelfTestNormal:
        setOperationMode(eBmm350NormalMode);
        setMeasurementXYZ();
        sBmm350MagData_t magData = getGeomagneticData();
        if ((magData.x < 2000) && (magData.x > -2000))
        {
            result += "x aixs self test success!\n";
        }
        else
        {
            result += "x aixs self test failed!\n";
        }
        if ((magData.y < 2000) && (magData.y > -2000))
        {
            result += "y aixs self test success!\n";
        }
        else
        {
            result += "y aixs self test failed!\n";
        }
        if ((magData.z < 2000) && (magData.z > -2000))
        {
            result += "z aixs self test success!\n";
        }
        else
        {
            result += "z aixs self test failed!\n";
        }
        break;
    }
    return result;
}

void DFRobot_BMM350::setMeasurementXYZ(enum eBmm350XAxisEnDis_t enX, enum eBmm350YAxisEnDis_t enY, enum eBmm350ZAxisEnDis_t enZ)
{
    bmm350_enable_axes(enX, enY, enZ, &bmm350Sensor);
}

String DFRobot_BMM350::getMeasurementStateXYZ(void)
{
    uint8_t axisReg = 0;
    uint8_t enX = 0;
    uint8_t enY = 0;
    uint8_t enZ = 0;
    char result[100] = "";

    /* Get the configurations for ODR and performance */
    axisReg = bmm350Sensor.axisEn;

    /* Read the performance status */
    enX = BMM350_GET_BITS(axisReg, BMM350_EN_X);
    enY = BMM350_GET_BITS(axisReg, BMM350_EN_Y);
    enZ = BMM350_GET_BITS(axisReg, BMM350_EN_Z);

    strcat(result, (enX == 1 ? "The x axis is enable! " : "The x axis is disable! "));
    strcat(result, (enY == 1 ? "The y axis is enable! " : "The y axis is disable! "));
    strcat(result, (enZ == 1 ? "The z axis is enable! " : "The z axis is disable! "));
    return result;
}

sBmm350MagData_t DFRobot_BMM350::getGeomagneticData(void)
{ 
    sBmm350MagData_t magData;
    struct sBmm350MagTempData_t magTempData;
    memset(&magData, 0, sizeof(magData));
    memset(&magTempData, 0, sizeof(magTempData));
    bmm350GetCompensatedMagXYZTempData(&magTempData, &bmm350Sensor);
    magData.x = magTempData.x;
    magData.y = magTempData.y;
    magData.z = magTempData.z;
    magData.temperature = magTempData.temperature;
    magData.float_x = magTempData.x;
    magData.float_y = magTempData.y;
    magData.float_z = magTempData.z;
    magData.float_temperature = magTempData.temperature;
    return magData;
}

float DFRobot_BMM350::getCompassDegree(void)
{
    float compass = 0.0;
    sBmm350MagData_t magData = getGeomagneticData();
    compass = atan2(magData.x, magData.y);
    if (compass < 0)
    {
        compass += 2 * PI;
    }
    if (compass > 2 * PI)
    {
        compass -= 2 * PI;
    }
    return compass * 180 / M_PI;
}

void DFRobot_BMM350::setDataReadyPin(enum eBmm350InterruptEnableDisable_t modes, enum eBmm350IntrPolarity_t polarity)
{
    /* Variable to get interrupt control configuration */
    uint8_t regData = 0;
    /* Variable to store the function result */
    int8_t rslt;
    /* Get interrupt control configuration */
    rslt = bmm350GetRegs(BMM350_REG_INT_CTRL, &regData, 1, &bmm350Sensor);
    if (rslt == BMM350_OK)
    {
        regData = BMM350_SET_BITS_POS_0(regData, BMM350_INT_MODE, BMM350_PULSED);
        regData = BMM350_SET_BITS(regData, BMM350_INT_POL, polarity);
        regData = BMM350_SET_BITS(regData, BMM350_INT_OD, BMM350_INTR_PUSH_PULL);
        regData = BMM350_SET_BITS(regData, BMM350_INT_OUTPUT_EN, BMM350_MAP_TO_PIN);
        regData = BMM350_SET_BITS(regData, BMM350_DRDY_DATA_REG_EN, (uint8_t)modes);
        /* Finally transfer the interrupt configurations */
        rslt = bmm350SetRegs(BMM350_REG_INT_CTRL, &regData, 1, &bmm350Sensor);
    }
}

bool DFRobot_BMM350::getDataReadyState(void)
{
    uint8_t drdyStatus = 0x0;
    bmm350GetInterruptStatus(&drdyStatus, &bmm350Sensor);
    if (drdyStatus & 0x01)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void DFRobot_BMM350::setThresholdInterrupt(uint8_t modes, int8_t threshold, enum eBmm350IntrPolarity_t polarity)
{
    if (modes == LOW_THRESHOLD_INTERRUPT)
    {
        __thresholdMode = LOW_THRESHOLD_INTERRUPT;
        setDataReadyPin(BMM350_ENABLE_INTERRUPT, polarity);
        this->threshold = threshold;
    }
    else
    {
        __thresholdMode = HIGH_THRESHOLD_INTERRUPT;
        setDataReadyPin(BMM350_ENABLE_INTERRUPT, polarity);
        this->threshold = threshold;
    }
}

sBmm350ThresholdData_t DFRobot_BMM350::getThresholdData(void)
{
    sBmm350MagData_t magData;
    memset(&magData, 0, sizeof(magData));
    thresholdData.mag_x = NO_DATA;
    thresholdData.mag_y = NO_DATA;
    thresholdData.mag_z = NO_DATA;
    thresholdData.interrupt_x = 0;
    thresholdData.interrupt_y = 0;
    thresholdData.interrupt_z = 0;
    bool state = getDataReadyState();
    if (state == true)
    {
        magData = getGeomagneticData();
        if (__thresholdMode == LOW_THRESHOLD_INTERRUPT)
        {
            if (magData.x < (int32_t)threshold * 16)
            {
                thresholdData.mag_x = magData.x;
                thresholdData.interrupt_x = 1;
            }
            if (magData.y < (int32_t)threshold * 16)
            {
                thresholdData.mag_y = magData.y;
                thresholdData.interrupt_y = 1;
            }
            if (magData.z < (int32_t)threshold * 16)
            {
                thresholdData.mag_z = magData.z;
                thresholdData.interrupt_z = 1;
            }
        }
        else if (__thresholdMode == HIGH_THRESHOLD_INTERRUPT)
        {
            if (magData.x > (int32_t)threshold * 16)
            {
                thresholdData.mag_x = magData.x;
                thresholdData.interrupt_x = 1;
            }
            if (magData.y > (int32_t)threshold * 16)
            {
                thresholdData.mag_y = magData.y;
                thresholdData.interrupt_y = 1;
            }
            if (magData.z > (int32_t)threshold * 16)
            {
                thresholdData.mag_z = magData.z;
                thresholdData.interrupt_z = 1;
            }
        }
    }

    return thresholdData;
}

static int8_t bmm350I2cReadData(uint8_t Reg, uint8_t *Data, uint32_t len, void *intfPtr)
{
    uint8_t deviceAddr = *(uint8_t *)intfPtr;
    _pWire->begin();
    int i = 0;
    _pWire->beginTransmission(deviceAddr);
    _pWire->write(Reg);
    if (_pWire->endTransmission() != 0)
    {
        return -1;
    }
    _pWire->requestFrom(deviceAddr, (uint8_t)len);
    while (_pWire->available())
    {
        Data[i++] = _pWire->read();
    }
    return 0;
}

static int8_t bmm350I2cWriteData(uint8_t Reg, const uint8_t *Data, uint32_t len, void *intfPtr)
{
    uint8_t deviceAddr = *(uint8_t *)intfPtr;
    _pWire->begin();
    _pWire->beginTransmission(deviceAddr);
    _pWire->write(Reg);
    for (uint8_t i = 0; i < len; i++)
    {
        _pWire->write(Data[i]);
    }
    _pWire->endTransmission();
    return 0;
}

DFRobot_BMM350_I2C::DFRobot_BMM350_I2C(TwoWire *pWire, uint8_t addr) : DFRobot_BMM350(bmm350I2cReadData, bmm350I2cWriteData, bmm350DelayUs, addr)
{
    _pWire = pWire;
    bmm350I2CAddr = addr;
}

uint8_t DFRobot_BMM350_I2C::begin()
{
    _pWire->begin();
    _pWire->beginTransmission(bmm350I2CAddr);
    if (_pWire->endTransmission() == 0)
    {
        if (sensorInit())
        {
            return 0;
        }
        else
        {
            DBG("Chip id error ,please check sensor!");
            return 2;
        }
    }
    else
    {
        DBG("I2C device address error or no connection!");
        return 1;
    }
}
