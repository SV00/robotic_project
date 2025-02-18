# Robotic Project
Fundament of Robotic project year 2023/2024

Authors:
1. Gabriel Fumagalli
2. Simone Vigasio
3. Stefan Gore

## Project Description

The objective of this project was to create a self-sufficient robot capable of executing pick-and-place operations. The robotic arm employed for manipulation is the UR5, equipped with a zed camera for perception. The pick-and-place operation involves selecting various bricks and accurately positioning them in their predetermined locations. The robot possesses the capability to autonomously identify the bricks and execute the pick-and-place task. 

https://github.com/user-attachments/assets/e65fe301-f40d-4c30-a8f2-4b8ba58d5de7

## Folder

```
.
├── CMakeLists.txt
├── include
│   └── robotic_project
├── models
├── package.xml
├── src
│   └── motion
│       ├── Eigen
│       ├── kinematics.cpp
│       └── main.cpp
├── srv
│   └── ObtainBrickPose.srv
└── vision
    ├── best.pt
    ├── vision.py
    └── yolov5
```
This project consists of two ROS nodes. One is specifically designed for object detection of the bricks on the table, and it calculates their localization and orientation. The other node is focused on robot motion and task planning.

The link between the two nodes is established through a service. The "vision.py" file functions as the server, while the "main.cpp" file acts as the client, tasked with patiently waiting until the brick detection is complete.

## Installation
The project was tested on ubuntu 20.04 with ROS Noetic. 
To make it works we use [locosim](https://github.com/mfocchi/locosim) repository. 
So, first of all clone locosim repository on your machine following the respective instructions.

Inside the ros_ws/src where you have imported the locosim package, type this command:
```
git clone https://github.com/SV00/robotic_project.git
```
Now, you have to install the vision component. Inside the ros_ws directory type:
```
cd src/robotic_project/vision
git clone https://github.com/ultralytics/yolov5.git
cd yolov5
pip3 install -r requirements.txt
pip install torchvision
```
Finally, always inside ros_ws folder compile the project:
```
catkin_make
. devel/setup.bash
```
## Run Project

### Setup
Inside this file ``ros_ws/src/locosim/robot_control/base_controllers/params.py`` change the line 32 with:
```
'gripper_sim': True,
```
Now, into the ``robotic_project/world`` folder bring the "robotic_project.world" file into the ``ros_ws/src/locosim/ros_impedance_controller/worlds`` folder.
Then, into ``ros_ws/src/locosim/robot_control/base_controllers/ur5_generic.py`` change the line 71 with:
```
self.world_name = 'robotic_project.world'
```
In the bottom of the file ``~/.bashrc`` add:
```
export GAZEBO_MODEL_PATH=$GAZEBO_MODEL_PATH:/$HOME/ros_ws/src/robotic_project/models
```
This command is for allowing ros to detect the models imported by the repository

Finally, compile the project into the ros_ws folder again to save the changes:
```
catkin_make
. devel/setup.bash
```
### Run
In order to run the project you have to open three terminals.

In the first terminal type:
```
python3 -i ros_ws/src/locosim/robot_control/base_controllers/ur5_generic.py
```

In the second terminal type:
```
rosrun robotic_project vision
```
wait until the message ``the vision is ready to deliver the block position`` is prompted.

Then, in the third terminal type:
```
rosrun robotic_project main
```
