#include <ros/ros.h>
#include <ros/package.h>
#include <geometry_msgs/PoseStamped.h>
#include <nav_msgs/Odometry.h>
#include <nav_msgs/Path.h>


#include <fstream>
#include <iostream>

using namespace std;

class LocalizationPathExtractor
{
public:
    LocalizationPathExtractor(ros::NodeHandle &nh):
    localization_sub(nh.subscribe("/odometry/filtered",100,&LocalizationPathExtractor::path_CB,this)),
    path_pub(nh.advertise<nav_msgs::Path>("/path",10)),
    loop_rate(20)
    {
        init();
    }

    void init()
    {
        initPath();
    }

    void spin()
    {
        string file_path = ros::package::getPath("localization_test") + "/paths/path.txt"; 
        ofstream fout(file_path);

        while(ros::ok())
        {
            ros::spinOnce();
            path_pub.publish(path);
            if(posestamped.pose.position.x != 0.0 && posestamped.pose.position.y != 0.0)
                fout << posestamped.pose.position.x << " " << posestamped.pose.position.y << "\n";
            loop_rate.sleep();
        }

        fout.close();
    }

    void initPath(void)
    {

        posestamped.header.frame_id = "odom";
        posestamped.pose.position.x = 0;
        posestamped.pose.position.y = 0;
        posestamped.pose.position.z = 0;

        posestamped.pose.orientation.x = 0.0;
        posestamped.pose.orientation.y = 0.0;
        posestamped.pose.orientation.z = 0.0;
        posestamped.pose.orientation.w = 1.0;

        path.header = posestamped.header;
        path.poses.push_back(posestamped);

    }

    void path_CB(const nav_msgs::Odometry &odom_filtered)
    {

        posestamped.header = odom_filtered.header;
        posestamped.pose.position.x = odom_filtered.pose.pose.position.x;
        posestamped.pose.position.y = odom_filtered.pose.pose.position.y;
        posestamped.pose.position.z = odom_filtered.pose.pose.position.z;

        posestamped.pose.orientation.x = odom_filtered.pose.pose.orientation.x;
        posestamped.pose.orientation.y = odom_filtered.pose.pose.orientation.y;
        posestamped.pose.orientation.z = odom_filtered.pose.pose.orientation.z;
        posestamped.pose.orientation.w = odom_filtered.pose.pose.orientation.w;

        path.header = posestamped.header;
        path.poses.push_back(posestamped);

    }

    

private:
    /*Subscriber */
    ros::Subscriber localization_sub;
    
    /*Publisher */
    ros::Publisher path_pub;

    /*Transform Broadcaster */

    /*Parameter*/

    /*ROS Variable*/
    ros::NodeHandle nh;
    ros::Rate loop_rate;  

    /*Message Types*/
    geometry_msgs::PoseStamped posestamped;
    nav_msgs::Path path;
		
};

int main(int argc, char **argv)
{
    ros::init(argc, argv, "localization_path_extractor");
    ros::NodeHandle nh;
    LocalizationPathExtractor core(nh);
    core.spin();
    return 0;
}
