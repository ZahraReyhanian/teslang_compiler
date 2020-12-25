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
string token;
int num_err;
int count_line;
string IF = "if", ELSE = "else", IN = "in", WHILE = "while", FOR = "for", RETURN = "return", LIST = "list", NUM = "num", NIL = "nil",
NUMREAD = "numread", NUMPRINT = "numprint", LISTLEN = "listlen", EXIT = "exit", MAKELIST = "makelist";//main
bool ret;
string return_type;

struct table
{
    string var;
    string type;
    bool isFunc;
    int numberOfparam;
    vector<table> list;

};
vector<table> symbolTable;

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
string expr();
string assign_expr();
string cond_expr();
string or_expr();
string and_expr();
string equ_expr();
string relational_expr();
string add_expr();
string mul_expr();
string unary_expr();
string postfix_expr();
string prim_expr();
void unary_oprator();
int flist(vector<table> &list);
void clist(string var, int i);
string type();
string func_type();
void num();
void iden();
bool check_same_type(string type, int i, int j);

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

    prog();
    while (symbolTable.size() != 0)
    {
        symbolTable.erase(symbolTable.end());
    }
    file.close();

    return 0;
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

void func(){
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
        string type = expr();
        ret = true;
        if(return_type == NIL) syntax_error("this function should not have a return statement!");
        if(return_type != NIL){
            if(type != return_type) syntax_error("return type mismatch!");
        }
        semicol();
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
                 string type = expr();
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

string expr(){
    return assign_expr();
}

//assign-expr -> cond-expr |
     //               unary-expr  = assign-expr
string assign_expr(){
    string type = cond_expr();
    string type2;
    bool err = false;
    if (getToken() == "=")
    {
        while (getToken() == "=")
        {
            dropToken();
            type2 = cond_expr();
            if(type != type2){
                err = true;
                syntax_error("illegal assignment!");
            }
        }
    }  
    return err ? NIL : type; 
}

string cond_expr(){
    return or_expr();
}
    
string or_expr(){
    string type = and_expr();
    string type2;
    bool err = false;
    while (getToken() == "||")
    {
        dropToken();
        type2 = and_expr();
        if(type != type2){
            err = true;
            syntax_error("illegal or expression !");
        } 
    }
    return err ? NIL : type; 
}

string and_expr(){
    string type = equ_expr();
    string type2;
    bool err = false;
    while (getToken() == "&&")
    {
        dropToken();
        type2 = equ_expr();
        if(type != type2){
            err = true;
            syntax_error("illegal and expression !");
        } 
    }
    return err ? NIL : type;
}

string equ_expr(){
    string type = relational_expr();
    string type2;
    bool err = false;
    while (getToken() == "==" || getToken() == "!=")
    {
        dropToken();
        type2 = relational_expr();
        if(type != type2){
            err = true;
            syntax_error("illegal equality expression !");
        } 
    }
    return err ? NIL : type;
}

string relational_expr(){
    string type = add_expr();
    string type2;
    bool err = false;
    while (getToken() == ">" || getToken() == "<" || getToken() == ">=" || getToken() == "<=")
    {
        dropToken();
        type2 = add_expr();
        if(type != type2){
            err = true;
            syntax_error("illegal relational expression !");
        } 
    }
    return err ? NIL : type;
}

string add_expr(){
    string type = mul_expr();
    bool in = false;
    while (getToken() == "+" || getToken() == "-")
    {
        if(type != NUM) syntax_error("incompatible operands!");
        dropToken();
        type = mul_expr();
        in = true;
    }
    if(type != NUM && in) syntax_error("incompatible operands!");
    return type;
}

string mul_expr(){
    string type = unary_expr();
    bool in = false;
    while (getToken() == "*" || getToken() == "/" || getToken() == "%")
    {
        if(type != NUM) syntax_error("incompatible operands!");
        dropToken();
        type = unary_expr();
        in = true;
    }
    if(type != NUM && in) syntax_error("incompatible operands!");
    return type;
}

string unary_expr(){
    unary_oprator();
    while (getToken() == "+" || getToken() == "-" || getToken() == "!")
    {
        unary_oprator();
    }
    return postfix_expr();
}

string postfix_expr(){
    string type = prim_expr();
    int isList = false;
    if(getToken() == "[")
        if(type != LIST)
            syntax_error("The identifier should be in type list");
    while (getToken() == "[")
    {
        isList = true;
        dropToken();
        type = expr();
        
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
    return isList ?  NUM : type; 

}

string prim_expr(){
    if (isIdentifier(getToken()))
    {
        string var = getToken();
        int i ;
        iden();
        if(getToken() == "("){
            i = isInSymbolTable(var, true);
            if(i == -1) syntax_error(var + " is not defined!");
            dropToken();
            clist(var, i);
            if (getToken() == ")")
            {
                dropToken();
            }
            else
            {
                syntax_error("expected )");
            }
        
        }
        else{
            i = isInSymbolTable(var, false);
            if(i == -1) syntax_error(var + " is not defined!");
        }
        if(i == -1) return NIL;
        else return symbolTable[i].type;
    }
    else if(isNum(getToken()))
    {
        num();
        return NUM;
    }
    else if(getToken() == "("){
        dropToken();
        string type = expr();
        if (getToken() == ")")
        {
            dropToken();
        }
        else
        {
            syntax_error("expected )");
        }
        return type;
    }
    else if(getToken() == ")" || getToken() == "]" || getToken() == ";" || getToken() == "}")
    {
        //todo something to check
        return NIL;
    }
    
    else
    {
        syntax_error("primary expression is not ok!");
        dropToken();
        return NIL;
    }
    
    
}

void unary_oprator(){
    if(getToken() == "+" || getToken() == "-" || getToken() == "!") dropToken();        
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
        list.push_back(variable);
        n ++;
    }
    return n;
    
}

void clist(string var, int i){
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
            
        }
        return;
    }
    int n = 1;
    // should get the type of expression and compare with parameter of var and when they dismathch should call syntax error(actually this is an semantic error)
    string t = expr();
    int j = 0;
    bool same = check_same_type(t, i, j);// t is type, i is the index in symbolTable and j is index of list in the function in symbolTable
    if(!same) syntax_error("illegal parameter!");
    while(! isFileEnd() && getToken() != ")" && getToken() == "," )
    {
        dropToken();
        t = expr();
        j++;
        same = check_same_type(t, i, j);
        if(!same) syntax_error("illegal parameter!");
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