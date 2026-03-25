//Borders.h
#pragma once
#include "MISC.h"
#include <vector>
#include <string>
#include <fstream>
#include <utility>
#include <array>
#include "Functions.h"

enum BoundryCond
{
   DIRIH,
	ROBIN,
   NEUMAN,
	NO,
	OUTLINE
};
template <Field T>
struct Node
{
	Vector2<T> cords;
	size_t n;
	size_t nBound;
	BoundryCond bound;
	Vector2<T> h;
	Node(T x, T y, size_t _n,T hX,T hY,size_t _nBound, BoundryCond _bound) : cords(Vector2<T>(x, y)), n(_n),h(Vector2<T> (hX,hY)),nBound(_nBound), bound(_bound) {};
	Node() = default;
};


template <typename T>
class BaseBoundry
{
public:
	T x1, x2, y1, y2;
   
   BaseBoundry(T x1, T x2, T y1,T y2) : x1(x1), x2(x2),y1(y1),y2(y2) {};
   virtual BoundryCond getType() = 0;
   virtual void apply(std::vector<Node<T>>::iterator node, Fun<T> &f, std::vector<std::vector<T> *> &D, std::array<int, 5> &I, std::vector<T> &F) = 0;
};

template <typename T>
class DirihBoundry : public BaseBoundry<T>
{
   Fun<T> *cons;
public:
   DirihBoundry(T x1, T x2, T y1, T y2, Fun<T> *f) : BaseBoundry<T>(x1,x2,y1,y2), cons(f) {};

   BoundryCond getType() override
   {
      return BoundryCond::DIRIH;
   }

   void apply(std::vector<Node<T>>::iterator node, Fun<T> &f, std::vector<std::vector<T> *> &D, std::array<int, 5> &I, std::vector<T> &F) override
   {
		T val = cons->evaluate(node->cords);
		F[node->n] = val;
		
		for (int j = 0; j < 2; j++)
		{
			int buff = node->n - I[j];
			if (buff > 0 && buff < D[0]->size())
			{
				if ((*D[j])[buff] != 0)
				{
					F[buff] = F[buff] - (*D[j])[buff] * val;
					(*D[j])[buff] = 0;

				}
			}
		}
		for (int j = 3; j < 5; j++)
		{
			int buff = node->n - I[j];
			if (buff > 0 && buff < D[0]->size())
			{
				if ((*D[j])[buff] != 0)
				{
					F[buff] = F[buff] - (*D[j])[buff] * val;
					(*D[j])[buff] = 0;
				}
			}
		}
		
   }

};

template <typename T>
class NeumannBoundry : public BaseBoundry<T>
{
	using BaseBoundry<T>::x1;
	using BaseBoundry<T>::x2;
	using BaseBoundry<T>::y1;
	using BaseBoundry<T>::y2;
	Fun<T> *g;
	T lambda;
	Vector2<T> direction;
public:
	NeumannBoundry(T x1, T x2, T y1, T y2,T lambda,T x,T y, Fun<T> *f) : BaseBoundry<T>(x1, x2, y1, y2), lambda(lambda), direction(Vector2<T>(x,y)), g(f) {};

	BoundryCond getType() override
	{
		return BoundryCond::NEUMAN;
	}

	void apply(std::vector<Node<T>>::iterator node, Fun<T> &f, std::vector<std::vector<T> *> &D, std::array<int, 5> &I, std::vector<T> &F) override
	{
		size_t i = node->n;
		T hx2 = node->h.x * node->h.x;
		T hy2 = node->h.y * node->h.y;
		double h = 0;
		if (direction.y == 0)
		{
			if (direction.x < 0)
			{
				h = abs(node->cords.x - (node + I[3])->cords.x);
				(*D[0])[i] = -2/ (node->h.y + (node + I[1])->h.y) / (node + I[1])->h.y;
				(*D[1])[i] = 0;
				(*D[2])[i] = 1 / (h * (h + node->h.x)) + 2 / (node->h.y * (node + I[1])->h.y);
				(*D[3])[i] = -1 / hx2;
				(*D[4])[i] = -2 / (node->h.y + (node + I[1])->h.y) / node->h.y;
				F[i] = f.evaluate(node->cords) - g->evaluate(node->cords) / (lambda * h * node->h.x);
			}
			else
			{
				h = abs(node->cords.x - (node + I[1])->cords.x);
				(*D[0])[i] = -2 / (node->h.y + (node + I[1])->h.y) / (node + I[1])->h.y;
				(*D[1])[i] = -1 / hx2;
				(*D[2])[i] = 2 / (h * (h + node->h.x)) + 2 / (node->h.y * (node + I[1])->h.y);;
				(*D[3])[i] = 0;
				(*D[4])[i] = -2 / (node->h.y + (node + I[1])->h.y) / node->h.y;
				F[i] = f.evaluate(node->cords) + g->evaluate(node->cords) / (lambda * h * node->h.x);
			}
			
		}
		else
		{
			if (direction.y < 0)
			{
				h = abs(node->cords.y - (node + I[4])->cords.y);
				(*D[0])[i] = 0;
				(*D[1])[i] = -2 / (node->h.x + (node + I[1])->h.x) / (node + I[1])->h.x;
				(*D[2])[i] = 2 / (h * (h + node->h.y)) + 2 / (node->h.x * (node + I[1])->h.x);
				(*D[3])[i] = -2 / (node->h.x + (node + I[1])->h.x) / node->h.x;
				(*D[4])[i] = -1 / hy2;
				F[i] = f.evaluate(node->cords) - g->evaluate(node->cords) / (lambda * h * node->h.y);
			}
			else
			{
				
				h = abs(node->cords.y - (node + I[0])->cords.y);
				(*D[0])[i] = -2 / (h * (h + node->h.y));
				(*D[1])[i] = -2 / (node->h.x + (node + I[1])->h.x) / (node + I[1])->h.x;
				(*D[2])[i] = 2 / (h*(h+node->h.y)) + 2 / (node->h.x * (node + I[1])->h.x);
				(*D[3])[i] = -2 / (node->h.x + (node + I[1])->h.x) / node->h.x;
				(*D[4])[i] = 0;
				F[i] = f.evaluate(node->cords) + g->evaluate(node->cords) / (lambda*h*node->h.y);
				
			}

		}
	}

};


template <typename T>
class RobinBoundry : public BaseBoundry<T>
{
	Fun<T> *ub;
	T lambda;
	T beta;
	Vector2<T> direction;
public:
	RobinBoundry(T x1, T x2, T y1, T y2, T lambda,T beta, T x, T y, Fun<T> *f) : BaseBoundry<T>(x1, x2, y1, y2), lambda(lambda),beta(beta), direction(Vector2<T>(x, y)), ub(f) {};

	BoundryCond getType() override
	{
		return BoundryCond::ROBIN;
	}

	void apply(std::vector<Node<T>>::iterator node, Fun<T> &f, std::vector<std::vector<T> *> &D, std::array<int, 5> &I, std::vector<T> &F) override
	{
		T buff = beta / lambda;
		size_t i = node->n;
		double h = 0;
		if (direction.y == 0)
		{
			if (direction.x < 0)
			{
				h = abs(node->cords.x - (node + I[3])->cords.x);
				(*D[0])[i] = 0;
				(*D[1])[i] = 0;
				(*D[2])[i] = 1 / h + buff;
				(*D[3])[i] = -1 / h;
				(*D[4])[i] = 0;
				F[i] = buff * ub->evaluate(node->cords);
			}
			else
			{
				h = abs(node->cords.x - (node + I[1])->cords.x);
				(*D[0])[i] = 0;
				(*D[1])[i] = -1 / h;
				(*D[2])[i] = 1 / h + buff;
				(*D[3])[i] = 0;
				(*D[4])[i] = 0;
				F[i] = buff * ub->evaluate(node->cords);
			}

		}
		else
		{
			if (direction.y < 0)
			{
				h = abs(node->cords.y - (node + I[4])->cords.y);
				(*D[0])[i] = 0;
				(*D[1])[i] = 0;
				(*D[2])[i] = 1 / h + buff;
				(*D[3])[i] = 0;
				(*D[4])[i] = -1 / h;
				F[i] = buff * ub->evaluate(node->cords);
			}
			else
			{
				h = abs(node->cords.y - (node + I[0])->cords.y);
				(*D[0])[i] = -1 / h;
				(*D[1])[i] = 0;
				(*D[2])[i] = 1 / h + buff;
				(*D[3])[i] = 0;
				(*D[4])[i] = 0;
				F[i] = buff * ub->evaluate(node->cords);
			}
		}
	}

};

template<Field T>
struct Border
{
	T x1, x2, y1, y2;
	size_t n;
	Border(T x1, T x2, T y1, T y2,size_t n) : x1(x1),x2(x2),y1(y1),y2(y2),n(n) {};
	Border() = default;
	bool onBorder(const Vector2<T> &node) const
	{
		if(x1 == x2)
		{
			if(node.x == x1 && y1 <= node.y && y2 >= node.y)
			{
				return true;
			}
		}
		else
		{
			if (node.y == y1 && x1 <= node.x && x2 >= node.x)
			{
				return true;
			}
		}
		return false;
	}



};


