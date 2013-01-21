// OptFrame - Optimization Framework

// Copyright (C) 2009, 2010, 2011
// http://optframe.sourceforge.net/
//
// This file is part of the OptFrame optimization framework. This framework
// is free software; you can redistribute it and/or modify it under the
// terms of the GNU Lesser General Public License v3 as published by the
// Free Software Foundation.

// This framework is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License v3 for more details.

// You should have received a copy of the GNU Lesser General Public License v3
// along with this library; see the file COPYING.  If not, write to the Free
// Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
// USA.

#ifndef OPTFRAME_NSSEQUNIONADAPTER_HPP_
#define OPTFRAME_NSSEQUNIONADAPTER_HPP_

// Framework includes
//#include "../../../Move.hpp"
#include "../../NSSeq.hpp"

#include "./Moves/MoveNSSeqUnion.hpp"
#include "./NSIterators/IteratorNSSeqUnion.hpp"

using namespace std;

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS, class MOVE = MoveNSSeqUnion<R, ADS, DS> >
class NSSeqUnionAdapter: public NSSeq<R, ADS, DS>
{
private:
	vector<NSSeq<R, ADS, DS>*> ns;

public:

	NSSeqUnionAdapter(NSSeq<R, ADS, DS>& _n1, NSSeq<R, ADS, DS>& _n2)
	{
	   ns.push_back(&_n1);
	   ns.push_back(&_n2);
	}

	void add_ns(NSSeq<R, ADS, DS>& _ns)
	{
	   ns.push_back(&_ns);
	}

	virtual ~NSSeqUnionAdapter()
	{
	}

	Move<R, ADS, DS>& move(const R& r)
	{
      int x = rand() % ns.size();

      return *new MOVE(x, ns[x]->move(r));
   }

	virtual NSIterator<R, ADS, DS>& getIterator(const R& r)
	{
      vector<NSIterator<R, ADS, DS>*> it;
      for(unsigned int i = 0; i < ns.size(); i++)
         it.push_back(&ns[i]->getIterator(r));

      return *new IteratorNSSeqUnion<R, ADS, DS, MOVE> (it);
   }

	virtual string toString() const
	{
		stringstream ss;
		ss << "NSSeqUnionAdapter: [";
		for(unsigned int i = 0; i < ns.size(); i++)
		{
           ss << ns[i]->toString();

           if(i != ns.size()-1)
        	   ss << ",";
		}
		ss << "] ";
		return ss.str();
	}
};

#endif /*OPTFRAME_NSSEQUNIONADAPTER_HPP_*/
