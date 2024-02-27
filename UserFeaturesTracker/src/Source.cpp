#include "UserFeaturesTracker.hpp"
void userPositionCallback(int x, int y)
{

}
void userGazeCallback(int x, int y)
{

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