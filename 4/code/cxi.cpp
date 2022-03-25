// $Id: cxi.cpp,v 1.1 2020-11-22 16:51:43-08 - - $

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <fstream>
using namespace std;

#include <libgen.h>
#include <sys/types.h>
#include <unistd.h>

#include "protocol.h"
#include "logstream.h"
#include "sockets.h"

logstream outlog (cout);
struct cxi_exit: public exception {};

unordered_map<string,cxi_command> command_map {
   {"exit", cxi_command::EXIT},
   {"help", cxi_command::HELP},
   {"ls"  , cxi_command::LS  },
   {"put" , cxi_command::PUT },
   {"get" , cxi_command::GET },
   {"rm"  , cxi_command::RM  },
};

static const char help[] = R"||(
exit         - Exit the program.  Equivalent to EOF.
get filename - Copy remote file to local host.
help         - Print help summary.
ls           - List names of files on remote server.
put filename - Copy local file to remote host.
rm filename  - Remove file from remote server.
)||";

void cxi_help() {
   cout << help;
}

bool check_file(string file)
{
   bool valid = true;
   int length = static_cast<int>(file.length());

   for(int i = 0; i < length; ++i)
   {
      if(file.at(i) == '/')
      {
         cerr << "Error char at index " << i << endl;
         valid = false;
         return valid;
      }
      if(i == length and file.at(i) != '\0')
      {
         valid = false;
         return valid;
      }
   }

   return valid;
}

void cxi_ls (client_socket& server) {
   cxi_header header;
   header.command = cxi_command::LS;
   outlog << "sending header " << header << endl;
   send_packet (server, &header, sizeof header);
   recv_packet (server, &header, sizeof header);
   outlog << "received header " << header << endl;
   if (header.command != cxi_command::LSOUT) {
      outlog << "sent LS, server did not return LSOUT" << endl;
      outlog << "server returned " << header << endl;
   }else {
      size_t host_nbytes = ntohl (header.nbytes);
      auto buffer = make_unique<char[]> (host_nbytes + 1);
      recv_packet (server, buffer.get(), host_nbytes);
      outlog << "received " << host_nbytes << " bytes" << endl;
      buffer[host_nbytes] = '\0';
      cout << buffer.get();
   }
}


void usage() {
   cerr << "Usage: " << outlog.execname() << " [host] [port]" << endl;
   throw cxi_exit();
}

void cxi_put (client_socket& server, string file)
{
   cxi_header header;
   header.command = cxi_command::PUT;

   if(not check_file(file))
   {
      outlog << "Error: invalid file name " << file << endl;
   }
   else
   {
      ifstream input(file, ifstream::binary);
      if(not input)
      {
         outlog << "Error: file " << file << " not found." << endl;
      }
      else
      {
         int size = sizeof(header.filename);
         strncpy(header.filename, file.c_str(), size);
         char buffer[0x1000];
         input.seekg(0,input.end);
         int len = input.tellg();
         input.seekg(0, input.beg);
         input.read(buffer, len);
         header.nbytes = len;
         outlog << "sending header " << header << endl;
         send_packet (server, &header, sizeof header);
         send_packet (server, buffer, len);
         recv_packet (server, &header, sizeof header);
         outlog << "received header " << header << endl;

         if(header.command != cxi_command::ACK)
         {
            outlog << "Sent put, server did not return ACK" << endl;
            outlog << "Server returned " << header << endl;
         }
         else
         {
            outlog << "Put command successful" << endl;
         }
         input.close();
      }
   }
}

void cxi_get (client_socket& server, string file)
{
   cxi_header header;
   header.command = cxi_command::GET;

   if(not check_file(file))
   {
      cerr << "Error: invalid file name " << file << endl;
   }
   else
   {
      int SIZE = sizeof(header.filename);
      strncpy(header.filename, file.c_str(), SIZE);
      outlog << "sending header " << header << endl;
      send_packet (server, &header, sizeof header);
      recv_packet (server, &header, sizeof header);
      outlog << "received header " << header << endl;

      if (header.command != cxi_command::FILEOUT) 
      {
         outlog << "sent get, server did not return FILEOUT" << endl;
         outlog << "server returned " << header << endl;
      }
      else 
      {
         auto buffer = make_unique<char[]> (header.nbytes + 1);
         recv_packet (server, buffer.get(), header.nbytes);
         outlog << "received " << header.nbytes << " bytes" << endl;
         buffer[header.nbytes] = '\0';
         ofstream output(file);
         output.write(buffer.get(), header.nbytes);
         output.close();
      }
   }
}

void cxi_rm (client_socket& server, string file)
{
   cxi_header header;
   header.command = cxi_command::RM;

   if(not check_file(file))
   {
      cerr << "Error: invalid file name " << file << endl;
   }
   else
   {
      int SIZE = sizeof(header.filename);
      strncpy(header.filename, file.c_str(), SIZE);
      outlog << "sending header " << header << endl;
      send_packet (server, &header, sizeof header);
      recv_packet (server, &header, sizeof header);
      outlog << "received header " << header << endl;

      if(header.command != cxi_command::ACK) 
      {
         outlog << "Sent rm, server didn't return ACK" << endl;
         outlog << "Server returned " << header << endl;
      }
      else 
      {
         outlog << "rm command successful" << endl;
      }
   }
}

vector<string> split (const string &line, const string &cut)
{
   vector<string> result;
   size_t end = 0;

   for(;;)
   {
      size_t begin = line.find_first_not_of(cut, end);

      if(begin == string::npos)
      {
         break;
      }
      end = line.find_first_of(cut, begin);
      result.push_back(line.substr(begin, end - begin));
   }

   return result;
}

int main (int argc, char** argv) {
   outlog.execname (basename (argv[0]));
   outlog << "starting" << endl;
   vector<string> args (&argv[1], &argv[argc]);
   if (args.size() > 2) usage();
   string host = get_cxi_server_host (args, 0);
   in_port_t port = get_cxi_server_port (args, 1);
   outlog << to_string (hostinfo()) << endl;
   try {
      outlog << "connecting to " << host << " port " << port << endl;
      client_socket server (host, port);
      outlog << "connected to " << to_string (server) << endl;
      for (;;) {
         string line;
         getline (cin, line);
         if (cin.eof()) throw cxi_exit();

         vector<string> words = split(line, " \t");
         if(words.size() == 0)
         {
            continue;
         }

         outlog << "command " << line << endl;
         const auto& itor = command_map.find (words[0]);
         cxi_command cmd = itor == command_map.end()
                         ? cxi_command::ERROR : itor->second;

         string file;
         if(cmd == cxi_command::PUT
            or cmd == cxi_command::GET
            or cmd == cxi_command::RM)
         {
            if(words.size() < 2)
            {
               outlog << words[0] << ":requires filename" << endl;
               continue;
            }
            if(words.size() > 2)
            {
               outlog << words[0] << ": too many arguments" << endl;
               continue;
            }
            file = words[1];
         }

         switch (cmd) {
            case cxi_command::EXIT:
               throw cxi_exit();
               break;
            case cxi_command::HELP:
               cxi_help();
               break;
            case cxi_command::LS:
               cxi_ls (server);
               break;
            case cxi_command::PUT:
               cxi_put (server, file);
               break;
            case cxi_command::GET:
              cxi_get (server, file);
              break;
            case cxi_command::RM:
              cxi_rm (server, file);
              break;
            default:
               outlog << words[0] << ": invalid command" << endl;
               break;
         }
      }
   }catch (socket_error& error) {
      outlog << error.what() << endl;
   }catch (cxi_exit& error) {
      outlog << "caught cxi_exit" << endl;
   }
   outlog << "finishing" << endl;
   return 0;
}
