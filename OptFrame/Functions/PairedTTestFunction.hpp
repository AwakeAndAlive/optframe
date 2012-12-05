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

#ifndef OPTFRAME_T_TEST_FUNCTION_HPP_
#define OPTFRAME_T_TEST_FUNCTION_HPP_

#include <iostream>
#include <ostream>

#include <vector>
#include <map>

#include "../Scanner++/Scanner.h"

#include "../OptFrameFunction.hpp"

#include "../OptFrameList.hpp"

#include <algorithm>

class PairedTTestFunction : public OptFrameFunction
{
public:

	virtual ~PairedTTestFunction()
	{
	}

	virtual string id()
	{
		return "paired_t_test";
	}

	virtual string usage()
	{
		return "paired_t_test( list1 list2 ) : return p-value\npaired t-test => requires: near-normality from each input data (use shapiro_test or kolmogorov_test); variances are equal (use var_test); data sampled in pairs\n null hypothesis: means are equal (after treatment), if p-value < alpha reject null hypothesis.";
		//return "paired_t_test( list1 signal list2 ) : return p-value\npaired t-test => requires: near-normality from each input data (use shapiro_test or kolmogorov_test); variances are equal (use var_test); data sampled in pairs\n null hypothesis: means are equal (after treatment), if p-value < alpha reject null hypothesis.\n'signal' can be '<', '>' or '=='";
	}

	virtual string formatNumber(double v)
	{
		stringstream ss;
		ss << v;
		return ss.str();
	}

	virtual pair<string, string>* run(vector<OptFrameFunction*>& allFunctions, map< string,vector<string> >& ldictionary, string body)
	{
		//cout << "paired_t_test run:'" << body << "'" << endl;
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

		//string signal = scanner.next();

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

		if(list1.size() != list2.size())
		{
			cout << "paired_t_test function: lists should have same size!" << endl;
			return NULL;
		}

		// CARE WITH CONSTANT LISTS IN A PAIRED TEST!

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

		scommand << "),";

		scommand << "paired=TRUE";

		/*
		scommand << ",alternative=";

		if(signal=="<")
			scommand << "'l'"; // less
		else if(signal==">")
			scommand << "'g'"; // greater
		else if(signal=="==")
			scommand << "'t'"; // two.sided
		else
		{
			cout << "paired_t_test function: unknown signal '" << signal << "'" << endl;
			return NULL;
		}
		*/

		scommand << ")\" | R --no-save | grep p-value";

		//cout << scommand.str() << endl;

		FILE* pPipe = popen(scommand.str().c_str(), "r");
		if (pPipe == NULL)
		{
		    cout << "paired_t_test function: PIPE NOT OPEN!" << endl;
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

		//cout << "paired_t_test function: OUTPUT '" << output << "'" << endl;
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
		// WARNING: p-value can be 'NA'
		string spvalue = scan_out.next();
		double pvalue;
		if(spvalue == "NA")
		{
			cerr << "paired_t_test function warning: returning 'NA' result! p-value = 1.0" << endl;
			pvalue = 1;
		}
		else
			pvalue = Scanner::parseDouble(spvalue);

		scanner.next(); // drop ')'

		return new pair<string, string>(formatNumber(pvalue), scanner.rest());
	}
};

#endif /* OPTFRAME_T_TEST_FUNCTION_HPP_ */
