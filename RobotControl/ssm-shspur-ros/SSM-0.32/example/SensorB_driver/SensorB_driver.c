#include<ssm_common.h>
#include<Sensor_B.h>
#include<unistd.h>

void receive_sensorB(SensorB *data, double *time){
  static double a;
  static int b;
  a+=0.1;
  b++;
  data->a = a;
  data->b = b;
  *time = gettimeSSM();
  usleep(50000);
}

int main(int argc, char *argv[]){
SensorB sens_data;
double measured_time;
SSM_sid sensB_sid;

  initSSM();
  //̾����sensor_B�ǡ�ID��0�ǡ�0.05�ä��������ǡ�����5�ô��ݻ�����褦�ΰ����ݡ� 
  sensB_sid = createSSM_time("sensor_B", 0, sizeof(SensorB), 5, 0.05);

  while(1){
   //SensorB�Υǡ����򲿤��������ˡ���㤨�Х��ꥢ���̿��ˤǼ���������ĥ�äƴ�¬�������ꤹ��ؿ���
   receive_sensorB(&sens_data, &measured_time);
   //SSM�ؤΥǡ����ν񤭹��ߡ�
   writeSSM(sensB_sid, (char*)&sens_data, measured_time);
  }
}  
