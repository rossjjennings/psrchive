#include <string>

using namespace std;

// a simple command for replacing the extension on a filename
string replace_extension (string filename, const string& ext)
{
  if (!(ext.length() && filename.length()))
    return;

  unsigned index = filename.find_last_of( ".", filename.length() );
  if (index == string::npos)
    index = filename.length();

  filename = filename.substr(0, index);

  if (ext[0] != '.')
    filename += ".";

  filename += ext;

  return filename;
}
