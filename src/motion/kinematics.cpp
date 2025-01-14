// Header
#include "robotic_project/kinematics.h"

// Eigen 
#include "Eigen/Dense"
#include "Eigen/QR"
#include "Eigen/Geometry"

// ROS
#include "ros/ros.h"
#include "sensor_msgs/JointState.h"
#include "boost/shared_ptr.hpp"
#include "std_msgs/Float64MultiArray.h"

// Standard
#include <iostream>
#include <cmath>
#include <stdexcept>

M4d T10f(double th1)
{
    return M4d {
        {cos(th1) ,-sin(th1) ,0 ,0},
        {sin(th1) ,cos(th1) ,0 ,0},
        {0 ,0 ,1 ,d(0)},
        {0 ,0 ,0 ,1}
    };
}

M4d T21f(double th2)
{
    return M4d {
        {cos(th2) ,-sin(th2) ,0 ,0},
        {0 ,0 ,-1 ,0},
        {sin(th2) ,cos(th2) ,0 ,0},
        {0 ,0 ,0 ,1}
    };
}

M4d T32f(double th3)
{
    return M4d {
        {cos(th3) ,-sin(th3) ,0 ,a(1)},
        {sin(th3) ,cos(th3) ,0 ,0},
        {0 ,0 ,1 ,d(2)},
        {0 ,0 ,0 ,1}
    };
}

M4d T43f(double th4)
{
    return M4d {
        {cos(th4) ,-sin(th4) ,0 ,a(2)},
        {sin(th4) ,cos(th4) ,0 ,0},
        {0 ,0 ,1 ,d(3)},
        {0 ,0 ,0 ,1}
    };
}

M4d T54f(double th5)
{
    return M4d {
        {cos(th5) ,-sin(th5) ,0 ,0},
        {0 ,0 ,-1 ,-d(4)},
        {sin(th5) ,cos(th5) ,0 ,0},
        {0 ,0 ,0 ,1}
    };
}

M4d T65f(double th6)
{
    return M4d {
        {cos(th6) ,-sin(th6) ,0 ,0},
        {0 ,0 ,1 ,d(5)},
        {-sin(th6) ,-cos(th6) ,0 ,0},
        {0 ,0 ,0 ,1}
    };
}

M3d rotation_matrix_z_axis(double alpha)
{
    return M3d {
        {cos(alpha), -sin(alpha), 0},
        {sin(alpha), cos(alpha), 0},
        {0, 0, 1}
    };
}

M4d direct_kin(V6d js)
{
    return T10f(js(0)) * T21f(js(1)) * T32f(js(2)) * T43f(js(3)) * T54f(js(4)) * T65f(js(5));
}

Jacobian jacobian(V6d js)
{
    Jacobian J;
    J.setZero();
    V6d J1(6, 1);
    J1 << d(4) * (cos(js(0)) * cos(js(4)) + cos(js(1) + js(2) + js(3)) * sin(js(0)) * sin(js(4))) + d(2) * cos(js(0)) + d(3) * cos(js(0)) - a(2) * cos(js(1) + js(2)) * sin(js(0)) - a(1) * cos(js(1)) * sin(js(0)) - d(4) * sin(js(1) + js(2) + js(3)) * sin(js(0)),
        d(4) * (cos(js(4)) * sin(js(0)) - cos(js(1) + js(2) + js(3)) * cos(js(0)) * sin(js(4))) + d(2) * sin(js(0)) + d(3) * sin(js(0)) + a(2) * cos(js(1) + js(2)) * cos(js(0)) + a(1) * cos(js(0)) * cos(js(1)) + d(4) * sin(js(1) + js(2) + js(3)) * cos(js(0)),
        0,
        0,
        0,
        1;
    V6d J2(6, 1);
    J2 << -cos(js(0)) * (a(2) * sin(js(1) + js(2)) + a(1) * sin(js(1)) + d(4) * (sin(js(1) + js(2)) * sin(js(3)) - cos(js(1) + js(2)) * cos(js(3))) - d(4) * sin(js(4)) * (cos(js(1) + js(2)) * sin(js(3)) + sin(js(1) + js(2)) * cos(js(3)))),
        -sin(js(0)) * (a(2) * sin(js(1) + js(2)) + a(1) * sin(js(1)) + d(4) * (sin(js(1) + js(2)) * sin(js(3)) - cos(js(1) + js(2)) * cos(js(3))) - d(4) * sin(js(4)) * (cos(js(1) + js(2)) * sin(js(3)) + sin(js(1) + js(2)) * cos(js(3)))),
        a(2) * cos(js(1) + js(2)) - (d(4) * sin(js(1) + js(2) + js(3) + js(4))) / 2 + a(1) * cos(js(1)) + (d(4) * sin(js(1) + js(2) + js(3) - js(4))) / 2 + d(4) * sin(js(1) + js(2) + js(3)),
        sin(js(0)),
        -cos(js(0)),
        0;
    V6d J3(6, 1);
    J3 << cos(js(0)) * (d(4) * cos(js(1) + js(2) + js(3)) - a(2) * sin(js(1) + js(2)) + d(4) * sin(js(1) + js(2) + js(3)) * sin(js(4))),
        sin(js(0)) * (d(4) * cos(js(1) + js(2) + js(3)) - a(2) * sin(js(1) + js(2)) + d(4) * sin(js(1) + js(2) + js(3)) * sin(js(4))),
            a(2) * cos(js(1) + js(2)) - (d(4) * sin(js(1) + js(2) + js(3) + js(4))) / 2 + (d(4) * sin(js(1) + js(2) + js(3) - js(4))) / 2 + d(4) * sin(js(1) + js(2) + js(3)),
        sin(js(0)),
        -cos(js(0)),
        0;
    V6d J4(6, 1);
    J4 << d(4) * cos(js(0)) * (cos(js(1) + js(2) + js(3)) + sin(js(1) + js(2) + js(3)) * sin(js(4))),
        d(4) * sin(js(0)) * (cos(js(1) + js(2) + js(3)) + sin(js(1) + js(2) + js(3)) * sin(js(4))),
        d(4) * (sin(js(1) + js(2) + js(3) - js(4)) / 2 + sin(js(1) + js(2) + js(3)) - sin(js(1) + js(2) + js(3) + js(4)) / 2),
        sin(js(0)),
        -cos(js(0)),
        0;
    V6d J5(6, 1);
    J5 << -d(4) * sin(js(0)) * sin(js(4)) - d(4) * cos(js(1) + js(2) + js(3)) * cos(js(0)) * cos(js(4)),
        d(4) * cos(js(0)) * sin(js(4)) - d(4) * cos(js(1) + js(2) + js(3)) * cos(js(4)) * sin(js(0)),
        -d(4) * (sin(js(1) + js(2) + js(3) - js(4)) / 2 + sin(js(1) + js(2) + js(3) + js(4)) / 2),
        sin(js(1) + js(2) + js(3)) * cos(js(0)),
        sin(js(1) + js(2) + js(3)) * sin(js(0)),
        -cos(js(1) + js(2) + js(3));
    V6d J6(6, 1);
    J6 << 0,
        0,
        0,
        cos(js(4)) * sin(js(0)) - cos(js(1) + js(2) + js(3)) * cos(js(0)) * sin(js(4)),
        -cos(js(0)) * cos(js(4)) - cos(js(1) + js(2) + js(3)) * sin(js(0)) * sin(js(4)),
        -sin(js(1) + js(2) + js(3)) * sin(js(4));
    J << J1, J2, J3, J4, J5, J6;
    return J;
}

/*
    @brief compute the point in the instant t of the linear interpolation of x1 and x2

    @param[in] x1 and x2: 3D points
    @param[in] t: time
*/
V3d x(double t, V3d x1, V3d x2)
{
    const double n_t = t / d_path;
    if (n_t > 1) return x2;
    else return (n_t * x2) + ((1 - n_t) * x1);
}

/*
    @brief compute the quaternion in the instant t of the slerp formed by q1 and q2

    @param[in] q1 and q2: initial quaternion and finale quaternion
    @param[in] t: time
*/
Qd slerp(double t, Qd q1, Qd q2)
{
    const double n_t = t / d_path;
    if (n_t > 1) return q2;
    else return q1.slerp(n_t, q2);
}

/*
    @brief compute the path of the robot accoring to the initial joints values

    @param[in] mr: robotic mesures of the joints and gripper
    @param[in] i_p, f_p: initial and final point of the path
    @param[in] i_q, f_q: initial and final quaternion of the path
*/
Path differential_inverse_kin_quaternions(V8d mr, V3d i_p, V3d f_p, Qd i_q, Qd f_q)
{
    /*
        gs is the gripper actual opening
        js_k and ks_k_dot are joints values in the instant k and its derivative dot in the same insatnt
    */
    V2d gs {mr(6), mr(7)};
    V6d js_k, js_dot_k; 

    /*
        angular and positional velocities combined with the correction error
    */
    V6d fv;

    /*
        path of the robot
    */
    Path path;

    /*
        transformation matrix in the instant k 
    */
    M4d tm_k;

    /*
        position of the robot in the instant k
    */
    V3d p_k;

    /*
        rotation matrix of the robot in the instant k
    */
    M3d rm_k;

    /*
        quaternion related to the rotational matrix of the robot in the instant k
    */
    Qd q_k;

    /*
        angular and positional velocities of the robot in the instant k
    */
    V3d av_k, pv_k;

    /*
        quaternion velocity related to the angular velocity of the robot in the instant k
    */
    Qd qv_k;

    /*
        quaternion error of the rotational path (slerp) of the robot
    */
    Qd qerr_k;

    /*
        positional error of the linear path (x) of the robot
    */
    V3d perr_k;

    /*
        geometric jacobian and inverse geometric jacobian of the robot in the instant k
    */
    Jacobian j_k, invj_k;

    /*
        Kp is for positional correction 
        Kq is for quaternion correction 
    */
    M3d Kp, Kq;
    Kp = M3d::Identity() * 10;
    Kq = M3d::Identity() * 1;

    /*
        insert the starting point to the path
    */
    for (int i = 0; i < 6; ++i) js_k(i) = mr(i);
    path = insert_new_path_instance(path, js_k, gs);

    /*
        each delta time (dt) compute the joints state to insert into the path 
    */
    for (double t = dt; t < d_path; t += dt) 
    {
        /*
            compute the direct kinematics in the instant k 
        */
        tm_k = direct_kin(js_k);
        p_k = tm_k.block(0, 3, 3, 1);
        rm_k = tm_k.block(0, 0, 3, 3);
        q_k = rm_k;

        /*
            compute the velocities in the instant k
        */
        pv_k = (x(t, i_p, f_p) - x(t - dt, i_p, f_p)) / dt;
        qv_k = slerp(t + dt, i_q, f_q) * slerp(t, i_q, f_q).conjugate(); 
	    av_k = (qv_k.vec() * 2) / dt;

        /* 
            compute the jacobian and its inverse in the instant k
        */
        j_k = jacobian(js_k);
        invj_k = (j_k.transpose() * j_k + Jacobian::Identity() * 0.0001).inverse() * j_k.transpose();
        if (abs(j_k.determinant()) < 0.00001) 
        {
            ROS_WARN("Near singular configuration");
        }
            
        /*
            compute the errors in the path
        */
        qerr_k = slerp(t, i_q, f_q) * q_k.conjugate();
        perr_k = x(t, i_p, f_p) - p_k;
        
        /*
            compute the vector of the velocities composition with a parameter of correction
        */
        fv << pv_k + (Kp * perr_k), av_k + (Kq * qerr_k.vec());

        /*
            compute the joints state in the instant k
        */
        js_dot_k = invj_k * fv;
        js_k = js_k + (js_dot_k * dt);

        /*
            add it to the path
        */
        path = insert_new_path_instance(path, js_k, gs);
    }

    return path;
}

/*
    @brief insert a new joint state into the path of the robot

    @param[in] p 
    @def path of the robot

    @param[in] js
    @def joints state to insert into the path

    @param[in] gs
    @def gripper state of the opening
*/
Path insert_new_path_instance(Path p, V6d js, V2d gs)
{
    p.conservativeResize(p.rows() + 1, p.cols());
    p.row(p.rows() - 1) = V8d {js(0), js(1), js(2), js(3), js(4), js(5), gs(0), gs(1)};
    return p;
}

/*
    @brief read the measures of the robot (joints, gripper)
*/
V8d read_robot_measures()
{
    /*
        read and store the measures of the joints and the gripper inside the mr vector
    */
    boost::shared_ptr<sensor_msgs::JointState const> mr;
    mr = ros::topic::waitForMessage<sensor_msgs::JointState>("/ur5/joint_states");

    /*
        save and arrange the data given in the mr vector into the m vector
    */
    V8d m;
    for (int i = 0; i < 8; ++i) m(i) = mr->position[i];
    return V8d {m(4), m(3), m(0), m(5), m(6), m(7), m(1), m(2)};
}

/*
    @brief get from the robot measures only the joints state

    @param[in] mr: robot measures 
*/
V6d get_joint_state(V8d mr)
{
    /*
        catch only the values representing the joint radians
    */
    return V6d {mr(0), mr(1), mr(2), mr(3), mr(4), mr(5)};
}

/*
    @brief change point in the world frame to base frame of the robot

    @param[in] xw: 3D point in the world frame
*/
V3d world_to_base(V3d xw)
{
    /*
        transformation matrix used to change frame
    */
    M4d T;

    /*
        point in the base robot frame
    */
    V3d xb; 

    /*
        temp point used for the computation
    */
    V4d xt;

    /*
        initialize the T matrix 
    */
    T << 1.0, 0.0, 0.0, 0.5,
        0.0, -1.0, 0.0, 0.35,
        0.0, 0.0, -1.0, 1.75,
        0.0, 0.0, 0.0, 1.0;

    /*
        compute the word position in the xt vector with 4 items
    */
    xt = T.inverse() * V4d(xw(0), xw(1), xw(2), 1.0);

    /*
        extract the last element of xt and save the rest into xb vector
    */
    xb << xt(0), xt(1), xt(2);
    return xb;
}

/*
    @brief apply the path desired

    @param[in] mv: desired path to apply for the robot
    @param[in] pub: ros publisher 
*/
void move(Path mv, ros::Publisher pub)
{
    /*
        frequency of how quickly the data are delivered to the robot in Hz
    */
    ros::Rate loop_rate(120);

    /*
        iterate the each row of the movement
    */
    for (int i = 0; i < mv.rows(); ++i)
    {
        /*
            save the joint and gripper values into a vector 
        */
        V8d joint_state;
        joint_state << mv(i, 0), mv(i, 1), mv(i, 2), mv(i, 3), mv(i, 4), mv(i, 5), mv(i, 6), mv(i, 7);

        /*
            feed the message to deliver to the robot with the values within the joint_state vector
        */
        std_msgs::Float64MultiArray joint_statem;
        joint_statem.data.resize(8);
        for (int j = 0; j < 8; j++) joint_statem.data[j] = joint_state(j);

        /*
            send the message
        */
        pub.publish(joint_statem);
        loop_rate.sleep();
    }
}

/*
    @brief set the save position

    @param[in] pub: ros publisher
*/
void set_safe_configuration(ros::Publisher pub)
{
    V6d joint_state = get_joint_state(read_robot_measures());
    V6d safe_joint_state; safe_joint_state << joint_state(0), safe_joint_conf;
    M4d transformation_matrix = direct_kin(safe_joint_state);
    M3d rotation_matrix = transformation_matrix.block(0, 0, 3, 3);
    V3d position = transformation_matrix.block(0, 3, 3, 1);

    /*
        apply movement 
    */
    move_end_effector(position, rotation_matrix, pub);

}

/*
    @brief give a sequence of points in the space to rich the final position in safe way

    @param[in] final_position: final position for the robot
*/
Trajectory build_trajectory(V3d final_position)
{  
    /*
        stationary points used to detect the best possible trajectory
    */
    int stationary_points_num = 7;
    Eigen::Matrix<double, 7, 3> stationary_points {
        {0.3, 0.1, 0.5}, {0.4, 0, 0.5},
        {0.3, -0.3, 0.5}, {0, -0.4, 0.5},
        {-0.3, -0.3, 0.5}, {-0.4, 0, 0.5},
        {-0.3, 0.1, 0.5}
    };

    /*
        read the initial position of the robot
    */
    V6d joint_state = get_joint_state(read_robot_measures());
    M4d transformation_matrix = direct_kin(joint_state);
    V3d init_position = transformation_matrix.block(0, 3, 3, 1);

    /*
        compute the starting position inside the stationaty points
    */
    int starting_position = -1;
    double min_distance = -1;
    double possible_min_distance;
    for (int i = 0; i < stationary_points_num; ++i)
    {
        possible_min_distance = abs(stationary_points(i, 0) - init_position(0));
        possible_min_distance = possible_min_distance + abs(stationary_points(i, 1) - init_position(1));
        if (min_distance == -1 || possible_min_distance < min_distance) 
        {
            min_distance = possible_min_distance; 
            starting_position = i;
        }
    }

    /*
        compute the ending position inside the stationary points
    */
    int ending_position = -1;
    min_distance = -1;
    for (int i = 0; i < stationary_points_num; ++i)
    {
        possible_min_distance = abs(stationary_points(i, 0) - final_position(0));
        possible_min_distance = possible_min_distance + abs(stationary_points(i, 1) - final_position(1));
        if (min_distance == -1 || possible_min_distance < min_distance)
        {
            min_distance = possible_min_distance; 
            ending_position = i;
        }
    }

    /*
        compose the trajectory 
    */
    bool trajectory_built = false;
    int i = starting_position;

    Trajectory trajectory;

    do
    {
        trajectory.conservativeResize(trajectory.rows() + 1, trajectory.cols());
        trajectory.row(trajectory.rows() - 1) = V3d {stationary_points(i, 0), stationary_points(i, 1), stationary_points(i, 2)};
        if (i == ending_position) trajectory_built = true;
        if (ending_position < starting_position) i = i - 1; else i = i + 1;
    }
    while (!trajectory_built);

    trajectory.conservativeResize(trajectory.rows() + 1, trajectory.cols());
    trajectory.row(trajectory.rows() - 1) = V3d {final_position(0), final_position(1), final_position(2)};

    return trajectory;
}

/*
    @brief move the robot from its initial position to the given final confinguration

    @param[in] fp: final position
    @param[in] feu: final euler angle_robots
    @param[in] pub: ros publisher
*/
void move_end_effector(V3d final_position, M3d final_rotation_matrix, ros::Publisher pub)
{
    /*
        compute the final quaternion
    */
    Qd final_quaternion(final_rotation_matrix);

    /*
        robot measures
    */
    V8d robot_measures = read_robot_measures();
    V6d joint_state = get_joint_state(robot_measures);

    /*
        set the initial position and quaternion
    */
    M4d transformation_matrix = direct_kin(joint_state);
    M3d rotation_matrix = transformation_matrix.block(0, 0, 3, 3);
    V3d position = transformation_matrix.block(0, 3, 3, 1);
    Qd init_quaternion(rotation_matrix);

    /*
        compute the trajectory to follow
    */
    Path p = differential_inverse_kin_quaternions(robot_measures, position, final_position, init_quaternion, final_quaternion);

    /*
        move the robot according the trajectory
    */
    move(p, pub);
}

/*
    @brief open/close the gripper

    @param[in] pub: ros publisher
*/
void toggle_gripper(ros::Publisher pub, bool force_opening)
{
    /*
        path, joint state and gripper values
    */
    Path p; 
    V8d mr = read_robot_measures();
    V6d joint_state = get_joint_state(mr);
    V2d gr {mr(6), mr(7)};

    /*
        number of steps to toggle the gripper 
    */
    const int steps = 50;

    /*
        measure of how much the gripper will close
    */
    const double closing = -0.50; 

    /*
        measure of how much the gripper will open
    */
    const double opening = 0.80;

    /*
        gripper right and left side initial position
    */
    const double grr = gr(0);
    const double grl = gr(1);

    /*
        compute the final value of the gripper
    */
    double toggle;
    if (grr > 0 && grl > 0) toggle = closing; else toggle = opening;
    if (force_opening) toggle = opening;

    /*
        assign the frame distances for each gripper side
    */
    const double distr = (toggle - grr) / steps;
    const double distl = (toggle - grl) / steps;

    /*
        compute the path divided into steps to toggle the gripper
    */
    for (int i = 1; i <= steps; ++i)
    {
        gr(0) = grr + distr * i; 
        gr(1) = grl + distl * i; 
        p = insert_new_path_instance(p, joint_state, gr);
    }

    /*
        move the gripper
    */
    move(p, pub);
}

/*
    @brief check if the point give is within the workspace

    @param[in] point: point to detect
*/
bool point_in_workspce(V3d point)
{
    /*
        limits of the workspace
    */
    V2d x_axis_limits(-0.50, 0.5);
    V2d y_axis_limits(0.10, -0.4);
    V2d z_axis_limits(0.00, 0.75);

    /*
        axes to detect
    */
    const int x_axis = 0;
    const int y_axis = 1;
    const int z_axis = 2;

    /*
        controls of the three axis of the point
    */
    if (point(x_axis) < x_axis_limits(0) || point(x_axis) > x_axis_limits(1)) return false;
    if (point(y_axis) > y_axis_limits(0) || point(y_axis) < y_axis_limits(1)) return false;
    if (point(z_axis) < z_axis_limits(0) || point(z_axis) > z_axis_limits(1)) return false;
    
    /*
        in case the point is inside the workspce return true
    */
    return true;
}

/*
    @brief given an object on the table, the robot will grasp and move it in a new position

    @param[in] object_position: position of the object
    @param[in] object_orientation: orientation of the object
    @param[in] final_object_position: position where the robot has to move the object
    @param[in] final_object_orientation: orientation that the object has to have in the final_object_position
*/
void grasp_and_move_object(V3d object_position, M3d object_orientation, V3d final_object_position, M3d final_object_orientation, ros::Publisher pub)
{   
    /*
        z axis values for the movement and grasp operation
    */
    const double move_height = 0.50;
    const double grasp_height = 0.74;
    const double leave_height = 0.70;
    const int z_axis = 2;

    /*
        impose a safe initial configuartion to establish a correct movement
    */
    set_safe_configuration(pub);
    
    /*
        impose the height of the movement
    */
    object_position(z_axis) = move_height;

    /*
        check the velodity of the position givem
    */
    if (!point_in_workspce(object_position)) 
    {
        ROS_ERROR("obj position is not inside the workspce");
        exit(1);
    }

    /*
        compute the path to do to reach the position of the object
    */
    Trajectory path = build_trajectory(object_position);

    /*
        move the robot following the path
    */
    for (int i = 0; i < path.rows(); ++i)
    {   
        V6d joint_state = get_joint_state(read_robot_measures()); 
        M4d transformation_matrix = direct_kin(joint_state);
        M3d rotation_matrix = M3d::Identity();
        if (i == path.rows() - 1) rotation_matrix = object_orientation;
        move_end_effector(path.row(i), rotation_matrix, pub);
    }

    /*
        open gripper
    */
    toggle_gripper(pub, true);

    /*
        move downswards
    */
    object_position(z_axis) = grasp_height;
    move_end_effector(object_position, object_orientation, pub);

    /*
        close gripper
    */
    toggle_gripper(pub);

    /*
        move upwards
    */
    object_position(z_axis) = move_height;
    move_end_effector(object_position, object_orientation, pub);

    /*
        impose the height for the movement
    */
    final_object_position(z_axis) = move_height;

    /*
        check the valodity of the position givem
    */
    if (!point_in_workspce(final_object_position)) 
    {
        ROS_ERROR("final obj position is not inside the workspce");
        exit(1);
    }

    /*
        compute the path to reach the final position to pose the object
    */
    path = build_trajectory(final_object_position);

    /*
        move the robot following the path
    */
    for (int i = 0; i < path.rows(); ++i)
    {
        V6d joint_state = get_joint_state(read_robot_measures());
        M4d transformation_matrix = direct_kin(joint_state);
        M3d rotation_matrix = M3d::Identity();
        if (i == path.rows() - 1) rotation_matrix = final_object_orientation;
        move_end_effector(path.row(i), rotation_matrix, pub);
    }

    /*
        move downwards
    */
    final_object_position(z_axis) = leave_height;
    move_end_effector(final_object_position, final_object_orientation, pub);

    /*
        open gripper
    */
    toggle_gripper(pub);

    /*
        move upwards
    */
    final_object_position(z_axis) = move_height;
    move_end_effector(final_object_position, final_object_orientation, pub);
}
