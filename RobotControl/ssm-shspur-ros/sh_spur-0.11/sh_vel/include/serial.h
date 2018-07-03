/* --------------------------------------------------- */
/*   SH2-PC�֤Υ��ꥢ���̿���DTC�ǹԤ�����Υץ����     */
/*                   ISS�Υץ�����١����˺���        */
/* --------------------------------------------------- */
#ifndef __SERIAL__
#define __SERIAL__

//defines
#define DTC_RECEIVE_BUFFER_SIZE 255
#define SER_STX 0x09 //start
#define SER_ETX 0x0a //end


// DTC�Υ����Υ��ɥ쥹����
struct dtc1 {                             /* struct DTC1  */
    union {                               /*    DTMR      */
	unsigned short WORD;              /*  Word Access */
	struct {                          /*  Bit  Access */
	    unsigned char SM1  :1;        /*     SM1      */
	    unsigned char SM0  :1;        /*     SM0      */
	    unsigned char DM1  :1;        /*     DM1      */
	    unsigned char DM0  :1;        /*     DM0      */
	    unsigned char MD1  :1;        /*     MD1      */
	    unsigned char MD0  :1;        /*     MD0      */
	    unsigned char SZ1  :1;        /*     SZ1      */
	    unsigned char SZ0  :1;        /*     SZ0      */
	    unsigned char DTS  :1;        /*     DTS      */
	    unsigned char CHNE :1;        /*     CHNE     */
	    unsigned char DISEL:1;        /*     DISEL    */
	    unsigned char NMIM :1;        /*     NMIM     */
	    unsigned char      :4;        /*              */
	}      BIT;                       /*              */
    }           DTMR;                     /*              */
    union {                               /*    DTCRA     */
        unsigned short WORD;              /*  Word Access */
        struct {                          /*  Byte Access */
            unsigned char H;              /*    DTCRAH    */
            unsigned char L;              /*    DTCRAL    */
        }     BYTE;                       /*              */
    }          DTCRA;                     /*              */
    unsigned int    DTIAR;                /*    DTIAR     */
    unsigned int    DTSAR;                /*    DTSAR     */
    unsigned int    DTDAR;                /*    DTDAR     */
}DTC1;


/*�����*/
void initSCI1(void);

/*�����Ѵؿ�*/
int encode(unsigned char *src,int len,unsigned char *dst,int buf_max);
void odometry_send(short cnt1, short cnt2, short pwm1,short pwm2 );

/*�����Ѵؿ�*/
void initDTC(void);// DTC�ν����
void startDTC(void);// ������������
void stopDTC(void);// �������ȥå�
int DTC_receive(void);/*DTC������ɤ߹��ߡ��ǥ�����*/

#endif
/*************************************************************/

