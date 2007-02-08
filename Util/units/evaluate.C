#include "Error.h"
#include "pad.h"
#include "tostring.h"

// evaluateExpression from Parsifal Software
#include "evaldefs.h"
#include "substitute.h"

#include <string>
#include <iostream>

using namespace std;

string evaluate (const string& text, char cstart, char cend)
{
  string remain = text;

  string::size_type start;
  string::size_type end;

  int(*pred)(int) = isdigit;

  while ( (start = remain.find(cstart)) != string::npos &&
	  (end = remain.find(cend, start)) != string::npos ) {

    // find the end of the variable name
    string eval = remain.substr (start+1, end-start-1);
      
    // string preceding the variable substitution
    string before = remain.substr (0, start);

    end ++;

    unsigned precision = 0;

    if (remain[end] == '%') {
      end ++;
      string::size_type pend = find_first_not_if (remain, pred, end);
      string::size_type len = (pend != string::npos ) ? pend-end : pend;
      precision = fromstring<unsigned> (remain.substr (end, len));
      end = pend;
    }

    // string following the variable substitution
    string after = (end != string::npos) ? remain.substr (end) : "";

    string result = "result";
    string expression = result + " = (" + eval + ")";

    // Call parser function from Parsifal Software
    int errorFlag = evaluateExpression (const_cast<char*>(expression.c_str()));

    if (errorFlag)
      throw Error (InvalidParam, "Pulsar::Interpreter::test",
		   "%s in expression\n\n  %s\n  %s",
		   errorRecord.message, eval.c_str(), 
		   pad (errorRecord.column-10, "^", false).c_str());
    
    double value = -1;
    for (unsigned i = 0; i < nVariables; i++)
      if (variable[i].name == result)
	value = variable[i].value;

    string subst;

    if (precision)
      subst = tostring (value, precision);
    else
      subst = tostring (value);

    remain = before + subst + after;
  }

  return remain;
}
