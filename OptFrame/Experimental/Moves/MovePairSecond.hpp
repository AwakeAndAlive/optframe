// OptFrame - Optimization Framework

// Copyright (C) 2009, 2010, 2011
// Igor Machado - Mario Henrique Perche - Pablo Munhoz - Sabir Ribas
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

#ifndef OPTFRAME_MOVEPAIRSECOND_ADAPTER_HPP_
#define OPTFRAME_MOVEPAIRSECOND_ADAPTER_HPP_

// Framework includes
#include "../../Move.hpp"

using namespace std;

template<class T1, class T2, class M = OPTFRAME_DEFAULT_MEMORY>
class MovePairSecond: public Move<pair<T1, T2> , M>
{
protected:
	Move<T2, M>& m;

public:

	MovePairSecond(Move<T2, M>& _m) :
		m(_m)
	{
	}

	Move<T2, M>& get_m()
	{
		return m;
	}

	virtual ~MovePairSecond()
	{
		delete &m;
	}

	bool canBeApplied(const pair<T1, T2>& rep)
	{
		return m.canBeApplied(rep.second);
	}

	Move<pair<T1, T2> , M>& apply(pair<T1, T2>& rep)
	{
		return *new MovePairSecond<T1, T2, M> (m.apply(rep.second));
	}

	virtual bool operator==(const Move<pair<T1, T2> , M>& _m) const
	{
		const MovePairSecond<T1, T2, M>& m1 = (const MovePairSecond<T1, T2, M>&) _m;
		return m == m1.m;
	}

	void print() const
	{
		cout << "MovePairSecond: move = ";
		m.print();
	}
};

#endif /*OPTFRAME_MOVEPAIRSECOND_ADAPTER_HPP_*/
