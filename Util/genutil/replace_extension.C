#include <string>

// a simple command for replacing the extension on a filename
std::string replace_extension (std::string filename, const std::string& ext)
{
  unsigned index = filename.find_last_of( ".", filename.length() );
  if (index == std::string::npos)
    index = filename.length();

  filename = filename.substr(0, index);

  if (ext[0] != '.')
    filename += ".";

  filename += ext;
}
