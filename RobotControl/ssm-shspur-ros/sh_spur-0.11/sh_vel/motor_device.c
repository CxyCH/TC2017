/* ------------------------------------------------------- */
/*   SH2/7045でモータを操作するための関数群 　　　            */
/*                             ISSのプログラムベースで作成   */
/* ------------------------------------------------------- */
#include<7040s.h>
#include<motor_device.h>
#include<globals.h>


/*-----------------PWMの関数-------------------*/
////////////////////////// init PWM //////////////////////////////////

void initPWM(){
  int i;
  
  PFC.PEIOR.WORD |= 0x0500;	/* use TIOC3A, TIOC3C port */
  MTU3.TCR.BYTE = 0x20;		/* cycle=TGRA, duty=TGRB */
  MTU3.TMDR.BYTE = 0xc2;	/* pwm mode 1, buffer disable */
  MTU3.TIOR.BYTE.H = 0x12;	/* CMA=0, CMB=1 */
  MTU3.TIOR.BYTE.L = 0x12;	/* CMC=0, CMD=1 */
  MTU3.TGRA = MTU3.TGRC = MTR_PWM_DEFAULT_MAX;	/* cycle */
  pwm_max[0] = MTR_PWM_DEFAULT_MAX;
  pwm_max[1] = MTR_PWM_DEFAULT_MAX;
  pwm_min[0] = -MTR_PWM_DEFAULT_MAX;
  pwm_min[1] = -MTR_PWM_DEFAULT_MAX;
  
  for (i = 0; i < 2; i++) {
    set_pwm(i, 0);
    set_mode(i, MTR_MODE_FREE);
  }

  PFC.PECR1.WORD |= 0x0011;	/* use TGRA, TGRC port */

  MTU.TSTR.BYTE |= 0xc0;	/* pwm active */

  PFC.PECR1.WORD &= ~0x0044;	/* TIOC3B, TIOC3D */
  PFC.PECR2.WORD &= ~0x4400;	/* TIOC1B, TIOC2B */
  PFC.PEIOR.WORD |= 0x0aa0;
}

/////////////////// デューティー比設定 pwm_set //////////////////////
void set_pwm(const unsigned char id, const int duty) {
  if (id == 0) {
    if (duty <= 0 || duty >= pwm_max[id]) {
      PFC.PECR1.WORD &= ~0x0001; /* PE8 */
      PE.DR.WORD &= ~0x0100;
      PE.DR.WORD |= (duty <= 0) ? 0 : 0x0100;
    } else {
      PFC.PECR1.WORD |= 0x0001; /* TIOC3A */
      MTU3.TGRB = duty;
    }
  } else if (id == 1) {
    if (duty <= 0 || duty >=  pwm_max[id]) {
      PFC.PECR1.WORD &= ~0x0010; /* PE10 */
      PE.DR.WORD &= ~0x0400;
      PE.DR.WORD |= (duty <= 0) ? 0 : 0x0400;
    } else {
      PFC.PECR1.WORD |= 0x0010; /* TIOC3C */
      MTU3.TGRD = duty;
    }
  }
}

/////////////////////////////////////////////////////

void put_pwm(int ch, int pwm){
  if(pwm < 0){
    set_mode(ch,  MTR_MODE_CW_BRAKE);
    set_pwm(ch, -pwm);
  }else{
    set_mode(ch,  MTR_MODE_CCW_BRAKE);
    set_pwm(ch, pwm);
  }
}
//
void set_mode(const unsigned char id, const int mode) {
  if (id == 0) {
    /* DIR(PE5), EN(PE7) */
    if (mode == MTR_MODE_CCW_BRAKE) {
      PE.DR.WORD &= ~0x0020;	/* DIR(PE5)=1 */
      PE.DR.WORD &= ~0x0080;	/* EN(PE7)=0 */
    } else if (mode == MTR_MODE_CW_BRAKE) {
      PE.DR.WORD |= 0x0020;	/* DIR(PE5)=1 */
      PE.DR.WORD &= ~0x0080;	/* EN(PE7)=0 */
    } else {			/* MTR_MODE_FREE */
      PE.DR.WORD |= 0x0080;	/* EN(PE7)=1 */
    }

  } else if (id == 1) {
    /* DIR(PE9), EN(PE11) */
    if (mode == MTR_MODE_CCW_BRAKE) {
      PE.DR.WORD &= ~0x0200;	/* DIR(PE9)=1 */
      PE.DR.WORD &= ~0x0800;	/* EN(PE11)=1 */
    } else if (mode == MTR_MODE_CW_BRAKE) {
      PE.DR.WORD |= 0x0200;	/* DIR(PE9)=0 */
      PE.DR.WORD &= ~0x0800;	/* EN(PE11)=1 */
    } else {			/* MTR_MODE_FREE */
      PE.DR.WORD |= 0x0800;	/* EN(PE11)=1 */
    }
  }
}


/*--------------------------カウンタの関数-----------------------------------*/

///////////////////////// init counter ///////////////////////////

void initCounter(void){
  MTU1.TMDR.BYTE = 0xc4;	/* encoder mode 1 */
  MTU2.TMDR.BYTE = 0xc4;	/* encoder mode 1 */
  MTU.TSTR.BYTE |= 0x06;	/* start count up */
  
  PFC.PECR2.WORD |= 0x1100;	/* TIOC1A, TIOC2A */
  MTU1.TIOR.BYTE = 0x0a;	/* input capture */
  MTU2.TIOR.BYTE = 0x0a;	/* input capture */

  PFC.PEIOR.WORD &= ~0x0050;	/* use TIOC1A, TIOC2A port */

  PFC.PACRL2.WORD &= ~0xf000;	/* TCLKA, TCLKB */
  PFC.PACRL2.WORD |= 0x5000;
  PFC.PACRL1.WORD &= ~0x000f;	/* TCLKC, TCLKD */
  PFC.PACRL1.WORD |= 0x0005;

  MTU1.TCNT = 0;		/* counter=0 */
  MTU2.TCNT = 0;
  MTU1.TGRA = 0;		/* buffer=0 */
  MTU2.TGRA = 0;
}
/////////////////// カウント値取り込み cnt_read ////////////////////

void cnt_read(void){

    static short cnt_old[2];
    short int i;
    
    counter[0] = (volatile)MTU1.TCNT;
    counter[1] = (volatile)MTU2.TCNT;

    for(i = 0; i < 2 ; i++){
      cnt_dif[i] = (short)(counter[i]-cnt_old[i]);
      cnt_old[i] = counter[i];
    }
}





