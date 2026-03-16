#include <iostream>
#include "MISC.h"
#include "FDM.h"
int main()
{
	std::map<std::string, double> arr;
	arr["x"] = 10;
	
	auto fun = [](const std::map<std::string, double> &args)
		{
			return 0;// 2 * (args.find("x")->second + args.find("y")->second);
		};
	StaticFun<double> f(fun);
	std::string conf = "testF1/config.txt";

	FDM<double> FDM(f, conf);
}


