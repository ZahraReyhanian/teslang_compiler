/*
    prog -> func | func prog
    func -> func-type iden ( flist ) { body } |
    body -> stmt |
            stmt body
    stmt -> expr ; |
            defvar ; |
            if ( expr ) stmt |
            if ( expr ) stmt else stmt |
            while  ( expr ) stmt |
            for (iden in expr ) stmt |
            return expr; |
            { body }
    defvar -> type iden

    expr -> assign-expr
    assign-expr -> cond-expr |
                    unary-expr  = assign-expr
    cond-expr -> or-expr
    or-expr -> and-expr |
                or-expr || and-expr
    and-expr -> equ-expr |
                and-expr && equ-expr
    equ-expr -> relational-expr |
                equ-expr == relational-expr
                equ-expr != relational-expr
    relational-expr -> add-expr |
                        relational-expr < add-expr |
                        relational-expr > add-expr |
                        relational-expr <= add-expr |
                        relational-expr >= add-expr
    add-expr -> mul-expr |
                add-expr + mul-expr
                add-expr - mul-expr
    mul-expr -> unary-expr |
                mul-expr * unary-expr |
                mul-expr / unary-expr |
                mul-expr % unary-expr 

    unary-expr -> postfix-expr |
                    unary-oprator unary-expr
    postfix-expr -> prim-expr  |
                    postfix-expr [expr]
    prim-expr -> iden |
                iden ( clist ) |
                num |
                (expr)
    unary-oprator -> + |
                    - |
                    !

    flist ->  |
            type iden |
            type iden , flist
    clist -> |
            expr |
            expr , clist
    func-type -> num |
                list |
                nil
    type -> num |
            list
    num -> [0-9]+
    iden -> [a-zA-Z_][a-zA-Z_0-9]
*/

#include <iostream>
#include <fstream>
#include <string>
#include <regex>
using namespace std;

char c;
ifstream file;
ofstream irfile;
string token;
int num_err;
int count_line;
string IF = "if", ELSE = "else", IN = "in", WHILE = "while", FOR = "for", RETURN = "return", LIST = "list", NUM = "num", NIL = "nil",
NUMREAD = "numread", NUMPRINT = "numprint", LISTLEN = "listlen", EXIT = "exit", MAKELIST = "makelist";//main
bool ret;
string return_type;
int label_number = -1;
int reg_number = -1;
bool isexpr = false;

struct valStruct
{
    int val;
    int index;
};

struct table
{
    string var;
    vector<valStruct> value;
    string type;
    bool isFunc;
    int numberOfparam;
    int reg = -1;
    vector<table> list;

};
vector<table> symbolTable;

struct exprVal
{
    string tok;
    string type;
    bool isFunc;
    vector<valStruct> value;
    int reg = -1;
    int index = 0;
};

string label();
string reg();
char getChar();
void ungetchar();
int isOpChar(char ch);
int next(char expected);
string checkOp(char ch);
string getString(string answer, char ch);
void skipComment();
char skipSpace(char ch);
string nextToken();
string getToken();
void syntax_error(string err);
void dropToken();
bool isFileEnd();
bool isKey(string tok);
bool isIdentifier(string tok);
bool isNum(string tok);
int isInSymbolTable(string name, bool isFunc);
void prog();
void func();
void body();
void stmt();
void semicol();
void if_stmt();
void while_stmt();
void for_stmt();
void defvar();
exprVal expr();
exprVal assign_expr();
exprVal cond_expr(exprVal temp);// exprVal temp ===>> temp is the token in the left of = e.g a = b ... temp is a
exprVal or_expr(exprVal temp);
exprVal and_expr(exprVal temp);
exprVal equ_expr(exprVal temp);
exprVal relational_expr(exprVal temp);
exprVal add_expr(exprVal temp);
exprVal mul_expr(exprVal temp);
exprVal unary_expr(exprVal temp);
exprVal postfix_expr(exprVal temp);
exprVal prim_expr(exprVal temp);
void unary_oprator();
int flist(vector<table> &list);
void clist(string var, int i, exprVal temp);
string type();
string func_type();
void num();
void iden();
bool check_same_type(string type, int i, int j);

void generateCodeFucDef(string name);
void generateCodeReturn(exprVal exp);
void generateCodeCall(string var, exprVal temp, int param, int i);
void generateCodeClist(exprVal *item, bool isLast);
void generateCodeMov(exprVal e1, exprVal e2);
void generateCodeArith(exprVal res, exprVal e1, exprVal e2 , string mode);
void generateCodeComp(exprVal res, exprVal e1, exprVal e2 , string mode);

int main () {
    file.open ("grammer.txt");
    if(! file){
        cout << "file didn't open successfully !";
        exit(1);
    }
    else
    {
        cout << "file opened successfully !\n";
    }

    table number;
    number.var = "n";
    number.type = NUM;
    number.isFunc = false;
    number.numberOfparam = 0;

    table list;
    list.var = "v";
    list.type = LIST;
    list.isFunc = false;
    list.numberOfparam = 0;

    table numread;
    numread.var = NUMREAD;
    numread.type = NUM;
    numread.isFunc = true;
    numread.numberOfparam = 0;

    table numprint;
    numprint.var = NUMPRINT;
    numprint.type = NIL;
    numprint.isFunc = true;
    numprint.numberOfparam = 1;
    numprint.list.push_back(number);

    table makelist;
    makelist.var = MAKELIST;
    makelist.type = LIST;
    makelist.isFunc = true;
    makelist.numberOfparam = 1;
    makelist.list.push_back(number);

    table listlen;
    listlen.var = LISTLEN;
    listlen.type = NUM;
    listlen.isFunc = true;
    listlen.numberOfparam = 1;
    listlen.list.push_back(list);

    table exit;
    exit.var = EXIT;
    exit.type = NIL;
    exit.isFunc = true;
    exit.numberOfparam = 1;
    exit.list.push_back(number);

    symbolTable.push_back(numread);
    symbolTable.push_back(numprint);
    symbolTable.push_back(makelist);
    symbolTable.push_back(listlen);
    symbolTable.push_back(exit);

    count_line = 1;
    token = nextToken();
    num_err = 0;
    ret = false;

    irfile.open("ir.txt");

    prog();
    while (symbolTable.size() != 0)
    {
        symbolTable.erase(symbolTable.end());
    }
    //irfile << "write to file";
    file.close();
    irfile.close();

    return 0;
}

string label(){
    string l = "label"; 
    return l.append(to_string(++label_number));
}

string reg(int number){
    string r = "r"; 
    return r.append(to_string(number));
}

char getChar(){
    if(file.is_open())
        if(file.get(c)){
            return c ;
        }
        
    return '\0';
    
}

void ungetchar(){
    file.seekg(-1,ios::cur);
}

int isOpChar(char ch){
    if(ch == '[' || ch == ']' || ch == '(' || ch == ')' || ch == ',' || ch == '{' || ch == '}' ||
                 ch == '=' || ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '%' || 
                 ch == '<' || ch == '>' || ch == '!' || ch == '|' || ch == '&' || ch == ';' )
                 return 1;
    return 0;
}

int next(char expected){
    if(getChar() == expected) return 1;
    ungetchar();
    return 0;
}

string checkOp(char ch){
    string a = string(1,ch);
    switch(ch){
        case '!': next('=') == 1 ? a = "!=" : a = "!"; break;
        case '=': next('=') == 1 ? a = "==" : a = "="; break;
        case '>': next('=') == 1 ? a = ">=" : a = ">"; break;
        case '<': next('=') == 1 ? a = "<=" : a = "<"; break;
        case '|': next('|') == 1 ? a = "||" : a = "|"; break;
        case '&': next('&') == 1 ? a = "&&" : a = "&"; break;
    }
    return a;
}


string getString(string answer, char ch){
    while (! file.eof() && ch != '\0' && ! isOpChar(ch) && ch != ' ' && ch != '\n' && ch != '#' && ch != '\r' && ch != '\t')
    {
            answer += ch;
            ch = getChar();
    }
    if (isOpChar(ch) || ch == '#' || ch == ' ' || ch == '\n')
    {
        ungetchar();
    }
    return answer;
}

void skipComment(){
    string temp;
    ungetchar();
    count_line++;
    getline(file, temp);
}

char skipSpace(char ch){
    while (!file.eof() && (ch == ' ' || ch == '\n' || ch == '\r'  || ch == '\t')){
        if(ch == '\n') count_line ++;
        ch = getChar();
    }
    return ch;
}

string nextToken(){
    string answer;
    char ch ;
    ch = getChar();
    if (file.eof()){
        return "";
    }
    if (ch == '#'){
        skipComment();
        ch = getChar();
    }

    ch = skipSpace(ch);
    //////////////////////////////////
    if(isOpChar(ch)){
        return checkOp(ch);
    }
    else {
        answer = getString(answer, ch);
        if (answer == " " || answer == "\n" || answer == "\r" || answer == "\t" || answer == ""){
            answer = nextToken();
        }
        return answer;
    }
    

}

string getToken(){
    return token;
}

void dropToken(){
    token = nextToken();
}

bool isFileEnd(){
    return file.eof();
}

void syntax_error(string err){
    cout << count_line << ": " << err << endl;
    num_err ++;
}

bool isKey(string tok){
    if (tok == IF || tok == ELSE || tok == IN || tok == WHILE || tok == FOR || tok == RETURN || tok == LIST || tok == NUM ||
    tok == NUMREAD || tok == NUMPRINT || tok == LISTLEN || tok == EXIT || tok == MAKELIST)
        return true;
    else return false;   
    
}

bool isIdentifier(string tok){
    if(regex_match(tok, regex("[a-zA-Z_][a-zA-Z_0-9]*"))) return true;
    else return false;
}

bool isNum(string tok){
    if(regex_match(tok, regex("[0-9]+"))) return true;
    else return false;
}

int isInSymbolTable(string name, bool isFunc ){
    for (int i = symbolTable.size() - 1; i >= 0; i--)
    {
        if (symbolTable[i].var == name && symbolTable[i].isFunc == isFunc)
        {
            return i;
        }
        
    }
    return -1;
}

void prog(){
    while (true)
    {
        if(isFileEnd()) break;
        func();
    }
    if(getToken() == "\0"){
        if (num_err == 0)
        {
            cout << "compiled successfully !\n";
            exit(0); 
        }
        else
        {
            cout << "number of errors : " << num_err << endl;
            cout << "compilation was failed !";
            exit(1); 
        }        
       
    }
    else {
        syntax_error("Unexcepted Token " + getToken());
        cout << "number of errors : " << num_err << endl;
        cout << "compilation was failed !";
        exit(1); 
    }
}

///****func -> func-type iden ( flist ) { body }********////////
void func(){
    reg_number = -1;
    string type = func_type();
    table func;
    func.isFunc = true;
    func.type = type;
    bool err = false;
    if (isIdentifier( getToken() ))
    {
        if (isKey(getToken()))
        {
            syntax_error(getToken() + " is a key word !");
            err = true;
        }
        if (isInSymbolTable(getToken(), true) != -1)
        {
            syntax_error(getToken()+ " function defined before!");
            err = true;
        }
        
    }
    func.var = getToken();
    generateCodeFucDef(func.var);

    iden();

    if(getToken() == "("){
        dropToken();
        int n = flist(func.list);
        func.numberOfparam = n;
        if(!err){
            symbolTable.push_back(func);
        }
        if(getToken() == ")"){
            dropToken();
            int size = symbolTable.size();
            for (int i = 0; i < n; i++)
            {
                symbolTable.push_back(func.list[i]);
            }
            
            if (getToken() == "{"){
                dropToken();
                return_type = type;
                body();
                if(type != NIL && !ret) syntax_error("this function should have a return statement!");
                
                ret = false;
                if(getToken() == "}"){
                    dropToken();
                }
                else syntax_error("expected }");
            }
            else syntax_error("expected {");
            while (size != symbolTable.size())
            {
                symbolTable.erase(symbolTable.end());
            }
        
        }
        else syntax_error("expected )");
    }
    else syntax_error("expected (");
}

void body(){
    while (! isFileEnd() && getToken() != "}"){
        stmt();
    }
}

/******
 * stmt -> expr ; |
            defvar ; |
            if ( expr ) stmt |
            if ( expr ) stmt else stmt |
            while  ( expr ) stmt |
            for (iden in expr ) stmt |
            return expr; |
            { body }
            *******/
void stmt(){
    string tok = getToken();
    int size = symbolTable.size();
    if(tok == NUM || tok == LIST)
    {
        defvar();
        semicol();
    }
    else if(tok == IF){
        if_stmt();
        while (size != symbolTable.size())
        {
            symbolTable.erase(symbolTable.end());
        }
    }
    else if(tok == WHILE)
    {
        while_stmt();
        while (size != symbolTable.size())
        {
            symbolTable.erase(symbolTable.end());
        }
    }
    else if(tok == FOR)
    {
        for_stmt();
        while (size != symbolTable.size())
        {
            symbolTable.erase(symbolTable.end());
        }
    }
    else if(tok == RETURN){
        dropToken();
        exprVal t = expr();
        string type = t.type;
        ret = true;
        if(return_type == NIL) syntax_error("this function should not have a return statement!");
        if(return_type != NIL){
            if(type != return_type) syntax_error("return type mismatch!");
        }
        semicol();
        generateCodeReturn(t);
    }
    else if(tok == "{")
    {
        dropToken();
        body();
        if(getToken() == "}"){
            dropToken();
        }
        else
        {
            syntax_error("expected }");
        }
        
    }
    else
    {
        expr();
        semicol();
    }
    
}

void if_stmt(){
    if (getToken() == IF)
    {
        dropToken();
        if(getToken() == "("){
            dropToken();
            expr();
            if (getToken() == ")")
            {
                dropToken();
                stmt();
                if(getToken() == ELSE)
                {
                    dropToken();
                    stmt();
                }
                return;
            }
            else
            {
                syntax_error("expected )");
            }
            
        }
        else
        {
            syntax_error("expected (");
        }
        
    }
    
}

void while_stmt(){
    if(getToken() == WHILE){
        dropToken();
        if(getToken() == "("){
            dropToken();
            expr();
            if (getToken() == ")")
            {
                dropToken();
                stmt();
                return;
            }
            else
            {
                syntax_error("expected )");
            }
            
        }
        else
        {
            syntax_error("expected (");
        }
    }
}

void for_stmt(){
     if(getToken() == FOR)
     {
         dropToken();
         if(getToken() == "("){
            dropToken();
            if (isKey(getToken()))
            {
                syntax_error(getToken() + " is a key word !");
            }
            
            iden();
            if(getToken() == IN){
                 dropToken();
                 exprVal t = expr();
                 string type = t.type;
                 if(type != LIST) syntax_error("invalid type! it should be list !");
                 if(getToken() == ")"){
                     dropToken();
                     stmt();
                 }
                 else
                 {
                     syntax_error("expected )");
                 }
                 
            }
            else
            {
                syntax_error("expected in");
            }
         }
         else
         {
             syntax_error("expected (");
         }
         
     }
}

void semicol(){
    if(getToken() == ";"){
        dropToken();
    }
    else{
        syntax_error("expected ;");
    }
}

void defvar(){
    table variable;
    variable.type = type();
    variable.isFunc = false;
    variable.numberOfparam = 0;
    bool err = false;
    string tok = getToken();

    if (isKey(tok))
    {
        syntax_error(tok + "is a key word");
        err = true;
    }
    if(isInSymbolTable(tok, false) != -1)
    {
        syntax_error(tok + " is defined before");
        err = true;
    }
    if(!err && isIdentifier(tok)){
        variable.var = tok;
        symbolTable.push_back(variable);
    }
    iden();
}

exprVal expr(){
    return assign_expr();
}

//assign-expr -> cond-expr |
     //               unary-expr  = assign-expr
exprVal assign_expr(){
    exprVal t1, t2, tt;
    isexpr = false;
    t1 = cond_expr(tt);
    string type = t1.type;
    string type2;
    bool err = false;
    if (getToken() == "=")
    {
        if (isexpr)
        {
            syntax_error("lvalue required as left operand of assignment!");
        }
        if(isNum(t1.tok))
        {
            syntax_error("lvalue required as left operand of assignment! num value???");
        }
        
        isexpr = false;
        while (getToken() == "=")
        {
            dropToken();
            
            t2 = cond_expr(t1);
            type2 = t2.type;
            if(type != type2){
                err = true;
                syntax_error("illegal assignment!");
            }
            /// check register ///
            if (t2.tok == NUMREAD){
                continue;
            }
            else if (t2.reg == -1)
            {//todo check
                if (t1.reg == -1)
                {
                    t1.reg = ++ reg_number;
                }
                
                if(isNum(t2.tok)){
                    generateCodeMov(t1, t2);
                }
            }
            else if(t1.reg != t2.reg && t1.tok == t2.tok) {
                // sum = a + b + c;
                t1.reg = t2.reg;
            }
            else if(t1.reg != t2.reg && t1.tok != t2.tok){
                // b = a;
                if(t1.reg == -1){
                   t1.reg = ++ reg_number; 
                }
                
                generateCodeMov(t1, t2);
            }
            int i = isInSymbolTable(t1.tok, false);
            if(i != -1 && t1.reg != -1){
                symbolTable[i].reg = t1.reg;
            }
            /// end check register ///

            t1 = t2;

        }

    }  
    if (!err)
    {
        t1.type = type;
    }
    return  t1;
}

exprVal cond_expr(exprVal temp){
    return or_expr(temp);
}
    
exprVal or_expr(exprVal temp){
    exprVal t1, t2;
    t1 = and_expr(temp);
    string type = t1.type;
    string type2;
    bool err = false;
    while (getToken() == "||")
    {
        isexpr = true;
        dropToken();
        t2 = and_expr(temp);
        type2 = t2.type;
        if(type != type2){
            err = true;
            syntax_error("illegal or expression !");
        } 
    }
    if (!err)
    {
        t1.type = type;
    }

    return t1; 
}

exprVal and_expr(exprVal temp){
    exprVal t1, t2;
    t1 = equ_expr(temp);
    string type = t1.type;
    string type2;
    bool err = false;
    while (getToken() == "&&")
    {
        isexpr = true;
        dropToken();
        t2 = equ_expr(temp);
        type2 = t2.type;
        if(type != type2){
            err = true;
            syntax_error("illegal and expression !");
        } 
    }
    if (!err)
    {
        t1.type = type;
    }
    
    return t1;
}

exprVal equ_expr(exprVal temp){
    exprVal t1 , t2;
    t1 = relational_expr(temp);
    string type = t1.type;
    string type2;
    bool err = false;
    while (getToken() == "==" || getToken() == "!=")
    {
        isexpr = true;
        dropToken();
        t2 = relational_expr(temp);
        type2 = t2.type;
        if(type != type2){
            err = true;
            syntax_error("illegal equality expression !");
        } 
    }
    if(!err)
    {
        t1.type = type;
    }
    return t1;
}

exprVal relational_expr(exprVal temp){
    exprVal t1, t2;
    t1 = add_expr(temp);
    string type = t1.type;
    string type2;
    bool err = false;
    bool in = false;
    while (getToken() == ">" || getToken() == "<" || getToken() == ">=" || getToken() == "<=")
    {
        string tok = getToken();
        isexpr = true;
        dropToken();
        t2 = add_expr(temp);
        type2 = t2.type;
        if(type != type2){
            err = true;
            syntax_error("illegal relational expression !");
        } 
        if(in == false){
            if (temp.reg == -1)
            {
                reg_number ++;
                temp.reg = reg_number;
            }
            generateCodeComp(temp, t1, t2 , tok);
        }
        else{
            generateCodeComp(temp, temp, t2 , tok);
        }
        in = true;
    }
    if (!err)
    {
        t1.type = type;
    }
    
    if(in) return temp;
    else return t1;
}

exprVal add_expr(exprVal temp){
    exprVal t1 , t2;
    t1 = mul_expr(temp);
    string type = t1.type;
    //temp = t1;
    bool in = false;
    bool add;
    while (getToken() == "+" || getToken() == "-")
    {
        isexpr = true;
        if(getToken() == "+") add = true;
        else add = false;
        if(type != NUM) syntax_error("incompatible operands!");
        dropToken();
        t2 = mul_expr(temp);
        type = t2.type;
        if(in == false){
            if (temp.reg == -1)
            {
                reg_number ++;
                temp.reg = reg_number;
            }
            if(add) generateCodeArith(temp, t1, t2, "add ");
            else generateCodeArith(temp, t1, t2, "sub ");
        }
        else{
            if(add) generateCodeArith(temp, temp, t2, "add ");
            else generateCodeArith(temp, temp, t2, "sub ");
        }
        in = true;
        
    }
    if (type == NUM)
    {
        temp.type = NUM;
    }
    
    if(type != NUM && in) syntax_error("incompatible operands!");
    if(in) return temp;
    else return t1;
}

exprVal mul_expr(exprVal temp){
    exprVal t1 , t2;
    t1 = unary_expr(temp);
    string type = t1.type;
    bool in = false;
    while (getToken() == "*" || getToken() == "/" || getToken() == "%")
    {
        string tok = getToken();
        isexpr = true;
        if(type != NUM) syntax_error("incompatible operands!");
        dropToken();
        t2 = unary_expr(temp);
        type = t2.type;
        if(in == false){
            if (temp.reg == -1)
            {
                reg_number ++;
                temp.reg = reg_number;
            }
            if(tok == "*") generateCodeArith(temp, t1, t2, "mul ");
            else if(tok == "/") generateCodeArith(temp, t1, t2, "div ");
            else generateCodeArith(temp, t1, t2, "mod ");
        }
        else{
            if(tok == "*") generateCodeArith(temp, temp, t2, "mul ");
            else if(tok == "/") generateCodeArith(temp, temp, t2, "div ");
            else generateCodeArith(temp, temp, t2, "mod ");
        }
        in = true;
    }
    if(type != NUM && in) syntax_error("incompatible operands!");
    if(in) return temp;
    else return t1;
}

exprVal unary_expr(exprVal temp){
    unary_oprator();
    while (getToken() == "+" || getToken() == "-" || getToken() == "!")
    {
        isexpr = true;
        unary_oprator();
    }
    return postfix_expr(temp);
}

exprVal postfix_expr(exprVal temp){
    exprVal t1, t2;
    t1 = prim_expr(temp);
    string type = t1.type;
    int isList = false;
    if(getToken() == "[")
        if(type != LIST)
            syntax_error("The identifier should be in type list");
    while (getToken() == "[")
    {
        isList = true;
        dropToken();
        t2 = expr();
        type = t2.type;
        
        //expr should be num
        if(type != NUM)
            syntax_error("index of list should be in type num");

        if(getToken() == "]"){
            dropToken();
        }
        else
        {
            syntax_error("expected ]");
        }
        
    }
    if (isList)
    {
        t1.type = NUM;
    }
    else
    {
        t1.type = type;
    }
    
    return t1; 

}

/****
 * prim-expr -> iden |
                iden ( clist ) |
                num |
                (expr)
                *****/
exprVal prim_expr(exprVal temp){
    exprVal t;
    if (isIdentifier(getToken()))
    {
        string var = getToken();
        int i ;
        iden();
        if(getToken() == "("){// cal function
            i = isInSymbolTable(var, true);
            if(i == -1) syntax_error(var + " is not defined!");
            dropToken();
            clist(var, i, temp);
            if (getToken() == ")")
            {
                dropToken();
            }
            else
            {
                syntax_error("expected )");
            }
            t.isFunc = true;
        
        }
        else{// identifier
            i = isInSymbolTable(var, false);
            if(i == -1) syntax_error(var + " is not defined!");
            t.isFunc = false;
        }
        //// todo check this
        if(i != -1) {
            t.tok = var;
            t.reg = symbolTable[i].reg;
            t.type = symbolTable[i].type;
        } 
        return t;
    }
    else if(isNum(getToken()))
    {
        valStruct v;
        v.index = 0;
        v.val = stoi(getToken());
        t.value.push_back(v);
        t.type = NUM;
        t.tok = getToken();
        num();
        return t;
    }
    else if(getToken() == "("){
        dropToken();
        t = expr();
        if (getToken() == ")")
        {
            dropToken();
        }
        else
        {
            syntax_error("expected )");
        }
        return t;
    }
    else if(getToken() == ")" || getToken() == "]" || getToken() == ";" || getToken() == "}")
    {
        //todo something to check
        return t;
    }
    
    else
    {
        syntax_error("primary expression is not ok!");
        dropToken();
        return t;
    }
    
    
}

void unary_oprator(){
    if(getToken() == "+" || getToken() == "-" || getToken() == "!"){
        dropToken(); 
        isexpr = true;
    }        
}

int flist(vector<table> &list){
    if (getToken() == ")")
    {
        return 0;
    }
    table variable;
    int n = 1;
    variable.type = type();
    if (isKey(getToken()))
    {
        syntax_error(getToken() + " is s key word");
    }
    variable.isFunc = false;
    variable.numberOfparam = 0;
    variable.var = getToken();
    iden();
    variable.reg = ++reg_number;
    list.push_back(variable);
    while (! isFileEnd() && getToken() == ",")
    {
        dropToken();
        variable.type = type();
        if (isKey(getToken()))
        {
            syntax_error(getToken() + " is s key word");
        }
        variable.isFunc = false;
        variable.numberOfparam = 0;
        variable.var = getToken();
        iden();
        variable.reg = ++reg_number;
        list.push_back(variable);
        n ++;
    }
    return n;
    
}

void clist(string var, int i, exprVal temp){// var if function name , i is index of fuction in symbolTable
    bool err = false;
    if (i == -1)
    {
        syntax_error(var + " function is not defined !");
        err = true;
    }
    if (getToken() == ")")
    {
        if(!err)
        {
            //check if number of parameters is 0 or not. if not 0 should see an error
            if (symbolTable[i].numberOfparam != 0)
            {
                syntax_error("illegal number of parameter !");
            }
            generateCodeCall(var, temp, symbolTable[i].numberOfparam, i);
        }
        return;
    }
    int n = 1;
    // should get the type of expression and compare with parameter of var and when they dismathch should call syntax error(actually this is an semantic error)
    exprVal exp = expr();
    string t = exp.type;
    int j = 0;
    bool same = check_same_type(t, i, j);// t is type, i is the index in symbolTable and j is index of list in the function in symbolTable
    if(!same) syntax_error("illegal parameter!");
    if (!err)
    {
        generateCodeCall(var, temp, symbolTable[i].numberOfparam, i);//todo check tabe haye to dar to
    }
    generateCodeClist(&exp, symbolTable[i].numberOfparam -1 == j);
    symbolTable[i].reg = exp.reg;
    while(! isFileEnd() && getToken() != ")" && getToken() == "," )
    {
        dropToken();
        exp = expr();
        t = exp.type;
        j++;
        same = check_same_type(t, i, j);
        if(!same) syntax_error("illegal parameter!");
        generateCodeClist(&exp, symbolTable[i].numberOfparam -1 == j);
        n ++;
    }
    if(i != -1){
       if (n != symbolTable[i].numberOfparam)
        {
            syntax_error("illegal number of parameter !");
        } 
    }

    
}

string func_type(){
    string tok = getToken();
    if(tok == NUM) {
        dropToken();
        return tok;
    }
    else if(tok == LIST)
    {
        dropToken();
        return tok;
    }
    else if(tok == NIL)
    {
        dropToken();
        return tok;
    }
    else
    {
        syntax_error("expected num or list or nil");
        return NIL;
    }
}

string type(){
    string tok = getToken();
    if(tok == NUM) {
        dropToken();
        return tok;
    }
    else if(tok == LIST)
    {
        dropToken();
        return tok;
    }
    else
    {
        syntax_error("expected num or list");
        return NIL; // means that type of variable is not defined !
    }
    
}

void num(){
    string tok = getToken();
    if(isNum(tok)){
         dropToken();
     }
     else {
         syntax_error("Invalid number!");
     }
}

void iden(){
    string tok = getToken();
    if(isIdentifier(tok)){
        dropToken();         
     }
     else {
         syntax_error("Invalid identifier!");
         dropToken();
     }
}

bool check_same_type(string type, int i, int j){
    string t = NIL;
    if(i != -1)
    {
        if(j >= 0 && j < symbolTable[i].list.size())
            t = symbolTable[i].list[j].type;
    }
    if (t == type) return true;
    else return false;
    
}

void generateCodeFucDef(string name){
    irfile << "proc  " << name << '\n';
}

void generateCodeReturn(exprVal exp){
    if(exp.reg != 0){
        string str = "mov r0,";
        if (isNum(exp.tok)) // todo check neg num
        {
            str.append(exp.tok);
        }
        else
        {
            str.append(reg(exp.reg));
        }
        irfile << str << '\n';
    }
    
    irfile << "ret\n\n";
    
}

void generateCodeCall(string var, exprVal temp, int param, int i){
    string str = "call ";
    if(var == NUMREAD){
        if(temp.reg == -1){
            int i = isInSymbolTable(temp.tok, temp.isFunc);
            if(i != -1){
                if(symbolTable[i].reg == -1){
                    temp.reg = ++ reg_number;
                    symbolTable[i].reg = temp.reg;
                }
                else{
                    temp.reg = symbolTable[i].reg;
                }
            }
        }
        str.append("iget, ");
        str.append(reg(temp.reg));
        symbolTable[i].reg = temp.reg;
    }
    else if(var == NUMPRINT){
        str.append("iput");
    }
    else{
        str.append(var);
    }
    if (param == 0) irfile << str << '\n';
    else irfile << str;    
}

void generateCodeClist(exprVal *item , bool isLast){
    string str = ", ";
    if(item->reg == -1){
        int i = isInSymbolTable(item->tok, item->isFunc);
        if(i != -1){
            if(symbolTable[i].reg == -1){
                item->reg = ++ reg_number;
                symbolTable[i].reg = item->reg;
            }
            else{
                item->reg = symbolTable[i].reg;
            }
        }
    }
    str.append(reg(item->reg));
    if(isLast) irfile << str << '\n';
    else irfile << str;
}

void generateCodeMov(exprVal e1, exprVal e2){
    string str = "mov ";
    str.append(reg(e1.reg));
    str.append(", ");
    if(isNum(e2.tok)){//todo check neg num
        str.append(e2.tok);
        irfile << str << '\n';
    }
    else {
        str.append(reg(e2.reg));
        irfile << str << '\n';
    }
    
}

void generateCodeArith(exprVal res, exprVal e1, exprVal e2, string mode){
    if (isNum(e1.tok))
    {
        e1.reg = ++ reg_number;
        generateCodeMov(e1, e1);
        if(isNum(e2.tok))
        {
            e2.reg = ++ reg_number;
            generateCodeMov(e2, e2);
            irfile << mode << reg(res.reg) << ", " << to_string(reg_number - 1) << ", " << to_string(reg_number) << '\n';            
            return;
        }
        else if(e2.reg == -1)
        {
            int i = isInSymbolTable(e2.tok, false);
            if(i != -1 && symbolTable[i].reg == -1){
                e2.reg = ++ reg_number;
                symbolTable[i].reg = e2.reg;
            }
            else if(i != -1){
                e2.reg = symbolTable[i].reg;
            }
        }
        irfile << mode << reg(res.reg) << ", " << reg(e1.reg) << ", " << reg(e2.reg) << '\n';
        return;
    }
    else if(isNum(e2.tok))
    {
        e2.reg = ++ reg_number;
        generateCodeMov(e2, e2);
        if(e1.reg == -1)
        {
            int i = isInSymbolTable(e1.tok, false);
            if(i != -1 && symbolTable[i].reg == -1){
                e1.reg = ++ reg_number;
                symbolTable[i].reg = e1.reg;
            }
            else if(i != -1){
                e1.reg = symbolTable[i].reg;
            }
        }
        irfile << mode << reg(res.reg) << ", " << reg(e1.reg) << ", " << reg(e2.reg) << '\n';
        return;
    }
    if(e1.reg == -1){
        int i = isInSymbolTable(e1.tok, false);
        if(i != -1 && symbolTable[i].reg == -1){
            e1.reg = ++ reg_number;
            symbolTable[i].reg = e1.reg;
        }
        else if(i != -1){
            e1.reg = symbolTable[i].reg;
        }
    }
    if(e2.reg == -1)
    {
        int i = isInSymbolTable(e2.tok, false);
        if(i != -1 && symbolTable[i].reg == -1){
            e2.reg = ++ reg_number;
            symbolTable[i].reg = e2.reg;
        }
        else if(i != -1){
            e2.reg = symbolTable[i].reg;
        }
    }
    irfile << mode << reg(res.reg) << ", " << reg(e1.reg) << ", " << reg(e2.reg) << '\n';
    return;
    
}

void generateCodeComp(exprVal res, exprVal e1, exprVal e2 , string mode){
    if (isNum(e1.tok))
    {
        e1.reg = ++ reg_number;
        generateCodeMov(e1, e1);
        if(isNum(e2.tok))
        {
            e2.reg = ++ reg_number;
            generateCodeMov(e2, e2);
            irfile << "comp" << mode << " " << reg(res.reg) << ", " << to_string(reg_number - 1) << ", " << to_string(reg_number) << '\n';            
            return;
        }
        else if(e2.reg == -1)
        {
            int i = isInSymbolTable(e2.tok, false);
            if(i != -1 && symbolTable[i].reg == -1){
                e2.reg = ++ reg_number;
                symbolTable[i].reg = e2.reg;
            }
            else if(i != -1){
                e2.reg = symbolTable[i].reg;
            }
        }
        irfile << "comp" << mode << " " << reg(res.reg) << ", " << reg(e1.reg) << ", " << reg(e2.reg) << '\n';
        return;
    }
    else if(isNum(e2.tok))
    {
        e2.reg = ++ reg_number;
        generateCodeMov(e2, e2);
        if(e1.reg == -1)
        {
            int i = isInSymbolTable(e1.tok, false);
            if(i != -1 && symbolTable[i].reg == -1){
                e1.reg = ++ reg_number;
                symbolTable[i].reg = e1.reg;
            }
            else if(i != -1){
                e1.reg = symbolTable[i].reg;
            }
        }
        irfile << "comp" << mode << " " << reg(res.reg) << ", " << reg(e1.reg) << ", " << reg(e2.reg) << '\n';
        return;
    }
    if(e1.reg == -1){
        int i = isInSymbolTable(e1.tok, false);
        if(i != -1 && symbolTable[i].reg == -1){
            e1.reg = ++ reg_number;
            symbolTable[i].reg = e1.reg;
        }
        else if(i != -1){
            e1.reg = symbolTable[i].reg;
        }
    }
    if(e2.reg == -1)
    {
        int i = isInSymbolTable(e2.tok, false);
        if(i != -1 && symbolTable[i].reg == -1){
            e2.reg = ++ reg_number;
            symbolTable[i].reg = e2.reg;
        }
        else if(i != -1){
            e2.reg = symbolTable[i].reg;
        }
    }
    irfile << "comp" << mode << " " << reg(res.reg) << ", " << reg(e1.reg) << ", " << reg(e2.reg) << '\n';
}
