/* ------------------------------------------------------- */
/*   SH2/7045�ǥ⡼�������뤿��δؿ�����������������������*/
/*��           ����    ISS�Υץ�����١����˺���          */
/* ------------------------------------------------------- */
#ifndef __MOTOR_CONTROL__
#define __MOTOR_CONTROL__

#define MOTOR_NUM 2

#define MTR_MODE_FREE 0
#define MTR_MODE_CW_BRAKE 1
#define MTR_MODE_CCW_BRAKE 2

#define MTR_PWM_DEFAULT_MAX 255

/////////////////// �ǥ塼�ƥ��������� pwm_set //////////////////////
void set_pwm(const unsigned char id, const int duty);
void put_pwm(int ch, int pwm);
void set_mode(const unsigned char id, const int mode);

///////////////////////// init counter ///////////////////////////
void initCounter(void);
////////////////////////// init PWM //////////////////////////////////
void initPWM();
/////////////////// ��������ͼ����� cnt_read ////////////////////
void cnt_read(void);
#endif
