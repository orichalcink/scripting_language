#include "io/args.hpp"
#include "errors/errors.hpp"
#include <sstream>

inline static std::string decoy = "";

Args::Args(Catcher& catcher, std::string& command)
   : catcher(catcher), command(command)
{
   std::stringstream stream (this->command);

   bool first = true;
   size_t index = 0;
   std::string arg;

   while ((stream >> arg))
   {
      if (this->args.find(arg) != this->args.end())
      {
         this->catcher.insert(err::arg_redefined);
         return;
      }
      size_t pos = arg.find("=");

      if (pos == arg.npos)
         this->args.insert({arg, 1});
      else
      {
         try
         {
            size_t number = std::stoul(arg.substr(pos + 1));
            arg = arg.substr(0, pos);
            this->args.insert({arg, number});
         }
         catch (...)
         {
            this->catcher.insert(err::invalid_run_arg);
         }
      }
      this->indexes.insert({index++, arg});

      // Wow...
      if (first && arg != "quit" && arg != "run" && arg != "help" && arg != "version" && arg != "cat")
         break;
      first = false;
   }
}

bool Args::empty() const
{
   return this->args.empty();
}

size_t Args::size() const
{
   return this->args.size();
}

bool Args::contains(const std::string& argument) const
{
   return this->args.find(argument) != this->args.end();
}

size_t Args::get_arg(const std::string& argument) const
{
   if (this->args.find(argument) == this->args.end())
      return 0;
   return this->args.at(argument);
}

std::string& Args::at(size_t index)
{
   if (this->indexes.find(index) == this->indexes.end())
   {
      this->catcher.insert(err::out_of_bounds_arg);
      return decoy;
   }
   return this->indexes.at(index);
}
