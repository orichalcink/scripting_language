#include "errors/catcher.hpp"
#include "errors/errors.hpp"
#include "lexer/lexer.hpp"
#include "preprocessor/preprocessor.hpp"
#include "io/files.hpp"
#include "io/args.hpp"
#include <iostream>

int main()
{
   std::cout << "REPL for an interpreted scripting language." << std::endl;
   Catcher catcher;

   while (true)
   {
      std::cout << "> ";
      std::string input;
      std::getline(std::cin, input);

      Args args (catcher, input);

      if (args.empty())
      {
         catcher.error(err::invalid_input);
         continue;
      }

      if (args.size() == 1 && args.at(0) == "quit")
      {
         std::cout << "Quitting..." << std::endl;
         return 0;
      }
      else if (args.size() == 1 && args.at(0) == "help")
      {
         std::cout << "TODO: add help command" << std::endl;
      }
      else if (args.size() >= 2 && args.at(0) == "run")
      {
         std::string file_name;

         if (is_file(args.at(1)))
         {
            file_name = args.at(1);
            input = read_file(catcher, file_name);

            if (catcher.display())
               continue;
         }
         else
         {
            catcher.error(err::invalid_run_command);
            continue;
         }

         auto start_lex = std::chrono::high_resolution_clock::now();
         Lexer lexer (catcher, input);
         auto& tokens = lexer.tokenize();
         auto end_lex = std::chrono::high_resolution_clock::now();

         if (catcher.display())
            continue;

         if (args.get_arg("--log-lexer"))
         {
            std::cout << "\nTokens after lexing:\n";
            for (const auto& token : tokens)
               printf("%-13s - \"%s\"\n", token_to_string(token.type), token.lexeme.c_str());
         }


         auto start_pre = std::chrono::high_resolution_clock::now();
         if (!args.get_arg("--skip-preprocessor"))
         {
            Preprocessor preprocessor (catcher, tokens, file_name, args.get_arg("--no-predefined-macros"));

            if (args.contains("--macro-depth"))
               preprocessor.specify_max_macro_depth(args.get_arg("--macro-depth"));

            preprocessor.process();

            if (catcher.display())
               continue;
         }
         auto end_pre = std::chrono::high_resolution_clock::now();

         if (args.get_arg("--log-preprocessor"))
         {
            std::cout << "\nTokens after preprocessing:\n";
            for (const auto& token : tokens)
               printf("%-13s - \"%s\"\n", token_to_string(token.type), token.lexeme.c_str());
         }

         if (args.get_arg("--bench"))
         {
            auto lex = std::chrono::duration_cast<std::chrono::microseconds>(end_lex - start_lex).count();
            auto pre = std::chrono::duration_cast<std::chrono::microseconds>(end_pre - start_pre).count();

            printf("Benchmark:\n");
            printf("%-16s %ld μs\n", "Lexing time:", lex);
            printf("%-16s %ld μs\n", "Processing time:", pre);
            printf("%-16s %ld μs\n", "Total:", lex + pre);
         }
      }
      else
      {
         Lexer lexer (catcher, input);
         auto& tokens = lexer.tokenize();

         if (catcher.display())
            continue;

         Preprocessor preprocessor (catcher, tokens, "", false);
         preprocessor.process();

         if (catcher.display())
            continue;
      }
   }
}