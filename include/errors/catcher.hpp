#ifndef CATCHER_HPP
#define CATCHER_HPP

#include <vector>

class Catcher
{
public:
   Catcher() = default;
   ~Catcher() = default;

   void insert(const char* error);
   void error(const char* error);
   bool empty() const;
   bool display();

private:
   std::vector<const char*> errors;
};

#endif // CATCHER_HPP
