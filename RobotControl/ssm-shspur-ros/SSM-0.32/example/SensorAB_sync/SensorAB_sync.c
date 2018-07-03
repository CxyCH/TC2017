#include<ssm_common.h>
#include<unistd.h>
#include<stdio.h>
#include<Sensor_A.h>
#include<Sensor_B.h>

int main(int argc, char *argv[]){
SensorA sens_dataA;
SensorB sens_dataB;
double measured_time;
SSM_sid sensA_sid, sensB_sid;
int tid;
double time;

 initSSM();
 
 sensA_sid = openSSM("sensor_A", 0, 0);
 sensB_sid = openSSM("sensor_B", 0, 0);
 
 while(1){
   //$B%;%s%5(BA$B$N:G?7%G!<%?$r<hF@(B
   tid = readSSM(sensA_sid, (char*)&sens_dataA, &measured_time, -1);
   printf("hoge\n");
   //$BF1;~9o$N%G!<%?!J$b$C$H$b6a$/!"8E$$$b$N!K$r<hF@(B
   readSSM_time(sensB_sid, (char*)&sens_dataB, measured_time, &time);
   printf("timeA=%f timeB=%f A.a=%f B.a=%f\n",measured_time,time,sens_dataA.a, sens_dataB.a);   
   sleep(1); 
 }
}

