// $Id: main.cpp,v 1.11 2018-01-25 14:19:29-08 - - $

#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <regex>

using namespace std;

#include "listmap.h"
#include "xpair.h"
#include "util.h"

using str_str_map = listmap<string,string>;
using str_str_pair = str_str_map::value_type;

void scan_options (int argc, char** argv) {
   opterr = 0;
   for (;;) {
      int option = getopt (argc, argv, "@:");
      if (option == EOF) break;
      switch (option) {
         case '@':
            debugflags::setflags (optarg);
            break;
         default:
            complain() << "-" << char (optopt) << ": invalid option"
                       << endl;
            break;
      }
   }
}

string trimstring(string word){
   for(unsigned int i = 0; i < word.length(); i++){
      if(word.at(i) == 32 ){
          word.erase(i,1);
          i = -1;
          continue;
      }else{
          break;
      }
   }
   for(unsigned int i = word.length()-1; i>0; i--){
      if(word.at(i) == 32 ){
          word.erase(i,1);
          i = word.length();
          continue;
      }else{
          break;
      }
   }
return word;
}

int main (int argc, char** argv) {

  str_str_map map;
  fstream file;
  std:: string line;
  smatch match;
  string arg;
  string arg2;

  regex comment{("^\\s*#.*$")};
  regex key{("^\\s*[^#.*=]*[^=]$")};
  regex key_equals{("(^\\s*[^=]+)=\\s*$")};
  regex key_equals_value{("(^\\s*[^=]+)=(\\s*\\S.*)$")};
  regex equals{("^\\s*=\\s*$")};
  regex equals_value{("^\\s*=(\\s*\\S.*$)")};

if(argc != 1){
  //Loop to handle file inputs
  for(int i = 1; i < argc; i++){
  map.initialize();
  int linenum = 0;
  string argument = argv[i];
  if(argument == "-"){
        goto cin_loop;
  }
  file.open(argv[i]);
  while(std::getline(file,line)){
        linenum++;
        cout << argv[i] << ": " << linenum << ": " << line << endl;
        if(regex_search(line,match,comment)){
               continue;
        }
        if(regex_search(line,match,key)){
               arg = match[0];
               arg = trimstring(arg);
               if(map.find(arg) == map.end()){
                        try{
                        string error = arg;
                        throw runtime_error(error + 
                        ": key not found");
                        }
                        catch (runtime_error err)
                        {cerr << err.what() << endl;}
                        continue;
               }else{
                        cout << map.find(arg)->first << " = "
                        << map.find(arg)->second << endl;
               }
        }
        if(regex_search(line,match,key_equals)){
               arg = match[1];
               arg = trimstring(arg);
               if(map.find(arg) != map.end()){
                        map.erase(map.find(arg));
               }
               continue;
        }
        if(regex_search(line,match,key_equals_value)){
                arg = match[1];
                arg2 = match[2];
                arg = trimstring(arg);
                arg2 = trimstring(arg2);
                cout << arg << " = " << arg2 << endl;
                str_str_pair pair(arg,arg2);
                map.insert(pair);
                continue;
        }
        if(regex_search(line,match,equals)){
                map.print();
                continue;
        }
        if(regex_search(line,match,equals_value)){
               arg = match[1];
               arg = trimstring(arg);
               map.print_value(arg);
               continue;
        } 
   }
   file.close();
   map.~listmap();
  }
}

return 0;

cin_loop:
   string userinput;
   cin >> userinput;
   int linenumber = 0;
   while (userinput != "exit"){
	    line = userinput;
		linenumber++;
		cout << "cin: " << linenumber << ": " << line << endl;
        if(userinput == "h"){
             return 1;
        }
		if(regex_search(line,match,comment)){
               continue;
        }
        if(regex_search(line,match,key)){
               arg = match[0];
               arg = trimstring(arg);
               if(map.find(arg) == map.end()){
                        try{
                        string error = arg;
                        throw runtime_error(error + 
                        ": key not found");
                        }
                        catch (runtime_error err)
                        {cerr << err.what() << endl;}
                        continue;
               }else{
                        cout << map.find(arg)->first << " = "
                        << map.find(arg)->second << endl;
               }
        }
        if(regex_search(line,match,key_equals)){
               arg = match[1];
               arg = trimstring(arg);
               if(map.find(arg) != map.end()){
                        map.erase(map.find(arg));
               }
               continue;
        }
        if(regex_search(line,match,key_equals_value)){
                arg = match[1];
                arg2 = match[2];
                arg = trimstring(arg);
                arg2 = trimstring(arg2);
                cout << arg << " = " << arg2 << endl;
                str_str_pair pair(arg,arg2);
                map.insert(pair);
                continue;
        }
        if(regex_search(line,match,equals)){
                map.print();
                continue;
        }
        if(regex_search(line,match,equals_value)){
               arg = match[1];
               arg = trimstring(arg);
               map.print_value(arg);
               continue;
        } 
        cin >> userinput;
   }
  /* sys_info::execname (argv[0]);
   scan_options (argc, argv);

   str_str_map test;
   for (char** argp = &argv[optind]; argp != &argv[argc]; ++argp) {
      str_str_pair pair (*argp, to_string<int> (argp - argv));
      cout << "Before insert: " << pair << endl;
      test.insert (pair);
   }

   for (str_str_map::iterator itor = test.begin();
        itor != test.end(); ++itor) {
      cout << "During iteration: " << *itor << endl;
   }

   str_str_map::iterator itor = test.begin();
   test.erase (itor);

   cout << "EXIT_SUCCESS" << endl;*/
   return EXIT_SUCCESS;
}

