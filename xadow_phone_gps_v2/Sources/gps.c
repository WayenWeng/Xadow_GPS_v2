
#include <stdbool.h>
#include "board.h"
#include "gps.h"


static uint8_t NMEA_CMD=NMEA_NULL;        //NMEA 语句
static uint8_t NMEA_CMD_Buff[]="$GPxxx,"; //NMEA 语句类型缓存
static uint8_t NMEA_CMD_Index=0;          //读取 CMD字符的个数
static bool NMEA_CMD_Parsered=0;          //CMD类型解析完毕
static uint8_t NMEA_DAT_Block=0;          //NMEA 数据字段号 从0开始
static uint8_t NMEA_DAT_BlockIndex=0;     //NMEA 数据每个字段内字符索引 从0开始
static bool NMEA_CMD_Start=0;             //NMEA 语句开始. 检测到 $ 时置1
static bool ReciveFlag=0;                 //数据接收完成. 最后一条 GPRMC 语句发送完毕置1,

static uint8_t ucTempA=0;                 //存储解析两位数字用的的十位临时变量
static uint8_t SateInfoIndex=0;           //
//static uint8_t ucTemp[5];

stru_GPSRMC GPS_RMC_Data;
stru_GPSGGA GPS_GGA_Data;
stru_GPSGSA GPS_GSA_Data;
stru_GPSGSV GPS_GSV_Data;


void gps_sate_data_init(void)
{
    uint8_t i;
    for(i=0;i<12;i++)
    {
        GPS_GSV_Data.SatInfo[i].SatID=0x00;
        GPS_GSA_Data.HDOP[0]=0x01;
        GPS_GSA_Data.HDOP[1]=0x01;
        GPS_GSA_Data.HDOP[2]=0x01;
        GPS_GSA_Data.HDOP[3]=0x01;
    }
}

bool gps_recive_ok(void)
{
    if (ReciveFlag)
    {
        ReciveFlag=0;
        return 1;
    }
    else
    {
        return 0;
    }
}

void gps_data_convert(void)
{
	int8_t i;
	uint32_t Data;

	Data = 0;
	for(i=2;i<9;i++)
	{
		if(i == 4)continue;
		Data = Data * 10 + GPS_RMC_Data.Latitude[i] - '0';
	}

	Data = Data * 5 / 3; // *100/60

	GPS_RMC_Data.Latitude[2] = '.';

	for(i=8;i>2;i--)
	{
		GPS_RMC_Data.Latitude[i] = Data % 10 + '0';
		Data = Data / 10;
	}

	Data = 0;
	for(i=2;i<10;i++)
	{
		if(i == 5)continue;
		Data = Data * 10 + GPS_RMC_Data.Longitude[i] - '0';
	}

	Data = Data * 5 / 3; // *100/60

	GPS_RMC_Data.Longitude[3] = '.';
	for(i=9;i>3;i--)
	{
		GPS_RMC_Data.Longitude[i] = Data % 10 + '0';
		Data = Data / 10;
	}
}

static void ParserGPGGA(void)
{
    switch(SBUF)
    {
        case '*': //语句结束
            NMEA_CMD_Start=0;
            break;
        case ',': //该字段结束
            NMEA_DAT_Block++;
            NMEA_DAT_BlockIndex=0;
            break;
        default:    //字段字符
            switch(NMEA_DAT_Block) //判断当前处于哪个字段
            {

                case 5:             //<6> GPS状态 0=未定位, 1=非差分定位, 2=差分定位, 6=正在估算
                    GPS_GGA_Data.PositionFix=SBUF;
                    break;
                case 6:             //<7> 正在使用的卫星数量 00~12
                    GPS_GGA_Data.SatUsed[NMEA_DAT_BlockIndex]=SBUF;
                    break;

                    case 8:         //<9> 海拔高度 -9999.9~99999.9
                    GPS_GGA_Data.Altitude[NMEA_DAT_BlockIndex]=SBUF;
                    break;
            }
            NMEA_DAT_BlockIndex++;     //字段字符索引++, 指向下一个字符
    }
}

static void ParserGPRMC(void)
{
    switch(SBUF)
    {
        case '*':
            NMEA_CMD_Start=0;
            ReciveFlag=1;     //接收完毕, 可以处理
            break;
        case ',':
            NMEA_DAT_Block++;
            NMEA_DAT_BlockIndex=0;
            break;
        default:
            switch(NMEA_DAT_Block)
            {
                case 0:         //<1> UTC时间 hhmmss.mmm
                    switch(NMEA_DAT_BlockIndex)
                    {
                        case 0:
                        case 2:
                        case 4:
                            ucTempA=SBUF-'0';
                            break;
                        case 1:
                            GPS_RMC_Data.UTCDateTime[3]=ucTempA*10+SBUF-'0';
                            break;
                        case 3:
                            GPS_RMC_Data.UTCDateTime[4]=ucTempA*10+SBUF-'0';
                            break;
                        case 5:
                            GPS_RMC_Data.UTCDateTime[5]=ucTempA*10+SBUF-'0';
                            break;
                    }
                    break;
                case 1:         //<2> 定位状态 A=有效定位, V=无效定位
                    GPS_RMC_Data.Status=SBUF;
                    break;
                case 2:         //<3> 纬度 ddmm.mmmm
                    GPS_RMC_Data.Latitude[NMEA_DAT_BlockIndex]=SBUF;
                    break;
                case 3:         //<4> 纬度半球 N/S
                    GPS_RMC_Data.NS=SBUF;
                    break;
                case 4:         //<5> 经度 dddmm.mmmm
                    GPS_RMC_Data.Longitude[NMEA_DAT_BlockIndex]=SBUF;
                    break;
                case 5:         //<6> 经度半球 E/W
                    GPS_RMC_Data.EW=SBUF;
                    break;
                case 6:         //<7> 地面速率 000.0~999.9 节
                    GPS_RMC_Data.Speed[NMEA_DAT_BlockIndex]=SBUF;
                    break;
                case 7:         //<8> 地面航向 000.0~359.9 度, 以真北为参考基准
                    GPS_RMC_Data.Course[NMEA_DAT_BlockIndex]=SBUF;
                    break;
                case 8:         //<9> UTC日期 ddmmyy
                    switch(NMEA_DAT_BlockIndex)
                    {
                        case 0:
                        case 2:
                        case 4:
                            ucTempA=SBUF-'0';
                            break;
                        case 1:
                            GPS_RMC_Data.UTCDateTime[2]=ucTempA*10+SBUF-'0';
                            break;
                        case 3:
                            GPS_RMC_Data.UTCDateTime[1]=ucTempA*10+SBUF-'0';
                            break;
                        case 5:
                            GPS_RMC_Data.UTCDateTime[0]=ucTempA*10+SBUF-'0';
                            break;
                    }
                    break;
            }
            NMEA_DAT_BlockIndex++;
    }
}

static void ParserGPGSA(void)
{
    switch(SBUF)
    {
        case '*':
            NMEA_CMD_Start=0;
            break;
        case ',':
            NMEA_DAT_Block++;
            NMEA_DAT_BlockIndex=0;
            //清空已使用卫星号, 避免上次数据干扰
            if((NMEA_DAT_Block>=2)||(NMEA_DAT_Block<=13))
                GPS_GSA_Data.SatUsedList[NMEA_DAT_Block-2]=0x00;
            break;
        default:
            switch(NMEA_DAT_Block)
            {
                case 0:         //<1>模式 M=手动, A=自动
                    GPS_GSA_Data.Mode=SBUF;
                    break;
                case 1:         //<2>定位型式 1=未定位, 2=二维定位, 3=三维定位
                    GPS_GSA_Data.Mode2=SBUF;
                    break;
                case 2:         //<3> PRN 01~32 使用中的卫星编号
                case 3:
                case 4:
                case 5:
                case 6:
                case 7:
                case 8:
                case 9:
                case 10:
                case 11:
                case 12:
                case 13:
                    switch(NMEA_DAT_BlockIndex)
                    {
                        case 0:
                            ucTempA=SBUF-'0';
                            break;
                        case 1:
                            GPS_GSA_Data.SatUsedList[NMEA_DAT_Block-2]=ucTempA*10+SBUF-'0';
                            break;
                    }
                    break;
                case 14:        //<4> PDOP 位置精度因子 0.5~99.9
                    GPS_GSA_Data.PDOP[NMEA_DAT_BlockIndex]=SBUF;
                    break;
                case 15:        //<5> HDOP 水平精度因子 0.5~99.9
                    GPS_GSA_Data.HDOP[NMEA_DAT_BlockIndex]=SBUF;
                    break;
                case 16:        //<6> VDOP 垂直精度因子 0.5~99.9
                    GPS_GSA_Data.VDOP[NMEA_DAT_BlockIndex]=SBUF;
                    break;
                }
            NMEA_DAT_BlockIndex++;
    }
}

static void ParserGPGSV(void)
{
    switch(SBUF)
    {
        case '*':
            NMEA_CMD_Start=0;
            break;
        case ',':
            NMEA_DAT_Block++;
            NMEA_DAT_BlockIndex=0;
            break;
        default:
            switch(NMEA_DAT_Block)
            {

                case 1:         //<2> 本句GSV的编号
                    if (SBUF=='1') SateInfoIndex=0;
                    //解析到第一句 GSV 语句 则判断卫星信息从心开始
                    break;
                case 2:         //<3> 可见卫星的总数 00~12
                    switch(NMEA_DAT_BlockIndex)
                    {
                        case 0:
                            ucTempA=SBUF-'0';
                            break;
                        case 1:
                            GPS_GSV_Data.SatInView=ucTempA*10+SBUF-'0';
                            break;
                    }
                    break;
                case 3:         //<4> 卫星编号 01~32
                case 7:
                case 11:
                case 15:
                    switch(NMEA_DAT_BlockIndex)
                    {
                        case 0:
                            ucTempA=SBUF-'0';
                            break;
                        case 1:
                            GPS_GSV_Data.SatInfo[SateInfoIndex].SatID=ucTempA*10+SBUF-'0';

                            GPS_GSV_Data.SatInfo[SateInfoIndex].SNR=0x00;
                            //清空信噪比数据, 避免上次数据干扰
                            //因为当卫星信噪比无时GPS输出的NMEA数据中信噪比字段没有,
                            //而导致直接跳到下一字段, 此时上次若有数据则直接会残留下来

                            SateInfoIndex++;
                            //卫星信息索引+1, 以下都-1处理
                            //同上, 避免放于"信噪比"字段为空时处理不到

                            break;
                    }
                    break;

                case 6:         //<7>讯号噪声比 C/No 00~99
                case 10:
                case 14:
                case 18:
                    switch(NMEA_DAT_BlockIndex)
                    {
                        case 0:
                            ucTempA=SBUF-'0';
                            break;
                        case 1:
                            GPS_GSV_Data.SatInfo[SateInfoIndex-1].SNR=ucTempA*10+SBUF-'0';
                            break;
                    }
                    break;
             }
        NMEA_DAT_BlockIndex++;
    }
}

void gps_parser(void)
{
    if(NMEA_CMD_Start)
    {             //解析到以$开始的 NMEA 语句, 进入NMEA 解析流程:
        if(NMEA_CMD_Parsered)
        {         //CMD语句类型解析完毕, 根据类型条用解析函数
            switch(NMEA_CMD)
            {
                case NMEA_GPGGA:
                    ParserGPGGA();
                    break;
                case NMEA_GPGSA:
                    ParserGPGSA();
                    break;
                case NMEA_GPGSV:
                    ParserGPGSV();
                    break;
                case NMEA_GPRMC:
                    ParserGPRMC();
                    break;
                default:    //无法识别的格式, 复位
                    NMEA_CMD=NMEA_NULL;
                    NMEA_CMD_Parsered=0;
                    NMEA_CMD_Index=1;
                    NMEA_CMD_Start=0;
            }
        }
        else
        {         //需要解析CMD语句类型
            switch(SBUF)
            {
                case ',':     //第一个字段结束
                    if(NMEA_CMD_Buff[4]=='G'&&NMEA_CMD_Buff[5]=='A') NMEA_CMD=NMEA_GPGGA;
                    if(NMEA_CMD_Buff[4]=='S'&&NMEA_CMD_Buff[5]=='A') NMEA_CMD=NMEA_GPGSA;
                    if(NMEA_CMD_Buff[5]=='V') NMEA_CMD=NMEA_GPGSV;
                    if(NMEA_CMD_Buff[5]=='C') NMEA_CMD=NMEA_GPRMC;
                    //此处如果都不成立, 即语句不被识别, 则NMEA_CMD为NULL或其他,
                    //则转为根据类型解析时会跳转到无法识别的格式, 而后复位
                    NMEA_CMD_Parsered=1;
                    NMEA_CMD_Index=1;
                    NMEA_DAT_Block=0;
                    NMEA_DAT_BlockIndex=0;
                    break;
                case '*':
                    NMEA_CMD_Start=0;
                    break;
                default:        //处于第一个字段中, 继续接收
                    NMEA_CMD_Buff[NMEA_CMD_Index]=SBUF;
                    NMEA_CMD_Index++;
                    if(NMEA_CMD_Index>6) NMEA_CMD_Start=0;
                    // CMD 超过6个字符, (数组越界, 导致死机)
                    // 则判断不是正常的CMD语句, 则略过此句.
            }
        }
    }
    else
    {             //未解析到$, 循环接收并判断 直到 $
        if (SBUF=='$')
        {         //接收到$, 下一个字符即为类型判断字符, 先进行相关变量初始化
            NMEA_CMD_Buff[0]=SBUF;
            NMEA_CMD_Start=1;     //下次调用则进入NMEA 解析流程:
            NMEA_CMD_Index=1;     //从头存放GPS类型字符到变量
            NMEA_CMD_Parsered=0;
            NMEA_CMD=NMEA_NULL;
            NMEA_DAT_Block=0;
            NMEA_DAT_BlockIndex=0;
        }
    }
}
