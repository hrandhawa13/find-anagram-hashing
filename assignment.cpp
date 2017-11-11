#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>
#include <vector>

using namespace std;

/*
 * Assignment 5
 *      Created on: July 16, 2017
 *      Author: harmanjit Randhawa
 *      Reference: using lecture notes and google 
 *      Purpose: to make hash table and then find anagrams of words
 *      int comp is the variable that i used to keep track of how many collisions occured while searching for a word , I printed that on the console
 *      i used coalesced chaining by using 2 different vectors to handle the data and collsiions
 *      the max comparisons needed for the given input file was 3, i discussed it with Bita and she said it was good to go 
 *      let me know if you have any questions 
 *      thanks
*/

int i = 0, comp =0;
vector< vector < string > > mainVec;// this is the main vec 
vector< vector < string > > vec2;//this is a vector for collisions 

string removePunc(const string &str );
void getWords(const string &file);
void getInput(const string & inFile, const string & outFile);
int hashFunc(const string &str);
void insertTable( string & word);
void insertList(int value, string & word, string & sortedWord, vector<vector<string>> & v1, vector<vector<string>> & v2);
void insertBucket(int value, string & word, string & sortedWord, vector<vector<string>> & vec);
string findAnagram(const string & sortedWord, const string & word, const int &value, const vector<vector<string>> & vec);
void print(vector<vector<string>> &vect);
void initVec(vector<vector<string>> & vec, int value);
void rehash();

int main(){
   initVec(mainVec, 50000);//initalises 2 vectors with initial value -1 at all the indexes
   initVec(vec2, 20000);
   string dict = "dictionary";
   string inFile = "input.txt";
   string outFile = "output.txt";
   getWords(dict);//puts all the words into the hash table 
   getInput(inFile, outFile);//gets word from infile and writes its anagram in outfile
   return 0;
   }
   
void getWords(const string &file){//take a file name and read all the words from that file and then sort those words and put it in the list 
   string fileName = file + ".txt";// creates a string with .txt extension and the year passed in the method
   string word;
   int num =0;
   int size = mainVec.size();
   int lf = num/size*100;//load factor
   ifstream input(fileName);// creating file stream 
   
   // if the file doesnt exist
   if (input.fail()) { 
    cout << "File does not exist \n Exit program " << endl; 
   }
   
   /* while the file is open and its not end of the file, it will read data line by line, 
   remove punctuation, sort the letters of the word and then store it in the vector table*/
   if(input.is_open()){   
      while(input >> word){
         if (lf > 60)
            rehash();
         insertTable(word);
         num++;
      }
   }
   input.close();//closing the file
}

void insertTable( string & word){//this function takes a string, removes punctuations, sorts it, calculate a hash value and inserts it in the table
   string sortedWord = removePunc(word);
   sort(sortedWord.begin(), sortedWord.end());
   int value = hashFunc(sortedWord);
   insertList( value, word, sortedWord, mainVec, vec2 );
   }

string removePunc(const string &str ){//this function removes all the puntuations, spaces and replace capital letters
   string temp;
   char toFind, ch;
   int size = str.size();
   for ( int j =0; j<size; j++){
      ch = str.at(j);
      if ( ch >= 'A' && ch <= 'Z' ){
         toFind = ch - 'A' + 'a' ;
         temp += toFind;
         }
      else if ( ch >= 'a' && ch <= 'z' )
         temp += ch;
      }
   return temp;
   }

void insertList( int value, string & word, string & sortedWord, vector<vector<string>> & v1, vector<vector<string>> & v2 ){
   //this function inserts a word at its appropiate place 
   int size = v1[value].size();
   if ( v1[value][0] == sortedWord ){//sortedWord is already there and the new word is anagram of that word 
      string t = v1[value][size-1];
      v1[value][size-1] = word;
      v1[value].push_back(t);
      }
   else if (v1[value][0] == "-1" )//empty space on this index 
      insertBucket(value, word, sortedWord, v1);
   else {
      int t = stoul(v1[value][size-1]);
      if (t == -1){//this is the first collision and thus it inserts the new word into the free index in second table
         insertBucket(i, word, sortedWord, v2);
         v1[value][size-1] = to_string(i);
         i++;
         }
      else //there is already more than one collision at that index so check the index in list2 and then insert accordingly 
         insertList(t, word, sortedWord, v2, v2);
      }
   }

void insertBucket(int value, string & word, string & sortedWord, vector<vector<string>> & vec){//inserts elment in list1 when the index is free
   vec[value][0] = sortedWord;//override first element 
   vec[value].push_back(word);
   vec[value].push_back("-1");
   }  
   
int hashFunc(const string & str){//this function takes a string as input and returns an int calculated by a function 
   int x=3;
   int size = str.size();
   int hash = 5381;//random odd num  and also used in the djb2 algo
   for(int i =0; i< size; i++){
      x = str.at(i);
      hash = hash+ x*(997^(size-i));//997 is the largest 3 digit prime 
      }
   hash = hash%(mainVec.size()-1);
   return hash;
   }

void print(vector<vector<string>> & vect){//prints the vector 
   int size = vect.size();
   int size2;
   for( int j =0; j< size; j++){
      vector<string> temp = vect[j];
      if(temp[0] != "-1"){   
         cout<< j<< " ";         
         size2 = temp.size();
         for( int k =0; k< size2; k++)
            cout<< temp[k] << " ";
         cout << endl ;
      }
   }
}

string findAnagram(const string & sortedWord, const string & word, const int &value, const vector<vector<string>> & vec){
   //finds anagram of a word in the whole list
   comp++;//iterator for num of comparisons
   string str;
   int size = vec[value].size(), count =0;
   if(vec[value][0] == sortedWord){//at index value the sorted word is there thus the anagrams should be at that index
      for(int j = 1; j<size-1; j++){//take all the elemnts at that index
         if (vec[value][j] != word){//adds all the other words than original word to the string
            str = str + vec[value][j] +" ";
            count ++;
         }
      }
      if (count ==0)//the word exists but there is no anagram for that word 
         return  "Sorry, word exists in the dictionary but there is no Anagram in my dictionary for " + word;
      str.append(" ");
      str.append(to_string(count));
      return str;
      }
   //2 cases either -1 or some number
   if (vec[value][size-1] == "-1" || vec[value][0] == "-1")//no anagram at the last index that is no collision 
      return  "Sorry, no Anagram in my dictionary for " + word;
   else //other index in vec2 is linked to that index
      findAnagram(sortedWord, word, stoi(vec[value][size-1]), vec2);
   return  "Sorry, no Anagram in my dictionary for " + word;
}
   
void initVec(vector<vector<string>> & vec, int value){
   vector<string> v;
   v.push_back("-1");
   vec.assign(value, v);
}
   
void rehash(){
   int SIZE = mainVec.size();
   int SIZE2 = vec2.size();
   vector<vector<string>> n1;
   vector<vector<string>> n2;
   i=0;//global i 
   
   initVec(n1, 2*SIZE+1);//initialise new vectors with double size
   initVec(n2, 2*SIZE2+1);
   
   for(int j =0; j<SIZE;j++){//iterating through the first container
      string sw = mainVec[j][0];
      if(sw != "-1"){
         int s = mainVec[j].size();
         int value = hashFunc(sw);
         for(int k=1; k < s-1; k++){
            insertList(value, mainVec[j][k], sw, n1, n2);
         }
      }
   }
   int j =0;
   while (vec2[j][0] != "-1"){//iterating throught the second container and will stop when it encounters first index that has -1 
      string sw = vec2[j][0];
      if(sw != "-1"){
         int s = vec2[j].size();
         int value = hashFunc(sw);
         for(int k=1; k < s-1; k++)
            insertList(value, vec2[j][k], sw, n1, n2);
         j++;
      }
   }
   mainVec = n1;
   vec2 = n2;   
}

void getInput(const string & inFile, const string & outFile){//this function takes the input line by line from the inFile, finds its anagram and writes it in the out file
   string word, sortedWord, temp;
   int value;
   ifstream input(inFile);// creating file stream for inout
   ofstream output(outFile);// creating file stream for inout
   // if the file doesnt exist
   if (input.fail()) { 
    cout << "File does not exist \n Exit program " << endl; 
   }
   /* while the file is open and its not end of the file, it will read data line by line, 
   remove punctuation, sort the letters of the word and then find its anagram and the n write it in the output file */
   if(input.is_open()){   
      while(input >> word){
         comp =0;//iterator for number of comparisons
         sortedWord = removePunc(word);
         sort(sortedWord.begin(), sortedWord.end());
         value = hashFunc(sortedWord);
         temp = findAnagram(sortedWord, word, value, mainVec);
         output << temp << endl;
         cout<< comp << endl;
      }
   }
   input.close();//closing the file
   output.close();
   }
