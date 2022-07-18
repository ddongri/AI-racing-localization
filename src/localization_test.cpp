#include <ros/ros.h>
#include <ros/time.h>
#include <geometry_msgs/Vector3.h>
#include <geometry_msgs/Twist.h>
#include <sensor_msgs/NavSatFix.h>
#include <sensor_msgs/Imu.h>
#include <nav_msgs/Odometry.h>
#include <tf/tf.h>
#include <tf2_ros/static_transform_broadcaster.h>
#include <tf2_ros/transform_broadcaster.h>
#include <robot_localization/navsat_conversions.h>

using namespace std;

class localizationTest
{
public:
    localizationTest(ros::NodeHandle &nh):
    navsat_sub(nh.subscribe("/ublox_gps/fix",100,&localizationTest::fix_CB,this)),
    imu_sub(nh.subscribe("/mavros/imu/data",100,&localizationTest::imu_CB,this)),
    map2gps_pub(nh.advertise<nav_msgs::Odometry>("/map2gps_pose",10)),
    loop_rate(20)
    {
        init();
    }
    void init()
    {
        initMap2GPS();
    }

    void spin()
    {
        while(ros::ok())
        {
            ros::spinOnce();
            map2gps_pub.publish(map2gps);
            loop_rate.sleep();
        }
    }

    void initMap2GPS(void)
    {
       
        map2gps.header.frame_id = "odom";
        map2gps.child_frame_id = "base_footprint";
        map2gps.pose.pose.position.x = 0.0;
        map2gps.pose.pose.position.y = 0.0;
        map2gps.pose.pose.position.z = 0.0;

        map2gps.pose.pose.orientation.x = 0.0;
        map2gps.pose.pose.orientation.y = 0.0;
        map2gps.pose.pose.orientation.z = 0.0;
        map2gps.pose.pose.orientation.w = 1.0;

        map2gps.twist.twist.linear.x = 0.0;
        map2gps.twist.twist.angular.z = 0.0;

    }




    void fix_CB(const sensor_msgs::NavSatFix &navsat_msg)
    {
        double utm_x = 0, utm_y = 0;
        std::string utm_zone;
        //convert lat/long to utm
        RobotLocalization::NavsatConversions::LLtoUTM(navsat_msg.latitude, navsat_msg.longitude, utm_x, utm_y, utm_zone);

        if(!navSat_msg_received){
            if(gps_origin_x_ == 0 && gps_origin_y_ == 0)
            {
            gps_origin_x_ = utm_x;
            gps_origin_y_ = utm_y;
            }
            odom_origin_x_ = utm_x;
            odom_origin_y_ = utm_y;
            navSat_msg_received = true;
        }

        map2gps.header.stamp = ros::Time::now();
        map2gps.pose.pose.position.x = utm_x - odom_origin_x_;
        map2gps.pose.pose.position.y = -(utm_y - odom_origin_y_);
        map2gps.pose.pose.position.z = 0;
    }

    void imu_CB(const sensor_msgs::Imu &imu_msg)
    {
      map2gps.pose.pose.orientation.x = imu_msg.orientation.x;
      map2gps.pose.pose.orientation.y = imu_msg.orientation.y;
      map2gps.pose.pose.orientation.z = imu_msg.orientation.z;
      map2gps.pose.pose.orientation.w = imu_msg.orientation.w;
        // tf::Quaternion q(
        // imu_msg.orientation.x,
        // imu_msg.orientation.y,
        // imu_msg.orientation.z,
        // imu_msg.orientation.w);
        // tf::Matrix3x3 m(q);
        // double roll, pitch, yaw;
        // m.getRPY(roll, pitch, yaw);
        // yaw += M_PI/2;
        // map2gps.pose.pose.orientation = tf::createQuaternionMsgFromRollPitchYaw(roll,pitch,yaw);
    }

private:
    /*Subscriber */
    ros::Subscriber navsat_sub;
    ros::Subscriber imu_sub;
    
    /*Publisher */
    ros::Publisher odom_pub;
    ros::Publisher input_pub;
    ros::Publisher map2gps_pub;

    /*Transform Broadcaster */

    /*Parameter*/
    bool navSat_msg_received = false;
    double gps_origin_x_ = 0;
    double gps_origin_y_ = 0;
    double odom_origin_x_ = 0;
    double odom_origin_y_ = 0;


    /*ROS Variable*/
    ros::NodeHandle nh;
    ros::Rate loop_rate;  

    /*Message Types*/
    nav_msgs::Odometry map2gps;
		
};

int main(int argc, char **argv)
{
    ros::init(argc, argv, "localization_test");
    ros::NodeHandle nh;
    localizationTest core(nh);
    core.spin();
    return 0;
}
