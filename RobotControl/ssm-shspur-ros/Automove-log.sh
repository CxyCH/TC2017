#!/bin/bash

LOG=1
if [[ $1 == "" ]]; then
echo "Please add the path to record rosbag"
LOG=0;
else
destination=$1;
fi

source ~/ssm-shspur-ros/catkin_ws/devel/setup.bash

killall ssm
killall sh_spur
sleep 2

ssm&
sleep 2

cd sh_spur-0.11/sh_spur/
sh_spur -p Chinouka_okugai_robo.param&
sleep 2

#cd ~/ssm-shspur-ros/catkin_ws
#rosrun auto_sh_spur auto_sh_spur &

cd ~/ssm-shspur-ros
roslaunch kerberos_all_node-no-joy.launch &

if [[ $LOG == 1 ]]; then
roslaunch rosbag-dvs.launch destination:="$destination"
fi