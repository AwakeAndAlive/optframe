// OptFrame 4.2 - Optimization Framework
// Copyright (C) 2009-2021 - MIT LICENSE
// https://github.com/optframe/optframe
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef OPTFRAME_NSSEQVRP0ROPT1_HPP_
#define OPTFRAME_NSSEQVRP0ROPT1_HPP_

// Framework includes
#include "../../../../Move.hpp"
#include "../../../../NSSeq.hpp"

using namespace std;

//template<class T, class ADS = OPTFRAME_DEFAULT_ADS>
/*
template<class T, class ADS, XBaseSolution<vector<vector<T> >,ADS> S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>>
class MoveVRPOrOpt1: public Move<XES, XEv>
{
	typedef vector<vector<T> > Routes;
*/
template<XESolution XES, class P = OPTFRAME_DEFAULT_PROBLEM>
class MoveVRPOrOpt1 : public Move<XES, typename XES::second_type>
{
   using XEv = typename XES::second_type;
   typedef vector<vector<int>> Routes;

public:
   Routes& (*getRoutes)(const XES&); // function to get routes from type 'R'

protected:
   int r;   // route id
   int c;   // client
   int pos; // position

private:
   P* problem;

public:
   MoveVRPOrOpt1(Routes& (*_getRoutes)(const XES&), int _r, int _c, int _pos, P* _problem = nullptr)
     : getRoutes(_getRoutes)
     , r(_r)
     , c(_c)
     , pos(_pos)
     , problem(_problem)
   {
   }

   virtual ~MoveVRPOrOpt1()
   {
   }

   int getRoute()
   {
      return r;
   }

   int getClient()
   {
      return c;
   }

   int getPosition()
   {
      return pos;
   }

   virtual bool canBeApplied(const XES& se) override
   {
      //const Routes& rep = getRoutes(se);//se.first.getR();
      bool all_positive = (r >= 0) && (c >= 0) && (pos >= 0);
      return all_positive && (c != pos) && (c + 1 != pos);
   }

   /*
	virtual void updateNeighStatus(ADS& ads)
	{
	}
*/

   virtual uptr<Move<XES>> apply(XES& se) override
   {
      Routes& rep = getRoutes(se); //se.first.getR();
      int aux;
      int i;
      if (c < pos) {
         aux = rep.at(r).at(c);
         for (i = c; i < (pos - 1); i++) {
            //aux = rep.at(r).at(i);
            rep.at(r).at(i) = rep.at(r).at(i + 1);
            //rep.at(r).at(i + 1) = aux;
         }
         rep.at(r).at(i) = aux;
         return uptr<Move<XES>>(new MoveVRPOrOpt1<XES, P>(getRoutes, r, pos - 1, c));
      } else {
         aux = rep.at(r).at(c);
         for (i = c; i > pos; i--) {
            rep.at(r).at(i) = rep.at(r).at(i - 1);
         }
         rep.at(r).at(i) = aux;
         return uptr<Move<XES>>(new MoveVRPOrOpt1<XES, P>(getRoutes, r, pos, c + 1));
      }

      return uptr<Move<XES>>(new MoveVRPOrOpt1<XES, P>(getRoutes, -1, -1, -1));
      ///exit(1);
   }

   virtual bool operator==(const Move<XES>& _m) const
   {
      const MoveVRPOrOpt1<XES, P>& m1 = (const MoveVRPOrOpt1<XES, P>&)_m;
      return (m1.r == r) && (m1.c == c) && (m1.pos == pos);
   }

   void print() const
   {
      cout << "MoveVRPOrOpt1";
      cout << "( " << r << " , " << c << " , " << pos << " )" << endl;
   }
};

//template<class T, class ADS = OPTFRAME_DEFAULT_ADS, class MOVE = MoveVRPOrOpt1<T, ADS> , class P = OPTFRAME_DEFAULT_PROBLEM>
/*
template<class T, class ADS, XBaseSolution<vector<vector<T>>,ADS> S, class MOVE = MoveVRPOrOpt1<XES, P>, class P = OPTFRAME_DEFAULT_PROBLEM, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>>
class NSIteratorVRPOrOpt1: public NSIterator<XES, XEv>
{
	typedef vector<vector<T> > Routes;
*/
template<XESolution XES, class P = OPTFRAME_DEFAULT_PROBLEM, class MOVE = MoveVRPOrOpt1<XES, P>>
class NSIteratorVRPOrOpt1 : public NSIterator<XES, typename XES::second_type>
{
   typedef vector<vector<int>> Routes;

protected:
   uptr<Move<XES>> m;
   vector<uptr<Move<XES>>> moves;
   int index;                        //index of moves
   Routes& (*getRoutes)(const XES&); // function to get routes from type 'R'
   const Routes& rep;
   P* p; // has to be the last

   NSIteratorVRPOrOpt1(Routes& (*getRoutes)(const XES&), const XES& se, P* _p = nullptr)
     : getRoutes(getRoutes)
     , rep{ getRoutes(se) }
     , p(_p)
   {
      m = nullptr;
      index = 0;
   }

   virtual ~NSIteratorVRPOrOpt1()
   {
   }

   virtual void first() override
   {

      for (int r = 0; r < (int)rep.size(); r++) {
         for (int c = 0; c < (int)rep.at(r).size(); c++) {
            for (int pos = 0; pos <= (int)rep.at(r).size(); pos++) {
               if ((c != pos) && (c + 1 != pos)) {
                  moves.push_back(uptr<Move<XES>>(new MOVE(getRoutes, r, c, pos, p)));
               }
            }
         }
      }

      if (moves.size() > 0) {
         m = std::move(moves[index]); // stealing from vector... verify if this is correct! otherwise, must need clone() on Move
      } else
         m = nullptr;
   }

   virtual void next() override
   {
      index++;
      if (index < (int)moves.size()) {
         m = std::move(moves[index]); // stealing from vector... verify if this is correct! otherwise, must need clone() on Move
      } else
         m = nullptr;
   }

   virtual bool isDone() override
   {
      return m == nullptr;
   }

   virtual uptr<Move<XES>> current() override
   {
      if (isDone()) {
         cout << "There isnt any current element!" << endl;
         cout << "VRPOrOpt1. Aborting." << endl;
         exit(1);
      }

      uptr<Move<XES>> m2 = std::move(m);
      m = nullptr;

      return m2;
   }
};

//template<class T, class ADS = OPTFRAME_DEFAULT_ADS, class MOVE = MoveVRPOrOpt1<T, ADS> , class P = OPTFRAME_DEFAULT_PROBLEM, class NSITERATOR = NSIteratorVRPOrOpt1<T, ADS, MOVE, P> >
/*
template<class T, class ADS, XBaseSolution<vector<vector<T>>,ADS> S, class MOVE = MoveVRPOrOpt1<XES, P>, class P = OPTFRAME_DEFAULT_PROBLEM, class NSITERATOR = NSIteratorVRPOrOpt1<T, ADS, S, MOVE, P>, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, XSearch<XES> XSH = std::pair<S, XEv>>
class NSSeqVRPOrOpt1: public NSSeq<XES, XEv, XSH>
{
	typedef vector<vector<T> > Routes;

private:
	P* p; // has to be the last
*/
template<XESolution XES, class P = OPTFRAME_DEFAULT_PROBLEM, class MOVE = MoveVRPOrOpt1<XES, P>, class NSITERATOR = NSIteratorVRPOrOpt1<XES, P, MOVE>>
class NSSeqVRPOrOpt1 : public NSSeq<XES, typename XES::second_type, XES>
{
   typedef vector<vector<int>> Routes;

public:
   Routes& (*getRoutes)(const XES&); // function to get routes from type 'R'

private:
   P* p; // has to be the last (?)

public:
   NSSeqVRPOrOpt1(Routes& (*_getRoutes)(const XES&), P* _p = nullptr)
     : getRoutes(_getRoutes)
     , p(_p)
   {
   }

   virtual ~NSSeqVRPOrOpt1()
   {
   }

   uptr<Move<XES>> randomMove(const XES& se) override
   {
      const Routes& rep = getRoutes(se); //se.first.getR();
      int r = rand() % rep.size();

      if (rep.at(r).size() < 2)
         return uptr<Move<XES>>(new MOVE(getRoutes, -1, -1, -1, p));

      int c = rand() % rep.at(r).size();

      int pos = rand() % (rep.at(r).size() + 1);

      return uptr<Move<XES>>(new MOVE(getRoutes, r, c, pos, p));
   }

   virtual uptr<NSIterator<XES>> getIterator(const XES& se) override
   {
      //XSolution& s = se.first;
      return uptr<NSIterator<XES>>(new NSITERATOR(getRoutes, se, p));
   }

   virtual string toString() const
   {
      stringstream ss;
      ss << "NSSeqVRPOrOpt1 with move: " << MOVE::idComponent();
      return ss.str();
   }
};

#endif /*OPTFRAME_NSSEQVRP0ROPT1_HPP_*/
