#include "errors/errors.hpp"
#include "io/files.hpp"
#include <fstream>

bool is_file(const fs::path& path)
{
   return fs::is_regular_file(path);
}

std::string read_file(Catcher& catcher, const fs::path& path)
{
   std::ifstream file (path);

   if (!file.is_open())
   {
      catcher.error(err::cannot_open_file);
      return "";
   }

   std::string result;
   std::string line;

   while (std::getline(file, line))
      result += line + '\n';
   
   file.close();
   return result;
}
