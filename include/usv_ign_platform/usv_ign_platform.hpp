/*!*******************************************************************************************
 *  \file       ignition_platform.hpp
 *  \brief      Implementation of an Ignition Gazebo UAV platform
 *  \authors    Miguel Fernández Cortizas
 *              Pedro Arias Pérez
 *              David Pérez Saura
 *              Rafael Pérez Seguí
 *
 *  \copyright  Copyright (c) 2022 Universidad Politécnica de Madrid
 *              All Rights Reserved
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ********************************************************************************/

#ifndef IGNITION_PLATFORM_HPP_
#define IGNITION_PLATFORM_HPP_

#include <memory>
#include <cmath>
#include <string>
#include <iostream>
#include <memory>
#include <rclcpp/logging.hpp>

#include <unordered_map>
#include <Eigen/Dense>
#include <Eigen/src/Core/Matrix.h>
// #include <math.h>
#include <as2_core/core_functions.hpp>
#include <as2_core/aerial_platform.hpp>
#include <as2_core/sensor.hpp>
#include <as2_core/names/topics.hpp>
#include <geometry_msgs/msg/pose_stamped.hpp>
#include <geometry_msgs/msg/twist_stamped.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <tf2_geometry_msgs/tf2_geometry_msgs.h>
#include <image_transport/image_transport.hpp>
#include <std_msgs/msg/float64.hpp>

#include "ignition_bridge.hpp"

#define CMD_FREQ 10  // miliseconds

namespace ignition_platform
{
    using Vector3d = Eigen::Vector3d;

    class USVIgnitionPlatform : public as2::AerialPlatform
    {
    public:
        USVIgnitionPlatform();
        ~USVIgnitionPlatform(){};

    public:
    rclcpp::Subscription<geometry_msgs::msg::TwistStamped>::SharedPtr twist_command_sub_;

    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr left_thrust_pub_;
    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr right_thrust_pub_;

    public:
        void configureSensors() override;
        bool ownSendCommand() override;
        bool ownSetArmingState(bool state) override;
        bool ownSetOffboardControl(bool offboard) override;
        bool ownSetPlatformControlMode(const as2_msgs::msg::ControlMode &msg) override;

        static std::unique_ptr<as2::sensors::Sensor<geometry_msgs::msg::PoseStamped>> pose_ptr_;
        static void poseCallback(const geometry_msgs::msg::PoseStamped &msg);

        static std::unique_ptr<as2::sensors::Sensor<nav_msgs::msg::Odometry>> odometry_raw_estimation_ptr_;
        static void odometryCallback(const nav_msgs::msg::Odometry &msg);

        static std::unordered_map<std::string, as2::sensors::Camera> callbacks_camera_;
        static void cameraCallback(const sensor_msgs::msg::Image &msg, const std::string &sensor_name);
        static void cameraInfoCallback(const sensor_msgs::msg::CameraInfo &msg, const std::string &sensor_name);

        static std::unordered_map<std::string, as2::sensors::Sensor<sensor_msgs::msg::LaserScan>> callbacks_laser_scan_;
        static void laserScanCallback(const sensor_msgs::msg::LaserScan &msg, const std::string &sensor_name);

        static std::unordered_map<std::string, as2::sensors::Sensor<sensor_msgs::msg::PointCloud2>> callbacks_point_cloud_;
        static void pointCloudCallback(const sensor_msgs::msg::PointCloud2 &msg, const std::string &sensor_name);


    private:
        std::shared_ptr<IgnitionBridge> ignition_bridge_;
        static bool odometry_info_received_;
        as2_msgs::msg::ControlMode control_in_;
        static double yaw_;
        double yaw_rate_limit_ = M_PI_2;
        void speedController(const Eigen::Vector3d& vel_flu);
        void PublishUSVCommands(const Eigen::Vector2d &motor_thrust_cmd);

    private:
        void resetCommandTwistMsg();
        Eigen::Vector3d convertENUtoFLU(const float yaw_angle, Eigen::Vector3d &enu_vec);
    };
}

#endif // IGNITION_PLATFORM_HPP_