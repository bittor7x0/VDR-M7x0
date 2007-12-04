#ifndef __VAR_PARSER_INC__
#define __VAR_PARSER_INC__

#include <string>
#include <vector>
#include <vdr/config.h>

using std::string;
using std::vector;

typedef enum
{
    condEq = 0,
    condNeq
} condOperator;

class cVarParser
{
 public:
    string varName;
    string condEqLeft;
    string condEqRight;
    condOperator condOp;
    string condvarTrue;
    string condvarFalse;
    string compExpr;
    cCommand* cmd;
    string cmdArgs;

    cVarParser() : cmd(NULL) {}
    bool Parse(const string& input);
    bool ParseExp(const string& input);
    bool IsCondExpr();
    bool IsShellCmd();
 private:
    bool ParseAssign(const string& input);
    bool ParseShellCmd(const string& input);
    bool ParseCondExp(const string& input);
    bool ParseEquality(const string& input);
    bool ParseVar(const string& input);
    string Strip(const string& input);
};

#endif
