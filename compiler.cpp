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

struct table
{
    string var;
    // vector<table> values;//for list type
    string type;
    bool isFunc;
    int numberOfparam;
    int reg = -1;
    vector<table> list;
    // int index = -1;//index of list item in values vector
};
vector<table> symbolTable;

struct exprVal
{
    string tok;
    string type;
    // int val;
    bool isFunc;
    bool itemList = false;
    int reg = -1;
    int index = -1;// the index of variable in symbolTable
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
int searchTok(string name);
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
string unary_oprator();
int flist(vector<table> &list);
void clist(string var, int i, exprVal temp);
string type();
string func_type();
void num();
void iden();
bool check_same_type(string type, int i, int j);

void generateCodeFucDef(string name);
void generateCodeReturn(exprVal exp);
void generateCodeCall(string var, exprVal temp, int param, int i, string items);
void generateCodeMov(exprVal e1, exprVal e2);
void generateCodeLd(exprVal e1, exprVal e2);
void generateCodeArith(exprVal res, exprVal e1, exprVal e2 , string mode);
void generateCodeComp(exprVal res, exprVal e1, exprVal e2 , string mode);
void generateCodeJmpCond(exprVal exp, string mode, string label);
void generateCodeJmp(string label);
void generateCodeLable(string label);

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
    if(tok == "-1") return true;
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

int searchTok(string name){
    for (int i = symbolTable.size() - 1; i >= 0; i--)
    {
        if (symbolTable[i].var == name)
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
            exprVal t = expr();
            if(t.reg == -1){
                if(t.tok == "")
                    syntax_error("expected primary-expression before ')' token");
                else
                {
                    if(isNum(t.tok) && t.reg == -1){
                        t.reg = ++ reg_number;
                        generateCodeMov(t, t);
                    }
                    else {
                        int i = searchTok(t.tok);
                        if(i != -1){//todo check type and isFunc
                            if(symbolTable[i].reg != -1){
                                t.reg = symbolTable[i].reg;
                            }
                            else{
                                symbolTable[i].reg = ++ reg_number;
                                t.reg = symbolTable[i].reg;
                                if (symbolTable[i].type == NUM)
                                {
                                    exprVal tt;
                                    tt.tok = "0";
                                    generateCodeMov(t, tt);
                                }
                                else if(symbolTable[i].type == NIL)
                                {
                                    syntax_error("could not convert from nil to num");
                                }
                            }
                        }
                    }
                }
                
            }
            if (getToken() == ")")
            {
                dropToken();
                string lbl = label();
                string lbl2 = label();
                generateCodeJmpCond(t, "jz", lbl);
                stmt();
                //todo check
                if(getToken() == ELSE)
                {
                    generateCodeJmp(lbl2);
                    generateCodeLable(lbl);
                    dropToken();
                    stmt();
                    generateCodeLable(lbl2);
                }
                else {
                    generateCodeLable(lbl);
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
        //// generate ir code for while statement ////
        string beg = label();
        string end = label();
        irfile << beg << endl;
        if(getToken() == "("){
            dropToken();
            exprVal t = expr();
            generateCodeJmpCond(t, "jz", end);
            if (getToken() == ")")
            {
                dropToken();
                stmt();
                generateCodeJmp(beg);
                generateCodeLable(end);
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
    bool il = t1.itemList; 
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
            else if(il) {//a[2] = 7
                if(isNum(t2.tok) && t2.reg == -1){
                    t2.reg = ++ reg_number;
                    generateCodeMov(t2, t2);
                }
                irfile << "st " << reg(t2.reg) << ", " << reg(t1.reg) << '\n';
            }
            else if(t2.itemList)
            {
                if (t1.reg == -1)
                {
                    t1.reg = ++ reg_number;
                }
                irfile << "ld " << reg(t1.reg) << ", " << reg(t2.reg) << '\n';
            }
            else if (t2.reg == -1)
            {//todo check that maybe num token has register
                
                if (t1.reg == -1)
                {
                    t1.reg = ++ reg_number;
                }
                
                if(isNum(t2.tok)){
                    //symbolTable[t1.index].val = t2.val;
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
            int i = t1.index;
            if(i != -1 && t1.reg != -1 && il == false){
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
    string lbl;
    if(getToken() == "||"){
        lbl = label();
    }
    bool in = false;
    while (getToken() == "||")
    {
        if(!in){
            if(temp.reg == -1){
                temp.reg = ++reg_number;
                if(temp.index != -1){
                    symbolTable[temp.index].reg = temp.reg;
                }
            }
            generateCodeMov(temp,t1);
            generateCodeJmpCond(temp, "jnz", lbl);
        } 
        else generateCodeJmpCond(t2, "jnz", lbl);
        in = true;
        isexpr = true;
        dropToken();
        t2 = and_expr(temp);
        type2 = t2.type;
        if(type != type2){
            err = true;
            syntax_error("illegal or expression !");
        } 
    }
    if(in){
        if(t2.reg == -1){
            t2.reg = ++reg_number;
            if(t2.index != -1){
                symbolTable[t2.index].reg = t2.reg;
            }
        }
        if(temp.index != -1) temp.reg = symbolTable[temp.index].reg;
        generateCodeMov(temp, t2);
        generateCodeLable(lbl); 
        temp.type = NUM;
        return temp;
    }
    if (!err)
    {
        t1.type = NUM;
    }

    return t1; 
}

exprVal and_expr(exprVal temp){
    exprVal t1, t2;
    t1 = equ_expr(temp);
    string type = t1.type;
    string type2;
    bool err = false;
    string lbl;
    if(getToken() == "&&"){
        lbl = label();
    }
    bool in = false;
    while (getToken() == "&&")
    {
        if(!in) generateCodeJmpCond(t1, "jz", lbl);
        else generateCodeJmpCond(t2, "jz", lbl);
        in = true;
        isexpr = true;
        dropToken();
        t2 = equ_expr(temp);
        type2 = t2.type;
        if(type != type2){
            err = true;
            syntax_error("illegal and expression !");
        } 
    }
    if(in){
        if(t2.reg == -1){
            t2.reg = ++reg_number;
            if(t2.index != -1){
                symbolTable[t2.index].reg = t2.reg;
            }
        }
        if(temp.index != -1) temp.reg = symbolTable[temp.index].reg;
        if(temp.reg == -1){
            temp.reg = ++reg_number;
            if(temp.index != -1){
                symbolTable[temp.index].reg = temp.reg;
            }
        }
        generateCodeMov(temp, t2);
        generateCodeLable(lbl); 
        temp.type = NUM;
        return temp;
    }
    if (!err)
    {
        t1.type = NUM;
    }
    
    return t1;
}

exprVal equ_expr(exprVal temp){
    exprVal t1 , t2;
    t1 = relational_expr(temp);
    string type = t1.type;
    string type2;
    bool err = false;
    bool in = false;
    while (getToken() == "==" || getToken() == "!=")
    {
        string tok = getToken();
        isexpr = true;
        dropToken();
        t2 = relational_expr(temp);
        type2 = t2.type;
        if(type != type2){
            err = true;
            syntax_error("illegal equality expression !");
        }
        if(in == false){
            if (temp.reg == -1)
            {
                reg_number ++;
                temp.reg = reg_number;
            }
            generateCodeComp(temp, t1, t2 , "=");
            if(tok == "!=") {
                string lbl = label();
                string lbl1 = label();
                generateCodeJmpCond(temp, "jz", lbl);
                irfile << "mov " << reg(temp.reg) << ", " << "0\n";
                generateCodeJmp(lbl1);
                generateCodeLable(lbl);
                irfile << "mov " << reg(temp.reg) << ", " << "1\n";
                generateCodeLable(lbl1);
            }
        }
        else{
            generateCodeComp(temp, temp, t2 , "=");
            if(tok == "!=") {
                string lbl = label();
                string lbl1 = label();
                generateCodeJmpCond(temp, "jz", lbl);
                irfile << "mov " << reg(temp.reg) << ", " << "0\n";
                generateCodeJmp(lbl1);
                generateCodeLable(lbl);
                irfile << "mov " << reg(temp.reg) << ", " << "1\n";
                generateCodeLable(lbl1);
            }
        }
        in = true;
    }
    if (!err)
    {
        t1.type = type;
    }
    if (type == NUM)
    {
        temp.type = NUM;
    }
    if(in) return temp;
    else return t1;
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
    if (type == NUM)
    {
        temp.type = NUM;
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
    if (type == NUM)
    {
        temp.type = NUM;
    }
    if(in) return temp;
    else return t1;
}

exprVal unary_expr(exprVal temp){
    bool in = false;
    string tok = unary_oprator();   
    if(tok == "-" || tok == "!") in = true; 
    while (getToken() == "+" || getToken() == "-" || getToken() == "!")
    {
        isexpr = true;
        tok.append(unary_oprator());
        cout << tok;
    }
    exprVal t = postfix_expr(temp);
    if(in){
        if(t.reg == -1){ // todo check
            t.reg = ++reg_number;
            if(t.index != -1) symbolTable[t.index].reg = reg_number;
            if(isNum(t.tok)) generateCodeMov(t, t);
        }
        exprVal n;
        n.tok = "-1";
        n.type = NUM;
        n.reg = ++reg_number;
        generateCodeMov(n, n);

        int k = 0;
        for (int i = tok.size() - 1; i >= 0; i--)
        {   
            if(tok[i] == '-'){
                generateCodeArith(t, t, n, "mul ");
            }
            else if(tok[i] == '!' && k % 2 == 0)
            {
                irfile << "mov " << reg(t.reg) << ", 0\n";
                k++;
            }
            else if(tok[i] == '!' && k % 2 == 1)
            {
                irfile << "mov " << reg(t.reg) << ", 1\n";
                k++;
            }
        }
        
    }
    
    return t;
    
}

exprVal postfix_expr(exprVal temp){//todo add regiter to values list of identifier in symbolTable
    exprVal t1, t2;
    t1 = prim_expr(temp);
    string type = t1.type;
    if(getToken() == "[")
        if(type != LIST)
            syntax_error("The identifier should be in type list");
    bool isList = false;
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
        
        //// check register ////
        if(t1.reg == -1)
        {
            if(t1.index != -1)
            {
                if(symbolTable[t1.index].reg == -1){
                    symbolTable[t1.index].reg = ++ reg_number;
                    t1.reg = reg_number;
                }
            }
        }
        if(t2.reg == -1){
            if(isNum(t2.tok) && t2.reg == -1) {
                if(t2.tok == "-1") syntax_error("index can not be -1");
                t2.reg = ++ reg_number;
                generateCodeMov(t2, t2);
            }
            else if(t2.index != -1)
            {
                if(symbolTable[t2.index].reg == -1){
                    symbolTable[t2.index].reg = ++ reg_number;
                    t2.reg = reg_number;
                }
            }
        }
        ///// generate code for ir file /////
        irfile << "mov " << reg(++reg_number) << ", 1\n" ;
        irfile << "add " << reg(reg_number) << ", " << reg(reg_number) << ", " << reg(t2.reg) << '\n';
        irfile << "mov " << reg(++reg_number) << ", 8\n" ;
        irfile << "mul " << reg(reg_number) << ", " << reg(reg_number) << ", " << reg(reg_number -1) << '\n';
        irfile << "add " << reg(t1.reg) << ", " << reg(t1.reg) << ", " << reg(reg_number) << '\n';
        irfile << "mov " << reg(++reg_number) << ", " <<  reg(t1.reg) << '\n';
        irfile << "sub " << reg(t1.reg) << ", " << reg(t1.reg) << ", " << reg(reg_number - 1) << '\n';
        t1.reg = reg_number;
        t1.type = NUM;

    }
    t1.itemList = isList;
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
            t.index = i;
        } 
        return t;
    }
    else if(isNum(getToken()))
    {
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

string unary_oprator(){
    string tok = getToken();
    if(getToken() == "+" || getToken() == "-" || getToken() == "!"){
        dropToken(); 
        isexpr = true;
    }
    return tok;
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
    string items = "";
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
            generateCodeCall(var, temp, symbolTable[i].numberOfparam, i, "");
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
    if(isNum(exp.tok) && exp.reg == -1){
        exp.reg = ++ reg_number;
        generateCodeMov(exp, exp);
    }
    if(exp.reg == -1){
        exp.reg = ++ reg_number;
    }
    else if(exp.itemList){
        irfile << "ld " << reg(++reg_number) << ", "<< reg(exp.reg) << endl;
        exp.reg = reg_number;
    }
    items.append(reg(exp.reg));
    symbolTable[i].reg = exp.reg;
    string cama = ", ";
    while(! isFileEnd() && getToken() != ")" && getToken() == "," )
    {
        dropToken();
        exp = expr();
        t = exp.type;
        j++;
        same = check_same_type(t, i, j);
        if(!same) syntax_error("illegal parameter!");
        
        if(isNum(exp.tok) && exp.reg == -1){
            exp.reg = ++ reg_number;
            generateCodeMov(exp, exp);
        }
        if(exp.reg == -1){
            exp.reg = ++ reg_number;
        }
        else if(exp.itemList){
            irfile << "ld " << reg(++reg_number) << ", "<< reg(exp.reg) << endl;
            exp.reg = reg_number;
        }
        items.append(cama.append(reg(exp.reg)));
        cama =", ";
        n ++;
    }
    if (!err)
    {
        generateCodeCall(var, temp, symbolTable[i].numberOfparam, i, items);//todo check tabe haye to dar to
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

void generateCodeCall(string var, exprVal temp, int param, int i, string items){
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
    else if(var == MAKELIST){
        reg_number++;
        irfile << "mov " << reg(reg_number) << ", 8" << '\n';
        irfile << "mul " << items << ", " << items << ", "<< reg(reg_number) << '\n';
        str.append("mem");
    }
    else{
        str.append(var);
    }
    if (param == 0) irfile << str << '\n';
    else irfile << str << ", " << items << '\n';    
}


void generateCodeMov(exprVal e1, exprVal e2){
    string str = "mov ";
    str.append(reg(e1.reg));
    str.append(", ");
    if(isNum(e2.tok)){
        str.append(e2.tok);
        irfile << str << '\n';
    }
    else {
        str.append(reg(e2.reg));
        irfile << str << '\n';
    }
    
}

void generateCodeLd(exprVal e1, exprVal e2){
    irfile << "ld " << reg(e1.reg) << ", " << reg(e2.reg) << '\n';
}

void generateCodeArith(exprVal res, exprVal e1, exprVal e2, string mode){
    if (isNum(e1.tok))
    {
        if(e1.reg == -1) 
        {
            e1.reg = ++ reg_number;
            generateCodeMov(e1, e1);
        }
        if(isNum(e2.tok))
        {
            if(e2.reg == -1) {
                e2.reg = ++ reg_number;
                generateCodeMov(e2, e2);
            }
            irfile << mode << reg(res.reg) << ", " << reg(e1.reg) << ", " << reg(e2.reg) << '\n';            
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
        if(e2.reg == -1){
            e2.reg = ++ reg_number;
            generateCodeMov(e2, e2);
        }
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
        if(e1.reg == -1){
            e1.reg = ++ reg_number;
            generateCodeMov(e1, e1);
        }
        
        if(isNum(e2.tok))
        {
            if(e2.reg == -1){
                e2.reg = ++ reg_number;
                generateCodeMov(e2, e2);
            }
            
            irfile << "cmp" << mode << " " << reg(res.reg) << ", " << reg(reg_number - 1) << ", " << reg(reg_number) << '\n';            
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
        irfile << "cmp" << mode << " " << reg(res.reg) << ", " << reg(e1.reg) << ", " << reg(e2.reg) << '\n';
        return;
    }
    else if(isNum(e2.tok))
    {
        if(e2.reg == -1){
            e2.reg = ++ reg_number;
            generateCodeMov(e2, e2);
        }
        
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
        irfile << "cmp" << mode << " " << reg(res.reg) << ", " << reg(e1.reg) << ", " << reg(e2.reg) << '\n';
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
    irfile << "cmp" << mode << " " << reg(res.reg) << ", " << reg(e1.reg) << ", " << reg(e2.reg) << '\n';
}

void generateCodeJmpCond(exprVal exp, string mode, string label){
    if(exp.reg == -1){
        exp.reg = ++reg_number;
        if(exp.index != -1) symbolTable[exp.index].reg = exp.reg;
    }
    irfile << mode << " " << reg(exp.reg) << ", " << label << '\n';
}

void generateCodeJmp(string label) {
    irfile << "jmp " << label << '\n';
}

void generateCodeLable(string label){
    irfile << label << ":\n";
}

