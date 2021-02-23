#include "spi.h"

SPI::SPI(QObject *parent) : QObject(parent)
{

}

/*!
 * \brief Reads from AD9957
 *
 * This function reads a register from a specified address within the device
 *
 * \param[in] address: Address of the register to read
 * \param[in] *data: Data read from register
 * \return Returns status code of type FT_STATUS(see D2XX Programmer's Guide)
 * \sa Operating guide see AD9957 Datasheet
 * \note
 * \warning
 */
FT_STATUS SPI::readReg(uint8 address, uint8 *data, uint8 size)
{
    uint32 sizeToTransfer = 0;
    uint32 sizeTransfered = 0;
    FT_STATUS status;
    static const uint8 AD9957_RWBIT_WRITE = 0x80;

    /* Write register */
    /*     buffer[0]          |      buffer[1] */
    /* 7   6 5 4  3   2  1  0 | 7 6 5 4 3 2 1 0*/
    /*R/Wn X X A4 A3 A2 A1 A0 | D7...........D0*/
    /*>start-------------------------------end>*/

    /*	Loading Instruction Byte */
    if(address>0x7F){
        printf("Wrong chipAddress or Register Address!\n");
        // return status;
    }
    buffer[0]=(address | AD9957_RWBIT_WRITE);

    /* Write Instruction Byte */
    sizeToTransfer = 1;
    status = SPI_Write(ftHandle, buffer, sizeToTransfer, &sizeTransfered,
        SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES|
        SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE);
    APP_CHECK_STATUS(status);

    /*Read data bytes*/
    sizeToTransfer = size;
    status = SPI_Read(ftHandle, buffer, sizeToTransfer, &sizeTransfered,
        SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES|
        SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
    APP_CHECK_STATUS(status);
    for(int i=0;i<size;i++){
        data[i] = buffer[i];
    }

    return status;
}

/******************************************************************************/
/*						Public function definitions						  		   */
/******************************************************************************/
/*!
 * \brief Writes to AD9957
 *
 * This function writes a register on specified address within the device
 *
 * \param[in] address: Address of the register to write
 * \param[in] data: Data write to register
 * \return Returns status code of type FT_STATUS(see D2XX Programmer's Guide)
 * \sa Operating guide see AD9957 Datasheet
 * \note
 * \warning
 */
FT_STATUS SPI::writeReg(uint8 address, uint8 *data, uint8 size)
{
    uint32 sizeToTransfer = 1 + size;
    uint32 sizeTransfered = 0;
    // uint8 buffer[2]; //Localize SPI Buffer here!
    FT_STATUS status;
    static const uint8 AD9957_RWBIT_WRITE = 0x00;

    /* Write register */
    /*     buffer[0]          |      buffer[1] */
    /* 7   6 5 4  3   2  1  0 | 7 6 5 4 3 2 1 0*/
    /*R/Wn X X A4 A3 A2 A1 A0 | D7...........D0*/
    /*>start-------------------------------end>*/

    /*	Loading Instruction Byte */
    if(address>0x7F){
        printf("Wrong chipAddress or Register Address!\n");
        // return status;
    }
    buffer[0]=(address | AD9957_RWBIT_WRITE);

    /*	Loading Data Byte to */
    for(int i=0;i<size;i++){
        buffer[i+1]=data[i];
    }

    status = SPI_Write(ftHandle, buffer, sizeToTransfer, &sizeTransfered,
        SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES|
        SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE|
        SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
    APP_CHECK_STATUS(status);

#ifndef __linux__
    Sleep(10);
#endif
    return status;
}

/******************************************************************************/
/*						Public function definitions						  		   */
/******************************************************************************/
/*!
 * \brief Writes to AD9957
 *
 * This function writes a register on specified address within the device
 *
 * \param[in] address: Address of the register to write
 * \param[in] data: Data write to register
 * \return Returns status code of type FT_STATUS(see D2XX Programmer's Guide)
 * \sa Operating guide see AD9957 Datasheet
 * \note
 * \warning
 */
FT_STATUS SPI::WriteGPIOPin(uint8 dir,uint8 *val,uint8 pin, bool pinval)
{
    FT_STATUS status;
    uint8 writeVal = 0x00;
    uint8 originVal = *val;
    if(pin > 7){
        printf("Pin number out of range!\n");
        return status;
    }
    if(pinval)
        writeVal = ((uint8)(1 << pin)) | originVal;
    else
        writeVal = (~(uint8)(1 << pin)) & originVal;

    status = FT_WriteGPIO(ftHandle,dir,writeVal);
    APP_CHECK_STATUS(status);
    *val = writeVal;

#ifndef __linux__
    Sleep(10);
#endif
    return status;
}

/******************************************************************************/
/*						Public function definitions						  		   */
/******************************************************************************/
/*!
 * \brief Writes to AD9957
 *
 * This function writes a register on specified address within the device
 *
 * \param[in] address: Address of the register to write
 * \param[in] data: Data write to register
 * \return Returns status code of type FT_STATUS(see D2XX Programmer's Guide)
 * \sa Operating guide see AD9957 Datasheet
 * \note
 * \warning
 */
FT_STATUS SPI::WriteRegBit(uint8 address,uint8 *val,uint8 size,uint8 bytenum,uint8 bitnum, bool bitval)
{
    FT_STATUS status;
    if(bitnum > 7){
        printf("Pin number out of range!\n");
        return status;
    }
    bytenum = size-1-bytenum;
    if(bitval)
        val[bytenum] = ((uint8)(1 << bitnum)) | val[bytenum];
    else
        val[bytenum] = (~(uint8)(1 << bitnum)) & val[bytenum];


    status = writeReg(address,val,size);
    APP_CHECK_STATUS(status);

#ifndef __linux__
    Sleep(10);
#endif
    return status;
}

int SPI::openChannel()
{
    FT_STATUS status = FT_OK;
    FT_DEVICE_LIST_INFO_NODE devList = {0};
    ChannelConfig channelConf = {0};
    uint8 address = 0;
    uint8 size = 0;
    uint8 *data;
    uint8 bytenum = 0;
    uint8 bitnum = 1;
    bool bitval = 1;
    uint32 channels = 0;
    uint8 i = 0;
    uint8 latency = 255;
    uint8 GPIODir = 0xFF;
    uint8 GPIOVal = 0x00;

    uint8 CFR1Val[CFR1_W] = {0x00,0x00,0x00,0x00};
    uint8 CFR2Val[CFR2_W] = {0x00,0x00,0x00,0x00};
    uint8 IO_UP_RATEVal[IO_UP_RATE_W] = {0x00,0x00,0x01,0x00}; //256 div
    uint8 PROFILE0Val[PROFILE0_W] = {0x3F,0xFF, 0x00,0x00, 0x11,0xEB,0x85,0x1F}; //2'b00,14'd16383,16'd0,32'd300647711

    uint8 regVal[2] = {0x00,0x00};
    uint8 config2Val[2] = {0x20,0x02};

    channelConf.ClockRate = 5000; //div 5 for sclk frequency on FT232D
    channelConf.LatencyTimer = latency;
    channelConf.configOptions = SPI_CONFIG_OPTION_MODE0 | SPI_CONFIG_OPTION_CS_DBUS3 | SPI_CONFIG_OPTION_CS_ACTIVELOW;// | SPI_CONFIG_OPTION_CS_ACTIVELOW;
    channelConf.Pin = 0x90B090B0;/*FinalVal-FinalDir-InitVal-InitDir (for dir 0=in, 1=out)*/
    /*BIT7  -BIT0:   Initial direction of the pins	*/
    /*BIT15 -BIT8:   Initial values of the pins		*/
    /*BIT23 -BIT16: Final direction of the pins		*/
    /*BIT31 -BIT24: Final values of the pins		*/

    /* init library */
#ifdef _MSC_VER
    Init_libMPSSE();
#endif
    status = SPI_GetNumChannels(&channels);
    APP_CHECK_STATUS(status);
    printf("Number of available SPI channels = %d\n",(int)channels);

    uint32 channel_to_open = 0;
    if(channels>0)
    {
        for(i=0;i<channels;i++)
        {
            status = SPI_GetChannelInfo(i,&devList);
            if (strcmp(devList.SerialNumber, "A") == 0)
            {
                channel_to_open = i;
            }
            APP_CHECK_STATUS(status);
            printf("Information on channel number %d:\n",i);
            /* print the dev info */
            printf("		Flags=0x%x\n",devList.Flags);
            printf("		Type=0x%x\n",devList.Type);
            printf("		ID=0x%x\n",devList.ID);
            printf("		LocId=0x%x\n",devList.LocId);
            printf("		SerialNumber=%s\n",devList.SerialNumber);
            printf("		Description=%s\n",devList.Description);
            printf("		ftHandle=0x%x\n",(unsigned int)devList.ftHandle);/*is 0 unless open*/
        }

        /* Open the first available channel */
        status = SPI_OpenChannel(channel_to_open,&ftHandle);
        APP_CHECK_STATUS(status);
        printf("\nhandle=0x%x status=0x%x SPI Channel Opened. All pins are inputs(float) now!\n",(unsigned int)ftHandle,status);
//        getchar();
        status = SPI_InitChannel(ftHandle,&channelConf);
        APP_CHECK_STATUS(status);
        printf("\nhandle=0x%x status=0x%x SPI Channel Init done. All pins are configured!\n",(unsigned int)ftHandle,status);
//        getchar();

        /* Enable SDO */
        printf("Enable SDO:\n");
        address = 0x02;
        size = 2;
        data = config2Val;
        bytenum = 0;
        bitnum = 7;
        bitval = 1;
        WriteRegBit(address,data,size,bytenum,bitnum,bitval);
        printf("Write Reg %2x done : ",address);
        for(int i=0;i<size;i++){
            printf("%2x ",data[i]);
        }
        printf("\n");

        address = 0x00;
        size = 2;
        data = regVal;
        for (uint8 address = 0x00; address <= 0x7F; address++)
        {
            readReg(address,data,size);
            printf("Read  Reg %2x done : ",address);
            for(int i=0;i<size;i++){
                printf("%2x ",data[i]);
            }
            printf("\n");
        }
    }
    return 0;
}

FT_STATUS SPI::closeChannel()
{
    FT_STATUS status = FT_OK;
    status = SPI_CloseChannel(ftHandle);
    printf("SPI Channel Closed.\n");
#ifdef _MSC_VER
    Cleanup_libMPSSE();
#endif
    return status;
}

