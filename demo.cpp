#include <cstdio>
#include <cmath>
#include "GPIOlib.h"

using namespace std;
using namespace GPIO;

int readingLeft=0,readingRight=0;

int main()
{
	init();
                turnTo(0);
	controlLeft(FORWARD,10);
	controlRight(FORWARD,10);
	
	while(1)
	{
		resetCounter();
		delay(100);
		getCounter(&readingLeft,&readingRight);
		if(readingLeft==-1||readingRight==-1)
		{
			printf("Error!\n");
			continue;
		}
		//Distance is in mm.
		double distanceLeft=readingLeft*63.4*M_PI/390;
		double distanceRight=readingRight*63.4*M_PI/390;
		printf("Left wheel moved %.2lf cm, right wheel moved %.2lf cm in last second.\n",distanceLeft/10,distanceRight/10);
		if(distanceLeft>distanceRight){
			controlLeft(FORWARD,8);
			controlRight(FORWARD,10);
		}
		else if(distanceLeft<distanceRight){
			controlRight(FORWARD,8);
			controlLeft(FORWARD,10);
		}
		if (distanceLeft >= 1700){
			break;
		}
	}
	stopLeft();
	stopRight();
}
