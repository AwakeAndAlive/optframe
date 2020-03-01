// OptFrame - Optimization Framework

// Copyright (C) 2009-2015
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

#ifndef EtII_NSSEQSwapSide_HPP_
#define EtII_NSSEQSwapSide_HPP_

// Framework includes
#include "../../OptFrame/NSSeq.hpp"
#include "../../OptFrame/RandGen.hpp"

// Own includes
#include "ProblemInstance.h"
#include "Solution.h"

using namespace std;
using namespace optframe;

namespace EtII
{

class MoveSwapSide: public Move<SolutionEtII>
{
protected:
	int x1, y1, x2, y2;

public:

	MoveSwapSide(int _x1, int _y1, int _x2, int _y2) :
		x1(_x1), y1(_y1), x2(_x2), y2(_y2)
	{
	}

	virtual ~MoveSwapSide()
	{
	}

	bool canBeApplied(const SolutionEtII& s) override
	{
      const RepEtII& rep = s.getR();
		bool left_upper = ((x1 == 0) && (y1 == 0)) || ((x2 == 0) && (y2 == 0));
		bool right_upper = ((x1 == 0) && (y1 == (int)rep.getNumCols() - 1)) || ((x2 == 0) && (y2 == (int)rep.getNumCols() - 1));
		bool left_lower = ((x1 == (int)rep.getNumRows() - 1) && (y1 == 0)) || ((x2 == (int)rep.getNumRows() - 1) && (y2 == 0));
		bool right_lower = ((x1 == (int)rep.getNumRows() - 1) && (y1 == (int)rep.getNumCols() - 1)) || ((x2 == (int)rep.getNumRows() - 1) && (y2 == (int)rep.getNumCols() - 1));

		return !left_upper && !right_upper && !left_lower && !right_lower;
	}

	uptr<Move<SolutionEtII>> apply(SolutionEtII& s) override
	{
      RepEtII& rep = s.getR();
		Piece p = rep(x1, y1);
		rep(x1, y1) = rep(x2, y2);
		rep(x2, y2) = p;

		if (x1 == 0)
			while (rep(x1, y1).up != 0)
				rep(x1, y1).rotate();

		if (x1 == (int)rep.getNumRows() - 1)
			while (rep(x1, y1).down != 0)
				rep(x1, y1).rotate();

		if (y1 == 0)
			while (rep(x1, y1).left != 0)
				rep(x1, y1).rotate();

		if (y1 == (int)rep.getNumCols() - 1)
			while (rep(x1, y1).right != 0)
				rep(x1, y1).rotate();

		// ==========

		if (x2 == 0)
			while (rep(x2, y2).up != 0)
				rep(x2, y2).rotate();

		if (x2 == (int)rep.getNumRows() - 1)
			while (rep(x2, y2).down != 0)
				rep(x2, y2).rotate();

		if (y2 == 0)
			while (rep(x2, y2).left != 0)
				rep(x2, y2).rotate();

		if (y2 == (int)rep.getNumCols() - 1)
			while (rep(x2, y2).right != 0)
				rep(x2, y2).rotate();

		// ==========

		return uptr<Move<SolutionEtII>>(new MoveSwapSide(x2, y2, x1, y1));
	}

	uptr<Move<SolutionEtII>> applyUpdate(pair<SolutionEtII, Evaluation<>>& se) override
	{
      SolutionEtII& s = se.first;
      Evaluation<>& e = se.second;

      RepEtII& rep = s.getR();
		int f = 0;
		if (((y1 - 1) >= 0) && (rep(x1, y1).left == rep(x1, y1 - 1).right))
			f++;
		if (((x1 - 1) >= 0) && (rep(x1, y1).up == rep(x1 - 1, y1).down))
			f++;
		if (((y1 + 1) < (int)rep.getNumCols()) && (rep(x1, y1).right == rep(x1, y1 + 1).left))
			f++;
		if (((x1 + 1) < (int)rep.getNumRows()) && (rep(x1, y1).down == rep(x1 + 1, y1).up))
			f++;

		int g = 0;
		if (((y2 - 1) >= 0) && (rep(x2, y2).left == rep(x2, y2 - 1).right) && !((x2 == x1) && ((y2 - 1) == y1)))
			g++;
		if (((x2 - 1) >= 0) && (rep(x2, y2).up == rep(x2 - 1, y2).down) && !(((x2 - 1) == x1) && (y2 == y1)))
			g++;
		if (((y2 + 1) < (int)rep.getNumCols()) && (rep(x2, y2).right == rep(x2, y2 + 1).left) && !((x2 == x1) && ((y2 + 1) == y1)))
			g++;
		if (((x2 + 1) < (int)rep.getNumRows()) && (rep(x2, y2).down == rep(x2 + 1, y2).up) && !(((x2 + 1) == x1) && (y2 == y1)))
			g++;

		uptr<Move<SolutionEtII>> rev = apply(s);

		int f2 = 0;
		if (((y1 - 1) >= 0) && (rep(x1, y1).left == rep(x1, y1 - 1).right))
			f2++;
		if (((x1 - 1) >= 0) && (rep(x1, y1).up == rep(x1 - 1, y1).down))
			f2++;
		if (((y1 + 1) < (int)rep.getNumCols()) && (rep(x1, y1).right == rep(x1, y1 + 1).left))
			f2++;
		if (((x1 + 1) < (int)rep.getNumRows()) && (rep(x1, y1).down == rep(x1 + 1, y1).up))
			f2++;

		int g2 = 0;
		if (((y2 - 1) >= 0) && (rep(x2, y2).left == rep(x2, y2 - 1).right) && !((x2 == x1) && ((y2 - 1) == y1)))
			g2++;
		if (((x2 - 1) >= 0) && (rep(x2, y2).up == rep(x2 - 1, y2).down) && !(((x2 - 1) == x1) && (y2 == y1)))
			g2++;
		if (((y2 + 1) < (int)rep.getNumCols()) && (rep(x2, y2).right == rep(x2, y2 + 1).left) && !((x2 == x1) && ((y2 + 1) == y1)))
			g2++;
		if (((x2 + 1) < (int)rep.getNumRows()) && (rep(x2, y2).down == rep(x2 + 1, y2).up) && !(((x2 + 1) == x1) && (y2 == y1)))
			g2++;

		e.setObjFunction(e.getObjFunction()+(f2 - f));
		e.setObjFunction(e.getObjFunction()+(g2 - g));

		return rev;
	}

	virtual bool operator==(const Move<SolutionEtII>& _m) const
	{
		const MoveSwapSide& m = (const MoveSwapSide&) _m;
		return (m.x1 == x1) && (m.y1 == y1) && (m.x2 == x2) && (m.y2 == y2);
	}

	void print() const
	{
		cout << "MoveSwapSide: (" << x1 << "," << y1 << ") <=> (" << x2 << "," << y2 << ")" << endl;
	}

	string id() const
	{
		return "OptFrame:Move:MoveSwapSide";
	}
};

class NSIteratorSwapSide: public NSIterator<SolutionEtII>
{
private:
	int x1, y1, x2, y2;
	int nRows, nCols;
public:
	NSIteratorSwapSide(int _nRows, int _nCols) :
		nRows(_nRows), nCols(_nCols)
	{
	}

	virtual ~NSIteratorSwapSide()
	{
	}

	virtual void first() override
	{
		x1 = 0;
		y1 = 1;
		x2 = 0;
		y2 = 2;
	}

	virtual void next() override
	{
		int inc = nCols - 1;
		if ((x2 == 0) || (x2 == nRows - 1))
			inc = 1;
		y2 += inc;

		if (y2 >= nCols)
		{
			y2 = 0;
			x2++;
			if (x2 >= nRows)
			{
				inc = nCols - 1;
				if ((x1 == 0) || (x1 == nRows - 1))
					inc = 1;
				y1 += inc;

				if (y1 >= nCols)
				{
					y1 = 0;
					x1++;

					inc = nCols - 1;
					if ((x1 == 0) || (x1 == nRows - 1))
						inc = 1;

					x2 = x1;
					y2 = y1 + inc;

					if (y2 >= nCols)
					{
						y2 = 0;
						x2++;
					}
				}
				else
				{
					inc = nCols - 1;
					if ((x1 == 0) || (x1 == nRows - 1))
						inc = 1;

					x2 = x1;
					y2 = y1 + inc;

					if (y2 >= nCols)
					{
						y2 = 0;
						x2++;
					}
				}
			}
		}
	}

	virtual bool isDone() override
	{
		return x2 >= nRows;
	}

	virtual uptr<Move<SolutionEtII>> current() override
	{
		return uptr<Move<SolutionEtII>>(new MoveSwapSide(x1, y1, x2, y2));
	}
};

template<class MOVE = MoveSwapSide>
class NSSeqSwapSide: public NSSeq<SolutionEtII>
{
private:
	RandGen& rg;
public:

	
	NSSeqSwapSide(RandGen& _rg): rg(_rg)
	{
	}

	virtual ~NSSeqSwapSide()
	{
	}

	virtual uptr<Move<SolutionEtII>> randomMove(const SolutionEtII& s) override
	{
      const RepEtII& rep = s.getR();
		int x1, y1;

		if ((rg.rand(2)) == 0) // vert
		{
			x1 = rg.rand((rep.getNumRows() - 2)) + 1;
			y1 = rg.rand(2) * (rep.getNumCols() - 1);
		}
		else // horz
		{
		x1 = (rg.rand(2)) * (rep.getNumRows() - 1);
			y1 = rg.rand((rep.getNumCols() - 2)) + 1;
		}

		int x2 = x1;
		int y2 = y1;
		while ((x2 == x1) && (y2 == y1))
			if ((rg.rand(2)) == 0) // vert
			{
				x2 = rg.rand((rep.getNumRows() - 2)) + 1;
				y2 = (rg.rand(2)) * (rep.getNumCols() - 1);
			}
			else // horz
			{
				x2 = (rg.rand(2)) * (rep.getNumRows() - 1);
				y2 = rg.rand((rep.getNumCols() - 2)) + 1;
			}

		return uptr<Move<SolutionEtII>>(new MOVE(x1, y1, x2, y2));
	}

	virtual uptr<NSIterator<SolutionEtII>> getIterator(const SolutionEtII& s) override
	{
      const RepEtII& rep = s.getR();
		return uptr<NSIterator<SolutionEtII>>(new NSIteratorSwapSide(rep.getNumRows(), rep.getNumCols()));
	}

	virtual void print() const
	{
		cout << "NSSeqSwapSide" << endl;
	}

	string id() const
	{
		return "OptFrame:NS:NSSeqSwapSide";
	}
};

}

#endif /*EtII_NSSEQSwapSide_HPP_*/

