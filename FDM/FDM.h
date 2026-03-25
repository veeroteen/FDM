//FDM.h
#pragma once
#include "MISC.h"
#include <vector>
#include <string>
#include <fstream>
#include <utility>
#include <array>
#include "DMatrix.h"
#include "Functions.h"
#include "MeshBuilder.h"
#include <iomanip>
template <Field T>
class FDM
{
	std::vector <std::vector<T> *> D;
	std::array<int, 5> I;
	std::vector<T> F;



public:

	FDM(std::string folder)
	{
		folder += "/";
		std::ifstream funs(folder + "fun.txt");
		std::string fun = "";
		std::getline(funs, fun);
		funs.close();
		StringFun<T> f(fun);
		std::vector<BaseBoundry<T> *> *boundries = new std::vector<BaseBoundry<T>*>();
		std::vector<Node<T>> *nodes = new std::vector<Node<T>>;
		std::pair<int, int> size;
		T lambda = 1, gamma = 0;
		MeshBuilder<T>(folder, boundries, nodes, size, lambda, gamma);

		size_t count = size.first * size.second;
		D.resize(5);
		F.resize(count);
		I = { -size.first,-1,0,1,size.first };

		{
			for (size_t i = 0; i < I.size(); i++)
			{
				D[i] = new std::vector<T>(count, I[i] == 0 ? 1 : 0);
			}
		}
		std::vector<size_t> boundNodes;
		auto &nod = *nodes;
		for (size_t i = 0; i < nod.size(); i++)
		{
			if (nod[i].bound == BoundryCond::OUTLINE)
			{
				continue;
			}
			else if (nod[i].bound != BoundryCond::NO)
			{
				boundNodes.push_back(i);
			}
			else
			{
				F[i] = f.evaluate(nod[i].cords) / lambda;
				(*D[0])[i] = -2 / (nod[i - size.first].h.y * (nod[i].h.y + nod[i - size.first].h.y));
				(*D[1])[i] = -2 / (nod[i - 1].h.x * (nod[i].h.x + nod[i - 1].h.x));
				(*D[2])[i] = 2 / (nod[i].h.x * nod[i - 1].h.x) + 2 / (nod[i].h.y * nod[i - size.first].h.y) + gamma / lambda;
				(*D[3])[i] = -2 / (nod[i].h.x * (nod[i].h.x + nod[i - 1].h.x));
				(*D[4])[i] = -2 / (nod[i].h.y * (nod[i].h.y + nod[i - size.first].h.y));
			}
		}
		auto &bou = *boundries;
		for (size_t i = 0; i < boundNodes.size(); i++)
		{
			bou[nod[boundNodes[i]].nBound]->apply(nod.begin() + boundNodes[i], f, D, I, F);
		}

		ZJMatrix<T> solver(D, I, F);

		auto x = solver.ZegelJac(20000, solver.optimalW(true), 1E-16, true);
		std::ofstream res(folder + "res.csv");
		std::string mat = folder + "matrix.txt";
		//solver.printA(mat);
		for (size_t i = 0; i < nod.size(); i++)
		{
			if (nod[i].bound == BoundryCond::OUTLINE)
			{
				continue;
			}
			else
			{
				res << std::setprecision(16) << nod[i].cords.x << "\t" << nod[i].cords.y << "\t" << (*x)[i] << std::endl;
			}

		}

		res.close();


		return;



	}

};