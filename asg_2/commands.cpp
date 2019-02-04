// $Id: commands.cpp,v 1.17 2018-01-25 14:02:55-08 - - $

#include "commands.h"
#include "debug.h"
#include <string.h>

command_hash cmd_hash {
   {"cat"   , fn_cat   },
   {"cd"    , fn_cd    },
   {"echo"  , fn_echo  },
   {"exit"  , fn_exit  },
   {"ls"    , fn_ls    },
   {"lsr"   , fn_lsr   },
   {"make"  , fn_make  },
   {"mkdir" , fn_mkdir },
   {"prompt", fn_prompt},
   {"pwd"   , fn_pwd   },
   {"rm"    , fn_rm    },
};

command_fn find_command_fn (const string& cmd) {
   // Note: value_type is pair<const key_type, mapped_type>
   // So: iterator->first is key_type (string)
   // So: iterator->second is mapped_type (command_fn)
   DEBUGF ('c', "[" << cmd << "]");
   const auto result = cmd_hash.find (cmd);
   if (result == cmd_hash.end()) {
      throw command_error (cmd + ": no such function");
   }
   return result->second;
}

command_error::command_error (const string& what):
            runtime_error (what) {
}

int exit_status_message() {
   int exit_status = exit_status::get();
   cout << execname() << ": exit(" << exit_status << ")" << endl;
   return exit_status;
}

// ACTUALLY GO BACK AND MAKE IT LOOP THROUGH MULTIPLE ARGUMENTS
// cat command completei
void fn_cat (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   //Checks if arguments are given
   if(words.size() == 1){
        throw command_error("error: no file name given");
   }
   //Iterates through directory to find any file of the given name
   inode_ptr cwdir = state.getcwd();
   map<string,inode_ptr> cdirents = cwdir->getcontents()->getdirents();
   bool filefound = false;
   inode_ptr cfile; 
   for(unsigned int j = 1; j < words.size(); j++){
   std::map<std::string, inode_ptr>::iterator it = cdirents.begin();
        while(it != cdirents.end()){
                if(it->first == words[j]){
                        filefound = true;
                        cfile = it->second;
                }
        it++;
        }
   //If file is found, checks if its a directory and throws
   //an error if it is
   if(filefound == true){
        if(cfile->getcontents()->checkifdir() == true){
                throw command_error("is a directory");
        }
        wordvec op = cfile->getcontents()->readfile();
        for(unsigned int i = 0; i < op.size(); i++){
                if( i == op.size() - 1){
                        cout << op[i] << endl;
                }else{
                        cout << op[i] + " ";
                }                
        }
   }else{
        throw command_error("file does not exist");
   }
   }
}

void fn_cd (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   if(words.size() == 1){
        throw command_error("no path specified");
   }
   if(words.size() > 2){
        throw command_error("more than one operand given");
   }
   string pth = words[1];
   if(words.at(0) == "/"){

   } 
   
}

// Echo command complete
void fn_echo (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   cout << word_range (words.cbegin() + 1, words.cend()) << endl;
}


// Exit command complete
void fn_exit (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   throw ysh_exit();
}

// IN PROGRESS
void fn_ls (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   inode_ptr cwd = state.getcwd();
   if(words.size() != 1){
        string test = words[1];
        if(test.at(0) != '/'){
                return; //THROW AN ERROR, PATH MUST START WITH ROOT
        }
        bool breakloop = false;
        while(breakloop == false){
              //  string direc;
        }
   }
   

   map<string,inode_ptr> dirents = 
   cwd->getcontents()->getdirents();
   std::map<std::string, inode_ptr>::iterator it = dirents.begin();
   cout << state.getcwd()->getpath() << ":" << endl;
   while(it != dirents.end()){
        int nodenumber =  it->second->get_inode_nr();
        int filesize = it->second->getcontents()->size();
        string nodespaces;
        string filespaces;
        if(nodenumber < 10) {nodespaces = "     ";}
        else if(nodenumber < 100) {nodespaces = "    ";}
        else if(nodenumber < 1000) {nodespaces = "   ";}
        else if(nodenumber < 10000) {nodespaces = "  ";}
        else if(nodenumber < 100000) {nodespaces = " ";}

        if(filesize < 10) {filespaces = "     ";}
        else if(filesize < 100) {filespaces = "    ";}
        else if(filesize < 1000) {filespaces = "   ";}
        else if(filesize < 10000) {filespaces = "  ";}
        else if(filesize < 100000) {filespaces = " ";}

        if(it->second->getcontents()->checkifdir() == true){
                cout << nodespaces << it->second->get_inode_nr() << " "
                << filespaces << it->second->getcontents()->size() <<
                " " << it->first;
                if( it->first == "." || it->first == ".."){
                        cout << endl;
                }else{
                        cout << "/" << endl;
                } 
        }else{
                cout << nodespaces << it->second->get_inode_nr() << " "
                << filespaces << it->second->getcontents()->size() <<
                " " << it->first << endl;
        }
        it++;
   } 
}

void fn_lsr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

// make command complete
void fn_make (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   if(words.size() == 1){
        throw command_error("error: no file name given");
   }
   //Checks if file exists. If it does, checks if file is a directory
   //Throws an error if it is, else it overwrites contents
   inode_ptr cwdir = state.getcwd();
   map<string,inode_ptr> cdirents1 = cwdir->getcontents()->getdirents();
   std::map<std::string, inode_ptr>::iterator it = cdirents1.begin();
   while(it != cdirents1.end()){
        if(it->first == words[1]){
               if(it->second->getcontents()->checkifdir() == true){
                        throw command_error("is a directory");
               }else{
                        wordvec data0;
                        for(unsigned int i = 2; i < words.size(); i++){
                             data0.push_back(words[i]);
                        }
                        it->second->getcontents()->writefile(data0);
                        return;
               }
        }
        it++;
   }

   //Initializes name and inode
   string name = words[1];
   inode_ptr newfileptr = make_shared<inode>(file_type::PLAIN_TYPE);
   //Checks to see if arguments are given after namei
   inode_ptr currdir = state.getcwd();
   if(words.size() == 2){
        newfileptr->getcontents()->setname(name);
        currdir->getcontents()->adddirents(name, newfileptr);
        return;
   }
   //Pushes data into a temporary wordvec
   wordvec data;
   for(unsigned int i = 2; i < words.size(); i++){
        data.push_back(words[i]);
   }
   //Writes contents into new file inode and names it
   newfileptr->getcontents()->writefile(data);
   newfileptr->getcontents()->setname(name);
   //Sets pointers so inode can be added to current directory
   currdir->getcontents()->adddirents(name, newfileptr);
}

// mkdir command completei
void fn_mkdir (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   if(words.size() == 1){
        throw command_error("no name given");
   }
   inode_ptr cwd = state.getcwd();
   bool error = false;
   for(unsigned int i = 1; i < words.size(); i++){
         //Checks if name exists (file or directory)
         //skips over it if it does and throws an error
         //at the end
         map<string,inode_ptr> cdirents1 = cwd->getcontents()->
         getdirents();
         std::map<std::string, inode_ptr>::iterator it
         = cdirents1.begin();
         while(it != cdirents1.end()){
                if(it->first == words[i]){
                        error = true;
                        it++;
                        continue;
                }
                it++;
         }
         //Makes new directory if no name is found
         inode_ptr newdirptr = make_shared<inode>
         (file_type::DIRECTORY_TYPE);
         newdirptr->getcontents()->setname(words[i]);
         newdirptr->setname(words[i]);
         newdirptr->getcontents()->adddirents(".", newdirptr);
         newdirptr->getcontents()->adddirents("..", state.getroot()); 
         cwd->getcontents()->adddirents(words[i], newdirptr);
   }
   //Throws error if name exists already, it is placed here
   //so that the code may iterate through all arguments before
   //throwing an error
   if(error == true){
        throw command_error("name already exists");
   }
}

// Prompt command complete
void fn_prompt (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   string newprompt = "";
   //Does not change prompt if no input is given
   if(words.size() == 1){
        return;
   }
   //Changes prompt to given input
   //**** WORDS IS A VECTOR OF WORDS, NOT CHARACTERS
   //INDEX AT 1 NOT 0 ****
   for(unsigned int i = 1; i < words.size(); i++){
        newprompt += words[i] + " ";
   }
   //Calls prompt setter to change prompt
   state.inode_state::setprompt(newprompt);
}

// pwd command (THEORETICALLY COMPLETE)
void fn_pwd (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   //Takes inode state, gets inode pointer of cwd, then
   //retrieves path string of that inode
   string output = state.getcwd()->getpath();
   cout << output << endl;
}

// rm command complete
void fn_rm (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   inode_ptr cwd = state.getcwd();
   map<string,inode_ptr> cdirents1 = cwd->getcontents()->getdirents();
   std::map<std::string, inode_ptr>::iterator it;
   bool filefound = false;
   bool error = false;
   if(words.size() == 1){
        throw command_error("no name specified");
   }
   for(unsigned int i = 1; i < words.size(); i++){
        it = cdirents1.begin();
        filefound = false;
        while(it != cdirents1.end()){
                if(it->first == words[i]){
                        filefound = true;
                        cwd->getcontents()->remove(words[i]);
                        break;
                }  
        it++;
        }
        if(filefound == false){
                error = true;
        }
   }
   if(error == true){
        throw command_error("no file found");
   }
}

void fn_rmr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

