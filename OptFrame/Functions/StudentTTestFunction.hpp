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

#ifndef OPTFRAME_STUDENT_T_TEST_FUNCTION_HPP_
#define OPTFRAME_STUDENT_T_TEST_FUNCTION_HPP_

#include <iostream>
#include <ostream>

#include <vector>
#include <map>

#include "../Scanner++/Scanner.h"

#include "../OptFrameFunction.hpp"

#include "../OptFrameList.hpp"

#include <algorithm>

class StudentTTestFunction : public OptFrameFunction
{
public:

	virtual ~StudentTTestFunction()
	{
	}

	virtual string id()
	{
		return "student_t_test";
	}

	virtual string usage()
	{
		return "student_t_test( list1 list2 ) : return p-value\nStudent's t-test => requires: near-normality from each input data (use shapiro_test or kolmogorov_test); variances are equal (use var_test); data independently sampled\n null hypothesis: means are equal; if p-value < alpha reject null hypothesis.";
	}

	virtual string formatNumber(double v)
	{
		stringstream ss;
		ss << v;
		return ss.str();
	}

	virtual pair<string, string>* run(vector<OptFrameFunction*>& allFunctions, map< string, string >&, map< string,vector<string> >& ldictionary, string body)
	{
		Scanner scanner(body);

		vector<string>* plist1 = OptFrameList::readList(ldictionary, scanner);
		vector<string>  list1;
		if(plist1)
		{
			list1 = vector<string>(*plist1);
			delete plist1;
		}
		else
			return NULL;

		if(list1.size()==0)
			return NULL;

		vector<string>* plist2 = OptFrameList::readList(ldictionary, scanner);
		vector<string>  list2;
		if(plist2)
		{
			list2 = vector<string>(*plist2);
			delete plist2;
		}
		else
			return NULL;

		if(list2.size()==0)
			return NULL;

		stringstream scommand;
		scommand << "echo \"t.test( x=c(";

		for(unsigned i=0; i<list1.size(); i++)
		{
			scommand << list1.at(i);
			if(i != list1.size()-1)
				scommand << ",";
		}

		scommand << "), y=c(";

		for(unsigned i=0; i<list2.size(); i++)
		{
			scommand << list2.at(i);
			if(i != list2.size()-1)
				scommand << ",";
		}

		scommand << ") )\" | R --no-save | grep p-value";

		//cout << scommand.str() << endl;

		FILE* pPipe = popen(scommand.str().c_str(), "r");
		if (pPipe == NULL)
		{
		    cout << "general_t_test function: PIPE NOT OPEN!" << endl;
		    return NULL;
		}

		char line[100];

		string output = "";

		while(fgets(line, 100, pPipe) != NULL)
		{
			string sline = line;
			output.append(sline);
		}

		pclose(pPipe);

		//cout << "general_t_test function: OUTPUT '" << output << "'" << endl;
		if(output=="") // POSSIBLE ERROR: 'sh: R: not found'
			return NULL;

		Scanner scan_out(output); //example: 't = -2.2156, df = 18, p-value = 0.01992'
		scan_out.next(); // drop 't'
		scan_out.next(); // drop '='
		scan_out.next(); // drop '-2.2156,'
		scan_out.next(); // drop 'df'
		scan_out.next(); // drop '='
		scan_out.next(); // drop '18,'
		scan_out.next(); // drop 'p-value'
		scan_out.next(); // drop '='
		double pvalue = scan_out.nextDouble();

		scanner.next(); // drop ')'

		return new pair<string, string>(formatNumber(pvalue), scanner.rest());
	}
};

#endif /* OPTFRAME_STUDENT_T_TEST_FUNCTION_HPP_ */
