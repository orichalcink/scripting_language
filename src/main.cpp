#include "errors/catcher.hpp"
#include "errors/errors.hpp"
#include "lexer/lexer.hpp"
#include <iostream>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

int main()
{
   std::cout << "REPL for an interpreted scripting language." << std::endl;
   Catcher catcher;

   while (true)
   {
      std::cout << "> ";
      std::string input;
      std::getline(std::cin, input);

      if (input == "")
      {
         catcher.error(err::invalid_input);
         continue;
      }

      if (input == "quit")
      {
         std::cout << "Quitting..." << std::endl;
         return 0;
      }

      if (fs::is_regular_file(input))
      {
         std::ifstream file (input);
         input.clear();

         if (!file.is_open())
         {
            catcher.error(err::cannot_open_file);
            continue;
         }

         std::string line;
         while (std::getline(file, line))
            input += line + '\n';
         file.close();
      }
  
      Lexer lexer (catcher, input);
      const auto& tokens = lexer.tokenize();

      if (catcher.display())
         continue;

      for (const auto& token : tokens)
         std::cout << (int)token.type << " " << token.lexeme << std::endl;
   }
}