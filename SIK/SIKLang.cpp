/*
* File:   SIKLang.cpp
* Author: shlomi
*
*/

#include "SIKLang.hpp"
#include <sstream>
namespace sik {

	//Initialize maps:
	std::map<std::string, std::string> SIKLang::LangInverseDelimiter = std::map<std::string, std::string>{};
	std::map<std::string, std::string> SIKLang::LangInverseKeywords = std::map<std::string, std::string>{};
	std::map<std::string, std::string> SIKLang::LangInverseValues = std::map<std::string, std::string>{};
	std::map<std::string, int> SIKLang::LangInverseSystemLib = std::map<std::string, int>{};

	//Set Delimiters:
	std::map<std::string, std::string> SIKLang::LangDelimiter = {
		{ "string"            , "\"" }, // [x]
		{ "string-esc"        , "\\" }, // [x]
		{ "space"             , " " },	// [x]
		{ "plus"              , "+" },	// [x]
		{ "minus"             , "-" },	// [x]
		{ "inc"			      , "++" }, // [x]
		{ "dec"				  , "--" }, // [x]
		{ "multi"             , "*" },	// [x]
		{ "divide"            , "/" },	// [x]
		{ "equal"             , "=" },	// [x]
		{ "equal-add"         , "+=" }, // [x]
		{ "equal-sub"         , "-=" }, // [x]
		{ "pointer"           , "->" }, // []
		{ "braketOpen"        , "(" },	// [x]
		{ "braketClose"       , ")" },	// [x]
		{ "bracesOpen"        , "{" },	// [x]
		{ "bracesClose"       , "}" },	// [x]
		{ "sBraketOpen"       , "[" },	// []
		{ "sBraketClose"      , "]" },	// []
		{ "power"             , "^" },	// []
		{ "exclamation"       , "!" },	// []
		{ "greater"           , ">" },	// []
		{ "smaller"           , "<" },	// []
		{ "greater-equal"     , ">=" }, // []
		{ "smaller-equal"     , "<=" }, // []
		{ "comma"             , "," },  // [x]
		{ "scope"             , "~" },	// [x]
		{ "c-tequal"          , "=~" }, // [x]
		{ "c-ntequal"         , "!~" }, // [x]
		{ "c-equal"           , "==" }, // [x]
		{ "c-nequal"          , "!=" }, // [x]
		{ "and"               , "&&" }, // [x]
		{ "single-and"        , "&" },	// []
		{ "or"                , "||" }, // [x]
		{ "single-or"         , "|" },	// []
		{ "semicolon"         , ";" },	// [x]
		{ "macro-def"		  , "#" },	// []
		{ "macro-set"		  , ":" },	// []
		{ "child-set"		  , ":" },	// []
		{ "member-access"	  , "." }	// []
	};

	//Set Keywords:
	std::map<std::string, std::string> SIKLang::LangKeywords = {
		{ "variable"        , "let" },		// [x]
		{ "unset"           , "unset" },	// []
		{ "cond-if"         , "if" },		// [x]
		{ "cond-else"       , "else" },		// [x]
		{ "cond-elseif"     , "elseif" },	// [x]
		{ "cond-break"      , "breakif" },	// []
		{ "loop-while"      , "while" },	// []
		{ "loop-for"        , "for" },		// []
		{ "loop-each"       , "each" },		// []
		{ "loop-break"      , "break" },	// []
		{ "loop-continue"   , "continue" }, // []
		{ "function"        , "function" }, // []
		{ "return"          , "return" },	// []
		{ "print"           , "print" },	// [x]
		{ "get"             , "get" },		// []
		{ "exit"            , "exit" },		// []
		{ "sleep"           , "sleep" },	// []
		{ "pause"           , "pause" },	// []
	};

	//Set Values - set both smaller case and upper case:
	std::map<std::string, std::string> SIKLang::LangValues = {
		{ "true-lower"        , "true" },
		{ "true-upper"        , "TRUE" },
		{ "false-lower"       , "false" },
		{ "false-upper"       , "FALSE" },
		{ "null-lower"		  , "null" },
		{ "null-upper"        , "NULL" },

		{ "garbage-lower"     , "GC" },
		{ "garbage-upper"     , "gc" },
		{ "rst-lower"		  , "rst" },
		{ "rst-upper"         , "RST" }
	};


	//Set chars allowed as names:
	//if we do not define '"' as a character, no tokens will be generated for string quoetation marks
	std::vector<char> SIKLang::LangNamingAllowedChars = {
		'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
		'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',
		'_'
	};

	std::vector<char> SIKLang::LangDigits = {
		'0','1','2','3','4','5','6','7','8','9','.'
	};

	//Set allowed extensions:
	std::vector<std::string> SIKLang::extensionLib = {
		"sik",
		"SIK",
		"tsik",
		"TSIK"
	};

	//Set systemCalls extensions:
	std::map<int, std::string> SIKLang::LangSystemLib = {
		{ 1,  "rep" },
		{ 2,  "length" },
		{ 3,  "type" },
		{ 4,  "isNull" },
		{ 5,  "isPointer" },
		{ 6,  "isPointed" },
		{ 7,  "substr" },
		{ 8,  "join" },
		{ 9,  "split" },
		{ 10, "sum" },
		{ 11, "highest" },
		{ 12, "lowest" },
		{ 13, "tolower" },
		{ 14, "toupper" }
	};

	SIKLang::SIKLang() {
		//Inverse Delimiter:    
		for (auto const &ent1 : LangDelimiter) {
			LangInverseDelimiter.insert(std::pair<std::string, std::string>(ent1.second, ent1.first));
		}
		//Inverse Keywords:    
		for (auto const &ent2 : LangKeywords) {
			LangInverseKeywords.insert(std::pair<std::string, std::string>(ent2.second, ent2.first));
		}
		//Inverse Values naming:
		for (auto const &ent3 : LangValues) {
			LangInverseValues.insert(std::pair<std::string, std::string>(ent3.second, ent3.first));
		}
		//Inverse System functions:
		for (auto const &ent4 : LangSystemLib) {
			LangInverseSystemLib.insert(std::pair<std::string, int>(ent4.second, ent4.first));
		}

		//Populate the language definition:
		LangPopulate();
	}
	SIKLang::SIKLang(const SIKLang& orig) {

	}
	SIKLang::~SIKLang() {

	}


	char SIKLang::LangGeneralSpace = ' ';
	char SIKLang::LangStringIndicator = ' ';
	char SIKLang::LangSubObject = ' ';
	char SIKLang::LangFunctionOpenArguChar = ' ';
	char SIKLang::LangFunctionCloseArguChar = ' ';
	char SIKLang::LangBlockOpenChar = ' ';
	char SIKLang::LangBlockCloseChar = ' ';
	char SIKLang::LangArrayOpenChar = ' ';
	char SIKLang::LangArrayCloseChar = ' ';
	char SIKLang::LangArgumentSpacer = ' ';
	char SIKLang::LangStringEscape = ' ';
	char SIKLang::LangOperationEnd = ' ';
	char SIKLang::LangMacroIndicator = ' ';
	char SIKLang::LangMacroSetChar = ' ';
	char SIKLang::LangChildSetChar = ' ';
	char SIKLang::LangScopeChar    = ' ';

	std::string SIKLang::dicLang_space = "";
	std::string SIKLang::dicLang_plus = "";
	std::string SIKLang::dicLang_minus = "";
	std::string SIKLang::dicLang_inc = "";
	std::string SIKLang::dicLang_dec = "";
	std::string SIKLang::dicLang_multi = "";
	std::string SIKLang::dicLang_divide = "";
	std::string SIKLang::dicLang_equal = "";
	std::string SIKLang::dicLang_equalAdd = "";
	std::string SIKLang::dicLang_equalSub = "";
	std::string SIKLang::dicLang_pointer = "";
	std::string SIKLang::dicLang_braketOpen = "";
	std::string SIKLang::dicLang_braketClose = "";
	std::string SIKLang::dicLang_bracesOpen = "";
	std::string SIKLang::dicLang_bracesClose = "";
	std::string SIKLang::dicLang_sBraketOpen = "";
	std::string SIKLang::dicLang_sBraketClose = "";
	std::string SIKLang::dicLang_power = "";
	std::string SIKLang::dicLang_exclamation = "";
	std::string SIKLang::dicLang_greater = "";
	std::string SIKLang::dicLang_smaller = "";
	std::string SIKLang::dicLang_greater_equal = "";
	std::string SIKLang::dicLang_smaller_equal = "";
	std::string SIKLang::dicLang_comma = "";
	std::string SIKLang::dicLang_scope = "";
	std::string SIKLang::dicLang_c_tequal = "";
	std::string SIKLang::dicLang_c_ntequal = "";
	std::string SIKLang::dicLang_c_equal = "";
	std::string SIKLang::dicLang_c_nequal = "";
	std::string SIKLang::dicLang_and = "";
	std::string SIKLang::dicLang_or = "";
	std::string SIKLang::dicLang_s_and = "";
	std::string SIKLang::dicLang_s_or = "";
	std::string SIKLang::dicLang_semicolon = "";
	std::string SIKLang::dicLang_member_access = "";
	std::string SIKLang::dicLang_childSet = "";

	//String based keywords:
	std::string SIKLang::dicLangKey_variable = "";
	std::string SIKLang::dicLangKey_unset = "";
	std::string SIKLang::dicLangKey_sub_object = "";
	std::string SIKLang::dicLangKey_cond_if = "";
	std::string SIKLang::dicLangKey_cond_else = "";
	std::string SIKLang::dicLangKey_cond_elseif = "";
	std::string SIKLang::dicLangKey_cond_break = "";
	std::string SIKLang::dicLangKey_loop_while = "";
	std::string SIKLang::dicLangKey_loop_for = "";
	std::string SIKLang::dicLangKey_loop_each = "";
	std::string SIKLang::dicLangKey_loop_break = "";
	std::string SIKLang::dicLangKey_loop_continue = "";
	std::string SIKLang::dicLangKey_function = "";
	std::string SIKLang::dicLangKey_return = "";
	std::string SIKLang::dicLangKey_print = "";
	std::string SIKLang::dicLangKey_get = "";
	std::string SIKLang::dicLangKey_exit = "";
	std::string SIKLang::dicLangKey_sleep = "";
	std::string SIKLang::dicLangKey_pause = "";

	//Values based naming:
	std::string SIKLang::dicLangValue_true_lower = "";
	std::string SIKLang::dicLangValue_true_upper = "";
	std::string SIKLang::dicLangValue_false_lower = "";
	std::string SIKLang::dicLangValue_false_upper = "";
	std::string SIKLang::dicLangValue_null_lower = "";
	std::string SIKLang::dicLangValue_null_upper = "";

	//Internal values naming:
	std::string SIKLang::dicLangValue_garbage_upper = "GC";
	std::string SIKLang::dicLangValue_garbage_lower = "gc";
	std::string SIKLang::dicLangValue_rst_upper = "RST";
	std::string SIKLang::dicLangValue_rst_lower = "rst";

	/** Populate the language to cache symbols:
	*
	*/
	void SIKLang::LangPopulate() {

		//Char based:
		LangGeneralSpace = LangFindDelimiter("space")[0];
		LangStringIndicator = LangFindDelimiter("string")[0];
		LangSubObject = LangFindKeyword("sub-object")[0];
		LangFunctionOpenArguChar = LangFindDelimiter("braketOpen")[0];
		LangFunctionCloseArguChar = LangFindDelimiter("braketClose")[0];
		LangBlockOpenChar = LangFindDelimiter("bracesOpen")[0];
		LangBlockCloseChar = LangFindDelimiter("bracesClose")[0];
		LangArrayOpenChar = LangFindDelimiter("sBraketOpen")[0];
		LangArrayCloseChar = LangFindDelimiter("sBraketClose")[0];
		LangArgumentSpacer = LangFindDelimiter("comma")[0];
		LangStringEscape = LangFindDelimiter("string-esc")[0];
		LangOperationEnd = LangFindDelimiter("semicolon")[0];
		LangMacroIndicator = LangFindDelimiter("macro-def")[0];
		LangMacroSetChar = LangFindDelimiter("macro-set")[0];
		LangChildSetChar = LangFindDelimiter("child-set")[0];
		LangScopeChar    = LangFindDelimiter("scope")[0];

		//String based:
		dicLang_space = LangFindDelimiter("space");
		dicLang_plus = LangFindDelimiter("plus");
		dicLang_minus = LangFindDelimiter("minus");
		dicLang_inc = LangFindDelimiter("inc");
		dicLang_dec = LangFindDelimiter("dec");
		dicLang_multi = LangFindDelimiter("multi");
		dicLang_divide = LangFindDelimiter("divide");
		dicLang_equal = LangFindDelimiter("equal");
		dicLang_equalAdd = LangFindDelimiter("equal-add");
		dicLang_equalSub = LangFindDelimiter("equal-sub");
		dicLang_pointer = LangFindDelimiter("pointer");
		dicLang_braketOpen = LangFindDelimiter("braketOpen");
		dicLang_braketClose = LangFindDelimiter("braketClose");
		dicLang_bracesOpen = LangFindDelimiter("bracesOpen");
		dicLang_bracesClose = LangFindDelimiter("bracesClose");
		dicLang_sBraketOpen = LangFindDelimiter("sBraketOpen");
		dicLang_sBraketClose = LangFindDelimiter("sBraketClose");
		dicLang_power = LangFindDelimiter("power");
		dicLang_exclamation = LangFindDelimiter("exclamation");
		dicLang_greater = LangFindDelimiter("greater");
		dicLang_smaller = LangFindDelimiter("smaller");
		dicLang_greater_equal = LangFindDelimiter("greater-equal");
		dicLang_smaller_equal = LangFindDelimiter("smaller-equal");
		dicLang_comma = LangFindDelimiter("comma");
		dicLang_scope = LangFindDelimiter("scope");
		dicLang_c_tequal = LangFindDelimiter("c-tequal");
		dicLang_c_ntequal = LangFindDelimiter("c-ntequal");
		dicLang_c_equal = LangFindDelimiter("c-equal");
		dicLang_c_nequal = LangFindDelimiter("c-nequal");
		dicLang_and = LangFindDelimiter("and");
		dicLang_or = LangFindDelimiter("or");
		dicLang_s_and = LangFindDelimiter("single-and");
		dicLang_s_or = LangFindDelimiter("single-or");
		dicLang_semicolon = LangFindDelimiter("semicolon");
		dicLang_member_access = LangFindDelimiter("member-access");
		dicLang_childSet = LangFindDelimiter("child-set");

		//Keywords:
		dicLangKey_variable = LangFindKeyword("variable");
		dicLangKey_unset = LangFindKeyword("unset");
		dicLangKey_sub_object = LangFindKeyword("sub-object");
		dicLangKey_cond_if = LangFindKeyword("cond-if");
		dicLangKey_cond_else = LangFindKeyword("cond-else");
		dicLangKey_cond_elseif = LangFindKeyword("cond-elseif");
		dicLangKey_cond_break = LangFindKeyword("cond-break");
		dicLangKey_loop_while = LangFindKeyword("loop-while");
		dicLangKey_loop_for = LangFindKeyword("loop-for");
		dicLangKey_loop_each = LangFindKeyword("loop-each");
		dicLangKey_loop_break = LangFindKeyword("loop-break");
		dicLangKey_loop_continue = LangFindKeyword("loop-continue");
		dicLangKey_function = LangFindKeyword("function");
		dicLangKey_return = LangFindKeyword("return");
		dicLangKey_print = LangFindKeyword("print");
		dicLangKey_get = LangFindKeyword("get");
		dicLangKey_exit = LangFindKeyword("exit");
		dicLangKey_sleep = LangFindKeyword("sleep");
		dicLangKey_pause = LangFindKeyword("pause");
		
		//Values based naming:
		dicLangValue_true_lower = LangFindValueNaming("true-lower");
		dicLangValue_true_upper = LangFindValueNaming("true-upper");
		dicLangValue_false_lower = LangFindValueNaming("false-lower");
		dicLangValue_false_upper = LangFindValueNaming("false-upper");
		dicLangValue_null_lower = LangFindValueNaming("null-lower");
		dicLangValue_null_upper = LangFindValueNaming("null-upper");

		//Internal values naming:
		dicLangValue_garbage_lower = LangFindValueNaming("garbage-lower");
		dicLangValue_garbage_upper = LangFindValueNaming("garbage-upper");
		dicLangValue_rst_lower = LangFindValueNaming("rst-lower");
		dicLangValue_rst_upper = LangFindValueNaming("rst-upper");

	}
	/** Find the character that represent a delimiter name:
	*
	* @param string key
	* @return char
	*/
	std::string SIKLang::LangFindDelimiter(const std::string& key) {
		if (LangHasKeyDelimiter(key)) {
			return LangDelimiter.at(key);
		}
		return "";
	}
	/** Check whether a key delimiter is set:
	*
	* @param string key key name
	* @return bool
	*/
	bool SIKLang::LangHasKeyDelimiter(const std::string& key) {
		return LangDelimiter.count(key) == 1;
	}
	/** Find the string that represent a value name:
	*
	* @param string key
	* @return char
	*/
	std::string SIKLang::LangFindValueNaming(const std::string& key) {
		if (LangHasValueNaming(key)) {
			return LangValues.at(key);
		}
		return "";
	}
	/** Check whether a string value name is set:
	*
	* @param string key key name
	* @return bool
	*/
	bool SIKLang::LangHasValueNaming(const std::string& key) {
		return LangValues.count(key) == 1;
	}
	/** Check whether a string value name is BOOLEAN type of lang Definition:
	*
	* @param string key key name
	* @return int -> -1 no, 1 true, 0 false.
	*/
	int SIKLang::LangIsBoolean(const std::string& key) {
		if (key == SIKLang::LangValues["true-lower"])
			return 1;
		if (key == SIKLang::LangValues["false-lower"])
			return 0;
		if (key == SIKLang::LangValues["true-upper"])
			return 1;
		if (key == SIKLang::LangValues["false-upper"])
			return 0;
		return -1;
	}
	/** Check whether a string value name is NULL type of lang definition:
	*
	* @param string key key name
	* @return bool
	*/
	bool SIKLang::LangIsNull(const std::string& key) {
		if (key == SIKLang::LangValues["null-lower"])
			return true;
		if (key == SIKLang::LangValues["null-upper"])
			return true;
		return false;
	}

	/** Find the int that represent a function name:
	*
	* @param string key
	* @return char
	*/
	int SIKLang::LangFindSystemLib(const std::string& key) {
		if (key.empty()) return -1;
		if (LangHasSystemLib(key)) {
			return LangInverseSystemLib.at(key);
		}
		return -1;
	}
	/** Check whether a string function name is set:
	*
	* @param string key key name
	* @return bool
	*/
	bool SIKLang::LangHasSystemLib(const std::string& key) {
		return LangInverseSystemLib.count(key) == 1;
	}


	/** Check whether a char delimiter is an actual register delimiter:
	*
	* @param string key key name
	* @return bool
	*/
	bool SIKLang::LangIsDelimiter(const std::string& value) {
		char test = value[0];
		if (test == dicLang_space[0] && value == dicLang_space) return true;
		if (test == dicLang_plus[0] && value == dicLang_plus) return true;
		if (test == dicLang_minus[0] && value == dicLang_minus) return true;
		if (test == dicLang_inc[0] && value == dicLang_inc) return true;
		if (test == dicLang_dec[0] && value == dicLang_dec) return true;
		if (test == dicLang_multi[0] && value == dicLang_multi) return true;
		if (test == dicLang_divide[0] && value == dicLang_divide) return true;
		if (test == dicLang_equal[0] && value == dicLang_equal) return true;
		if (test == dicLang_equalAdd[0] && value == dicLang_equalAdd) return true;
		if (test == dicLang_equalSub[0] && value == dicLang_equalSub) return true;
		if (test == dicLang_pointer[0] && value == dicLang_pointer) return true;
		if (test == dicLang_braketOpen[0] && value == dicLang_braketOpen) return true;
		if (test == dicLang_braketClose[0] && value == dicLang_braketClose) return true;
		if (test == dicLang_bracesOpen[0] && value == dicLang_bracesOpen) return true;
		if (test == dicLang_bracesClose[0] && value == dicLang_bracesClose) return true;
		if (test == dicLang_sBraketOpen[0] && value == dicLang_sBraketOpen) return true;
		if (test == dicLang_sBraketClose[0] && value == dicLang_sBraketClose) return true;
		if (test == dicLang_power[0] && value == dicLang_power) return true;
		if (test == dicLang_exclamation[0] && value == dicLang_exclamation) return true;
		if (test == dicLang_greater[0] && value == dicLang_greater) return true;
		if (test == dicLang_smaller[0] && value == dicLang_smaller) return true;
		if (test == dicLang_greater_equal[0] && value == dicLang_greater_equal) return true;
		if (test == dicLang_smaller_equal[0] && value == dicLang_smaller_equal) return true;
		if (test == dicLang_comma[0] && value == dicLang_comma) return true;
		if (test == dicLang_scope[0] && value == dicLang_scope) return true;
		if (test == dicLang_c_tequal[0] && value == dicLang_c_tequal) return true;
		if (test == dicLang_c_ntequal[0] && value == dicLang_c_ntequal) return true;
		if (test == dicLang_c_equal[0] && value == dicLang_c_equal) return true;
		if (test == dicLang_c_nequal[0] && value == dicLang_c_nequal) return true;
		if (test == dicLang_and[0] && value == dicLang_and) return true;
		if (test == dicLang_or[0] && value == dicLang_or) return true;
		if (test == dicLang_s_and[0] && value == dicLang_s_and) return true;
		if (test == dicLang_s_or[0] && value == dicLang_s_or) return true;
		if (test == dicLang_semicolon[0] && value == dicLang_semicolon) return true;
		if (test == dicLang_member_access[0] && value == dicLang_member_access) return true;
		if (test == dicLang_childSet[0] && value == dicLang_childSet) return true;
		if (test == LangOperationEnd) return true;

		return false;
	}
	bool SIKLang::LangIsDelimiter(const char& value) {
		return LangIsDelimiter(std::string(1, value));
	}
	/** Checks if a char|string is a delimiter of comparison:
	*
	* @param char|string value
	* @return boolean
	*
	*/
	bool SIKLang::LangIsComparison(const char& value) {
		return LangIsComparison(std::string(1, value));
	}
	bool SIKLang::LangIsComparison(const std::string& value) {
		char test = value[0];
		if (test == dicLang_greater[0] && value == dicLang_greater) return true;
		if (test == dicLang_smaller[0] && value == dicLang_smaller) return true;
		if (test == dicLang_greater_equal[0] && value == dicLang_greater_equal) return true;
		if (test == dicLang_smaller_equal[0] && value == dicLang_smaller_equal) return true;
		if (test == dicLang_c_tequal[0] && value == dicLang_c_tequal) return true;
		if (test == dicLang_c_ntequal[0] && value == dicLang_c_ntequal) return true;
		if (test == dicLang_c_equal[0] && value == dicLang_c_equal) return true;
		if (test == dicLang_c_nequal[0] && value == dicLang_c_nequal) return true;
		return false;
	}
	/** Checks if a char|string is a delimiter of a condition such as AND OR etc:
	*
	* @param char|string value
	* @return boolean
	*
	*/
	bool SIKLang::LangIsOfCondition(const char& value) {
		return LangIsOfCondition(std::string(1, value));
	}
	bool SIKLang::LangIsOfCondition(const std::string& value) {
		if (value[0] == dicLang_and[0] && value == dicLang_and) return true;
		if (value[0] == dicLang_or[0] && value == dicLang_or) return true;
		return false;
	}
	/** Find the string that represent a keyword name (key):
	*
	* @param string key
	* @return string
	*/
	std::string SIKLang::LangFindKeyword(const std::string& key) {
		if (LangHasKeyKeyword(key)) {
			return LangKeywords.at(key);
		}
		return "";
	}
	/** Check whether a key keyword is set:
	*
	* @param string key key name
	* @return bool
	*/
	bool SIKLang::LangHasKeyKeyword(const std::string& key) {
		return LangKeywords.count(key) == 1;
	}
	/** Check whether a string is an actual register keyword:
	*
	* @param string key key name
	* @return bool
	*/
	bool SIKLang::LangIsKeyword(const std::string& value) {
		return LangInverseKeywords.count(value) == 1;
	}
	/** Check whether a key is an actual object callee keyword:
	*
	* @param char value
	* @return bool
	*/
	bool SIKLang::LangIsObjectCall(const char& value) {
		std::string str(1, value);
		if (str == dicLang_member_access) {
			return true;
		}
		return false;
	}
	/** Checks whether a letter is allowed as function or variable name:
	*
	* @param char|string[0] value
	* @return bool
	*/
	bool SIKLang::LangIsNamingAllowed(const char& value) {
		if (find(LangNamingAllowedChars.begin(), LangNamingAllowedChars.end(), value) != LangNamingAllowedChars.end()) {
			return true;
		}
		return false;
	}
	bool SIKLang::LangIsNamingAllowed(const std::string& value) {
		if (find(LangNamingAllowedChars.begin(), LangNamingAllowedChars.end(), value[0]) != LangNamingAllowedChars.end()) {
			return true;
		}
		return false;
	}

	bool SIKLang::LangIsNaming(const std::string& value) {
		for (unsigned int i = 0; i < value.length(); i++) {
			if (!LangIsNamingAllowed(value[i])) return false;
		}
		return true;
	}
	bool SIKLang::LangIsNumberAllowed(const char& value) {
		if (find(LangDigits.begin(), LangDigits.end(), value) != LangDigits.end()) {
			return true;
		}
		return false;
	}
	bool SIKLang::LangIsNumber(const std::string& value) {
		for (unsigned int i = 0; i < value.length(); i++) {
			if (!LangIsNumberAllowed(value[i])) return false;
		}
		return true;
	}
	/*
	* PRINTING STUFF:
	*/
	std::string SIKLang::toString(int value) {
		std::stringstream ss;
		ss << value;
		return ss.str();
	}
	void SIKLang::printHeader(std::string headername) {
		std::cout << std::endl << "-------------------------------------------------------------------" << std::endl;
		std::cout << "| SIK Debugger -> " << headername << std::endl;
		printSepLine(2);
	}
	void SIKLang::printSepLine(int breaks) {
		std::cout << "-------------------------------------------------------------------";
		for (int i = 0; i < breaks; i++) {
			std::cout << std::endl;
		}
	}
	void SIKLang::printEmpLine(int breaks) {
		for (int i = 0; i < breaks; i++) {
			std::cout << std::endl;
		}
	}

}