/* --------------------------------------------------- */
/*   SH2-PC�֤Υ��ꥢ���̿��򤹤뤿��δؿ���             */
/*                          ISS�Υץ�����١����˺���*/
/*           ������DTC����� */
/* --------------------------------------------------- */
#include <7040s.h>
#include <serial.h>
#include <globals.h>
#include <SH_vel.h>
#include <sh_vel_parameters.h>

/* ------------------------------------------------- */
/* SCI1 INITIALIZATION fixed baud at 38400           */
/* ------------------------------------------------- */
void initSCI1(void)
{
  int i;
  PFC.PACRL2.BIT.PA4MD = 1;/* 0:PA4, 1:TXD1 */
  PFC.PACRL2.BIT.PA3MD = 1;/* 0:PA3, 1:RXD1 */
  SCI1.SCR.BYTE = 0;       /* clear all flags */
  SCI1.SCR.BYTE = 0;       /* clear all flags */
  /* 2400-38400baud are available at n=0(cks1=0,cks2=0) */
  
  SCI1.SMR.BYTE = 0;//|0x40;       /* Ascnc, 7bit , NoParity, stop1, 1/1 */
  SCI1.BRR = 22;	         /* 38400baud (CPU=28.6MHz) */
  for(i=0;i<1000000;i++);      /* wait more than 1bit time */
  SCI1.SCR.BYTE = 0x30;    /* scr = 0011 0000 (TE=1,RE=1) */
  return;
}


/*----------------------����-----------------------------*/
/*�����ǡ����Υ��󥳡���*/
int encode(unsigned char *src,int len,unsigned char *dst,int buf_max)
{
     int pos,s_pos,w_pos;
     unsigned short b;
     pos = 0;   //read position
     w_pos = 1; //write_position
     s_pos = 0;
     b = 0;
   
     while(pos < len  || s_pos >= 6){
       if(s_pos >= 6){
	 dst[w_pos] = ((b>>10)&0x3f)+0x40;
	 w_pos++;if(w_pos >= buf_max)return(-1);
	 b = b<< 6;
	 s_pos-=6;
       }else{
	 b |= src[pos]<<(8-s_pos);
	 s_pos+=8;
	 pos++;
	 if(pos >= len)s_pos+=4;//�Ǹ�
       }
     }
     
     if(++w_pos >= buf_max)return(-1);
     dst[0] =0x09;// ECC_STX;    //0x09
     dst[w_pos-1] = 0x0a;// ECC_ETX;//0x0a  
     return w_pos;
}

/*���ɥ�ȥ�ǡ���������*/
void odometry_send(short cnt1, short cnt2, short pwm1,short pwm2 ){
  unsigned char tmp;
  char data[8], send_buf[16];
  int len,i;

  data[0] = ((Short_2Char)cnt1).byte[0];
  data[1] = ((Short_2Char)cnt1).byte[1];
  data[2] = ((Short_2Char)cnt2).byte[0];
  data[3] = ((Short_2Char)cnt2).byte[1];
  data[4] = ((Short_2Char)pwm1).byte[0];
  data[5] = ((Short_2Char)pwm1).byte[1];
  data[6] = ((Short_2Char)pwm2).byte[0];
  data[7] = ((Short_2Char)pwm2).byte[1];

  len = encode(data,8,send_buf,16);
  for(i = 0;i < len;i++){
    /*�����ޤ�*/
    do{
      tmp = SCI1.SSR.BYTE;
    }while((tmp & 0x80)==0);
    SCI1.TDR = send_buf[i];
    SCI1.SSR.BIT.TDRE = 0;
  }
}


/*----------------------���� with DTC-----------------------------*/
// DTC�ν����
void initDTC(void){
    DTC1.DTMR.BIT.SM1 = 0;
    DTC1.DTMR.BIT.DM1 = 1;
    DTC1.DTMR.BIT.DM0 = 0;
    DTC1.DTMR.BIT.MD1 = 0;
    DTC1.DTMR.BIT.MD0 = 1;
    DTC1.DTMR.BIT.SZ1 = 0;
    DTC1.DTMR.BIT.SZ0 = 0;
    DTC1.DTMR.BIT.DTS = 0;
    DTC1.DTMR.BIT.CHNE = 0;
    DTC1.DTMR.BIT.DISEL = 0;
    DTC1.DTCRA.BYTE.H = DTC_RECEIVE_BUFFER_SIZE;
    DTC1.DTCRA.BYTE.L = DTC_RECEIVE_BUFFER_SIZE;

    DTC1.DTIAR = (unsigned int) DTC_receive_buffer;
    DTC1.DTSAR = (unsigned int)&(SCI1.RDR);
    DTC1.DTDAR = (unsigned int) DTC_receive_buffer;

    DTC.DTBR = (unsigned short)(((unsigned int)&DTC1)>>16);
}


// ������������
void startDTC(void){
    DTC.DTED.BIT.RXI1 = 1;
    SCI1.SCR.BIT.RIE = 1;
}

// �������ȥå�
void stopDTC(void){
    DTC.DTED.BIT.RXI1 = 0;
    SCI1.SCR.BIT.RIE = 0;
}

/*************************************************************/


/*-----------�Хåե��ɤ߹��ߡ����ޥ�ɼ¹�-----------*/
int DTC_receive(void)
{
  //  static int state;/*�ɤ߹��߾���*/
  int DTC_receive_length = 0;
  //DTC1.DTCRA.BYTE.L
  while((int)DTC_receive_rp != (DTC1.DTDAR & 0x00ffffff)){
     
    /*�ǡ�������*/
    switch(DTC_receive_state){
      
      /********�������ȥХ����Ԥ����ơ���**********/
    case 0:/*�ǡ����Ԥ�*/
      if(*DTC_receive_rp == SER_STX){
	DTC_receive_state = 1;
	DTC_receive_data_pos = 0;
      }
      break;

      /************�ǡ����Υǥ����ɽ������ơ���***********/
      /***** 6/24 *****/
    case 1: 
      if(*DTC_receive_rp == SER_ETX){
	/*�ǥ����ɽ�λ*/
	DTC_receive_length = DTC_receive_data_pos;
	DTC_receive_state = 0;      
      }else{/*�ǡ����Υǥ�����*/
	DTC_receive_data[DTC_receive_data_pos] = 
	  ((*DTC_receive_rp - 0x40)<<2)&0xfc;
	DTC_receive_state = 2;
      }
      break;

      /***** 12/24 *****/
    case 2:
      if(*DTC_receive_rp == SER_ETX){
	/*�ǥ����ɽ�λ*/
	DTC_receive_length = DTC_receive_data_pos;
	DTC_receive_state = 0;
      }else{
	DTC_receive_data[DTC_receive_data_pos] |= 
	  (((*DTC_receive_rp - 0x40)>>4)&0x03);
	DTC_receive_data_pos++;
	DTC_receive_data[DTC_receive_data_pos] = 
	  ((*DTC_receive_rp - 0x40)<<4)&0xf0;
	DTC_receive_state = 3;
      }

      break;

      /***** 18/24 *****/
    case 3: 
      if(*DTC_receive_rp == SER_ETX){
	/*�ǥ����ɽ�λ*/
	DTC_receive_length = DTC_receive_data_pos;
	DTC_receive_state = 0;
      }else{
	DTC_receive_data[DTC_receive_data_pos] |= 
	  (((*DTC_receive_rp - 0x40)>>2)&0x0f);
	DTC_receive_data_pos++;
	DTC_receive_data[DTC_receive_data_pos] = 
	  ((*DTC_receive_rp - 0x40)<<6)&0xc0;
	DTC_receive_state = 4;
      }
      break;
      
      /***** 24/24 *****/
    case 4: 
      if(*DTC_receive_rp == SER_ETX){
	/*�ǥ����ɽ�λ*/
	DTC_receive_length = DTC_receive_data_pos;
	DTC_receive_state = 0;
      }else{
	DTC_receive_data[DTC_receive_data_pos] |= 
	  ((*DTC_receive_rp - 0x40)&0x3f);
	DTC_receive_data_pos++;
	DTC_receive_state = 1;
      }
      break;


    default:
      DTC_receive_state = 0;/*�۾����*/
      break;
    }

    /*���Υǡ�����*/
    DTC_receive_rp++;
    if(DTC_receive_rp >= DTC_receive_buffer + DTC_RECEIVE_BUFFER_SIZE){
      DTC_receive_rp = DTC_receive_buffer;
    }

    /*���ޥ�ɤ�������Ƥ����齪λ*/
    if(DTC_receive_length > 0){
      return DTC_receive_length;
    }
  }
  return 0;
}



/*************************************************************/

