#include<ssm_common.h>
#include<Sensor_A.h>
#include<unistd.h>
#include<stdio.h>

int main(int argc, char *argv[]){
SensorA sens_data;
double measured_time;
SSM_sid sensA_sid;
int tid;
double now_time;

 initSSM();
 sensA_sid = openSSM("sensor_A", 0, 0);

 while(1){
   now_time = gettimeSSM();  //$B8=:_;~9o$r<hF@(B

   //$B8=:_;~9o$N(B1$BICA0$N%G!<%?$r<hF@(B
   tid = readSSM_time(sensA_sid, (char*)&sens_data, now_time-1, &measured_time);
   printf("nowtime=%f\n   time=%f tid=%d a=%f b=%f c=%d\n", gettimeSSM()-1,measured_time, tid, sens_data.a, sens_data.b, sens_data.c);
   //$B$=$N0l$D8e$N%G!<%?$r<hF@(B
   //tid++;
   tid = readSSM(sensA_sid, (char*)&sens_data, &measured_time, tid+1);
   printf("   time=%f tid=%d a=%f b=%f c=%d\n\n",measured_time, tid, sens_data.a, sens_data.b, sens_data.c);
   
   sleep(1); 
 }
}

