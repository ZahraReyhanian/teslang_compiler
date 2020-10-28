// reading a text file
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

int invalidChar(char ch){
    return 0;
}

string getString(string answer, char ch){
    if(isdigit(ch)){
        return "error";
    }
    // while(ch == ' ' || ch == '\n'){
    //     ch = getChar();
    // }
    //cout << ch << endl;
    while (! file.eof() && ch != '\0' && ! isOpChar(ch) && ch != ' ' && ch != '\n' && ch != '#' && ! invalidChar(ch) )
    {
            answer += ch;
            ch = getChar();
            //cout << ch << endl;
    }
    if (isOpChar(ch) || ch == '#' || ch == ' ')
    {
        ungetchar();
    }
    return answer;
}

void skipComment(){
    // char ch = getChar();
    // while (ch != '\n'){
    //     //cout << "fasele\n";
    //     ch = getChar();
    //     //cout << "ch  " << ch << endl;
    // }
    //cout << "skip ssss      ............\n";
    string temp;
    ungetchar();
    getline(file, temp);
    //cout << "comment: " << temp;
    //cout << file.seekg(0);
}

char skipSpace(char ch){
    while (!file.eof() && (ch == ' ' || ch == '\n' || ch == '\t')){
        //cout << "fasele\n";
        ch = getChar();
        //cout << "ch  " << ch << endl;
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
        cout << "oooooooooooooooo";
        skipComment();
        cout << "aaaaaaaaaaaaaaaaaaaa";
        ch = getChar();
    }

    ch = skipSpace(ch);
    cout << "ch:" << ch;
    //////////////////////////////////
    if(isOpChar(ch)){
        //cout << "operation";
        return checkOp(ch);
    }
    else {
        //cout << "string ..  \n";
        answer = getString(answer, ch);
        if (answer == " " || answer == "\n"){
            nextToken();
        }
        return answer;
    }
    

}

int main () {
    file.open ("sample.txt");
    char ch1 ;
    string line;
    // cout << ch1 << endl;
    // ch1 = getChar();
    // cout << ch1 << endl;
    // ch1 = getChar();
    // cout << ch1 << endl;
    // ungetchar();

    //int i = 0;
    while(! file.eof() ){
        //i++;
        //cout << "next1111111..... \n";
        line = nextToken();
        cout << line << '\n';
        // cout << "next..... \n";
    }
    //cout << "i: " << i << '\n';
    // while (! file.eof()) {
    //     if (ch1 == '#')
    //     {
    //         cout << "comment .........\n";
    //     }
    //     cout << ch1 << '\n';
    //     ch1 = getChar();
    // }
  return 0;
}
// #include <stdio.h>

// int main ()
// {
//   FILE * pFile;
//   int c;
//   char buffer [256];

//   pFile = fopen ("sample.txt","rt");
//   if (pFile==NULL) perror ("Error opening file");
//   else while (!feof (pFile)) {
//     c=getc (pFile);
//     if (c == EOF) break;
//     if (c == '#') ungetc ('@',pFile);
//     else ungetc (c,pFile);
//     if (fgets (buffer,255,pFile) != NULL)
//       fputs (buffer,stdout);
//     else break;
//   }
//   return 0;
// }

// reading a text file
// #include <iostream>
// #include <fstream>
// #include <string>
// using namespace std;

// char ch;
// fstream fin("sample.txt", fstream::in);

// char getChar(){
//     if(fin >> noskipws >> ch){
//         return ch ;
//     }
//     return 0;
// }

// string next

// int main () {
    
//     return 0;
// }
