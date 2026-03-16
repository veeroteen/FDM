//DMatrix.h
#pragma once
#include <vector>
#include <fstream>
#include <string>
#include <iostream>
#include "MISC.h"
#include <iomanip>
#include <limits>


template <Field T>
class ZJMatrix
{
   std::vector <std::vector<T>*> &D;
   std::array <int,5> &I;
   std::vector <T> &F;
   std::vector <T> X;

protected:
   void ZegelJac(std::vector<T>* res, std::vector<T>* cur, T w, std::vector<T>* next = nullptr)
   {
      for (unsigned i = 0; i < cur->size(); i++)
      {
         T buff = 0;
         auto it = I.begin();
         unsigned j = 0;


         for (; *it != 0; it++)
         {
            if (abs(*it) <= i)
            {
               buff += (*D[j])[i] * (*next)[i + *it];

            }
            j++;

         }

         buff += (*D[j])[i] * (*cur)[i];
         j++;
         it++;

         for (; it != I.end(); it++)
         {
            if (F.size() - *it > i)
            {
               buff += (*D[j])[i] * (*cur)[i + *it];

            }
            j++;
;
         }

         (*res)[i] = w * (F[i] - buff) / (*D[D.size() / 2 + (D.size() % 2 ? 0 : -1)])[i] + (*cur)[i];

      }
   }
   T discrepancyRel(std::vector<T>* cur)
   {


      T dx = discrepancy(cur);
      T df = sqrt(scalar(F, F));
      return dx / df;
   }
   T discrepancy(std::vector<T>* cur)
   {
      std::vector<T> Dx(F.size(), 0);

      discrepancyVec(cur, Dx);
      return sqrt(scalar(Dx, Dx));
   }
   void discrepancyVec(std::vector<T>* cur, std::vector<T>& res)
   {
      Ax(*cur,res);

      for (unsigned i = 0; i < F.size(); i++)
      {
         res[i] = F[i] - res[i];
      }
   }
   void Ax(std::vector<T>&  x,std::vector<T>& res)
   {
      for (unsigned i = 0; i < x.size(); i++)
      {
         T buff = 0;
         auto it = I.begin();
         unsigned j = 0;

         for (; it != I.end(); it++)
         {
            if (*it + i < x.size())
            {
               res[i] += (*D[j])[i] * x[i + *it];
            }
            j++;
         }
      }
   }
   T condition(std::vector<T> *cur)
   {
      std::vector <T> buff(cur->size(), 0);
      for(unsigned i = 0; i < buff.size();i++)
      {
         buff[i] = (*cur)[i] - X[i];
      }
    
      T dx = sqrt(scalar(buff, buff));
      T x = sqrt(scalar(X, X));
      return dx / (x * discrepancyRel(cur));
   }

public: 
   ZJMatrix(std::istream &file)
   {
      unsigned n = 0;
      file >> n;

      unsigned m = 0;
      file >> m;
      D.resize(m);

      for (auto& i : I)
      {
         file >> i;
      }

      for (auto &arr : D)
      {
         std::vector<T>* buff = new std::vector<T>(n);
         for (auto &val : *buff)
         {
            file >> val;
         }
         arr = buff;
      }
      F.resize(n);
      X.resize(n);
      for (auto& val : X)
      {
         file >> val;
      }
      Ax(X,F);

   }
   ~ZJMatrix()
   {
      for(auto a : D)
      {
         if(a != nullptr)
         {
            delete a;
         }
      }
      return;
   }
   ZJMatrix(std::vector <std::vector<T> *> &D, std::array <int, 5> &I, std::vector <T> &F) : D(D), I(I),F(F)
   {
      X.resize(F.size());

   }



   std::vector<T>* ZegelJac(unsigned n,T w,T e,bool zeg = true)
   {
      std::vector<T>* res, *cur;
      T discreancy = 0;
      res = new std::vector<T>(F.size(), 0);
      cur = new std::vector<T>(F.size(), 0);
      unsigned i = 0;
      do
      {
         if (zeg)
         {
            ZegelJac(res, res, w, res);
         }
         else
         {
            ZegelJac(res, cur, w,cur);
         }
         auto b = res;
         res = cur;
         cur = b;
         discreancy = discrepancyRel(cur);
         T cond = condition(cur);
         

         i++;
      } while (i < n && discreancy >= e);
      T cond = condition(cur);
      std::cout << w << "   " <<  i << std::endl;
      
      delete res;
      return cur;
   }

   T optimalW(bool zeg = true)
   {
      std::pair<T, T> min(0,std::numeric_limits<T>::infinity());
      T w = 0.01;
      T h = 0.01;
      std::vector<T>* res, * cur;

      unsigned i = 0;
      for(;w <= (zeg ? 2:1);w += h )
      {
         res = new std::vector<T>(F.size(), 0);
         cur = new std::vector<T>(F.size(), 0);
         if (zeg)
         {
            ZegelJac(res, cur, w, res);
         }
         else
         {
            ZegelJac(res, cur, w,cur);
         }

         T dis = discrepancy(res);
         if(min.second > dis)
         {
            min.first = w;
            min.second = dis;
         }
        
         auto b = res;
         res = cur;
         cur = b;
      }
      return min.first;
   }

   void printA(std::string path)
   {
      std::ofstream file(path);
      int N = D[0]->size();
      for (int i = 0; i < N; i++) {
         int m = 0;

         for (int j = 0; j < D.size(); j++) {
            int col = i + I[j];
            if (col < 0 || col >= N) continue;

            while (m < col) {
               file << 0 << "\t";
               m++;
            }

            file << (*D[j])[i] << "\t";
            m++;
         }

         while (m < N) {
            file << 0 << "\t";
            m++;
         }
         file << F[i];
         file << '\n';

      }
   }

};

