// reading a text file
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

char c;
ifstream file;

char buffer [256];

char getChar(){
    if(file.is_open())
        if (!file.eof()) //eof check mikone ke be akhare file residim ya na
        {
            file >> c ;
            return c;
        }
    
    return '\0';
    
}

void ungetchar(){
    file.seekg(1,ios::cur);
}

int main () {
    file.open ("sample.txt");
    cout << getChar() << endl;
    cout << getChar() << endl;
   
    ungetchar();
    cout << getChar() << endl;
    

    // while (ch1  != 0) {
    //     if (ch1 == ' ')
    //     {
    //         cout << "return .........\n";
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
