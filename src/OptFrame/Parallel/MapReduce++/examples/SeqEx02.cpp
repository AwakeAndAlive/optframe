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

/* MapReduce Interface
Example: Basic example using a sequential string-based Mapper and Reducer with MapReduce Interface
Author : Sabir Ribas
Date   : 2009-02-09
*/

#include "../SeqMR/SeqMR.h"
#include <iostream>

class MyMapper : public SeqMR_StrMapper
{
public:
   MyMapper(SeqMR_StrMapReduce* mr)
     : SeqMR_StrMapper(mr){};
   virtual vector<pair<string, string>> map(pair<string, string> a)
   {
      vector<pair<string, string>> mapped;
      mapped.push_back(a);
      return mapped;
   };
};

class MyReducer : public SeqMR_StrReducer
{
public:
   MyReducer(SeqMR_StrMapReduce* mr)
     : SeqMR_StrReducer(mr){};
   virtual pair<string, string> reduce(pair<string, vector<string>> bs)
   {
      string reduced;
      for (vector<string>::iterator it = bs.second.begin(); it != bs.second.end(); ++it)
         reduced += *it;
      return pair<string, string>(bs.first, reduced);
   };
};

int
main()
{
   cout << "StrSeqMR test" << endl;

   vector<pair<string, string>> input;
   input.push_back(pair<string, string>("1", "Map"));
   input.push_back(pair<string, string>("1", "Reduce"));
   input.push_back(pair<string, string>("2", "Interface"));

   SeqMR_StrMapReduce mapReduce;
   MyMapper mapper(&mapReduce);
   MyReducer reducer(&mapReduce);

   vector<pair<string, string>> output = mapReduce.run(mapper, reducer, input);
   cout << output << endl;

   return 0;
}
