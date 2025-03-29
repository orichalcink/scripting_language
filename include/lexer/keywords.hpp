#ifndef KEYWORDS_H
#define KEYWORDS_H

#include <unordered_set>
#include <string>

using namespace std::string_literals;

static inline const std::unordered_set<std::string> keywords
{
   "let"s, "mut"s,
   "def"s, "defl"s, "undef"s,
   "import"s, "include"s,
   "if"s, "elif"s, "else"s, "endif"s,
   "error"s, "log"s, "logl"s, "assert"s
};

#endif // KEYWORDS_H
