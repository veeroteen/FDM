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
template <Field T>
class FDM
{
	std::vector <std::vector<T> *> D;
	std::array<int, 5> I;
	std::vector<T> F;



public:

	FDM(Fun<T> &f, std::string &cfg)
	{
		std::vector<BaseBoundry<T>*> *boundries = new std::vector<BaseBoundry<T>*>();
		std::vector<Node<T>> *nodes = new std::vector<Node<T>>;
		std::pair<int, int> size;

		MeshBuilder<T>(cfg, boundries, nodes, size);

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
		for(size_t i = 0; i < nod.size();i++)
		{
			if(nod[i].bound != BoundryCond::NO)
			{
				boundNodes.push_back(i);
			}
			else
			{
				F[i] = f.evaluate(nod[i].cords);
				(*D[0])[i] = -2 / (nod[i - size.first].h.y * (nod[i].h.y + nod[i - size.first].h.y));
				(*D[1])[i] = -2 / (nod[i - 1].h.x * (nod[i].h.x + nod[i - 1].h.x));
				(*D[2])[i] = 2 / (nod[i].h.x * nod[i - 1].h.x) + 2 / (nod[i].h.y * nod[i - size.first].h.y);
				(*D[3])[i] = -2 / (nod[i].h.x * (nod[i].h.x + nod[i - 1].h.x));
				(*D[4])[i] = -2 / (nod[i].h.y * (nod[i].h.y + nod[i - size.first].h.y));
			}
		}
		auto &bou = *boundries;
		for(size_t i = 0; i < boundNodes.size(); i++)
		{
			bou[nod[boundNodes[i]].nBound]->apply(nod[boundNodes[i]],f,D,I,F);
		}

		ZJMatrix<T> solver(D, I, F);
		solver.printA("mat1.txt");
		solver.ZegelJac(10000, solver.optimalW(false), 1E-16,false);


		return;



	}

};