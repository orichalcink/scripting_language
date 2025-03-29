#include "errors/catcher.hpp"
#include <iostream>

void Catcher::insert(const char* error)
{
   this->errors.push_back(error);
}

void Catcher::error(const char* error)
{
   insert(error);
   display();
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
   
   #if defined(__linux__) || defined(__APPLE__)
   std::cout << "\n\033[38;2;255;0;0m";
   std::cout << count << " error" << (count == 1 ? char{} : 's') << " occurred:";
   std::cout << "\033[0m\n";
   #else
   std::cout << "\n" << count << " error" << (count == 1 ? char{} : 's') << " occurred:\n";
   #endif

   for (const auto& error : this->errors)
      std::cout << error << "\n";
   
   std::cout << "\n";
   this->errors.clear();
   return true;
}
