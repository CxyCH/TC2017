#include<stdio.h>
#include<unistd.h>

#include<ssm_common.h>  /*SSM$B$rMxMQ$9$k:]$KI,MW(B*/
#include<Sensor_A.h>    /*sensor_A$B$N%G!<%?7?(B*/

SensorA sens_data;    /*sensor_A$B$N%G!<%?<hF@MQ(B*/

int main(int argc, char *argv[]){
  SSM_sid sensA_sid;    /*sensor_A$B$N%"%/%;%9MQ(B*/

  double measured_time; /*sensor_A$B$N7WB,;~9o<hF@MQ(B*/
  SensorA_Property sens_prop; /*sensor_A$B$N%W%m%Q%F%#<hF@MQ(B*/

 /*$B=i4|2=(B*/
 initSSM();
 
 /*sensor_A$B$N%*!<%W%s(B*/
 sensA_sid = openSSM("sensor_A", 0, 0);

 /*$B%W%m%Q%F%#$N<hF@(B*/
 get_propertySSM("sensor_A",0,(char*)&sens_prop);
 printf("[%s %f]\n",sens_prop.name,sens_prop.a);
 
 /*1$BIC$*$-$K%G!<%?$r<hF@(B*/
 while(1){
   /*$B:G?7$N%G!<%?$NFI$_9~$_(B*/
   readSSM(sensA_sid, (char*)&sens_data, &measured_time, -1);
   
   printf("time=%f a=%f b=%f c=%d\n",
	  measured_time, sens_data.a, sens_data.b, sens_data.c);
   
   sleep(1);
 }
}

