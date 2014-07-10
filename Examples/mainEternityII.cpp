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

// ===================================
// Main.cpp file generated by OptFrame
// Project Eternity II
// ===================================

#include <stdlib.h>
#include <math.h>

#include <iostream>

using namespace std;

#include <set>

#include "../OptFrame/Interactive.hpp"
#include "EternityII.h"

using namespace EtII;

int main(int argc, char **argv)
{
	Loader<RepEtII, OPTFRAME_DEFAULT_ADS, MemEtII> optframe;
	EtIIProblemCommand etii;
	etii.load("./EternityII/pieces_set_2/pieces_10x10.txt", optframe.factory, optframe.dictionary, optframe.ldictionary);


	cout << "Program ended successfully" << endl;

	return 0;
}
