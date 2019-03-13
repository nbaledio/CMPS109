// $Id: cix.cpp,v 1.7 2019-02-07 15:14:37-08 - - $
//Nathan Baledio
//ID: 1574354

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <fstream>
using namespace std;

#include <libgen.h>
#include <sys/types.h>
#include <unistd.h>

#include "protocol.h"
#include "logstream.h"
#include "sockets.h"

logstream log (cout);
struct cix_exit: public exception {};

unordered_map<string,cix_command> command_map {
   {"exit", cix_command::EXIT},
   {"help", cix_command::HELP},
   {"ls"  , cix_command::LS  },
   {"get" , cix_command::GET },
   {"put" , cix_command::PUT },
   {"rm"  , cix_command::RM  },
};

static const string help = R"||(
exit         - Exit the program.  Equivalent to EOF.
get filename - Copy remote file to local host.
help         - Print help summary.
ls           - List names of files on remote server.
put filename - Copy local file to remote host.
rm filename  - Remove file from remote server.
)||";

//rm command
void cix_rm(client_socket&server, string filename){
   cix_header header;
   snprintf(header.filename,sizeof(header.filename),filename.c_str());
   header.command = cix_command::RM;
   //log << "sending header " << header << endl;
   send_packet (server, &header, sizeof header);
   recv_packet (server, &header, sizeof header);
   //log << "received header " << header << endl;
   if(header.command != cix_command::ACK){
      fprintf(stderr, "Client: Sent RM, server did not return ACK\n");
      fprintf(stderr, "Server returned NAK\n");
   }
}

//put command (basically cixd get)
void cix_put(client_socket& server,string filename){
   cix_header header;
   std::ifstream file(filename, std::ios::binary);
   //Checks if file exists in local directory
   if(file.is_open() == false){
       fprintf(stderr, "Client: ");
       fprintf(stderr, filename.c_str());
       fprintf(stderr,  ": No such file or directory\n");
       return;
   }
   //If it exists, prepares to send file
   snprintf(header.filename,sizeof(header.filename),filename.c_str());
   header.command = cix_command::PUT;
   file.seekg (0, file.end);
   int length = file.tellg();
   file.seekg(0, file.beg);
   char * buffer = new char[length];
   file.read(buffer, length);
   header.nbytes = length;
   //log << "sending header " << header << endl;
   send_packet (server, &header, sizeof header);
   send_packet (server, buffer, length);
   //log << "sent " << length << " bytes" << endl;
   
   //Receives packet from server
   recv_packet (server, &header, sizeof header);
   if(header.command != cix_command::ACK){
      fprintf(stderr, "Client: Sent PUT, server did not return ACK\n");
      fprintf(stderr, "Server returned NAK\n");   
   }else{
   //log << "received header " << header << endl;
   delete buffer;
    }
}

//get command
void cix_get(client_socket& server, string filename){
   //Prepares to send file
   cix_header header;
   //strcpy(header.filename, filename.c_str()); <- Use snprintf
   snprintf(header.filename,sizeof(header.filename),filename.c_str());
   header.command = cix_command::GET;
   //log << "sending header " << header << endl;
   send_packet (server, &header, sizeof header);
   recv_packet (server, &header, sizeof header);
   //log << "received header " << header << endl;
   
   //Checks to see if a file was returned
   if(header.command != cix_command::FILEOUT){
       fprintf(stderr,
       "Client: Sent GET, server did not return FILEOUT\n");
       fprintf(stderr, "Server returned NAK\n");
   }else{
      //Writes file to current directory
      char* buffer = new char[header.nbytes];
      recv_packet (server, buffer, header.nbytes);
      //log << "received " << header.nbytes << " bytes" << endl;
      std::ofstream newFile(filename,std::ios::binary);
      newFile.write(buffer,header.nbytes);
      buffer[header.nbytes] = '\0';
      delete[] buffer; 
   }
}

//help command
void cix_help() {
   cout << help;
}

//ls command
void cix_ls (client_socket& server) {
   cix_header header;
   header.command = cix_command::LS;
   //log << "sending header " << header << endl;
   send_packet (server, &header, sizeof header);
   recv_packet (server, &header, sizeof header);
   //log << "received header " << header << endl;
   if (header.command != cix_command::LSOUT) {
      fprintf(stderr, 
      "Client: Sent LS, server did not return LSOUT\n");
      fprintf(stderr, "Server returned NAK\n");
   }else {
      auto buffer = make_unique<char[]> (header.nbytes + 1);
      recv_packet (server, buffer.get(), header.nbytes);
      //log << "received " << header.nbytes << " bytes" << endl;
      buffer[header.nbytes] = '\0';
      cout << buffer.get();
   }
}


void usage() {
   cerr << "Usage: " << log.execname() << " [host] [port]" << endl;
   throw cix_exit();
}

int main (int argc, char** argv) {
   log.execname (basename (argv[0]));
   //log << "starting" << endl;
   vector<string> args (&argv[1], &argv[argc]);
   if (args.size() > 2) usage();
   string host = get_cix_server_host (args, 0);
   in_port_t port = get_cix_server_port (args, 1);
   log << to_string (hostinfo()) << endl;
   try {
      log << "connecting to " << host << " port " << port << endl;
      client_socket server (host, port);
      log << "connected to " << to_string (server) << endl;
      for (;;) {
         string line;
         getline (cin, line);
         if (cin.eof()) throw cix_exit();
         //log << "command " << line << endl;
         //Tokenizes string (For filename inputs)
         vector <string> tokens;
         stringstream reader(line);
         string token;
         while(getline(reader, token, ' ')){
            tokens.push_back(token);
         } 
         const auto& itor = command_map.find (tokens[0]);
         cix_command cmd = itor == command_map.end()
                         ? cix_command::ERROR : itor->second;
         switch (cmd) {
            case cix_command::EXIT:
               if(tokens.size() != 1){goto defaultc;}
               throw cix_exit();
               break;
            case cix_command::HELP:
               if(tokens.size() != 1){goto defaultc;}
               cix_help();
               break;
            case cix_command::LS:
               if(tokens.size() != 1){goto defaultc;}
               cix_ls (server);
               break;
            case cix_command::GET:
               cix_get(server, tokens[1]);
               break;
            case cix_command::PUT:
               cix_put(server,tokens[1]);
               break;
            case cix_command::RM:
               cix_rm(server,tokens[1]);
               break;  
            default:
               defaultc:
               log << line << ": invalid command" << endl;
               break;
         }
      }
   }catch (socket_error& error) {
      log << error.what() << endl;
   }catch (cix_exit& error) {
      log << "caught cix_exit" << endl;
   }
   log << "finishing" << endl;
   return 0;
}

