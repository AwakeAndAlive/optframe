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

#ifndef OPTFRAME_ANOVA_FUNCTION_HPP_
#define OPTFRAME_ANOVA_FUNCTION_HPP_

#include <iostream>
#include <ostream>

#include <vector>
#include <map>

#include "../Scanner++/Scanner.h"

#include "../OptFrameFunction.hpp"

#include "../OptFrameList.hpp"

#include <algorithm>

class ANOVAFunction : public OptFrameFunction
{
public:

	virtual ~ANOVAFunction()
	{
	}

	virtual string id()
	{
		return "anova";
	}

	virtual string usage()
	{
		return "anova( list_of_list ) : return p-value\nnull hypothesis: no difference between values, if p-value < alpha reject null hypothesis.";
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

		vector<string>* plist = OptFrameList::readList(ldictionary, scanner);
		vector<string>  list;
		if(plist)
		{
			list = vector<string>(*plist);
			delete plist;
		}
		else
			return NULL;

		if(list.size()==0)
			return NULL;

		vector<vector<string> > list2;
		for(unsigned i=0; i<list.size(); i++)
		{
			Scanner scan(list[i]);
			vector<string>* plist1 = OptFrameList::readList(ldictionary, scan);
			vector<string>  list1;
			if(plist1)
			{
				list1 = vector<string>(*plist1);
				delete plist1;
			}
			else
				return NULL;

			list2.push_back(list1);
		}

		//cout << list2 << endl;

		stringstream scommand;
		scommand << "echo \"";

		stringstream ssx;
		ssx << "xv <- c(";

		unsigned nrows = list2.at(0).size();
		unsigned ncols = list2.size();

		for(unsigned i=0; i<ncols; i++)
			for(unsigned j=0; j<nrows; j++)
			{
				ssx << list2.at(i).at(j);

				if(!( (j == nrows-1) && (i == ncols-1)) )
					ssx << ",";
			}

		ssx << ")";

		//cout << "SSX: " << ssx.str() << endl;

		stringstream ssf;
		ssf << "fv <- factor(c(";

		for(unsigned i=0; i<ncols; i++)
			for(unsigned j=0; j<nrows; j++)
			{
				ssf << "'T"<<i<<"'";
				if(!( (j == nrows-1) && (i == ncols-1)) )
					ssf << ",";
			}

		ssf << "))";

		//cout << "SSF: " << ssf.str() << endl;

		scommand << ssx.str() << '\n' << ssf.str() << '\n';

		scommand << "summary(aov(xv ~ fv))\" | R --no-save | grep fv";

		//cout << "COMMAND: '" << scommand.str() << "'" << endl;

		FILE* pPipe = popen(scommand.str().c_str(), "r");
		if (pPipe == NULL)
		{
		    cout << "anova function: PIPE NOT OPEN!" << endl;
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

		//cout << "anova function: OUTPUT '" << output << "'" << endl;
		if(output=="") // POSSIBLE ERROR: 'sh: R: not found'
			return NULL;

		Scanner scan_out(output);
		//example: '> fv <- ... all values
		// > summary(aov(xv ~ fv))
		//fv           1  31088   31088   0.199  0.671
		//'
		scan_out.nextLine(); // drop '> fv <- ... all values'
		scan_out.nextLine(); // drop '> summary(aov(xv ~ fv))'
		scan_out.next(); // drop 'fv'
		scan_out.next(); // drop '1'
		scan_out.next(); // drop '31088,'
		scan_out.next(); // drop '31088'
		scan_out.next(); // drop '0.199'
		// WARNING: p-value can be 'NA' (NOT SURE, BUT FOR SAFETY...)
		string spvalue = scan_out.next();
		double pvalue;
		if(spvalue == "NA")
		{
			cerr << "anova function warning: returning 'NA' result! p-value = 1.0" << endl;
			pvalue = 1;
		}
		else
			pvalue = Scanner::parseDouble(spvalue);

		scanner.next(); // drop ')'

		return new pair<string, string>(formatNumber(pvalue), scanner.rest());
	}
};

#endif /* OPTFRAME_ANOVA_FUNCTION_HPP_ */
