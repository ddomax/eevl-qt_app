#ifndef SPI_H
#define SPI_H

#include <QObject>
/******************************************************************************/
/* 							 Include files										   */
/******************************************************************************/
/* Standard C libraries */
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
/* OS specific libraries */
#ifdef _WIN32
#include<windows.h>
#else
#include<unistd.h>
#endif

/* Include D2XX header*/
#include "SPI/ftd2xx.h"

/* Include libMPSSE header */
#include "SPI/libMPSSE_spi.h"

/* Include AD9957 header */
#include "SPI/AD9957Parameter.h"

/******************************************************************************/
/*								Macro and type defines							   */
/******************************************************************************/
/* Helper macros */

#define APP_CHECK_STATUS(exp) {if(exp!=FT_OK){printf("%s:%d:%s(): status(0x%x) \
!= FT_OK\n",__FILE__, __LINE__, __FUNCTION__,exp);exit(1);}else{;}};
#define CHECK_NULL(exp){if(exp==NULL){printf("%s:%d:%s():  NULL expression \
encountered \n",__FILE__, __LINE__, __FUNCTION__);exit(1);}else{;}};

/* Application specific macro definations */
#define SPI_DEVICE_BUFFER_SIZE		256
//#define CHANNEL_TO_OPEN			0	/*0 for first available channel, 1 for next... */

class SPI : public QObject
{
    Q_OBJECT
public:
    explicit SPI(QObject *parent = nullptr);
    FT_STATUS writeReg(uint8 address, uint8 *data, uint8 size);
    FT_STATUS readReg(uint8 address, uint8 *data, uint8 size);
    FT_STATUS WriteGPIOPin(uint8 dir,uint8 *val,uint8 pin, bool pinval);
    FT_STATUS WriteRegBit(uint8 address,uint8 *val,uint8 size,uint8 bytenum,uint8 bitnum, bool bitval);
    int openChannel();
    FT_STATUS closeChannel();

private:
    FT_HANDLE ftHandle;
    uint8 buffer[SPI_DEVICE_BUFFER_SIZE] = {0};

signals:

};

#endif // SPI_H
