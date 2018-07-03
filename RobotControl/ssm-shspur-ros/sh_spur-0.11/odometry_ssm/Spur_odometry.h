/*
 *  Spur odometry data header file for SSM
 *
 *  2003/01/19  
 *
 */

/**********memo************

����
�ƥ����ѤΥե�����

**************************/

#ifndef __SPURODOMETRY__
#define __SPURODOMETRY__

#define SNAME_ODOMETRY  "spur_odometry" 

typedef struct{
  double x;
  double y;
  double theta;
  double v;
  double w;
}Spur_Odometry;

typedef struct{
  double radius_r;
  double radius_l;
  double tread;
}Spur_Odometry_Property;
#endif
