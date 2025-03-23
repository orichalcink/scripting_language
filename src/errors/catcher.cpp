#include "errors/catcher.hpp"
#include <iostream>

void Catcher::insert(const char* error)
{
   this->errors.push_back(error);
}

void Catcher::error(const char* error)
{
   std::cout << std::endl << "1 error occurred:" << std::endl;
   std::cout << error << std::endl << std::endl;
}

bool Catcher::empty() const
{
   return this->errors.empty();
}

bool Catcher::display()
{
   const auto count = this->errors.size();
   if (!count)
      return false;
   
   std::cout << std::endl << count << " error" << (count == 1 ? 0 : 's') << " occurred:" << std::endl;
   for (const auto& error : this->errors)
      std::cout << error << std::endl;
   
   std::cout << std::endl;
   this->errors.clear();
   return true;
}
