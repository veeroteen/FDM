#include <iostream>
#include "MISC.h"
#include "FDM.h"
#include <cmath>
int main()
{
	std::map<std::string, double> arr;
	arr["x"] = 10;
	
	auto fun = [](const std::map<std::string, double> &args)
		{
			return 2 * sin(args.find("x")->second + args.find("y")->second);
		};
	StaticFun<double> f(fun);
	std::string conf = "testAprocs2/config.txt";

	FDM<double> FDM(f, conf);
}


