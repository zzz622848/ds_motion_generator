/*
 * Copyright (C) 2018 Learning Algorithms and Systems Laboratory, EPFL, Switzerland
 * Author:  Nadia Figueroa
 * email:   nadia.figueroafernandez@epfl.ch
 * website: lasa.epfl.ch
 *
 * This work was supported by the EU project Cogimon H2020-ICT-23-2014.
 *
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
 */

#ifndef __LPVDS_MOTION_GENERATOR_H__
#define __LPVDS_MOTION_GENERATOR_H__

#include "ros/ros.h"
#include "geometry_msgs/Pose.h"
#include "geometry_msgs/Twist.h"
#include "geometry_msgs/Quaternion.h"
#include "geometry_msgs/PointStamped.h"
#include <std_msgs/Bool.h>
#include "nav_msgs/Path.h"

#include <vector>
#include <mutex>
#include <stdlib.h> 

#include "MathLib.h"
#include "lpvDS.h"
#include "CDDynamics.h"

#include <dynamic_reconfigure/server.h>
#include <ds_motion_generator/lpvDS_paramsConfig.h>

class lpvDSMotionGenerator {

private:

	// DS variables
    int K_;
    int M_;
    double dt_;
    double max_desired_vel_;
    double pos_error_;
    bool   bPublish_DS_path_;
    double Mu_scale_, Sigma_scale_;
    std::vector<double>  Priors_;
    std::vector<double>  Mu_;
    std::vector<double>  Sigma_;
    std::vector<double>  A_;
    std::vector<double>  attractor_;
    std::unique_ptr<lpvDS> LPV_DS_;

    // Target Orientation variables
    double                qx, qy, qz, qw;

	// Filter variables
	std::unique_ptr<CDDynamics> CCDyn_filter_;

	double Wn_;
	MathLib::Vector accLimits_;
	MathLib::Vector velLimits_;


    // ROS system variables
    ros::NodeHandle           nh_;
    ros::Rate                 loop_rate_;

    // Publishers/Subscriber
    ros::Subscriber           sub_real_pose_;
    ros::Subscriber           sub_desired_target_;
    ros::Publisher            pub_desired_twist_;
    ros::Publisher            pub_desired_orientation_;
    ros::Publisher            pub_desired_twist_filtered_;
    ros::Publisher            pub_target_;
    ros::Publisher            pub_DesiredPath_;
    ros::Publisher            pub_tigger_passive_ds_;

    // Topic Names
    std::string               input_topic_name_;
    std::string               input_target_topic_name_;
    std::string               output_topic_name_;
    std::string               output_filtered_topic_name_;

    // Messages
    std_msgs::Bool            msg_passive_ds_;
    geometry_msgs::Pose       msg_real_pose_;
    geometry_msgs::Point      msg_desired_target_;
    geometry_msgs::Quaternion msg_quaternion_;
    geometry_msgs::Twist      msg_desired_velocity_;
    geometry_msgs::Twist      msg_desired_velocity_filtered_;

	nav_msgs::Path msg_DesiredPath_;
    double path_offset_;
    /* For iCub Visualization */
    //    int MAX_FRAME = 800;
    /* For KUKA Real-time */
    int MAX_FRAME = 200;

	//dynamic reconfig settig
    dynamic_reconfigure::Server<ds_motion_generator::lpvDS_paramsConfig> dyn_rec_srv_;
    dynamic_reconfigure::Server<ds_motion_generator::lpvDS_paramsConfig>::CallbackType dyn_rec_f_;


	// Class variables
	std::mutex mutex_;

	MathLib::Vector real_pose_;
    MathLib::Vector learned_att_;

    /* For dynamics target/chaning attractor */
    MathLib::Vector target_pose_;
    MathLib::Vector target_offset_;


	MathLib::Vector desired_velocity_;
	MathLib::Vector desired_velocity_filtered_;

	double scaling_factor_;
	double ds_vel_limit_;

    bool bDynamic_target_;


public:
	lpvDSMotionGenerator(ros::NodeHandle &n,
	                  double frequency,
	                  int K,
	                  int M,
	                  std::vector<double> Priors,
	                  std::vector<double> Mu,
	                  std::vector<double> Sigma,
                      std::vector<double> A,
	                  std::vector<double> attractor,
	                  std::string input_topic_name,
	                  std::string output_topic_name,
                      std::string output_filtered_topic_name,
                      std::string input_target_topic_name,
                      bool bPublish_DS_path,
                      bool bDynamic_target,
                      double path_offset);

    ~lpvDSMotionGenerator(void);

	bool Init();

	void Run();

private:

	bool InitializeDS();

	bool InitializeROS();

	void UpdateRealPosition(const geometry_msgs::Pose::ConstPtr& msg);

    void UpdateDynamicTarget(const geometry_msgs::Point::ConstPtr& msg);

	void ComputeDesiredVelocity();

	void PublishDesiredVelocity();

	void PublishFuturePath();

    void DynCallback(ds_motion_generator::lpvDS_paramsConfig &config, uint32_t level);

};


#endif
