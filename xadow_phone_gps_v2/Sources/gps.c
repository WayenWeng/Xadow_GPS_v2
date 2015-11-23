
#include <stdbool.h>
#include "board.h"
#include "gps.h"


static uint8_t NMEA_CMD=NMEA_NULL;        //NMEA ���
static uint8_t NMEA_CMD_Buff[]="$GPxxx,"; //NMEA ������ͻ���
static uint8_t NMEA_CMD_Index=0;          //��ȡ CMD�ַ��ĸ���
static bool NMEA_CMD_Parsered=0;          //CMD���ͽ������
static uint8_t NMEA_DAT_Block=0;          //NMEA �����ֶκ� ��0��ʼ
static uint8_t NMEA_DAT_BlockIndex=0;     //NMEA ����ÿ���ֶ����ַ����� ��0��ʼ
static bool NMEA_CMD_Start=0;             //NMEA ��俪ʼ. ��⵽ $ ʱ��1
static bool ReciveFlag=0;                 //���ݽ������. ���һ�� GPRMC ��䷢�������1,

static uint8_t ucTempA=0;                 //�洢������λ�����õĵ�ʮλ��ʱ����
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
        case '*': //������
            NMEA_CMD_Start=0;
            break;
        case ',': //���ֶν���
            NMEA_DAT_Block++;
            NMEA_DAT_BlockIndex=0;
            break;
        default:    //�ֶ��ַ�
            switch(NMEA_DAT_Block) //�жϵ�ǰ�����ĸ��ֶ�
            {

                case 5:             //<6> GPS״̬ 0=δ��λ, 1=�ǲ�ֶ�λ, 2=��ֶ�λ, 6=���ڹ���
                    GPS_GGA_Data.PositionFix=SBUF;
                    break;
                case 6:             //<7> ����ʹ�õ��������� 00~12
                    GPS_GGA_Data.SatUsed[NMEA_DAT_BlockIndex]=SBUF;
                    break;

                    case 8:         //<9> ���θ߶� -9999.9~99999.9
                    GPS_GGA_Data.Altitude[NMEA_DAT_BlockIndex]=SBUF;
                    break;
            }
            NMEA_DAT_BlockIndex++;     //�ֶ��ַ�����++, ָ����һ���ַ�
    }
}

static void ParserGPRMC(void)
{
    switch(SBUF)
    {
        case '*':
            NMEA_CMD_Start=0;
            ReciveFlag=1;     //�������, ���Դ���
            break;
        case ',':
            NMEA_DAT_Block++;
            NMEA_DAT_BlockIndex=0;
            break;
        default:
            switch(NMEA_DAT_Block)
            {
                case 0:         //<1> UTCʱ�� hhmmss.mmm
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
                case 1:         //<2> ��λ״̬ A=��Ч��λ, V=��Ч��λ
                    GPS_RMC_Data.Status=SBUF;
                    break;
                case 2:         //<3> γ�� ddmm.mmmm
                    GPS_RMC_Data.Latitude[NMEA_DAT_BlockIndex]=SBUF;
                    break;
                case 3:         //<4> γ�Ȱ��� N/S
                    GPS_RMC_Data.NS=SBUF;
                    break;
                case 4:         //<5> ���� dddmm.mmmm
                    GPS_RMC_Data.Longitude[NMEA_DAT_BlockIndex]=SBUF;
                    break;
                case 5:         //<6> ���Ȱ��� E/W
                    GPS_RMC_Data.EW=SBUF;
                    break;
                case 6:         //<7> �������� 000.0~999.9 ��
                    GPS_RMC_Data.Speed[NMEA_DAT_BlockIndex]=SBUF;
                    break;
                case 7:         //<8> ���溽�� 000.0~359.9 ��, ���汱Ϊ�ο���׼
                    GPS_RMC_Data.Course[NMEA_DAT_BlockIndex]=SBUF;
                    break;
                case 8:         //<9> UTC���� ddmmyy
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
            //�����ʹ�����Ǻ�, �����ϴ����ݸ���
            if((NMEA_DAT_Block>=2)||(NMEA_DAT_Block<=13))
                GPS_GSA_Data.SatUsedList[NMEA_DAT_Block-2]=0x00;
            break;
        default:
            switch(NMEA_DAT_Block)
            {
                case 0:         //<1>ģʽ M=�ֶ�, A=�Զ�
                    GPS_GSA_Data.Mode=SBUF;
                    break;
                case 1:         //<2>��λ��ʽ 1=δ��λ, 2=��ά��λ, 3=��ά��λ
                    GPS_GSA_Data.Mode2=SBUF;
                    break;
                case 2:         //<3> PRN 01~32 ʹ���е����Ǳ��
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
                case 14:        //<4> PDOP λ�þ������� 0.5~99.9
                    GPS_GSA_Data.PDOP[NMEA_DAT_BlockIndex]=SBUF;
                    break;
                case 15:        //<5> HDOP ˮƽ�������� 0.5~99.9
                    GPS_GSA_Data.HDOP[NMEA_DAT_BlockIndex]=SBUF;
                    break;
                case 16:        //<6> VDOP ��ֱ�������� 0.5~99.9
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

                case 1:         //<2> ����GSV�ı��
                    if (SBUF=='1') SateInfoIndex=0;
                    //��������һ�� GSV ��� ���ж�������Ϣ���Ŀ�ʼ
                    break;
                case 2:         //<3> �ɼ����ǵ����� 00~12
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
                case 3:         //<4> ���Ǳ�� 01~32
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
                            //������������, �����ϴ����ݸ���
                            //��Ϊ�������������ʱGPS�����NMEA������������ֶ�û��,
                            //������ֱ��������һ�ֶ�, ��ʱ�ϴ�����������ֱ�ӻ��������

                            SateInfoIndex++;
                            //������Ϣ����+1, ���¶�-1����
                            //ͬ��, �������"�����"�ֶ�Ϊ��ʱ������

                            break;
                    }
                    break;

                case 6:         //<7>Ѷ�������� C/No 00~99
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
    {             //��������$��ʼ�� NMEA ���, ����NMEA ��������:
        if(NMEA_CMD_Parsered)
        {         //CMD������ͽ������, �����������ý�������
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
                default:    //�޷�ʶ��ĸ�ʽ, ��λ
                    NMEA_CMD=NMEA_NULL;
                    NMEA_CMD_Parsered=0;
                    NMEA_CMD_Index=1;
                    NMEA_CMD_Start=0;
            }
        }
        else
        {         //��Ҫ����CMD�������
            switch(SBUF)
            {
                case ',':     //��һ���ֶν���
                    if(NMEA_CMD_Buff[4]=='G'&&NMEA_CMD_Buff[5]=='A') NMEA_CMD=NMEA_GPGGA;
                    if(NMEA_CMD_Buff[4]=='S'&&NMEA_CMD_Buff[5]=='A') NMEA_CMD=NMEA_GPGSA;
                    if(NMEA_CMD_Buff[5]=='V') NMEA_CMD=NMEA_GPGSV;
                    if(NMEA_CMD_Buff[5]=='C') NMEA_CMD=NMEA_GPRMC;
                    //�˴������������, ����䲻��ʶ��, ��NMEA_CMDΪNULL������,
                    //��תΪ�������ͽ���ʱ����ת���޷�ʶ��ĸ�ʽ, ����λ
                    NMEA_CMD_Parsered=1;
                    NMEA_CMD_Index=1;
                    NMEA_DAT_Block=0;
                    NMEA_DAT_BlockIndex=0;
                    break;
                case '*':
                    NMEA_CMD_Start=0;
                    break;
                default:        //���ڵ�һ���ֶ���, ��������
                    NMEA_CMD_Buff[NMEA_CMD_Index]=SBUF;
                    NMEA_CMD_Index++;
                    if(NMEA_CMD_Index>6) NMEA_CMD_Start=0;
                    // CMD ����6���ַ�, (����Խ��, ��������)
                    // ���жϲ���������CMD���, ���Թ��˾�.
            }
        }
    }
    else
    {             //δ������$, ѭ�����ղ��ж� ֱ�� $
        if (SBUF=='$')
        {         //���յ�$, ��һ���ַ���Ϊ�����ж��ַ�, �Ƚ�����ر�����ʼ��
            NMEA_CMD_Buff[0]=SBUF;
            NMEA_CMD_Start=1;     //�´ε��������NMEA ��������:
            NMEA_CMD_Index=1;     //��ͷ���GPS�����ַ�������
            NMEA_CMD_Parsered=0;
            NMEA_CMD=NMEA_NULL;
            NMEA_DAT_Block=0;
            NMEA_DAT_BlockIndex=0;
        }
    }
}
