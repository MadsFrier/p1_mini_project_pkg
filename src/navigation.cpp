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
  double area_length, area_width, goal_x, goal_y, line_width, startx, starty; // The private variables are initialzed
  int state = 1;                      // The state of the turtle is initiated with a value of 1
public:
  Nav(double x, double y);                              // Constructor
  void calc_new_goal(double x, double y);               // Function 
  double get_x() { return goal_x; }    // Getter/accessor 
  double get_y() { return goal_y; }    // Getter/accessor
  double get_state() { return state; } // Getter/accessor
};

Nav::Nav(double x, double y)   // The no arg-constructor is set to ask the user for input to set the dimensions of the area, and distance between the lines.
{
  startx = x;
  starty = y;
  std::cout << "Insert length in meters: ";
  std::cin >> area_length;
  std::cout << "Insert width in meters: ";
  std::cin >> area_width;
  std::cout << "Insert line width in the search area in metres: ";
  std::cin >> line_width;
  
}

void Nav::calc_new_goal(double x, double y)   // void function that updates the current location, the new goal and checks if goal is within area size.
{
  double current_x = x; // current_x stores the x-location of the robot. This is updated each time the calc_new_goal is called.
  double current_y = y; // current_y stores the y-location of the robot. This is updated each time the calc_new_goal is called.
  switch (state){           // switch function that updates the goal location depending on the state.
    case 1:
    {
      goal_x = startx + area_length;
      goal_y = current_y;
      state++;
    }
    break;
    case 2:
    {
      goal_x = startx + area_length;
      goal_y = current_y + line_width;
      state++;
    }
    break;
    case 3:
    {
      goal_x = startx;
      goal_y = current_y;
      state++;
    }
    break;
    case 4:
    {
      goal_x = startx;
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
  double distance_tolerance, lin_speed_multi, ang_vel_multi, tPosex, tPosey, tPosetheta;
  public:
  ros::NodeHandle n;
  ros::Publisher velocity_publisher = n.advertise<geometry_msgs::Twist>("/turtle1/cmd_vel", 100);
  ros::Subscriber pose_sub = n.subscribe("/turtle1/pose", 10, &Turtle::poseCallback, this);
  Turtle();                                                                                                        
  void movetoGoal(double goal_x, double goal_y);    
  double getDistance(double x1, double x2, double y1, double y2);   
  void poseCallback(const turtlesim::Pose::ConstPtr& message); 
  double gettPosex() { return tPosex; }
  double gettPosey() { return tPosey; }         
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

double Turtle::getDistance(double x1, double y1, double x2, double y2){              
  return sqrt(pow((x2-x1),2)+pow((y2-y1),2));
}

void Turtle::movetoGoal(double goal_x, double goal_y){      
  geometry_msgs::Twist vel_msg; 
  ros::Rate loop_rate(10);
  while(getDistance(tPosex, tPosey, goal_x, goal_y)>distance_tolerance){
    ROS_INFO_STREAM(goal_x);
    ROS_INFO_STREAM(goal_y);
    // Linear velocities
    vel_msg.linear.x = lin_speed_multi*getDistance(tPosex, tPosey, goal_x, goal_y); 
    vel_msg.linear.y = 0;
    vel_msg.linear.z = 0;
    // Angular velocities
    vel_msg.angular.x = 0;
    vel_msg.angular.y = 0;
    vel_msg.angular.z = ang_vel_multi*(atan2(goal_y-tPosey, goal_x-tPosex)-tPosetheta);
    
    velocity_publisher.publish(vel_msg);

    ros::spinOnce();
    loop_rate.sleep();
  }
  // Ending the movement of the Turtle
    vel_msg.linear.y = 0;
    vel_msg.linear.x = 0;
    vel_msg.angular.z = 0;
    
    velocity_publisher.publish(vel_msg);
}
 void Turtle::poseCallback(const turtlesim::Pose::ConstPtr& message){
   tPosex = message->x;
   tPosey = message->y;
   tPosetheta = message->theta;
  }

int main(int argc, char **argv)        // Initation of main   
{
  ros::init(argc, argv, "navigation"); // initiation ROS
  double startx(1), starty(1);

  Turtle controller; 
  // Movement to initial point
  ros::spinOnce();
  controller.movetoGoal(startx, starty);

  Nav nav(startx, starty);
  while (nav.get_state() != 0)
  {
    ros::spinOnce();
    nav.calc_new_goal(controller.gettPosex(), controller.gettPosey());
    controller.movetoGoal(nav.get_x(), nav.get_y());
  }
  return 0;
}