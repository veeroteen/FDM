#pragma once
#include "MISC.h"
#include <vector>
#include <string>
#include <fstream>
#include <utility>
#include <array>
#include "Borders.h"

template <Field T>
size_t rayCastRight(const Vector2<T> &node, const std::vector<Border<T>> &borders);

template <Field T>
void MeshBuilder(const std::string &cfg, std::vector<BaseBoundry<T> *> *boundaries, std::vector<Node<T>> *nodes,std::pair<int,int> &size)
{
	std::ifstream config(cfg);
	std::string seg;
	std::getline(config, seg);
	std::vector<Segment<T>> segX;
	std::vector<Segment<T>> segY;
	{
		std::ifstream segments(seg);
		size_t n = 0;
		segments >> n;
		segX.resize(n);
		for(size_t i = 0; i < n; i++)
		{
			T a, b, q;
			size_t n;
			segments >> a >> b >> q >> n;
			segX[i] = Segment(a, b, q, n);
		}
		segments >> n;
		segY.resize(n);
		for (size_t i = 0; i < n; i++)
		{
			T a, b, q;
			size_t n;
			segments >> a >> b >> q >> n;
			segY[i] = Segment(a, b, q, n);
		}
	}
	{
		int rowLenght = 0;
		for (size_t i = 0; i < segX.size(); i++)
		{
			rowLenght += segX[i].n;
		}
		rowLenght -= (segX.size() - 1);
		int columtLength = 0;
		for (size_t i = 0; i < segY.size(); i++)
		{
			columtLength += segY[i].n;
		}
		columtLength -= (segY.size() - 1);

		size.first = rowLenght;
		size.second = columtLength;
	}
	std::vector<Border<T>> vertical, horizontal;

	{
		std::string drh;
		std::getline(config, drh);
		std::ifstream dirih(drh);
		T x1, x2, y1, y2;
		size_t n;
		dirih >> n;
			
		for(size_t i = 0; i < n; i++)
		{
			dirih >> x1 >> x2 >> y1 >> y2;
			
			std::getline(dirih, drh);
			StringFun<T> *fun = new StringFun<T>(drh);
			if (x1 == x2)
			{
				vertical.emplace_back(x1, x2, y1, y2, boundaries->size());
			}
			else
			{
				horizontal.emplace_back(x1, x2, y1, y2, boundaries->size());
			}
			boundaries->emplace_back(new DirihBoundry<T>(x1,x2, y1, y2, fun));
		}
	}

	{
		std::string neu;
		std::getline(config, neu);
		std::ifstream neumann(neu);
		T x1, x2, y1, y2, lambda,x,y;
		size_t n = 0;
		neumann >> n;

		for (size_t i = 0; i < n; i++)
		{
			neumann >> x1 >> x2 >> y1 >> y2 >> lambda >> x >> y;
			std::getline(neumann, neu);
			StringFun<T> *fun = new StringFun<T>(neu);
			if (x1 == x2)
			{
				vertical.emplace_back(x1, x2, y1, y2, boundaries->size());
			}
			else
			{
				horizontal.emplace_back(x1, x2, y1, y2, boundaries->size());
			}
			boundaries->emplace_back(new NeumannBoundry<T>(x1,x2, y1, y2, lambda,x,y, fun));
		}
	}

	{
		std::string rob;
		std::getline(config, rob);
		std::ifstream robin(rob);
		T x1, x2, y1, y2, lambda,beta,x,y;
		size_t n = 0;
		robin >> n;

		for (size_t i = 0; i < n; i++)
		{
			robin >> x1 >> x2 >> y1 >> y2 >> lambda >> beta >> x >> y;
			std::getline(robin, rob);
			StringFun<T> *fun = new StringFun<T>(rob);
			if (x1 == x2)
			{
				vertical.emplace_back(x1,x2, y1, y2, boundaries->size());
			}
			else
			{
				horizontal.emplace_back(x1,x2,y1,y2, boundaries->size());
			}
			boundaries->emplace_back(new RobinBoundry<T>(x1,x2, y1, y2, lambda, beta,x,y, fun));
		}
	}

	{
		Vector2<T> cords = Vector2<T>(0, 0);

		size_t counter = 0;
		for (size_t i = 0; i < segY.size(); i++)
		{
			cords.y = segY[i].a;
			T hY = segY[i].h0;
			for (size_t y = 0; y < segY[i].n - (i == segY.size() - 1 ? 0 : 1); y++)
			{
				for (size_t j = 0; j < segX.size(); j++)
				{
					T hX = segX[j].h0;
					cords.x = segX[j].a;
					for (size_t x = 0; x < segX[j].n - (j == segX.size()-1 ? 0 : 1); x++)
					{
						if(rayCastRight(cords,vertical)%2 == 1)
						{
							size_t nb = 0;
							BoundryCond bc = BoundryCond::NO;
							for(auto &a : horizontal)
							{
								if(a.onBorder(cords))
								{
									auto temp = (*boundaries)[a.n]->getType();
										
									if(temp < bc)
									{
										bc = temp;
										nb = a.n;
									}
								}
							}
							for (auto &a : vertical)
							{
								if (a.onBorder(cords))
								{
									auto temp = (*boundaries)[a.n]->getType();

									if (temp < bc)
									{
										bc = temp;
										nb = a.n;
									}
								}
							}
							nodes->emplace_back(cords.x, cords.y, counter, hX, hY,nb, bc);
						}

						cords.x += hX;
						hX *= segX[j].q;
						counter++;
					}
						
				}
				cords.x = segX[0].a;
				cords.y += hY;
				hY *= segY[i].q;
			}

		}
	}

}


template <Field T>
size_t rayCastRight(const Vector2<T> &node,const std::vector<Border<T>> &borders)
{
	size_t counter = 0;
	for(size_t i = 0; i < borders.size();i++)
	{
		if(node.y >= borders[i].y1 && node.y <= borders[i].y2 && node.x >= borders[i].x1)
		{
			if (borders[i].onBorder(node))
			{
				return 1;
			}
			counter++;
		}
	}
	return counter;


}