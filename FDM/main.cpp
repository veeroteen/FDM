#include <iostream>
#include "MISC.h"
#include "FDM.h"
#include <cmath>

void findMaxDes(std::string folder)
{
	std::ifstream anal(folder + "/anal.txt");
	std::string funs = "";
	std::getline(anal, funs);
	anal.close();
	StringFun<double> anals(funs);

	std::ifstream data(folder + "/res.csv");
	double x, y, un,ua,unt = 0 , max = 0;
	Vector2<double> xy(0, 0),txy(0,0);
	while(data >> x >> y >> un)
	{
		xy.x = x;
		xy.y = y;
		ua = anals.evaluate(xy);
		if(abs(ua - un) > max)
		{
			txy.x = x;
			txy.y = y;
			unt = un;
			max = abs(ua - un);
		}
	}
	std::cout << std::setprecision(16) << txy.x << "\t" << txy.y << "\t" << unt << "\t" << anals.evaluate(txy) << std::endl;

}

void main()
{
	
	std::string folder = "test3";

	FDM<double> FDM(folder);

	findMaxDes(folder);

}


