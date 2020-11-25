#include <iostream>
#include <fstream>
#include <string>
using namespace std;

char c;
ifstream file;

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
    if(ch == '[' || ch == ']' || ch == '(' || ch == ')' ||
                 ch == '=' || ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '%' || 
                 ch == '<' || ch == '>' || ch == '!' || ch == '|' || ch == '&' || ch == ';')
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
    //todo check string is valid or not
    while (! file.eof() && ch != '\0' && ! isOpChar(ch) && ch != ' ' && ch != '\n' && ch != '#' )
    {
            answer += ch;
            ch = getChar();
    }
    if (isOpChar(ch) || ch == '#' || ch == ' ')
    {
        ungetchar();
    }
    return answer;
}

void skipComment(){
    string temp;
    ungetchar();
    getline(file, temp);
}

char skipSpace(char ch){
    while (!file.eof() && (ch == ' ' || ch == '\n' || ch == '\r'  || ch == '\t')){
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
        if (answer == " " || answer == "\n" || answer == "\r" || answer == "\t"){
            nextToken();
        }
        return answer;
    }
    

}

int main () {
    cout << "Enter name of file\n";
    string name;
    cin >> name;
    file.open (name);
    string line;
    if(file.is_open()){
        while(! file.eof() ){
            line = nextToken();
            cout << line << '\n';
        }
    }
    

  return 0;
}

