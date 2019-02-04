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

// Cat command complete (CHECK ON DIRECTORY INPUTS)
void fn_cat (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   if(words.size() == 1){
        throw command_error("error: no file name given");
   }
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

void fn_ls (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   
}

void fn_lsr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

//IN PROGRESS
void fn_make (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   if(words.size() == 1){
        throw command_error("error: no file name given");
   }
   //MAKE SOMETHING TO CHECK IF FILE IS DIRECTORY
   
   //MAKE SOMETHING TO CHECK IF FILE NAME ALREADY EXISTS

   //Initializes name and inode
   string name = words[1];
   inode_ptr newfileptr = make_shared<inode>(file_type::PLAIN_TYPE);
   //Checks to see if arguments are given after namei
   inode_ptr currdir = state.getcwd();
   if(words.size() == 2){
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

void fn_mkdir (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
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

void fn_rm (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_rmr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

