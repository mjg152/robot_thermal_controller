#include<ros/ros.h> 
#include<std_msgs/Float64.h> 
#include<stdr_msgs/ThermalSensorMeasurementMsg.h> 
#include <geometry_msgs/Twist.h>
#include <nav_msgs/Odometry.h>
#include <math.h> 

float g_thermal_sensor_0_degrees=0; 
float g_thermal_sensor_1_degrees=0; 
float g_thermal_sensor_2_degrees=0; 
float g_thermal_sensor_3_degrees=0; 

float g_x_coordinate=0; 
float g_y_coordinate=0; 

geometry_msgs::Twist base_cmd;

void updateThermalSensor0(const stdr_msgs::ThermalSensorMeasurementMsg& message_holder) 
{ 
  
  //Now let's update our global variable so you can do something with it 
  //In the while loop below. We'll keep things simple and only use the most recent
  //Value
  
  g_thermal_sensor_0_degrees=message_holder.thermal_source_degrees[0];
  
  //First let's send our received value to the cmd line so we can 
  //be aware of what we're reading 
  ROS_INFO("received sensor 0 value is %f", g_thermal_sensor_0_degrees); 
  
} 

void updateThermalSensor1(const stdr_msgs::ThermalSensorMeasurementMsg& message_holder) 
{ 
  //Now let's update our global variable so you can do something with it 
  //In the while loop below. We'll keep things simple and only use the most recent
  //Value
  
  
  g_thermal_sensor_1_degrees=message_holder.thermal_source_degrees[0];
  //First let's send our received value to the cmd line so we can 
  //be aware of what we're reading 
  ROS_INFO("received sensor 1 value is %f",g_thermal_sensor_1_degrees); 
  
} 

void updateThermalSensor2(const stdr_msgs::ThermalSensorMeasurementMsg& message_holder) 
{ 
  //Now let's update our global variable so you can do something with it 
  //In the while loop below. We'll keep things simple and only use the most recent
  //Value
  
  
  g_thermal_sensor_2_degrees=message_holder.thermal_source_degrees[0];
  
  //First let's send our received value to the cmd line so we can 
  //be aware of what we're reading 
  ROS_INFO("received sensor 2 value is %f", g_thermal_sensor_2_degrees); 
  
} 

void updateThermalSensor3(const stdr_msgs::ThermalSensorMeasurementMsg& message_holder) 
{ 
   //Now let's update our global variable so you can do something with it 
  //In the while loop below. We'll keep things simple and only use the most recent
  //Value
  
  g_thermal_sensor_3_degrees=message_holder.thermal_source_degrees[0];
  
  //First let's send our received value to the cmd line so we can 
  //be aware of what we're reading 
  ROS_INFO("received sensor 3 value is %f", g_thermal_sensor_3_degrees); 
  
} 

void updateLocalOdometryInformation(const nav_msgs::Odometry& odom_info) 
{ 
   //Now let's update our global variable so you can do something with it 
  //In the while loop below. 
  
  g_x_coordinate=odom_info.pose.pose.position.x;
  g_y_coordinate=odom_info.pose.pose.position.y;
  
  //First let's send our received value to the cmd line so we can 
  //be aware of what we're reading 
  ROS_INFO("received sensor x coordinate is %f", g_x_coordinate); 
  ROS_INFO("received sensor y coordinate is %f", g_y_coordinate); 
} 


void sendSpeedCommand(float xVel, float zVel)
{
	base_cmd.linear.x = xVel;
	
	base_cmd.angular.z = zVel; 
	
	//negative z turns clockwise
	//positive z turns counter clockwise
	
	//positive x moves right on screen
	//negative x moves left on screen  
		
	ROS_INFO("We made it into the sendSpeedCommand Loop"); 
	
}

bool areWeStuck(float xCoord, float yCoord)
{
	static float previousXCoord = 0; 
	static float previousYCoord = 0; 
	
	float xCoordDifference=0; 
	float yCoordDifference=0; 
	
	xCoordDifference=fabs(xCoord-previousXCoord); 
	yCoordDifference=fabs(yCoord-previousYCoord); 
	
	//now let's update our previous coordinates for the next time this function is called
	previousXCoord=xCoord; 
	previousYCoord=yCoord; 
	
	ROS_INFO("xCoordDifference %f", xCoordDifference); 
	ROS_INFO("yCoordDifference %f", yCoordDifference); 
	
	if (xCoordDifference < 0.001 && yCoordDifference < 0.001)
	{
		ROS_INFO("We're stuck!!"); 
		return true; // we're def stuck :(
		
	}
	
	else 
	{
		ROS_INFO("We're not stuck!!!"); 
		return false; // still movin! 
	}
}

int main(int argc, char **argv) 
{ 
  ROS_INFO("Here we are in the robot thermal controller init code");
  
  ros::init(argc,argv,"robot_controller"); //name this node 
  // when this compiled code is run, ROS will recognize it as a node called "robot_controller" 
  ros::NodeHandle n; // need this to establish communications with our new node 
  //We'll create a series of subscriber objectes for each of the thermal sensors we expect to have
  
  ros::Rate naptime(1); //create a ros object from the ros “Rate” class; 
  // set our while loop to run at 1 Hz 
  ros::Subscriber thermal_sensor_0= n.subscribe("robot1/thermal_sensor_0",1,updateThermalSensor0); 
  ros::Subscriber thermal_sensor_1= n.subscribe("robot1/thermal_sensor_1",1,updateThermalSensor1);
  ros::Subscriber thermal_sensor_2= n.subscribe("robot1/thermal_sensor_2",1,updateThermalSensor2); 
  ros::Subscriber thermal_sensor_3= n.subscribe("robot1/thermal_sensor_4",1,updateThermalSensor3); 
  ros::Subscriber robot_geometry= n.subscribe("robot1/odom",1,updateLocalOdometryInformation); 
  
  //set up the publisher for the cmd_vel topic
  ros::Publisher cmd_vel_pub_ = n.advertise<geometry_msgs::Twist>("/robot1/cmd_vel", 1);
  
  base_cmd.linear.x = base_cmd.linear.y = base_cmd.angular.z = 0; //initialize all vel cmd components
  //to zero 
  
  uint8_t state=0; 
  uint8_t stateCounter=0; 

  while (ros::ok())	
  {
	ros::spinOnce(); //let's spin at the top of our loop so we know we've got some good info to act on
	
	if (areWeStuck(g_x_coordinate, g_y_coordinate))
	{
		switch(state)
		{
			case 0: // turn right at 0 forward speed  
			{
				if (stateCounter < 1) 
				{
					stateCounter++; 
					sendSpeedCommand(0,1.0);
				}
				else 
				{
					stateCounter=0;
					state=1; 
				}
				break; 
			}
			
			case 1: // try to move forward 
			{
				if (stateCounter < 1) 
				{
					stateCounter++; 
					sendSpeedCommand(1.0,0.0);
				}
				else 
				{
					stateCounter=0;
					state=2; 
				}
				break; 
			}
			
			case 2: // backup and turn left 
			{
				if (stateCounter < 4) 
				{
					stateCounter++; 
					sendSpeedCommand(-1.0,-1.0);
				}
				else 
				{
					stateCounter=0;
					state=3; 
				}
				break; 
			}
			
			case 3: // try to move forward 
			{
				if (stateCounter < 1) 
				{
					stateCounter++; 
					sendSpeedCommand(1.0, 0.0);
				}
				else 
				{
					stateCounter=0;
					state=0; 
				}
				break; 
			}
			
			default:
			{
				// do nothing 
			}
			
		}
	}
	
	else 
	{
		sendSpeedCommand(1.0, 0.0); // just move forward for now. 
		//This is where your code could go to determine how the robot finds
		// thermal sensors 
	}
		

	cmd_vel_pub_.publish(base_cmd); // tell the robot how we want it to move! 
	
	
	//we'll rely on our naptime call to control our CPU consumption 
	naptime.sleep(); 
  }
  return 0; //should never make it here 
} 
