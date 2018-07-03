#ifndef __GLOBALS__
#define __GLOBALS__

#ifdef GLOBAL_DEFINE
#define GLOBAL 
#else 
#define GLOBAL extern
#endif

#include<serial.h>
#include<motor_device.h>

GLOBAL char cnt_updated ;
GLOBAL short watch_dog;

GLOBAL short counter[MOTOR_NUM];      //�ƥ����ͥ�Υ�������
GLOBAL int cnt_dif[MOTOR_NUM];      //WAIT�֤Υ�����ȿ�
GLOBAL short  counter_buf[MOTOR_NUM];

/*�ѥ�᡼���ݸ���*/
GLOBAL char param_lock;

GLOBAL char param_change; //��ɸ����������ä��ݡ�
GLOBAL char servo_level;  //�����ܤξ���

/**/
GLOBAL int out_pwm[MOTOR_NUM];
GLOBAL int pwm_sum[MOTOR_NUM];
GLOBAL int pwm_buf[MOTOR_NUM];

GLOBAL int toq[MOTOR_NUM];
GLOBAL int toq_pi[MOTOR_NUM];

/*��ɸ��*/
GLOBAL int w_ref[MOTOR_NUM*2];
GLOBAL int w_ref_diff[MOTOR_NUM];
GLOBAL int w_ref_before[MOTOR_NUM];

/*�ե����ɥե���ɤǤΥȥ륯��PWM�Ѵ��ѥ�᡼��*/
GLOBAL int p_ki[MOTOR_NUM];
GLOBAL int p_kv[MOTOR_NUM];
  
/*�໤���*/
GLOBAL int p_fr_plus[MOTOR_NUM];
GLOBAL int p_fr_wplus[MOTOR_NUM];
GLOBAL int p_fr_minus[MOTOR_NUM];
GLOBAL int p_fr_wminus[MOTOR_NUM];

GLOBAL int p_toq_offset[MOTOR_NUM];

/*PWS�Υѥ�᡼��*/
GLOBAL int p_A,p_B,p_C,p_D,p_E,p_F;

/*PI����Υѥ�᡼��*/
GLOBAL int p_pi_kp[MOTOR_NUM];
GLOBAL int p_pi_ki[MOTOR_NUM];

/*PWM�Υ�ߥå�*/
GLOBAL int pwm_max[MOTOR_NUM];
GLOBAL int pwm_min[MOTOR_NUM];

/*�ȥ륯�Υ�ߥå�*/
GLOBAL int toq_max[MOTOR_NUM];
GLOBAL int toq_min[MOTOR_NUM];

/*��ʬ*/
GLOBAL int int_w[MOTOR_NUM];
GLOBAL int int_max[MOTOR_NUM];
GLOBAL int int_min[MOTOR_NUM];

/*�����å��ɥå������ޤΥ�ߥå�*/
GLOBAL int p_watch_dog_limit;

GLOBAL int s_a,s_b;

/*�̿���*/
GLOBAL char  DTC_receive_buffer[DTC_RECEIVE_BUFFER_SIZE];/*�����Хåե�*/
GLOBAL char* DTC_receive_rp;/*�����Хåե����ɤ߹��ߥݥ���*/
GLOBAL char  DTC_receive_data[100];/*�����ǡ����Υǥ����ɷ�̤���¸*/
GLOBAL int   DTC_receive_data_pos;/*�ν񤭹��߰��֡�������*/
GLOBAL int   DTC_receive_state;

#endif
