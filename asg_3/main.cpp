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

int main (int argc, char** argv) {
  if (argc == 0){

  }
  str_str_map map;
  fstream file;
  std:: string line;
  file.open(argv[1]);
  smatch match;

  regex empty{("\\s*")};
  regex comment{("^\\s*#.*$")};
  regex key{("^\\s*[^#.*=]*[^=]$")};
  regex key_equals{("(^\\s*[^=]+)=\\s*$")};
  regex key_equals_value{("(^\\s*[^=]+)=(\\s*\\S.*)$")};
  regex equals{("^\\s*=\\s*$")};
  regex equals_value{("^\\s*=(\\s*\\S.*$)")};

  map.initialize();
  while(std::getline(file,line)){
        if(regex_search(line,match,comment)){
               //Echo
               continue;
        }
        if(regex_search(line,match,key)){
               if(map.find(match[0]) == map.end()){
                        //throw error
               }else{
                        cout << map.find(match[0])->first << endl;
                        cout << map.find(match[0])->second << endl;
               }
        }
        if(regex_search(line,match,key_equals)){
               if(map.find(match[1]) != map.end()){
                        map.erase(map.find(match[1]));
               }
               continue;
        }
        if(regex_search(line,match,key_equals_value)){
                str_str_pair pair(match[1],match[2]);
                map.insert(pair);
                continue;
        }
        if(regex_search(line,match,equals)){
                map.print();
                continue;
        }
        if(regex_search(line,match,equals_value)){
               map.print_value(match[1]);
               continue;
        } 
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

