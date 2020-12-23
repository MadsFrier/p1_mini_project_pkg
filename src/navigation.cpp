// includes - reading of all files needed
#include <ros/ros.h>
#include <iostream>
#include <stdlib.h>
#include <geometry_msgs/Twist.h>
#include <turtlesim/Pose.h>
#include <math.h>

class Nav
{
private:
  float area_length, area_width, goal_x, goal_y, line_width; // The private variables are initialzed
  int state = 1;                      // The state of the turtle is initiated with a value of 1
public:
  Nav();                              // Constructor
  void calc_new_goal();               // Function 
  float get_x() { return goal_x; }    // Getter/accessor 
  float get_y() { return goal_y; }    // Getter/accessor
  float get_state() { return state; } // Getter/accessor
};

Nav::Nav()   // The no arg-constructor is set to ask the user for input to set the dimensions of the area, and distance between the lines.
{
  std::cout << "Insert length in meters: ";
  std::cin >> area_length;
  std::cout << "Insert width in meters: ";
  std::cin >> area_width;
  std::cout << "Insert line width in the search area in metres: ";
  std::cin >> line_width;
  /*
    Man kunne skrive 
    goal_x = 0;
    goal_y = 0;
    Men det ligger lidt implicit i constructoren, selv hvis man ikke skriver det. 
    */
}

void Nav::calc_new_goal()   // void function that updates the current location, the new goal and checks if goal is within area size.
{
  float current_x = goal_x; // current_x stores the x-location of the robot. This is updated each time the calc_new_goal is called.
  float current_y = goal_y; // current_y stores the y-location of the robot. This is updated each time the calc_new_goal is called.
  switch (state){           // switch function that updates the goal location depending on the state.
    case 1:
    {
      goal_x = area_length;
      goal_y = current_y;
      state++;
    }
    break;
    case 2:
    {
      goal_x = current_x;
      goal_y = current_y + line_width;
      state++;
    }
    break;
    case 3:
    {
      goal_x = current_x - area_length;
      goal_y = current_y;
      state++;
    }
    break;
    case 4:
    {
      goal_x = current_x;
      goal_y = current_y + line_width;
      state = 1;
    }
    default:
    {
      std::cout << "You messed up boy, check you states bitch";
    }
    break;
  }
  if (goal_y > area_width){                                                   // If-statement that checks to see if the switch put the goal outside the area
    goal_x = 0;
    goal_y = 0;
    state = 0;
  }
}

class Turtle 
{
  private:
  float distance_tolerance, lin_speed_multi, ang_vel_multi, inter_pub;
  public:
  ros::NodeHandle n;
  ros::Publisher velocity_publisher = n.advertise<geometry_msgs::Twist>("/turtle1/cmd_vel", 100);
  Turtle();                                                                                                        
  void movetoGoal(turtlesim::Pose turtlesim_Pose, float x, float y);    
  float getDistance(float x1, float x2, float y1, float y2);                   
  void  printPose(const turtlesim::Pose::ConstPtr& message);                 
};

Turtle::Turtle()                                                               
  {
    std::cout << "Insert distance tolerance in meters: ";
    std::cin >> distance_tolerance;
    std::cout << "Insert line speed multiplyer: ";
    std::cin >> lin_speed_multi;
    std::cout << "Insert angular velocity multiplyer: ";
    std::cin >> ang_vel_multi;
  }

float Turtle::getDistance(float x1, float x2, float y1, float y2){              
  return sqrt(pow((x2-x1),2)+pow((y2-y1),2));
}

void Turtle::movetoGoal(turtlesim::Pose turtlesim_Pose, float x, float y){      
  geometry_msgs::Twist vel_msg;                                                 

  do                                                                            
  {
    // Linear velocities
    vel_msg.linear.x = lin_speed_multi*getDistance(turtlesim_Pose.x, x, turtlesim_Pose.y, y); // Assigns linear velocity to x based on the distance to goal 
    vel_msg.linear.y = 0;
    vel_msg.linear.z = 0;
    // Angular velocities
    vel_msg.angular.x = 0;
    vel_msg.angular.y = 0;
    vel_msg.angular.z = ang_vel_multi*(atan2(y-turtlesim_Pose.y, x-turtlesim_Pose.x)-turtlesim_Pose.theta);  // Assigns angular velocity to z based on 
    
    velocity_publisher.publish(vel_msg);

  } while (getDistance(turtlesim_Pose.x, x, turtlesim_Pose.y, y)>distance_tolerance);
  // Ending the movement of the Turtle

    vel_msg.linear.y = 0;
    vel_msg.linear.x = 0;
    vel_msg.angular.z = 0;
    
    velocity_publisher.publish(vel_msg);
}
void Turtle::printPose(const turtlesim::Pose::ConstPtr& message)
{
  std::cout << "turtle at[" << message ->x << ", " << message->y <<"]" << std::endl;
}


int main(int argc, char **argv)        // Initation of main  Tjek det her @christianhjorth 
{
  std::cout << "We launched boyy";
  ros::init(argc, argv, "navigation"); // initiation ROS
  ROS_INFO("ROS initiation succeded");


  turtlesim::Pose turtlesim_Pose;
  ROS_INFO("Created instance for turtlesim Pose");

  Nav nav; 
  ROS_INFO("Created instance for navigation");

  Turtle controller; 
  ROS_INFO("Created instance for navigational controller"); 

  ros::spinOnce(); 
  /*
  ROS_INFO("Moving to starting location");
  controller.movetoGoal(turtlesim_Pose, 1.0, 1.0);
  ROS_INFO("Initial movement succeded");
  */
  while (nav.get_state() != 0)
  {
    ROS_INFO("LOOOOOP");
    nav.calc_new_goal();               
    ROS_INFO("Calculated new goal");
    ros::Duration(sleep(5));
    controller.movetoGoal(turtlesim_Pose, nav.get_x(), nav.get_y());
    ROS_INFO("Moved to goal"); 

  }
  return 0;
}
