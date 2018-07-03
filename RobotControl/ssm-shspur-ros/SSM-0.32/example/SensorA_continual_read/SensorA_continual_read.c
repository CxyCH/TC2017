#include<ssm_common.h>
#include<Sensor_A.h>
#include<unistd.h>
#include<stdio.h>

int main(int argc, char *argv[]){
SensorA sens_data;
double measured_time;
SSM_sid sensA_sid;
 int tid;
 
 initSSM();
 sensA_sid = openSSM("sensor_A", 0, 0);
 //$B:G?7$N%G!<%?$N(BTID$B$r<hF@$9$k!#(B
 tid = readSSM(sensA_sid, (char*)&sens_data, &measured_time, -1);
 while(1){
   while(readSSM(sensA_sid, (char*)&sens_data, &measured_time, tid) <= 0){
     usleep(20000);  //$B%G!<%?$,$^$@$-$F$$$J$1$l$P$A$g$C$HBT$C$F$_$k!#(B
   }
   printf("time=%f tid=%d a=%f b=%f c=%d\n",measured_time, tid, sens_data.a, sens_data.b, sens_data.c);
   tid++;
 }
}

