#include <iostream>
//#include <regex>
#include <sstream>
#include <string>
#include <vector>
// Microsoft C command line arguments parsing rule
// https://docs.microsoft.com/ja-jp/cpp/c-language/parsing-c-command-line-arguments?view=msvc-160

typedef std::vector<std::string> vstring;

//vstring parse_args_regex(const std::string &args_in)
//{
//  vstring ret;
//  std::string args
//      = std::regex_replace(args_in, std::regex("(?:\\\\)*\\\""), "\\");
//  std::cout << args_in << std::endl;
//  std::cout << args << std::endl;
//
//  //     std::regex re("\\s*(\"[^\"]+\"|\'[^\']+\'|[^\\s\"]+|[^\\s\']+)");
//  std::regex re("\\s*(\\S*(?=\")(?:[^\"]|\\\")+\"\\S*|[^\\s\"]+)");
//  //     const std::regex re("(abcde) (fghijk)");
//  auto abegin = std::sregex_iterator(args.begin(), args.end(), re);
//  auto aend = std::sregex_iterator();
//  for (std::sregex_iterator i = abegin; i != aend; ++i)
//    {
//      std::smatch match = *i;
//      ret.emplace_back(match.str());
//    }
//  return ret;
//
//  std::smatch match;
//  std::cout << "args => " << args << std::endl;
//  if (regex_match(args, match, re))
//    {
//      std::cout << "matched: " << std::endl;
//      for (int i = 1; i < match.size(); ++i) { ret.emplace_back(match[i]); }
//    }
//  return ret;
//}

/*!
 * @brief parse_arg 関数
 *
 * コマンドライン引数をパースする関数。以下の仕様に準じた解釈を行う。
 * https://docs.microsoft.com/ja-jp/cpp/c-language/parsing-c-command-line-arguments?view=msvc-160
 *
 * - 引数は、空白 (スペースまたはタブ) で区切る
 * - (追加仕様) 引用符は二重引用符 ( " ) および 単一引用符 ( ' ) をとも
 *   に同様に扱う。
 * - (省略仕様) 最初の引数 ( argv[0] ) は、特別に処理され、これはプログ
 *   ラム名を表す。有効なパス名である必要があるため、一部分を二重引用符
 *   ( " ) で囲むことが許可される。二重引用符は、argv[0] の出力には含ま
 *   れない。
 * - 二重引用符で囲まれた部分については、スペース文字またはタブ文字が引
 *   数の末尾として解釈されない。この一覧で後に示す規則は適用されない。
 * - 二重引用符で囲まれた文字列は、空白が含まれているかどうかにかかわら
 *   ず、1 つの引数として解釈される。(ex. "abc def" -> [abc def])
 * - 二重引用符で囲んだ文字列を引数に埋め込むこともできる。(ex. ab"cd
 *   ef"gh -> [abcd efgh])
 * - (省略仕様) 二重引用符で囲まれたキャレット ( ^ ) は、エスケープ文字
 *   または区切り記号として認識されない。(キャレット ( ^ )とともにPOSIX
 *   等における ( ¥a ) 等のエスケープシーケンスについても特段のしょrは行
 *   わない。)
 * - (省略仕様) 引用符で囲まれた文字列内では、二重引用符のペアは単一のエ
 *   スケープされた二重引用符として解釈される。
 * - 終了の二重引用符が検出される前にコマンドラインが終了した場合は、そ
 *   れまでに読み取られたすべての文字が最後の引数として出力される。
 *   (Linuxなどでは、終了の2重引用符がない場合はエラーとなるが、この関数
 *   ではエラーとしない) (ex. a"b"" c d -> [ab c d] )
 * - バックスラッシュを前に付けた二重引用符 ( \" ) は、リテラルの二重引
 *   用符文字( " ) として解釈される。 (ex. "ab\"c" d -> [ab"c] [d])
 * - 二重引用符の直前にあるバックスラッシュ以外は、バックスラッシュとし
 *   て文字どおり解釈されます。
 * - 二重引用符の直前にバックスラッシュが偶数個あると、バックスラッシュ
 *   のペア ( \\ ) ごとに1 個のバックスラッシュ ( \ ) が argv 配列に配置
 *   されます。この場合、二重引用符( " ) は文字列の区切り記号として解釈
 *   されます。奇数個のバックスラッシュの後に二重引用符がある場合は、
 *   バックスラッシュのペア ( \\ ) ごとに 1 個のバックスラッシュ ( \
 *   ) が argv 配列に配置されます。 二重引用符は残りのバックスラッシュに
 *   よってエスケープ シーケンスとして解釈され、リテラルの二重引用符 (")
 *   が argvに配置されます。
 *   - (ex. a\\\"b c" d e  -> [a\"b] [c d e])
 *   - (ex. a\\\\"b c" d e -> [a\\"b c"] [d] [e])
 */
vstring parse_args(const std::string &args)
{
  bool inArg(false);    // -> " " or "\t"
  bool inEscape(false); // -> "\"
  bool inDquote(false); // -> "\""
  bool inSquote(false); // -> "\'"
  vstring ret;
  std::string anArg;

  for (size_t i(0); i < args.size(); ++i)
    {
      if (args[i] == ' ' || args[i] == '\t')
        {
          // skip escaped spaces and/or spces in quotation
          if (inEscape || inDquote || inSquote)
            {
              anArg.push_back(args[i]);
              goto CONTINUE;
            }
          // skip spaces between args
          if (!inArg) { goto CONTINUE; }
          // end of arg
          if (inArg)
            {
              ret.push_back(anArg);
              anArg.clear();
              inArg = false; // exit arg
              goto CONTINUE;
            }
        }
      inArg = true;

      if (args[i] == '\\')
        {
          if (inEscape) { anArg.push_back(args[i]); }
          inEscape = !inEscape;
          goto CONTINUE;
        }

      if (args[i] == '\"')
        { // escaped (") is stored in arg in any cases
          if (inEscape)
            {
              inEscape = false;
              if (inSquote) { anArg.push_back('\\'); }
              anArg.push_back(args[i]);
              goto CONTINUE;
            }
          // (") in S-quote is stored in arg
          if (inSquote)
            {
              anArg.push_back(args[i]);
              goto CONTINUE;
            }
          // inDquote: enter(false->true), exit(true->false)
          inDquote = !inDquote;
          goto CONTINUE;
        }

      if (args[i] == '\'')
        { // escaped (') is stored in arg in any cases
          if (inEscape)
            {
              inEscape = false;
              if (inDquote) { anArg.push_back('\\'); }
              anArg.push_back(args[i]);
              goto CONTINUE;
            }
          // (') in S-quote is stored in arg
          if (inDquote)
            {
              anArg.push_back(args[i]);
              goto CONTINUE;
            }
          // inSquote: enter(false->true), exit(true->false)
          inSquote = !inSquote;
          goto CONTINUE;
        }

      // here arg[i] != (' ') or (\t) or (") or (')
      if (inEscape)
        {
          inEscape = false;
          if (inDquote || inSquote) { anArg.push_back('\\'); }
        }
      anArg.push_back(args[i]);
      goto CONTINUE;

    CONTINUE:
      if (false)
        {
          std::cout << "args[" << i << "] = " << args[i];
          std::cout << "\t anArg = " << anArg;
          std::cout << "\t\t";
          std::cout << "\tARG:" << inArg ? "T" : "F";
          std::cout << "\tESC:" << inEscape ? "T" : "F";
          std::cout << "\t'\"':" << inDquote ? "T" : "F";
          std::cout << "\t'\'':" << inSquote ? "T" : "F";
          std::cout << std::endl;
        }
    }
  ret.push_back(anArg);
  return ret;
}
int
main(void)
{
  // foo();
  // sreturn 0;
  vstring argvs;
  vstring anss;
  // clang-format off
  vstring arglist = {
    // two args
    "Hello, World!!",          "[Hello,] [World!!]",
    // escaped space
    "Hello,\\ World!!",        "[Hello, World!!]",
    // three args
    "abc def ghijk",            "[abc] [def] [ghijk]",
    // d-quoted args
    // "a b c" d e            -> [a b c] [d] [e]
    "\"a b c\" d e",            "[a b c] [d] [e]",
    // esc(") and quoted (/)
    // "ab\"c" "\\" d        -> [ab"c] [\] [d]
    "\"ab\\\"c\" \"\\\\\" d",   "[ab\"c] [\\] [d]",
    // consecutive backslash and ("") in args 
    // a\\\b d"e f"g h        -> [a\\\b] [de fg] [h]
    "a\\\\\\b d\"e f\"g h",   "[a\\b] [de fg] [h]", 
    // odd number backslash and (")
    // a\\\"b c" d e          -> [a\"b] [c d e] (odd \ + ")
    "a\\\\\\\"b c\" d e",       "[a\\\"b] [c d e]",
    // even number backslash and (") in arg
    // a\\\\"b c" d e         -> [a\\"b c"] [d] [e] (even \ + ")
    "a\\\\\\\\\"b c\" d e",     "[a\\\\b c] [d] [e]",
    // nested quotes (double quoted ('))
    // "a 'b c" "d \'e"       -> [a 'b c] [d \'e]
    "\"a \'b c\" \"d \\\'e\"", "[a \'b c] [d \\\'e]",
    // nested quotes (single quoted ("))
    // 'a \"b c' 'd \"e'      -> [a 'b c] [d \"e]
    "\'a \"b c\' \'d \\\"e\'", "[a \"b c] [d \\\"e]",
    // not closed (")
    // a"b"" c d              -> [ab c d] (error in Linux)
    "a\"b\"\" c d",             "[ab c d]",
  };
  // clang-format on

  for (size_t j = 0; j < arglist.size(); ++j, ++j)
    {
      std::cout << arglist[j] << std::endl;
      std::cout << arglist[j + 1] << std::endl;
      vstring args = parse_args(arglist[j]);
      std::stringstream str;
      for (int i = 0; i < args.size(); ++i)
        {
          str << "[" << args[i] << "] ";
          //          std::cout << "arg[" << i << "] = " << args[i] <<
          //          std::endl;
        }
      std::string ans = str.str();
      std::cout << ans << std::endl;
      ans.pop_back();
      if (ans == arglist[j + 1]) { std::cout << "OK!!" << std::endl; }
      else
        {
          std::cout << "NG" << std::endl;
        }
      std::cout << std::endl;
    }

  return 0;
}
