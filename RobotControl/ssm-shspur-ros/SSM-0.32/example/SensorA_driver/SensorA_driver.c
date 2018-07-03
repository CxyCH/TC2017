#include<ssm_common.h>
#include<Sensor_A.h>
#include<unistd.h>
#include<stdio.h>

SensorA sens_data;

void receive_sensorA(SensorA *data, double *time){
  static double a,b;
  static int c;
  a+=0.1;
  b+=0.01;
  c++;
  data->a = a;
  data->b = b;
  data->c = c;
  *time = gettimeSSM();
  usleep(100000);
}

int main(int argc, char *argv[]){

double measured_time;
SSM_sid sensA_sid;
SensorA_Property sens_prop;
 
  initSSM();
  //̾����sensor_A�ǡ�ID��0�ǡ�0.1�ä��������ǡ�����10�ô��ݻ�����褦�ΰ����ݡ� 
  sensA_sid = createSSM("sensor_A", 0, sizeof(SensorA), 10, 0.1);

  /*property������*/
  sens_prop.a = 100;
  sprintf(sens_prop.name, "sensorA");
  set_propertySSM("sensor_A",0,&sens_prop,sizeof(SensorA_Property));

  while(1){
    usleep(100);
    
    //SensorA�Υǡ����򲿤��������ˡ���㤨�Х��ꥢ���̿��ˤǼ���������ĥ�äƴ�¬�������ꤹ��ؿ���
    receive_sensorA(&sens_data, &measured_time);
    //SSM�ؤΥǡ����ν񤭹��ߡ�
    printf("%d\n", writeSSM(sensA_sid, (char*)&sens_data, measured_time));
  }
}  
