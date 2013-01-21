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

#ifndef InitServersMODULE_HPP_
#define InitServersMODULE_HPP_

#include "../OptFrameModule.hpp"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class InitServersModule : public OptFrameModule<R, ADS, DS>
{
public:

	virtual ~InitServersModule()
	{
	}

	string id()
	{
		return "initservers";
	}
	string usage()
	{
		return "initservers";
	}

	void run(vector<OptFrameModule<R, ADS, DS>*>&, vector<OptFrameFunction*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string,string>& dictionary,  map< string,vector<string> >& ldictionary, string rest)
	{
		factory->mapReduce->initServers(factory->argc,factory->argv);
	}

	virtual string preprocess(vector<OptFrameFunction*>& allFunctions, map<string,string>&,  map< string,vector<string> >&, string input)
	{
		Scanner scanner(input);

		// First, remove the comments '%'

		string input2 = "";

		while(scanner.hasNextChar())
		{
			char c = scanner.nextChar();
			if(c != '%')
				input2 += c;
			else
				break;
		}

		return input2;
	}

};

#endif /* InitServersMODULE_HPP_ */
