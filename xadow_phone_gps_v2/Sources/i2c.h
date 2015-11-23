
#ifndef SOURCES_I2C_H_
#define SOURCES_I2C_H_


#define I2C_DEVICE_ADDR    0x05
#define DATA_LENGTH    	   18

#define GPIO_TEST_CMD		0x54 // 'T'


typedef struct
{
    uint8_t *pTxBuff;   // pointer points to transmit buffer
    uint32_t txSize;    // Size of transmit buffer
    uint8_t *pRxBuff;   // pointer points to receive buffer
    uint32_t rxSize;    // Size of receive buffer
} call_param_t;


extern bool isDataValid;


void i2c_init(void);


#endif /* SOURCES_I2C_H_ */
