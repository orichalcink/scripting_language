#ifndef KEYWORDS_H
#define KEYWORDS_H

#include <unordered_set>
#include <string>

using namespace std::string_literals;

static inline const std::unordered_set<std::string> keywords
{
   "let"s, "mut"s,
   "def"s, "def"s, "define"s, "define_line"s, "undef"s, "undefine"s,
   "import"s, "include"s,
   "if"s, "ifn"s, "ifdef"s, "ifndef"s, "elif"s, "elifn"s, "elifdef"s, "elifndef"s, "else"s, "endif"s, "then"s
};

#endif // KEYWORDS_H
