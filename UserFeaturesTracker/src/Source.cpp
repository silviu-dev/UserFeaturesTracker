#include "UserFeaturesTracker.hpp"
#include <iostream>
void userPositionCallback(int x, int y)
{

}
void userGazeCallback(int x, int y)
{
	std::cout<<x<<"X"<<y<<std::endl;
}
void userMouthCallback(double x)
{

}
void userBlinkCallback(bool x, bool y)
{

}
int main()
{
	getUserFeatures(userPositionCallback, userGazeCallback, userMouthCallback, userBlinkCallback, true);
}