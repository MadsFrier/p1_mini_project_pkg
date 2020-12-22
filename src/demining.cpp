// includes - reading of all files needed
#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <iostream>
#include <stdlib.h>
#include <nav_msgs/Odometry.h>

typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient; // type definition - everything from left saved as MoveBaseClient

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

int main(int argc, char **argv) // initation of main
{
  ros::init(argc, argv, "demining_2"); // initiation ROS

  MoveBaseClient ac("move_base", true); // using the previous typedef to initiate client from actionlib.h as "ac"

  while (!ac.waitForServer(ros::Duration(5.0))) // connecting ac to server with 5 second buffer
  {
    ROS_INFO("Waiting for the move_base action server"); // printing statement to terminal
  }

  move_base_msgs::MoveBaseGoal goal; // creating instance of MoveBaseGoal named "goal"

  goal.target_pose.header.frame_id = "odom";        // initiating goal frame_id as "odom"
  goal.target_pose.header.stamp = ros::Time::now(); // initiation stamp as ros::Time::now()

  Nav nav; // creating instance of Nav class

  while (nav.state() != 0) //while loop running while nav.state differs from 0
  {
    nav.calc_new_goal(); // Calls the calc_new_goal function

    goal.target_pose.pose.position.x = nav.get_x(); // assigning new target_x from nav.goal_x
    goal.target_pose.pose.position.y = nav.get_y(); // assigning new target_y from nav.goal_y
    goal.target_pose.pose.orientation.w = 1.0;      // assign target orientation

    ac.sendGoal(goal); // sends the new goal to the server

    ac.waitForResult(); // wait for server to respond

    while (ac.getState() != actionlib::SimpleClientGoalState::SUCCEEDED) // while loop running while goal is not reached
    {
    }
  }
  return 0;
}