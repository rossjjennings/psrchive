/*

  This program generates the file, PromoteTraits.h

  This header file implements a type promotion template similar to
  that of Todd Veldhuizen, http://www.oonumerics.org/blitz/traits.html.
  However, by using some template template arguments, the template can
  be applied to other templates, like std::complex

  Willem van Straten 21 December 2004

*/

#include <fstream>
#include <vector>
#include <string>

using namespace std;

int main () {

  // it is assumed that header_PromoteTraits.h is copied to PromoteTraits.h
  // before this program is run
  ofstream out ("PromoteTraits.h", ios::app);
  
  vector<string> types;

  // types are added in order of increasing precedence
  types.push_back( "float" );
  types.push_back( "double" );
  types.push_back( "long double" );
  types.push_back( "MEAL::ScalarMath" );

  for (unsigned i=0; i<types.size(); i++)
    for (unsigned j=0; j<types.size(); j++)

      out << "class PromoteTraits< " <<types[i]<< ", " <<types[j]<< " > {\n"
	"public:\n"
	"  typedef " << types[max(i,j)] << " promote_type;\n"
	"};\n" << endl;

  out << "#endif // !__MEAL_PromoteTraits_h\n" << endl;

  out.close ();

};
