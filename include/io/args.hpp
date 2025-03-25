#ifndef ARGS_HPP
#define ARGS_HPP

#include "errors/catcher.hpp"
#include <string>
#include <map>

class Args
{
public:
   Args(Catcher& catcher, std::string& command);
   ~Args() = default;

   bool empty() const;
   size_t size() const;
   bool contains(const std::string& argument) const;
   size_t get_arg(const std::string& argument) const;
   std::string& at(size_t index);

private:
   Catcher& catcher;
   std::string& command;
   std::map<std::string, size_t> args;
   std::map<size_t, std::string> indexes;
};

#endif // ARGS_HPP
