
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "board.h"
#include "i2c.h"
#include "gps.h"
#include "fsl_port_hal.h"
#include "fsl_i2c_slave_driver.h"


uint8_t RegScanData[4] = {0, 0, 0, I2C_DEVICE_ADDR};
uint8_t TxBuff[DATA_LENGTH] = {0};
uint8_t RxBuff[DATA_LENGTH] = {0};
i2c_slave_state_t slave;
call_param_t callParam;
i2c_slave_user_config_t userConfig;

uint8_t i2c_send_count = 0;
uint8_t i2c_comand = 0;

extern unsigned char sys_gpio_test;


static void i2c_data_copy(uint8_t* strDes, uint8_t* strSrc, uint8_t num)
{
	uint8_t i;
	for(i=0;i<num;i++)strDes[i] = strSrc[i];
}

void i2c_slave_callback(uint8_t instance,i2c_slave_event_t i2cEvent,void *param)
{
    i2c_slave_state_t * slaveState = I2C_DRV_SlaveGetHandler(instance);
    call_param_t *userData = (call_param_t*)param;
    float fData;
    uint16_t iData;
    uint8_t i;
	uint32_t Data;

    switch(i2cEvent)
    {
        // Transmit request
        case kI2CSlaveTxReq:
            // Update information for transmit process
        	/*
        	if(i2c_send_count == 0)
        	{
        		memset(TxBuff, 0, DATA_LENGTH);
				switch(i2c_comand)
				{
					case GPS_SCAN_ID:
						TxBuff[0] = GPS_SCAN_ID;
						TxBuff[1] = GPS_SCAN_SIZE;
						i2c_data_copy(TxBuff + 2, RegScanData, 4);
					break;

					case GPS_UTC_DATE_TIME_ID:
						TxBuff[0] = GPS_UTC_DATE_TIME_ID;
						TxBuff[1] = GPS_UTC_DATE_TIME_SIZE;
						i2c_data_copy(TxBuff + 2, GPS_RMC_Data.UTCDateTime, 6);
					break;

					case GPS_STATUS_ID:
						TxBuff[0] = GPS_STATUS_ID;
						TxBuff[1] = GPS_STATUS_SIZE;
						TxBuff[2] = GPS_RMC_Data.Status;
					break;

					case GPS_LATITUDE_ID:
						TxBuff[0] = GPS_LATITUDE_ID;
						TxBuff[1] = GPS_LATITUDE_SIZE;
						i2c_data_copy(TxBuff + 2, GPS_RMC_Data.Latitude, 9);
					break;

					case GPS_NS_ID:
						TxBuff[0] = GPS_NS_ID;
						TxBuff[1] = GPS_NS_SIZE;
						TxBuff[2] = GPS_RMC_Data.NS;
					break;

					case GPS_LONGITUDE_ID:
						TxBuff[0] = GPS_LONGITUDE_ID;
						TxBuff[1] = GPS_LONGITUDE_SIZE;
						i2c_data_copy(TxBuff + 2, GPS_RMC_Data.Longitude, 10);
					break;

					case GPS_EW_ID:
						TxBuff[0] = GPS_EW_ID;
						TxBuff[1] = GPS_EW_SIZE;
						TxBuff[2] = GPS_RMC_Data.EW;
					break;

					case GPS_SPEED_ID:
						TxBuff[0] = GPS_SPEED_ID;
						TxBuff[1] = GPS_SPEED_SIZE;
						i2c_data_copy(TxBuff + 2, GPS_RMC_Data.Speed, 5);
					break;

					case GPS_COURSE_ID:
						TxBuff[0] = GPS_COURSE_ID;
						TxBuff[1] = GPS_COURSE_SIZE;
						i2c_data_copy(TxBuff + 2, GPS_RMC_Data.Course, 5);
					break;

					case GPS_POSITION_FIX_ID:
						TxBuff[0] = GPS_POSITION_FIX_ID;
						TxBuff[1] = GPS_POSITION_FIX_SIZE;
						TxBuff[2] = GPS_GGA_Data.PositionFix;
					break;

					case GPS_SATE_USED_ID:
						TxBuff[0] = GPS_SATE_USED_ID;
						TxBuff[1] = GPS_SATE_USED_SIZE;
						TxBuff[2] = GPS_GGA_Data.SatUsed[0];
						TxBuff[3] = GPS_GGA_Data.SatUsed[1];
					break;

					case GPS_ALTITUDE_ID:
						TxBuff[0] = GPS_ALTITUDE_ID;
						TxBuff[1] = GPS_ALTITUDE_SIZE;
						i2c_data_copy(TxBuff + 2, GPS_GGA_Data.Altitude, 7);
					break;

					case GPS_MODE_ID:
						TxBuff[0] = GPS_MODE_ID;
						TxBuff[1] = GPS_MODE_SIZE;
						TxBuff[2] = GPS_GSA_Data.Mode;
					break;

					case GPS_MODE2_ID:
						TxBuff[0] = GPS_MODE2_ID;
						TxBuff[1] = GPS_MODE2_SIZE;
						TxBuff[2] = GPS_GSA_Data.Mode2;
					break;

					default:
					break;
				}
        	}
        	*/
            slaveState->txSize = userData->txSize;
            slaveState->txBuff = userData->pTxBuff + i2c_send_count++;
            slaveState->isTxBusy = true;

			switch(i2c_comand)
			{
				case GPS_STATUS_ID: case GPS_NS_ID:	case GPS_EW_ID:
				case GPS_POSITION_FIX_ID: case GPS_MODE_ID:
				case GPS_MODE2_ID: case GPS_SATE_IN_VIEW_ID:
					if(i2c_send_count == (GPS_STATUS_SIZE+2))i2c_send_count = 0;
				break;

				case GPS_SATE_USED_ID:
					if(i2c_send_count == (GPS_SATE_USED_SIZE+2))i2c_send_count = 0;
				break;

				case GPS_SCAN_ID:
					if(i2c_send_count == (GPS_SCAN_SIZE+2))i2c_send_count = 0;
				break;

				case GPS_SPEED_ID: case GPS_COURSE_ID:
					if(i2c_send_count == (GPS_SPEED_SIZE+2))i2c_send_count = 0;
				break;

				case GPS_UTC_DATE_TIME_ID:
					if(i2c_send_count == (GPS_UTC_DATE_TIME_SIZE+2))i2c_send_count = 0;
				break;

				case GPS_ALTITUDE_ID:
					if(i2c_send_count == (GPS_ALTITUDE_SIZE+2))i2c_send_count = 0;
				break;

				case GPS_LATITUDE_ID:
					if(i2c_send_count == (GPS_LATITUDE_SIZE+2))i2c_send_count = 0;
				break;

				case GPS_LONGITUDE_ID:
					if(i2c_send_count == (GPS_LONGITUDE_SIZE+2))i2c_send_count = 0;
				break;

				default:
				break;
			}
        break;

        // Receive request
        case kI2CSlaveRxReq:
            // Update information for received process
            slaveState->rxSize = userData->rxSize;
            slaveState->rxBuff = userData->pRxBuff;
            slaveState->isRxBusy = true;
        break;

        // Transmit buffer is empty
        case kI2CSlaveTxEmpty:
            slaveState->isTxBusy = false;
        break;

        // Receive buffer is full
        case kI2CSlaveRxFull:
			i2c_comand = userData->pRxBuff[0];
			i2c_send_count = 0;

    		memset(TxBuff, 0, DATA_LENGTH);
			switch(i2c_comand)
			{
				case GPS_SCAN_ID:
					TxBuff[0] = GPS_SCAN_ID;
					TxBuff[1] = GPS_SCAN_SIZE;
					i2c_data_copy(TxBuff + 2, RegScanData, 4);
				break;

				case GPS_UTC_DATE_TIME_ID:
					TxBuff[0] = GPS_UTC_DATE_TIME_ID;
					TxBuff[1] = GPS_UTC_DATE_TIME_SIZE;
					i2c_data_copy(TxBuff + 2, GPS_RMC_Data.UTCDateTime, 6);
				break;

				case GPS_STATUS_ID:
					TxBuff[0] = GPS_STATUS_ID;
					TxBuff[1] = GPS_STATUS_SIZE;
					TxBuff[2] = GPS_RMC_Data.Status;
				break;

				case GPS_LATITUDE_ID: // Î³¶È
					TxBuff[0] = GPS_LATITUDE_ID;
					TxBuff[1] = GPS_LATITUDE_SIZE;
					i2c_data_copy(TxBuff + 2, GPS_RMC_Data.Latitude, 9);
				break;

				case GPS_NS_ID:
					TxBuff[0] = GPS_NS_ID;
					TxBuff[1] = GPS_NS_SIZE;
					TxBuff[2] = GPS_RMC_Data.NS;
				break;

				case GPS_LONGITUDE_ID: // ¾­¶È
					TxBuff[0] = GPS_LONGITUDE_ID;
					TxBuff[1] = GPS_LONGITUDE_SIZE;
					i2c_data_copy(TxBuff + 2, GPS_RMC_Data.Longitude, 10);
				break;

				case GPS_EW_ID:
					TxBuff[0] = GPS_EW_ID;
					TxBuff[1] = GPS_EW_SIZE;
					TxBuff[2] = GPS_RMC_Data.EW;
				break;

				case GPS_SPEED_ID:
					TxBuff[0] = GPS_SPEED_ID;
					TxBuff[1] = GPS_SPEED_SIZE;
					i2c_data_copy(TxBuff + 2, GPS_RMC_Data.Speed, 5);
				break;

				case GPS_COURSE_ID:
					TxBuff[0] = GPS_COURSE_ID;
					TxBuff[1] = GPS_COURSE_SIZE;
					i2c_data_copy(TxBuff + 2, GPS_RMC_Data.Course, 5);
				break;

				case GPS_POSITION_FIX_ID:
					TxBuff[0] = GPS_POSITION_FIX_ID;
					TxBuff[1] = GPS_POSITION_FIX_SIZE;
					TxBuff[2] = GPS_GGA_Data.PositionFix;
				break;

				case GPS_SATE_USED_ID:
					TxBuff[0] = GPS_SATE_USED_ID;
					TxBuff[1] = GPS_SATE_USED_SIZE;
					TxBuff[2] = GPS_GGA_Data.SatUsed[0];
					TxBuff[3] = GPS_GGA_Data.SatUsed[1];
				break;

				case GPS_ALTITUDE_ID:
					TxBuff[0] = GPS_ALTITUDE_ID;
					TxBuff[1] = GPS_ALTITUDE_SIZE;
					i2c_data_copy(TxBuff + 2, GPS_GGA_Data.Altitude, 7);
				break;

				case GPS_MODE_ID:
					TxBuff[0] = GPS_MODE_ID;
					TxBuff[1] = GPS_MODE_SIZE;
					TxBuff[2] = GPS_GSA_Data.Mode;
				break;

				case GPS_MODE2_ID:
					TxBuff[0] = GPS_MODE2_ID;
					TxBuff[1] = GPS_MODE2_SIZE;
					TxBuff[2] = GPS_GSA_Data.Mode2;
				break;

				case GPS_SATE_IN_VIEW_ID:
					TxBuff[0] = GPS_SATE_IN_VIEW_ID;
					TxBuff[1] = GPS_SATE_IN_VIEW_SIZE;
					TxBuff[2] = GPS_GSV_Data.SatInView;
				break;

				case GPIO_TEST_CMD:
					sys_gpio_test = 1;
				break;

				default:
				break;
			}

            slaveState->isRxBusy = false;
        break;

        default:
        break;
    }
}

void i2c_init(void)
{
	callParam.pTxBuff = TxBuff;
	callParam.pRxBuff = RxBuff;
	callParam.txSize = 1;
	callParam.rxSize = 1;

	userConfig.address        = I2C_DEVICE_ADDR;
	userConfig.slaveCallback  = i2c_slave_callback;
	userConfig.callbackParam  = &callParam;
	userConfig.slaveListening = true;
	userConfig.startStopDetect = true;

	PORT_HAL_SetMuxMode(PORTB,3u,kPortMuxAlt2);
	PORT_HAL_SetMuxMode(PORTB,4u,kPortMuxAlt2);

    I2C_DRV_SlaveInit(0, &userConfig, &slave);
}
