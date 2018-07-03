/*
  libSSM.c - SSM access APIs 02/12/13  Eijiro Takeuchi
 */

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/time.h>
#include <time.h>
#include "ssm_common.h"

//static int   shm_id=-1;   /*Shared memory ID*/
//static char* shm_ptr;     /*Shared memory top address*/
static int   msq_id=-1;   /*Message queue ID*/
static pid_t my_pid;      /*Own process ID*/
static char  err_msg[20];

static SSM_common_data *ssm_common_data;

char*  get_shm_address(SSM_sid sid);
YTime* get_shm_ytime_address(SSM_sid sid);
char*  get_shm_data_address(SSM_sid sid);

/*---------------------------------------
  Initialize shared memory and message queue
---------------------------------------*/
int initSSM(void)
{
  ssm_msg c_msg;
  //  int len;

  /*Open message queue*/
  if((msq_id = msgget((key_t)MSQ_KEY, 0666)) < 0) {
    return 0;
  }
  // sprintf(err_msg,"msq_id = %d",msq_id);
  my_pid = getpid();


  /*prepare message packet*/
  c_msg.msg_type = MSQ_CMD;
  c_msg.res_type = my_pid;  /*return pid*/
  c_msg.cmd_type = MCinit;
  c_msg.suid  = 0;
  c_msg.ssize = 0;
  c_msg.hsize = 0;
  c_msg.time  = 0;
   

  /*send*/
  if(msgsnd(msq_id, (void*)(&c_msg), (size_t)SSM_MSG_SIZE, 0) < 0) {
    perror("msgsnd");
    sprintf(err_msg,"msq send err %d",msq_id);
     return 0;
  }
  
  /*...processing on ssm side*/

  /*return (msgrcv is waiting mode)*/
  msgrcv(msq_id,&c_msg,SSM_MSG_SIZE,my_pid,0);

  if((ssm_common_data = (SSM_common_data*)shmat(c_msg.suid, 0, 0)) <
     (SSM_common_data*)0) {
    ssm_common_data  = 0;
    return 0;
  }

  return 1;
}


void errSSM(void)
{
    fprintf(stderr,"SSM Err: %s\n",err_msg);
}

/*----------------------API functions---------------------------*/
/*-Delete old message-*/
void del_msg(void)
{
  int len;
  ssm_msg c_msg;
  while((len = msgrcv(msq_id,&c_msg,SSM_MSG_SIZE,my_pid,IPC_NOWAIT)) > 0);
}


/*------------Arrocate sensor data space with time table on SSM  -----*/
SSM_sid createSSM(char *name, int sensor_uid, int ssm_size, YTime life, YTime cycle)
{
  ssm_msg c_msg;
  int len,open_mode=MCread|MCwrite,i;
  char* shm_p;
  
  /*initialize check*/
  if(msq_id<0){
    strcpy(err_msg,"msq id err");
    return 0;
  }
  
  if(strlen(name) >= SSM_SNAME_MAX){
    strcpy(err_msg,"name length");
    return 0;
  }

  /*prepare message packet*/
  c_msg.msg_type = MSQ_CMD;
  c_msg.res_type = my_pid;  /*return pid*/
  c_msg.cmd_type = MCcreate | open_mode;
  strcpy(c_msg.name,name);
  c_msg.suid  = sensor_uid;
  c_msg.ssize = ssm_size;
  c_msg.hsize = (int)((double)1.2* ((double)life / (double)cycle)); /*table size*/
  c_msg.time  = cycle;
   

  /*send*/
  if(msgsnd(msq_id, (void*)(&c_msg), (size_t)SSM_MSG_SIZE, 0) < 0) {
    perror("msgsnd");
    sprintf(err_msg,"msq send err %d",msq_id);
     return 0;
  }
  
  /*...processing on ssm side*/

  /*return (msgrcv is waiting mode)*/
  len = msgrcv(msq_id,&c_msg,SSM_MSG_SIZE,my_pid,0);
  //sprintf(err_msg,"%d %d",c_msg.suid,c_msg.ssize);
  if(len < 0){
    strcpy(err_msg,"receive data err");
    return 0;
  }
  
  /*return value: suid=sensors shm_id, ssize=offset*/
  
  /*Attach to shared memory*/ 
  if((shm_p = shmat(c_msg.suid, 0, 0)) < (char *)0) {
    shm_p  = 0;
    strcpy(err_msg,"shmat err");
    return 0;
  }
  //  sprintf(err_msg,"%d %p",c_msg.suid, shm_p);
  shm_p = shm_p + c_msg.ssize;  /*offset*/
  //sprintf(err_msg,"num  %d,size %d",((ssm_header*)shm_p)->num,((ssm_header*)shm_p)->size);

  //  for(i = 0;i < ((ssm_header*)shm_p)->table_size;i++)
  //    ((int*)((char*)shm_p + ((ssm_header*)shm_p)->table_off))[i] = -10000000;
  for(i = 0;i < ((ssm_header*)shm_p)->table_size;i++)
    ((int*)((char*)shm_p + ((ssm_header*)shm_p)->table_off))[i] = 0;
  ((int*)((char*)shm_p + ((ssm_header*)shm_p)->table_off))[0] = -1000000;

  
  
  return (SSM_sid)shm_p;
}



/*----------------------Open sensor data on SSM-----------------*/
SSM_sid openSSM(char *name,int sensor_uid,char open_mode)
{
  ssm_msg msg;
  int len;
  char* shm_p;
  
  /*check*/
  if(msq_id<0){
    strcpy(err_msg,"msq id err");
    return 0;
  }

  if(strlen(name) > SSM_SNAME_MAX){
    strcpy(err_msg,"name length");
    return 0;
  }

  /*prepare message packet*/
  msg.msg_type = MSQ_CMD;
  msg.res_type = my_pid;  /*return pid*/
  msg.cmd_type = MCopen | (int)open_mode;
  strcpy(msg.name,name);
  msg.suid = sensor_uid;
  msg.ssize = 0;
  msg.hsize = 0;
  msg.time =0;
  
  /*send*/
  if((msgsnd(msq_id, &msg, SSM_MSG_SIZE, 0)) < 0) {
    return 0;
  }
  
  /*receive(waiting)*/
  len = msgrcv(msq_id,&msg,SSM_MSG_SIZE,my_pid,0);
  if(len < 0)return 0;
  /*return value: suid=sensors shm_id ,ssize= offset*/
  
  /*attach to shared memory*/
  if(msg.suid<0)return 0;
  
  if((shm_p = shmat(msg.suid, 0, 0)) < (char *)0) {
    shm_p  = 0;
    return 0;
  }
  
  shm_p+=msg.ssize;  /*offset*/
  return (SSM_sid)shm_p;/*sensor sid*/
}



/*--------------Shared memory controll functions------------*/

/*----------------------get shared memory address -----------------*/
char* get_shm_address(SSM_sid sid)
{
  return (char*)sid;
}

YTime* get_shm_ytime_address(SSM_sid sid)
{
  return (YTime*)((char*)sid + ((ssm_header*)sid)->times_off);
}

char* get_shm_data_address(SSM_sid sid)
{
  return (char*)sid + ((ssm_header*)sid)->data_off;
}

/*----------------------get TID -----------------*/
int getTID(SSM_sid sid,YTime ytime)
{
  ssm_header *shm_p;
  int i,r_tid;
  
  /*get shared memory address of assigned sensor*/
  shm_p = (ssm_header*)get_shm_address(sid);

  /*search tid*/
  r_tid = shm_p->tid_top;
  for(i = 0;i < shm_p->num-SSM_MARGIN;i++){
    if(ytime >
       *((YTime*)((char*)shm_p + shm_p->times_off) + (r_tid%shm_p->num))){
      r_tid++;
      break;
    }
    r_tid--;
    if(r_tid<0){
      r_tid++;
      break;
    }
  }
  if(r_tid > shm_p->tid_top)return 0;/**/
  if(r_tid <= shm_p->tid_top - shm_p->num + SSM_MARGIN )/**/
    return shm_p->tid_top - shm_p->num + SSM_MARGIN + 1;

  return r_tid;
}

/*------------------ read sensor data -----------------*/
int readSSM(SSM_sid sid, char *data, YTime *ytime, int tid )
{
  ssm_header *shm_p;
  int r_tid;
  
  shm_p = (ssm_header*)get_shm_address(sid);
  
  /*if tid is negative then the function returns latest data*/
  if(tid < 0)tid = shm_p->tid_top;
  
  r_tid = tid%(shm_p->num);
  /*region check*/
  /* SSM_TID_SP <= tid_top-num+MARGIN < tid <= tid_top  */
  if(tid > shm_p->tid_top 
     || tid <= shm_p->tid_top - shm_p->num + SSM_MARGIN
     || tid < SSM_TID_SP)
   return -1;
  
  if(data!=0)
    memcpy(data, (char*)shm_p+shm_p->data_off+(shm_p->size)*r_tid, shm_p->size);

  /*get time stamp*/
  if(ytime){
    *ytime = ((YTime*)((char*)shm_p+shm_p->times_off))[r_tid];
  }
  //  printf("%d ",r_tid);
  return tid;  
}


/*--------------------------------------------------------------*/
/*search tid using time tabl*/
int getTID_time(ssm_header* shm_p,YTime ytime)
{
  int table_pos,tid;
  int *tid_p;
  YTime *time;
  long long table_pos_long;
  
  /*time check*/
  if(ytime >
     ((YTime*)((char*)shm_p+shm_p->times_off))[shm_p->tid_top%shm_p->num])
    return -1;
  
  /*time quantize*/
  table_pos_long =((long long)((double)ytime/(double)(shm_p->cycle)))%((long long)shm_p->table_size);
  table_pos = table_pos_long;
  tid_p = (int*)((char*)shm_p + shm_p->table_off);
  tid = tid_p[table_pos];

  /*tid search*/
  time = (YTime*)((char*)shm_p+shm_p->times_off);
  while(time[tid%(shm_p->num)] >ytime)tid--;    
    
  /*tid check*/
  if(tid > shm_p->tid_top - shm_p->num)return tid;

  return 0;
}


/*-----------------------------------------------------*/
/*        registrate tid in timetable                  */
int setTID_time(ssm_header* shm_p, YTime ytime ,int tid)
{
  int table_pos,i;
  int *tid_p;
  long long table_pos_long;

  /*time quantize*/
  table_pos_long =((long long)((double)ytime/(double)(shm_p->cycle)))
    %((long long)shm_p->table_size);
  table_pos = table_pos_long;
  // printf("<t%f %f %d %d>",ytime,shm_p->cycle,table_pos, shm_p->table_size);

  /* fill time table */
  tid_p = (int*)((char*)((char*)shm_p + shm_p->table_off));
  i = 0;
  tid_p[table_pos] = tid;/*register*/    
  
  for(i = 0; i < shm_p->table_size;i++ ){  
    table_pos--; 
    if(table_pos < 0)table_pos = shm_p->table_size-1;

    //found before tid
    //    if(tid_p[table_pos] > tid - 10 || tid_p[table_pos] <= 0  )return i;
    if(tid_p[table_pos] > tid - 2)return i+100;
    //if(tid_p[table_pos] <= 0  ){tid_p[table_pos] = 0;return i;}
           
    tid_p[table_pos] = tid-1;//regist  
  }

  return tid;
}




//--------------------���ֻ���ǥǡ������ɤ߹���--------------------------
//
// SID�ǻ��ꤷ�����󥵤Υǡ�����������Ǹ������ɤ߹���
//
//����ͭ����ΤߤΥ��������ʤΤǡ��ץ��������å���ɬ�פȤ��ʤ���
//������TID������㴳���֤������롣
// 
//int readSSM_time(
//               SSM_sid sid, ���󥵤�ID                                 
//		 char *data,  �ǡ������֤��ϥ�
//		 YTime ytime, �������
//		 YTime ret_time �����ǡ����λ���
//		 )
//
//-------------------------------------------------------------------*/

int readSSM_time(SSM_sid sid, /*���󥵤�ID*/
		 char *data,  /*�ǡ������֤��ϥ�*/
		 YTime ytime, /*�������*/
		 YTime *ret_time /*�����ǡ����λ���*/
		 )
{

  ssm_header *shm_p;
  int tid,r_tid;
  
  /*initialize*/
  shm_p = (ssm_header*)get_shm_address(sid);
  tid = 0;

   /*�ơ��֥뤫��TID�򸡺�*/
  if(ytime <= 0){/*tid����λ��Ϻǿ��ǡ������ɤ߹��ߤȤ���*/
    tid = shm_p->tid_top;
  }else{
    tid = getTID_time(shm_p, ytime); 
    //      sprintf(err_msg,"tid %d",tid); 
    if(tid < 0)return -1;
    if(tid == 0)return -5;

  }
  
  r_tid = tid%(shm_p->num);
  
  /* �׵ᤵ�줿�ǡ������ǡ����������ϰ��⤫��Ϥ���Ƥ���м��� */
  /* SSM_TID_SP <= tid_top-num+MARGIN < tid <= tid_top  */
  if(tid > shm_p->tid_top)return -2;
  if(tid <=shm_p->tid_top-shm_p->num+SSM_MARGIN)return -3;
  if(tid<SSM_TID_SP)return -4;
  
  /*�ǡ����򥳥ԡ�*/
  if(data!=0)
    memcpy(data , (char*)shm_p + shm_p->data_off +(shm_p->size)*r_tid , shm_p->size);
  
  /*��������*/
  if(ytime>0){
    ytime = ((YTime*)((char*)shm_p+shm_p->times_off))[r_tid];    
  }
  *ret_time = ytime;
  return tid;  
}


/*public�ؿ�*/
/*----------------------�ǡ�����񤭹���-----------------*/
int writeSSM(SSM_sid sid,char *data,YTime ytime)
{
  ssm_header *shm_p;
  int w_tid,tid,i;

  shm_p = (ssm_header*)get_shm_address(sid);
  i = 0;
  tid = (shm_p->tid_top)+1;
  w_tid = tid%(shm_p->num);

  i = setTID_time(shm_p, ytime, tid);
  memcpy((char*)shm_p+shm_p->data_off+(shm_p->size)*w_tid, data, shm_p->size);

  /*�������Ͽ������y_time��0�ξ��ϸ��ߤλ����*/
  ((YTime*)((char*)((char*)shm_p+shm_p->times_off)))[w_tid] = ytime;

  //else getYTime(shm_p->times+w_tid);
  //printf("");

  (shm_p->tid_top)++;
  return i;
}


/*----------------------�ǡ�����񤭹���-----------------*/
int writeSSM_time(SSM_sid sid,char *data,YTime ytime)
{
  return writeSSM(sid, data, ytime);
}


/*�⡼�ɤ˱����ƥǡ��������*/
int damp(SSM_sid sid,int mode,int num)
{
  int i,*p;
  ssm_header *shm_p;
  
  /*��ͭ����Υ��ɥ쥹���ä�*/
  shm_p = (ssm_header*)get_shm_address(sid);


  switch(mode){
  case 0:

    break;
  case 1:
   
    break;
  case 2:/*time table damp*/
    p = (int*)((char*)shm_p + shm_p->table_off);
    for(i = 0;i < num;i++){
      printf("table[%d] = %d \n ",i, p[i] );  
    }
     break;
  default:
    break; 
  }
  return num;
}


/*������Ͽ����Ƥ���SSM��������*/
int getSSM_num(void){
  ssm_msg c_msg;
  int len;
  
  /*�����Ǥ��Ƥ��ʤ���м���*/
  if(msq_id<0){
    strcpy(err_msg,"msq id err");
    return 0;
  }
  
  /*��å������򥻥å�*/
  c_msg.msg_type = MSQ_CMD;
  c_msg.res_type = my_pid;  /*��ʬ�ؤ��ֿ���ǧ�����뤿���ID*/
  c_msg.cmd_type = MCsensor_list_num;
  /*����*/
  if(msgsnd(msq_id, (void*)(&c_msg), (size_t)SSM_MSG_SIZE, 0) < 0) {
    perror("msgsnd");
    sprintf(err_msg,"msq send err %d",msq_id);
     return 0;
  }
  
  /*��ssm¦�ǽ�����*/

  /*�������Ԥ���*/
  len = msgrcv(msq_id,&c_msg,SSM_MSG_SIZE,my_pid,0);
  sprintf(err_msg,"%d",c_msg.suid);
  if(len < 0){
    strcpy(err_msg,"receive data err");
    return 0;
  }
  
  return c_msg.suid;/*�����֤�*/
}

/*n�֤�Υ��󥵤�̾��������ɤ߹���*/
int getSSM_name(int n, char *name, int *suid, int *size)
{
  ssm_msg c_msg;
  int len;
  
  /*�����Ǥ��Ƥ��ʤ���м���*/
  if(msq_id<0){
    strcpy(err_msg,"msq id err");
    return 0;
  }
  
  /*��å������򥻥å�*/
  c_msg.msg_type = MSQ_CMD;
  c_msg.res_type = my_pid;  /*��ʬ�ؤ��ֿ���ǧ�����뤿���ID*/
  c_msg.cmd_type = MCsensor_list_name;
  c_msg.suid = n;
  /*����*/
  if(msgsnd(msq_id, (void*)(&c_msg), (size_t)SSM_MSG_SIZE, 0) < 0) {
    perror("msgsnd");
    sprintf(err_msg, "msq send err %d", msq_id);
    return 0;
  }
  
  /*��ssm¦�ǽ�����*/

  /*�������Ԥ���*/
  len = msgrcv(msq_id, &c_msg, SSM_MSG_SIZE, my_pid, 0);
  sprintf(err_msg, "%d", c_msg.suid);
  if(len < 0){
    strcpy(err_msg, "receive data err");
    return -1;
  }
  if(suid)*suid = c_msg.suid;/*ID*/ 
  if(size)*size = c_msg.ssize;/*�ǡ���������*/
  if(name)strcpy(name, c_msg.name);/*̾��*/

  return c_msg.ssize;/*���������֤�(��ʤ�¸�ߤ��ʤ�)*/
}


/*���ꤷ��̾���Υ��󥵤ξ�����ɤ߹���*/
int getSSM_info(char *name, int suid, 
		int *size, int *num, 
		double *cycle,int *property_size)
{
  ssm_msg c_msg;
  int len;
  
    /*�����Ǥ��Ƥ��ʤ���м���*/
  if(msq_id<0){
    strcpy(err_msg,"msq id err");
    return 0;
  }
  
  /*��å������򥻥å�*/
  c_msg.msg_type = MSQ_CMD;
  c_msg.res_type = my_pid;  /*��ʬ�ؤ��ֿ���ǧ�����뤿���ID*/
  c_msg.cmd_type = MCsensor_list_info;
  c_msg.suid = suid;
  strcpy(c_msg.name, name);
  /*����*/
  if(msgsnd(msq_id, (void*)(&c_msg), (size_t)SSM_MSG_SIZE, 0) < 0) {
    perror("msgsnd");
    sprintf(err_msg, "msq send err %d", msq_id);
    return 0;
  }
  
  /*��ssm¦�ǽ�����*/

  /*�������Ԥ���*/
  len = msgrcv(msq_id, &c_msg, SSM_MSG_SIZE, my_pid, 0);
  sprintf(err_msg, "%d", c_msg.suid);
  if(len < 0){
    strcpy(err_msg, "receive data err");
    return -1;
  }
  if(size)*size = c_msg.ssize;/*�ǡ���������*/
  if(num)*num  = c_msg.hsize;/*�����*/
  if(cycle)*cycle = c_msg.time;/*����*/
  if(property_size)*property_size = c_msg.suid;/*����*/

  return c_msg.ssize;/*���������֤�(��ʤ�¸�ߤ��ʤ�)*/
}

/*���߻��������*/
double gettimeSSM(void){
  struct timeval current;
  double time, ssm_time;

  gettimeofday(&current, NULL); 
  time = current.tv_sec + current.tv_usec/1000000.0;

  ssm_time = (time-ssm_common_data->ssmtime_settime)*
    ssm_common_data->ssmtime_speed+ ssm_common_data->ssmtime_offset;
  return ssm_time;
}

/*����,®�٤���Ͽ*/
double settimeSSM(double offset_time, double speed){
  struct timeval current;
  double time, ssm_time;

  gettimeofday(&current, NULL); 
  time = current.tv_sec + current.tv_usec/1000000.0;
  
  //��Ͽ����򥻥å�
  ssm_common_data->ssmtime_settime= time;

  //���ե��åȤ򥻥å�(offset_time��0�ʲ��ξ�總�߻���)
  if(offset_time >0)
    ssm_common_data->ssmtime_offset = offset_time;
  else 
    ssm_common_data->ssmtime_offset = time;

  //���ԡ��ɤ򥻥å�(0�ʲ��ξ��ϣ�)
  if(speed>0)
    ssm_common_data->ssmtime_speed = speed;
  else
    ssm_common_data->ssmtime_speed = 1;

  ssm_time = (time-ssm_common_data->ssmtime_settime)*
    ssm_common_data->ssmtime_speed+ ssm_common_data->ssmtime_offset;

  return ssm_time;
}


/*----------------------���󥵤Υץ�ѥƥ��Υ��å�-----------------*/
int  set_propertySSM(char *name,int sensor_uid,char* data, int size)
{
  ssm_msg msg;
  int len;
  char *ptr;

  /*�����Ǥ��Ƥ��ʤ���м���*/
  if(msq_id<0){
    strcpy(err_msg,"msq id err");
    return 0;
  }

  if(strlen(name) > SSM_SNAME_MAX){
    strcpy(err_msg,"name length");
    return 0;
  }

  /*��å������򥻥å�*/
  msg.msg_type = MSQ_CMD;
  msg.res_type = my_pid;  /*��ʬ�ؤ��ֿ���ǧ�����뤿���ID*/
  msg.cmd_type = MCsensor_property_set;
  strcpy(msg.name, name);
  msg.suid  = sensor_uid;
  msg.ssize = size;
  msg.hsize = 0;
  msg.time  = 0;
  //  printf("send\n");
  /*����*/
  if((msgsnd(msq_id, &msg, SSM_MSG_SIZE, 0)) < 0) {
    return 0;
  }
  //printf("receive\n");
  /*�������Ԥ���*/
  len = msgrcv(msq_id,&msg,SSM_MSG_SIZE,my_pid,0);
  if(len < 0)return 0;
  if(!msg.ssize)return 0;/*���顼*/

  ptr = malloc(size+sizeof(long));
  memcpy(ptr+sizeof(long), data, size);
  *((long*)ptr) = msg.res_type;
  /*����*/
  //  printf("send2\n");
  msgsnd(msq_id, ptr, size, 0);
  free(ptr); 
  //printf("fin\n");
  return 1;/*���ɥ쥹�������㥹��*/
}

/*----------------------���󥵤Υץ�ѥƥ��μ���-----------------*/
int get_propertySSM(char *name,int sensor_uid,char* data)
{
  ssm_msg msg;
  int len,size;
  char *ptr;

  /*�����Ǥ��Ƥ��ʤ���м���*/
  if(msq_id<0){
    strcpy(err_msg,"msq id err");
    return 0;
  }

  if(strlen(name) > SSM_SNAME_MAX){
    strcpy(err_msg,"name length");
    return 0;
  }

  /*��å������򥻥å�*/
  msg.msg_type = MSQ_CMD;
  msg.res_type = my_pid;  /*��ʬ�ؤ��ֿ���ǧ�����뤿���ID*/
  msg.cmd_type = MCsensor_property_get;
  strcpy(msg.name,name);
  msg.suid = sensor_uid;
  msg.ssize = 0;
  msg.hsize = 0;
  msg.time =0;
  
  /*����*/
  if((msgsnd(msq_id, &msg, SSM_MSG_SIZE, 0)) < 0) {
    return 0;
  }
  
  /*�������Ԥ���*/
  len = msgrcv(msq_id,&msg,SSM_MSG_SIZE,my_pid,0);
  if(len < 0)return 0;
  if(!msg.ssize)return 0;/*���顼*/
  size = msg.ssize;

  ptr = malloc(size+sizeof(long));

  /*����*/
  msgrcv(msq_id, ptr, size, my_pid, 0);

  memcpy(data, ptr+sizeof(long), size);  
  free(ptr); 
  return 1;/*���ɥ쥹�������㥹��*/
}
