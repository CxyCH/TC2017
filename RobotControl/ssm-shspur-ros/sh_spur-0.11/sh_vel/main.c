/* --------------------------------------- */
/*          main�ؿ��ץ������               */
/*     ������ȥ����ܥ롼��,���ޥ�ɼ���       */
/*                  ISS�Τ�١����˺�������  */
/* --------------------------------------- */

#include <7040s.h>         // ���ɥ쥹���
#include <motor_device.h>
#include <serial.h>
#include <SH_vel.h>

#define GLOBAL_DEFINE //�������Х��ѿ�����
#include <globals.h>
#include <sh_vel_parameters.h>
void set_imask(unsigned int imask);

void set_imask(unsigned int imask) {
  int srreg;
  
  imask <<= 4;
  imask &= 0x00f0;
  asm("stc      sr, %0  ": "=r"(srreg):);
  srreg &= 0xffffff0f;
  srreg |= imask;
  asm("ldc      %0,sr   ": :"r"(srreg));
}

//���Τν����
void init(void){
  // ���ꥢ���̿��������󥿡�PWM���Ϥν����
  initSCI1();

  // ����ڥ��ޥå������ޤν����
  set_imask(14);              //��٥�14,15�ϳ����ߵ���
  initCMT();  
  
  initCounter();   //���󥳡��������󥿤ν����
  initPWM();       //PWM�ν����
  
  // DTC�ν���� ringbuff�˥ǡ�������������
  initDTC();
}


//�����ܥ롼�פγ���������
void initCMT(void){
  CMT.CMSTR.BIT.STR1=0; /* ����ڥ��ޥå�������1���ȥå�*/
  CMT1.CMCOR= 28636/8-1;
  CMT1.CMCSR.BIT.CKS=0; /* �դ����� */
  CMT1.CMCSR.BIT.CMIE=1; /* �����߲����� */
  INTC.IPRG.BIT._CMT1=15; /* �����ߥ���ȥ�����γ����ߥ�٥� */
}



///////////////////// �����ߴؿ� /////////////////////
////////////////������®������롼�� /////////////////////
#pragma interrupt
int_cmi1(){
  int i;

  /*���󥳡���������*/
  cnt_read();

  if(servo_level <=SERVO_LEVEL_STOP){//servo_level 0 (shut down)
    put_pwm(0, 0);
    put_pwm(1, 0);
    CMT1.CMCSR.BIT.CMF = 0;
    
  }else{//servo_level 1 (counter enable)
    
    if(param_change == 3){
      w_ref[0] = w_ref[2];
      w_ref[1] = w_ref[3];
      param_change = 0;
    }
    
    w_ref_diff[0] = w_ref[0]-w_ref_before[0];
    w_ref_diff[1] = w_ref[1]-w_ref_before[1];
    w_ref_before[0] = w_ref[0];
    w_ref_before[1] = w_ref[1];

    if(servo_level >= SERVO_LEVEL_TORQUE){  //servo_level 2(toque enable)
      if(servo_level >= SERVO_LEVEL_VELOCITY){ //servo_level 3 (speed enable)
	for(i = 0;i < MOTOR_NUM; i++){
	  /*��ʬ*/
	  int_w[i] += (w_ref[i]-cnt_dif[i]);
	  if(int_w[i] > int_max[i])int_w[i] = int_max[i];
	  if(int_w[i] < int_min[i])int_w[i] = int_min[i];
	  
	  /*PI����ʬ*/
	  toq_pi[i] = (w_ref[i] - cnt_dif[i])*p_pi_kp[i] + int_w[i]*p_pi_ki[i];
	}
	
	/*PWS�Ǥ���ߤαƶ����θ�����ե����ɥե����*/
	s_a = (toq_pi[0] + w_ref_diff[0]);
	s_b = (toq_pi[1] + w_ref_diff[1]);
	
	toq[0] = (s_a*p_A + s_b*p_C + w_ref[0]*p_E)>>8;
	toq[1] = (s_b*p_B + s_a*p_D + w_ref[1]*p_F)>>8;
      }else{//servo_level 2(toque enable)
	toq[0] = 0;
	toq[1] = 0;
      }
      /*������*/
      for(i = 0;i < MOTOR_NUM; i++){
	/*�ȥ륯�ǥ���åԥ�*/
	if(toq[i] >= toq_max[i]){toq[i] = toq_max[i];}
	if(toq[i] <= toq_min[i]){toq[i] = toq_min[i];}
	
	
	/*�໤�����������*/
	if(cnt_dif[i] > 0){
	  toq[i] += ( p_fr_wplus[i]*cnt_dif[i]+ p_fr_plus[i]);
	}else if(cnt_dif[i] < 0){
	  toq[i] -= ( p_fr_wminus[i]*(-cnt_dif[i])+ p_fr_minus[i]);
	}else{
	  toq[i] = toq[i];
	}
	
	/*�ȥ륯���*/
	toq[i] += p_toq_offset[i];
	
	/*�ȥ륯��pwm�Ѵ�*/
	out_pwm[i] = (toq[i]*p_ki[i] + cnt_dif[i]*p_kv[i])/65536;
	
	/*PWM�ǥ���åԥ�*/
	if(out_pwm[i] > pwm_max[i])out_pwm[i] = pwm_max[i];
	if(out_pwm[i] < pwm_min[i])out_pwm[i] = pwm_min[i];
      }
      
      /*����*/
      put_pwm(0, out_pwm[0]);
      put_pwm(1, out_pwm[1]);
      
      pwm_sum[0]+=out_pwm[0];
      pwm_sum[1]+=out_pwm[1];
    }//servo_level 2
    cnt_updated ++;
    watch_dog++;
    if(cnt_updated == 5){
      counter_buf[0] = counter[0];
      counter_buf[1] = counter[1];
      pwm_buf[0] = pwm_sum[0];
      pwm_buf[1] = pwm_sum[1];
      pwm_sum[0] = 0;
      pwm_sum[1] = 0;
    }
  }//servo_level 1
  CMT1.CMCSR.BIT.CMF = 0;   // ����ڥ��ޥå��ե�å��Υ��ꥢ
}



////////////////////////////////////////////////////
/*�����������ޥ�ɤβ���*/
int command_analyze(char *data, int len){
  int motor;
  Int_4Char i;
  i.byte[0] = data[2];
  i.byte[1] = data[3];
  i.byte[2] = data[4];
  i.byte[3] = data[5];

  motor = data[1];
  if(motor < 0 || motor >= 2)return 0;


  //if(data[0] != PARAM_w_ref)
  //  SCI1_printf("get %d %d %d\n",data[0],data[1],i.integer);    
  switch(data[0]){
  case PARAM_w_ref:
    w_ref[motor+2] = i.integer;
    param_change |= (1<<motor);
    watch_dog = 0;
    break;
  case PARAM_p_ki:
    p_ki[motor] = i.integer; 
    break;
  case PARAM_p_kv:
    p_kv[motor] = i.integer; 
    break;
  case PARAM_p_fr_plus:
    p_fr_plus[motor] = i.integer; 
    break;
  case PARAM_p_fr_wplus:
    p_fr_wplus[motor] = i.integer; 
    break;
  case PARAM_p_fr_minus:
    p_fr_minus[motor] = i.integer; 
    break;
  case PARAM_p_fr_wminus:
    p_fr_wminus[motor] = i.integer; 
    break;
  case PARAM_p_A:
    p_A = i.integer; 
    break;
  case PARAM_p_B:
    p_B = i.integer; 
    break;
  case PARAM_p_C:
    p_C = i.integer; 
    break;
  case PARAM_p_D:
    p_D = i.integer; 
    break;
  case PARAM_p_E:
    p_E = i.integer; 
    break;
  case PARAM_p_F:
    p_F = i.integer; 
    break;
  case PARAM_p_pi_kp:
    p_pi_kp[motor] = i.integer; 
    break;
  case PARAM_p_pi_ki:
    p_pi_ki[motor] = i.integer; 
    break;
  case PARAM_pwm_max:
    pwm_max[motor] = i.integer; 
    if(motor == 0)MTU3.TGRA = i.integer;
    if(motor == 1)MTU3.TGRC = i.integer;
    break;
  case PARAM_pwm_min:
    pwm_min[motor] = i.integer; 
    break;
  case PARAM_toq_max:
    toq_max[motor] = i.integer; 
    break;
  case PARAM_toq_min:
    toq_min[motor] = i.integer; 
    break;
  case PARAM_p_toq_offset:
    p_toq_offset[motor] = i.integer; 
    break;
  case PARAM_int_max:
    int_max[motor] = i.integer; 
    break;
  case PARAM_int_min:
    int_min[motor] = i.integer; 
    break;
  case PARAM_servo:

    if(servo_level < SERVO_LEVEL_VELOCITY &&
       i.integer  >= SERVO_LEVEL_VELOCITY){//servo level��®������˿�ܤ���
      int_w[0] = 0;
      int_w[1] = 0;
    }  
    servo_level = i.integer;
    
    //    SCI1_printf("start\n");
    break;
  case PARAM_watch_dog_limit:
    p_watch_dog_limit = i.integer;
    break;
  default:
    break;
  }
  return 0;
}


///////////////////////////////////////////////////////////
main(){
  int i;
  int len;
  short counter_buf2[2];

  param_change = 0;
  servo_level = SERVO_LEVEL_STOP;
  watch_dog = 0;
  p_watch_dog_limit = 100;
  
  /*�����*/
  init();

  counter_buf2[0] = 0;
  counter_buf2[1] = 0;

  /*��������*/
  startDTC();           // start DTC

  /*�����ܥ롼�׻�ư*/  
  CMT.CMSTR.BIT.STR1=1; //CMT1 start
    
  while(1){
    /*���ޥ�ɼ���*/
    if((len = DTC_receive())>0){
      command_analyze(DTC_receive_data, len);
    }
    
    /*���ɥ�ȥ����*/
    if(cnt_updated >= 5 ){/*��5ms����*/
      cnt_updated = 0;

      odometry_send((short)((short)counter_buf[0]-(short)counter_buf2[0]),
		    (short)((short)counter_buf[1]-(short)counter_buf2[1]),
		    pwm_buf[0], pwm_buf[1]);

      counter_buf2[0] = counter_buf[0];
      counter_buf2[1] = counter_buf[1];

    }
    if(watch_dog > p_watch_dog_limit){
      watch_dog = 0;
      param_change = 0;
      servo_level = SERVO_LEVEL_STOP;
    }
  }
}