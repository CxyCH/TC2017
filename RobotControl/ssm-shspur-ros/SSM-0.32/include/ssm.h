/*
  ���󥵶�ͭ�����������
  12/13 eto
*/

#ifndef __SSM_MANAGER__
#define __SSM_MANAGER__

#include "ymbc_time.h"
#include "ssm_common.h"


/*---------------------------defines------------------------------*/



/*---------------------------typedefs------------------------------*/
typedef struct ssm_list *SSM_ListPtr;
typedef struct ssm_list{
  char name[SSM_SNAME_MAX];/*���󥵤�̾��*/
  int  suid;           /*���󥵸�ͭ��ID*/
  int  shm_id;         /*�֥�å��Τ��붦ͭ�����ID*/
  char* shm_ptr;       /*�֥�å��Τ��붦ͭ����Υ��ɥ쥹(Manager��Τ�ͭ��)*/
  int   shm_offset;    /*��ͭ������ǥ֥�å��Τ�����*/
  int   size;          /*�֥�å��Υ�����*/
  ssm_header *header;  /*�֥�å��Υإå�*/
  SSM_ListPtr next;    /*���Υꥹ�Ȥ�*/
  char* property;      /*�ץ�ѥƥ��ǡ����ؤΥݥ���*/
  int   property_size;  /*�ץ�ѥƥ��ǡ������礭��*/
}SSM_List;


/*----------------------global variables--------------------------*/


/*----------------------function prototypes-----------------------*/
int alloc_ssm_block(int ssize,int hsize,YTime cycle, char **shm_h,int *offset);
int alloc_ssm_common_data(void);
SSM_List* add_ssm_list(char *name,int suid,int ssize,int hsize,YTime cycle);
void free_ssm_list(SSM_List* ssmp);
int ssm_init(void);
SSM_List* serch_SSM_List(char* name,int suid);
int msq_loop(void);
SSM_List* get_nth_SSM_List(int n);
int get_num_SSM_List(void);
long get_receive_id(void);

#endif
