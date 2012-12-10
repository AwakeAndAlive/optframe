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

#ifndef OPTFRAME_CONCAT_FUNCTION_HPP_
#define OPTFRAME_CONCAT_FUNCTION_HPP_

#include <iostream>
#include <ostream>

#include <vector>
#include <map>

#include "../Scanner++/Scanner.h"

#include "../OptFrameFunction.hpp"

class ConcatFunction : public OptFrameFunction
{
public:

	virtual ~ConcatFunction()
	{
	}

	virtual string id()
	{
		return "concat";
	}

	virtual string usage()
	{
		return "concat( string1 string2 ) : return string1string2";
	}

	virtual pair<string, string>* run(vector<OptFrameFunction*>& allFunctions, map< string, string >&, map< string,vector<string> >&, string body)
	{
		Scanner scanner(body);

		if(!scanner.hasNext())
			return NULL;

		string s1 = scanner.next();
		string s2 = scanner.next();

		scanner.next(); // drop ')'

		s1.append(s2);

		return new pair<string, string>(s1, scanner.rest());
	}
};

#endif /* OPTFRAME_CONCAT_FUNCTION_HPP_ */