// includes - reading of all files needed
#include <ros/ros.h>
#include <iostream>
#include <stdlib.h>
#include <geometry_msgs/Twist.h>
#include <turtlesim/Pose.h>
#include <math.h>

//typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient; // type definition - everything from left saved as MoveBaseClient

class Nav
{
private:
  float area_length, area_width, goal_x, goal_y, line_width; // The private variables are initialzed
  int state = 1;                    // The state of the turtle is initiated with a value of 1
public:
  Nav();                            // Constructor
  void calc_new_goal();             // Function 
  float get_x() { return goal_x; }  // Getter/accessor 
  float get_y() { return goal_y; }  // Getter/accessor
  float get_state() { return state; } // Getter/accessor
};
/*
Nav::Nav()
{
  ROS_INFO("Insert length of search area in metres");
  std::cin >> area_length;
  ROS_INFO("Insert width of search area in metres");
  std::cin >> area_width;
  ROS_INFO("Insert line width of search area in metres");
  std::cin >> line_width;
}
*/
Nav::Nav()   // The no arg-constructor is set to ask the user for input to set the dimensions of the area, and distance between the lines.
{
    std::cout << "Insert length in meters: ";
    std::cin >> area_length;
    std::cout << "Insert width in meters: ";
    std::cin >> area_width;
    std::cout << "Insert line width in the search area in metres";
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
  switch (state)            // switch function that updates the goal location depending on the state.
  {
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
  if (goal_y > area_width)  // If-statement that checks to see if the switch put the goal outside the area
    goal_x = 0;
    goal_y = 0;
    state = 0;
  }
}

class Turlte 
{
  private:
  double distance_tolerance;
  float lin_speed_multi, ang_vel_multi;
  public:
  Turtle();                                         // Constructor
  void movetoGoal();                                // Function
  double getDistance();                             // Function
  double getDisTol() { return distance_tolerance; } // Getter/accessor 
  float getLSM() { return lin_speed_multi; }        // Getter/accessor
  float getAVM() { return ang_vel_multi; }          // Getter/accessor
}

Turtle::Turtle()
{
    std::cout << "Insert distance tolerance in meters: ";
    std::cin >> distance_tolerance;
    std::cout << "Insert line speed multiplyer: ";
    std::cin >> lin_speed_multi;
    std::cout << "Insert angular velocity multiplyer";
    std::cin >> ang_vel_multi;
}

Turtle::getDistance(double x1, double x2, double y1, double y2){ // Calculates the distance between Turtle and goal
  return sqrt(pow((x2-x1),2)+pow((y2-y1),2));  // Distance 
}

Turtle::movetoGoal(turtlesim::Pose turtlesim_Pose, Turtle::getDisTol()){  // Calls instance "turtlesim_pose" and the getter "Turtle::getDisTol()"
  geometry_msgs::Twist vel_msg; // Creating an instance of the geometry_msgs called "vel_msg"

  do                            // Changes the velocities of the Turtle
  {
    // Linear velocities
    vel_msg.linear.x = Turtle::getLSM()*getDistance(turtlesim_Pose.x, turtlesim_Pose.y, nav.get_x(), nav.get_y()); // Assigns linear velocity to x based on the distance to goal 
    vel_msg.linear.y = 0;
    vel_msg.linear.z = 0;
    // Angular velocities
    vel_msg.angular.x = 0;
    vel_msg.angular.y = 0;
    vel_msg.angular.z = Turtle::getAVM()*(atan2(nav.get_y()-turtlesim_Pose.y, nav.get_x()-turtlesim_Pose.x)-turtle_Pose.theta); // Assigns angular velocity to z based on 
    
    velocity_publisher.publish(vel_msg);

  } while (getDistance(turtlesim_Pose.x, turtlesim_Pose.y, nav.get_x(), nav.get_y())>Turtle::getDisTol());
  // Ending the movement of the Turtle
  vel_msg.linear.x = 0;
  vel_msg.angular.z = 0;
  velocity_publisher.publish(vel_msg);
}

int main(int argc, char **argv)        // Initation of main
{
  ros::init(argc, argv, "navigation"); // initiation ROS
  ros::Nodehandle n;

  velocity_publisher = n.advertise<geometry_msgs::Twist>("/turtle1/cmd_vel", 100);
	pose_subscriber = n.subscribe("/turtle1/pose", 10);

  turtlesim::Pose turtlesim_Pose;      // Creating an instance of turtlesim::Pose called turtlesim_Pose
  Nav nav;                             // Creating instance of Nav class
  Turtle controller;                   // Creating instance of Turtle class

  // Har måske brug for en teleport til et andet sted end i midten

  while (nav.get_state() != 0)         //while loop running while nav.state differs from 0
  {
    nav.calc_new_goal();               // Calls the calc_new_goal function
    controller.movetoGoal();           // Calls the movetoGoal function. This also publishes the velocities. 
  }
  return 0;
}