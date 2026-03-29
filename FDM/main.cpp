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

void findDot(double x,double y,std::string folder)
{
	std::ifstream anal(folder + "/anal.txt");
	std::string funs = "";
	std::getline(anal, funs);
	anal.close();
	StringFun<double> anals(funs);

	std::ifstream data(folder + "/res.csv");
	double _x, _y, un,ua;
	Vector2<double> xy(0, 0);
	while (data >> _x >> _y >> un)
	{
		xy.x = _x;
		xy.y = _y;
		ua = anals.evaluate(xy);
		if (xy.x == x && xy.y == y)
		{
			std::cout << std::setprecision(16) << xy.x << "\t" << xy.y << "\t" << un << "\t" << anals.evaluate(xy) << std::endl;
			break;
		}
	}
	
}





void main()
{
	
	std::string folder = "test2";

	FDM<double> FDM(folder);

	findMaxDes(folder);
	findDot(4.0, 4.0,folder);
}


