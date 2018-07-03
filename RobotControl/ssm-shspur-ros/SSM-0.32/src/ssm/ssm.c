/*
 * ssm_manager.c - ��ͭ���ꡦ�����ץ����
 *
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
#include <signal.h>
#include <sys/msg.h>
#include "ssm_common.h"
#include "ssm.h"


#define VERSION "0.32"

int       msq_id=-1;   /*��å��������塼��ID*/
key_t     shm_key_num = 0;/*��ͭ����ο�*/
SSM_List* ssm_top = 0;
pid_t     my_pid;      /*��ʬ�Υץ���ID*/
int       verbosity_mode = 0;/*��å�����ɽ��*/

int shmid_ssm_common_data;
SSM_common_data *shm_ssm_common_data;

void escape_road(void);
struct sigaction sa_sigint;
static void emergency(int);
static void emergency(int sig){
  fprintf(stderr,"program stop [%d]\n",sig);  
  fprintf(stderr,"finalize...\n");  
  
  /*��λ����*/
  free_ssm_list(ssm_top);
  ssm_top = 0;
  fprintf(stderr,"- all allocated shared memory released\n");  
  shmdt((char*)shm_ssm_common_data);/*�ǥ��å�*/
  shmctl(shmid_ssm_common_data, IPC_RMID, 0);/*���*/
  while(1){
    fprintf(stderr,"please hit ctrl+c\n");
    sleep(1);
  };
}

void escape_road(void){
 sa_sigint.sa_handler = emergency;
 sa_sigint.sa_flags = SA_RESETHAND|SA_NODEFER;
 sa_sigint.sa_restorer = 0;

 if (sigaction(SIGINT, &sa_sigint, NULL) < 0) {
   perror("sigaction");
   exit(1);
 }
}


void print_list(SSM_List* p)
{
  while(p){
    printf("name : %s\n",p->name);
    printf("ID: %d  offset: %d  size: %d  address: %ld \n   |\n",p->shm_id,p->shm_offset,p->size,(long)p->shm_ptr);
    p=p->next;
  }
}

/*�������*/
long get_receive_id(void){
  static long id;
  if(id < MSQ_RES)id = MSQ_RES;
  
  id++;
  if(id > MSQ_RES_MAX)id = MSQ_RES;
  return id;
}

/*SSM�ζ��̥ǡ���*/
int alloc_ssm_common_data(void){
 /*��ͭ�����ΰ�򤲤�*/
  if((shmid_ssm_common_data =
      shmget(SHM_KEY, sizeof(SSM_common_data), IPC_CREAT | 0666)) < 0)
    return 0;
  shm_key_num++;

  /*�����ä�*/
  if((shm_ssm_common_data = 
      (SSM_common_data*)shmat(shmid_ssm_common_data, 0, 0)) < 
     (SSM_common_data *)0) {
    return 0;
  }

  shm_ssm_common_data->ssmtime_offset = 0;
  shm_ssm_common_data->ssmtime_settime = 0;
  shm_ssm_common_data->ssmtime_speed = 1;

  return 1;
}

/*�ǡ��������� ssize,�����hsize�ζ�ͭ������ΰ������*/
/*���ߤ�ñ���׵���˶�ͭ�����ΰ����ݤ���Τ�        */
/*����Ū�ˤϥ�������Ȼ����褦�ʵ���������롩        */
int alloc_ssm_block(int ssize, int hsize,YTime cycle, char **shm_h, int *offset)
{
  int s_id,i; /*  */

  /*��ͭ�����ΰ�򤲤�*/
  if((s_id = shmget(SHM_KEY+shm_key_num,
		    sizeof(ssm_header)+(ssize+sizeof(YTime)+sizeof(int))*hsize,
		    IPC_CREAT | 0666)) < 0)return 0;
  shm_key_num++;

  /*�����ä�*/
  if((*shm_h = shmat(s_id, 0, 0)) < (char *)0) {
    return 0;
  }

  /*���ǽ������*/
  for(i = 0; i < (int)(sizeof(ssm_header)+(ssize+sizeof(YTime)+sizeof(int))*hsize); i++)
    (*shm_h)[i] = 0;

  /*�����ǤϿ����������ΰ�ʤΤǡ�offset�ϣ�*/
  *offset = 0;
  if(cycle <= 0)cycle = 1;

  /*ssm_header �����*/
  ((ssm_header*)*shm_h)->tid_top = SSM_TID_SP-1;  /*�������*/ 
  ((ssm_header*)*shm_h)->size = ssize;            /*�ǡ���������*/
  ((ssm_header*)*shm_h)->num = hsize;             /*�����*/
  ((ssm_header*)*shm_h)->table_size = hsize;      /*�ơ��֥륵����*/
  ((ssm_header*)*shm_h)->cycle = cycle;           /*�ǡ����Ǿ���������*/
  ((ssm_header*)*shm_h)->data_off = sizeof(ssm_header);  /*�ǡ�������Ƭ���ɥ쥹*/
  ((ssm_header*)*shm_h)->times_off = sizeof(ssm_header) + ssize*hsize;/*�������Ƭ���ɥ쥹*/
  ((ssm_header*)*shm_h)->table_off =sizeof(ssm_header) + (ssize+sizeof(YTime))*hsize;/*time table����Ƭ���ɥ쥹*/

  return s_id;
}


/*�����ѥꥹ�Ȥ�Ǹ����˺��*/
SSM_List* add_ssm_list(char *name,
		   int suid,
		   int ssize,
		   int hsize,
		   YTime cycle)
{
  SSM_List *p,*q;
 
  p = (SSM_List*)malloc(sizeof(SSM_List));
  if(!p)return 0;
  strcpy(p->name,name);
  p->suid = suid;
  p->shm_id = alloc_ssm_block(ssize,hsize,cycle,&(p->shm_ptr),&(p->shm_offset));
  p->size =sizeof(ssm_header)+(ssize+sizeof(YTime)+sizeof(int))*hsize;
  p->header = (ssm_header*)(p->shm_ptr);
  p->next = 0;
  p->property = 0;
  p->property_size = 0;
  /*�ꥹ�ȤκǸ��p���ɲ�*/
  if(!ssm_top){
    ssm_top = p;
  }else{
    q = ssm_top;
    while(q->next){
      q = q->next;
    }
    q->next = p;
  }
  return p;
}

void free_ssm_list(SSM_List* ssmp){
  if(ssmp){
    if(ssmp->next)free_ssm_list(ssmp->next);
    if(ssmp->shm_ptr){
      shmdt(ssmp->shm_ptr);/*�ǥ��å�*/
      shmctl(ssmp->shm_id, IPC_RMID, 0);/*���*/
      printf("%s detached\n",ssmp->name);
    }/*
      ssmp->next = 0;
      
      if(ssmp->header)free(ssmp->header);
      if(ssmp->property)free(ssmp->property);
      free(ssmp);
    */
  }
}

void clean_shm(void){
  int i; 
  int s_id;

  printf("delete_all_sensor\n");
  
  for(i=0;i<40;i++){  
    if((s_id = shmget(SHM_KEY+i, 1, IPC_CREAT | 0666)) > 0){
      //shmdt(ssmp->shm_ptr);/*�ǥ��å�*/
      shmctl(s_id, IPC_RMID, 0);/*���*/
    }
  }
}


/*SSM�ν����*/
int ssm_init(void)
{
  /*��å��������塼�Υ����ץ�*/
  if((msq_id = msgget(MSQ_KEY, IPC_CREAT | 0666)) < 0) {
    return -1;
  }
  
  my_pid = getpid();
  ssm_top = 0;
  
  //���̶�ͭ����ξ������
  alloc_ssm_common_data();  

  printf("Message queue ready.\n");
  printf("Msg queue ID = %d \n",msq_id);
  //  printf("PID = %d\n",my_pid);
  return 1;
}


/*̾��������ͭID���饻�󥵤򸡺�����*/
/*̾��ͥ��ǡ�Ʊ��̾����ʪ������и�ͭID����Ƚ��*/

SSM_List* search_SSM_List(char* name,int suid)
{
  SSM_List *p,*pn,*pni,*pi;

  p = ssm_top;

  pn = 0;pni = 0;pi = 0;
  while(p){
    if(strcmp(p->name,name) == 0){
      /*̾����Ʊ��*/
      pn = p;
      if(p->suid == suid){
	/*̾����Ʊ��*/
	pni = p;
      }
    }
    if(p->suid == suid){
      /*suid ��Ʊ��*/
      pi = p;
      //break;
    }
    p = p->next;
  }

  if(pni)return pni;/*̾����ID������*/
  //if(pn)return pn;  /*̾��������*/
  //if(pi)return pi;  /*ID������*/
  return 0;
}

/*���󥵥ꥹ�Ȥ���Ͽ����Ƥ�������������*/
int get_num_SSM_List(void)
{
  SSM_List* p;
  int num;

  p = ssm_top;
  num = 0;  
  while(p){
    num++;
    p = p->next;
  }

  return num;
}

/*���󥵥ꥹ�Ȥ���n�֤�Υ��󥵤Υ��ɥ쥹���������*/
SSM_List* get_nth_SSM_List(int n)
{
  SSM_List* p;
  p = ssm_top;
  
  while(p){
    n--;
    if(n < 0)return p;
    p = p->next;
  }

  p = 0;
  return p;
}


/*��å������Τ��Ȥ�*/
int msq_loop(void)
{
  ssm_msg msg;
  SSM_List *slist;
  int len,num;
  
  while(1){
    /*�������Ԥ���*/
    if(verbosity_mode)printf("msg_ready\n");
    len = msgrcv(msq_id,&msg,SSM_MSG_SIZE,MSQ_CMD,0);
    if(len < 0)return 0;
     if(verbosity_mode)printf("msg_get\n");
    /*���줾��Υ��ޥ�ɤؤα���*/
    switch((msg.cmd_type)&0x1f){
    case MCinit:
      msg.msg_type = msg.res_type;/*�ֿ�*/
      msg.cmd_type = MCres;
      msg.suid  = shmid_ssm_common_data;
      msg.ssize = 0;
      /*����*/
      if((msgsnd(msq_id, &msg, SSM_MSG_SIZE, 0)) < 0)return 0;
      
       if(verbosity_mode)printf("init");
      break;
    case MCcreate:/*���󥵤Υ������*/
       if(verbosity_mode)printf("message:create!\n");

      /*�ꥹ�Ȥθ���*/
      slist = search_SSM_List(msg.name, msg.suid);

      /*Ʊ��ʪ��̵���ä���*/
      if(!slist){   if(verbosity_mode)printf("   |   :add\n");
	/*�ꥹ�Ȥ��ɲ�*/
	slist=add_ssm_list(msg.name, msg.suid, msg.ssize, msg.hsize, msg.time);
      }else{
	 if(verbosity_mode)printf("   |   :exist\n");
      }
      msg.msg_type = msg.res_type;/*�ֿ�*/
      msg.cmd_type = MCres;
      msg.suid  = slist->shm_id;
      msg.ssize = slist->shm_offset;
      /*����*/
      if((msgsnd(msq_id, &msg, SSM_MSG_SIZE, 0)) < 0)return 0;
      
       if(verbosity_mode)print_list(ssm_top);
       if(verbosity_mode)printf("   |   :received shm_id%d offset%d\n",msg.suid,msg.ssize);
      break;

    case MCopen:/*���󥵤Υ����ץ�*/
       if(verbosity_mode)printf("message:open!\n");
      slist = search_SSM_List(msg.name,msg.suid);
      if(slist){ if(verbosity_mode)printf("   |   :exist\n");
	msg.msg_type = msg.res_type;/*�ֿ�*/
	msg.cmd_type = MCres;
	msg.suid = slist->shm_id;
	msg.ssize = slist->shm_offset;
      }else{ if(verbosity_mode)printf("   |   :not found\n");
	msg.msg_type = msg.res_type;/*�ֿ�*/
	msg.cmd_type = MCres;
	msg.suid = -1;
	msg.ssize = 0;
      }
       /*����*/
      if((msgsnd(msq_id, &msg, SSM_MSG_SIZE, 0)) < 0)return 0;
       if(verbosity_mode)printf("   |   :received shm_id%d offset%d\n",msg.suid,msg.ssize);
      break;
      
    case MCclose:
      break;
      
    case MCgettid:
      break;

    case MCsensor_list_num:
      /*���󥵤Υꥹ�Ȥ���Ͽ����Ƥ�������������*/
      num = get_num_SSM_List();
      msg.msg_type = msg.res_type;/*�ֿ�*/
      msg.cmd_type = MCres;
      msg.suid = num;

      /*����*/
      if((msgsnd(msq_id, &msg, SSM_MSG_SIZE, 0)) < 0)return 0;
       if(verbosity_mode)printf("   |   :sensor num %d\n",msg.suid);
      break;

    case MCsensor_list_name:
      /*���󥵥ꥹ�Ȥ���n�֤�Τ�Τξ�����������*/
      num = msg.suid;
      slist = get_nth_SSM_List(num);
      if(slist){ if(verbosity_mode)printf("   |   :exist\n");
	msg.msg_type = msg.res_type;/*�ֿ�*/
	msg.cmd_type = MCres;
	msg.suid     = slist->suid;/*ID*/ 
	msg.ssize    = slist->header->size;/*�ǡ���������*/
	//msg.hsize    = slist->table_size;/*������*/
	//msg.time     = slist->cycle;/*ʿ�Ѽ���*/
	strcpy(msg.name, slist->name);/*̾��*/
      }else{ if(verbosity_mode)printf("   |   :not found\n");
	msg.msg_type = msg.res_type;/*�ֿ�*/
	msg.cmd_type = MCres;
	msg.suid = -1;
	msg.ssize = -1;
      }
      /*����*/
      if((msgsnd(msq_id, &msg, SSM_MSG_SIZE, 0)) < 0)return 0;
       if(verbosity_mode)printf("   |   :received num%d = %s[%d]\n",num,msg.name,msg.suid);
      break;

    case MCsensor_list_info:
      /*̾���ǻ��ꤷ�����󥵤ξ�����������*/
      if(verbosity_mode)printf("message:read_setting!\n");

      slist = search_SSM_List(msg.name, msg.suid);
      if(slist){ if(verbosity_mode)printf("   |   :exist\n");
	msg.msg_type = msg.res_type;/*�ֿ�*/
	msg.cmd_type = MCres;
	msg.ssize    = slist->header->size; /*�ǡ���������*/
	msg.suid     = slist->property_size; /*property size*/
	msg.time     = slist->header->cycle;/*ʿ�Ѽ���*/
	msg.hsize    = slist->header->num;  /*�����*/
      }else{ if(verbosity_mode)printf("   |   :not found\n");
	msg.msg_type = msg.res_type;/*�ֿ�*/
	msg.cmd_type = MCres;
	msg.suid  = -1;
	msg.ssize = -1;
      }
       /*����*/
      if((msgsnd(msq_id, &msg, SSM_MSG_SIZE, 0)) < 0)return 0;
       if(verbosity_mode)printf("   |   :received \n");
      break;

    case MCsensor_property_set:
      if(verbosity_mode)printf("message:property_set\n");

      slist = search_SSM_List(msg.name, msg.suid);
      if(slist){
	if(verbosity_mode)printf("   |   :exist\n");
	if(!slist->property){/*��꤬�ʤ���к��*/
	  slist->property = malloc(msg.ssize+sizeof(long));
	  slist->property_size = msg.ssize;
	}
	if(slist->property){/*��꤬�Ȥ�Ƥ�н񤭹���*/ 
	  /*��������OK���ֿ�*/
	  msg.msg_type = msg.res_type;/*�ֿ�*/
	  msg.res_type = get_receive_id();/*����Ϥ�����*/
	  msg.cmd_type = MCres;
	  msg.ssize    = slist->property_size; /*�ǡ���������*/
	  if((msgsnd(msq_id, &msg, SSM_MSG_SIZE, 0)) < 0)return 0;
	  /*�ǡ������ɤ߹���*/
	  msgrcv(msq_id, (char*)slist->property,
		 slist->property_size, msg.res_type, 0);
	}else{
	  if(verbosity_mode)printf("   |   :mem error\n");
	  msg.msg_type = msg.res_type;/*�ֿ�*/
	  msg.cmd_type = MCres;
	  msg.ssize    = 0;
	  if((msgsnd(msq_id, &msg, SSM_MSG_SIZE, 0)) < 0)return 0;	
	}
      }else{
	if(verbosity_mode)printf("   |   :not found\n");
	msg.msg_type = msg.res_type;/*�ֿ�*/
	msg.cmd_type = MCres;
	msg.ssize = 0;
	if((msgsnd(msq_id, &msg, SSM_MSG_SIZE, 0)) < 0)return 0;	  
      }
      break;

    case MCsensor_property_get:
      if(verbosity_mode)printf("message:propeerty_get\n");

      slist = search_SSM_List(msg.name, msg.suid);
      if(slist){
	if(verbosity_mode)printf("   |   :exist\n");
	if(slist->property && slist->property_size){/*��꤬�Ȥ�Ƥ�н񤭹���*/ 
	  /*��������OK���ֿ�*/
	  msg.msg_type = msg.res_type;/*�ֿ�*/
	  msg.cmd_type = MCres;
	  msg.ssize    = slist->property_size; /*�ǡ���������*/
	  if((msgsnd(msq_id, &msg, SSM_MSG_SIZE, 0)) < 0)return 0;
	  
	  /*�ǡ���������*/
	  *((long*)slist->property) = msg.res_type;/*�ֿ�*/
	  msgsnd(msq_id, (char*)slist->property,
		 slist->property_size, 0);
	}else{
	  if(verbosity_mode)printf("   |   :mem error\n");
	  msg.msg_type = msg.res_type;/*�ֿ�*/
	  msg.cmd_type = MCres;
	  msg.ssize    = 0;
	  if((msgsnd(msq_id, &msg, SSM_MSG_SIZE, 0)) < 0)return 0;	
	}
      }else{
	if(verbosity_mode)printf("   |   :not found\n");
	msg.msg_type = msg.res_type;/*�ֿ�*/
	msg.cmd_type = MCres;
	msg.ssize = 0;
	if((msgsnd(msq_id, &msg, SSM_MSG_SIZE, 0)) < 0)return 0;	  
      }
      break;
    default:
      break;
    }
  }

  return 1;
}


int main(int argc,char* argv[])
{
  printf("\n");
  printf(" --------------------------------------------\n");
  printf(" SSM(Sensor Sharing Manager).  \n");
  printf(" Ver. %s \n",VERSION);
  printf(" ------------------------------------------- \n\n");
  
  /*��������Ѱ�*/
  escape_road();
  
  ssm_top = 0;
   
  ssm_init();
  clean_shm();
  msq_loop();
  
  return 0;
}

