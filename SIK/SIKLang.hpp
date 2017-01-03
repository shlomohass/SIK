/*
* File:   SIKLANG.H
* Author: shlomi
*
*/

#ifndef SIKLANG_H
#define	SIKLANG_H


#include <string>
#include <iostream>
#include <algorithm>
#include <vector>
#include <map>

//Important Language Macros:
namespace sik
{

	class SIKLang {
	public:
		SIKLang();
		SIKLang(const SIKLang& orig);

		//Allowed extensions:
		static std::vector<std::string> extensionLib;

		//Delimiters Definition and functions:
		static std::map<std::string, std::string> LangDelimiter;
		static std::map<std::string, std::string> LangInverseDelimiter;
		static std::string LangFindDelimiter(const std::string& key);
		static bool LangHasKeyDelimiter(const std::string& key);
		static bool LangIsDelimiter(const char& value);
		static bool LangIsDelimiter(const std::string& value);
		static bool LangIsComparison(const char& value);
		static bool LangIsComparison(const std::string& value);
		static bool LangIsOfCondition(const char& value);
		static bool LangIsOfCondition(const std::string& value);

		//Digits Definition and functions:
		static std::vector<char> LangDigits;
		static bool LangIsNumberAllowed(const char& value);
		static bool LangIsNumber(const std::string& value);

		//Keywords Definition and functions:
		static std::map<std::string, std::string>     LangKeywords;
		static std::map<std::string, std::string>     LangInverseKeywords;
		static std::string LangFindKeyword(const std::string& key);
		static bool		   LangHasKeyKeyword(const std::string& key);
		static bool		   LangIsKeyword(const std::string& value);
		static bool		   LangIsObjectCall(const char& value);
		static void		   LangPopulate();

		//Naming system Definition and functions:
		static std::vector<char> LangNamingAllowedChars;
		static bool              LangIsNamingAllowed(const std::string& value);
		static bool              LangIsNamingAllowed(const char& value);
		static bool              LangIsNaming(const std::string& value);

		//Value and types syntax
		static std::map<std::string, std::string>     LangValues;
		static std::map<std::string, std::string>     LangInverseValues;
		static std::string LangFindValueNaming(const std::string& key);
		static bool LangHasValueNaming(const std::string& key);
		static int LangIsBoolean(const std::string& key);
		static bool LangIsNull(const std::string& key);

		//System function constants:
		static std::map<int, std::string> LangSystemLib;
		static std::map<std::string, int> LangInverseSystemLib;
		static int LangFindSystemLib(const std::string& key);
		static bool LangHasSystemLib(const std::string& key);

		//Define constant definition characters this is auto set and will insure speed up drasticly:

		//Char based:
		static char LangGeneralSpace;
		static char LangStringIndicator;
		static char LangSubObject;
		static char LangFunctionOpenArguChar;
		static char LangFunctionCloseArguChar;
		static char LangBlockOpenChar;
		static char LangBlockCloseChar;
		static char LangArrayOpenChar;
		static char LangArrayCloseChar;
		static char LangArgumentSpacer;
		static char LangStringEscape;
		static char LangOperationEnd;
		static char LangMacroIndicator;
		static char LangMacroSetChar;
		static char LangChildSetChar;
		static char LangScopeChar;

		//String based language symbols and keywords:
		static std::string dicLang_space;
		static std::string dicLang_plus;
		static std::string dicLang_minus;
		static std::string dicLang_multi;
		static std::string dicLang_inc;
		static std::string dicLang_dec;
		static std::string dicLang_divide;
		static std::string dicLang_equal;
		static std::string dicLang_equalAdd;
		static std::string dicLang_equalSub;
		static std::string dicLang_pointer;
		static std::string dicLang_braketOpen;
		static std::string dicLang_braketClose;
		static std::string dicLang_bracesOpen;
		static std::string dicLang_bracesClose;
		static std::string dicLang_sBraketOpen;
		static std::string dicLang_sBraketClose;
		static std::string dicLang_power;
		static std::string dicLang_exclamation;
		static std::string dicLang_greater;
		static std::string dicLang_smaller;
		static std::string dicLang_greater_equal;
		static std::string dicLang_smaller_equal;
		static std::string dicLang_comma;
		static std::string dicLang_scope;
		static std::string dicLang_c_tequal;
		static std::string dicLang_c_ntequal;
		static std::string dicLang_c_equal;
		static std::string dicLang_c_nequal;
		static std::string dicLang_and;
		static std::string dicLang_or;
		static std::string dicLang_s_and;
		static std::string dicLang_s_or;
		static std::string dicLang_semicolon;
		static std::string dicLang_objcall;
		static std::string dicLang_childSet;

		//String based keywords:
		static std::string dicLangKey_variable;
		static std::string dicLangKey_unset;
		static std::string dicLangKey_sub_object;
		static std::string dicLangKey_cond_if;
		static std::string dicLangKey_cond_else;
		static std::string dicLangKey_cond_elseif;
		static std::string dicLangKey_cond_break;
		static std::string dicLangKey_loop_while;
		static std::string dicLangKey_loop_for;
		static std::string dicLangKey_loop_each;
		static std::string dicLangKey_loop_break;
		static std::string dicLangKey_loop_continue;
		static std::string dicLangKey_function;
		static std::string dicLangKey_return;
		static std::string dicLangKey_print;
		static std::string dicLangKey_get;
		static std::string dicLangKey_exit;
		static std::string dicLangKey_sleep;
		static std::string dicLangKey_pause;

		//Values based naming:
		static std::string dicLangValue_true_lower;
		static std::string dicLangValue_true_upper;
		static std::string dicLangValue_false_lower;
		static std::string dicLangValue_false_upper;
		static std::string dicLangValue_null_lower;
		static std::string dicLangValue_null_upper;

		//Internal values naming:
		static std::string dicLangValue_garbage_upper;
		static std::string dicLangValue_garbage_lower;
		static std::string dicLangValue_rst_upper;
		static std::string dicLangValue_rst_lower;

		virtual ~SIKLang();

		//Printing stuff:
		static void printHeader(std::string headername);
		static void printSepLine(int breaks);
		static void printEmpLine(int breaks);

	};
}
#endif	/* SIKLANG_H */
