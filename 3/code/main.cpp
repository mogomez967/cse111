// $Id: main.cpp,v 1.13 2021-02-01 18:58:18-08 - - $

#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <unistd.h>
#include <regex>
#include <cassert>
#include <fstream>

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

void file_help(istream& infile, const string& filename, str_str_map& list)
{
   regex com {R"(^\s*(#.*)?$)"};
   regex key {R"(^\s*(.*?)\s*=\s*(.*?)\s*$)"};
   regex condense {R"(^\s*([^=]+?)\s*$)"};

   int line_count = 1;

   for(;;)
   {
      string line;
      getline(infile, line);
      if(infile.eof())
      {
         break;
      }

      cout << filename << ": " << line_count << ": ";
      cout << line << endl;
      smatch result;

      if(regex_search(line, result, com))
      {
         ++line_count;
         continue;
      }

      if(regex_search(line, result, key))
      {
         if(result[1] == "" && result[2] == "")
         {
            for(str_str_map::iterator itor = list.begin(); itor != list.end(); ++itor)
            {
               cout << itor->first << " = " << itor->second << endl;
            }
         }
         else if(result[1] != "" && result[2] == "")
         {
            str_str_map::iterator itor = list.find(result[1]);
            if(itor != list.end())
            {
               list.erase(itor);
            }
         }
         else if (result[1] == "" && result[2] != "") 
         {
            for (str_str_map::iterator itor = list.begin(); itor != list.end(); ++itor) 
            {
               if((*itor).second == result[2]) 
               {
                  cout<<itor->first<<" = "<<itor->second<<endl;
               }
            }
         }
         else
         {
            str_str_pair new_entry (result[1], result[2]);
            list.insert(new_entry);
            cout << result[1] << " = " << result[2] << endl;
         }
      }
      else if(regex_search(line, result, condense))
      {
         str_str_map::iterator itor = list.find(result[1]);
         if(itor == list.end())
         {
            cerr << result[1] << ": key not found" << endl;
         }
         else
         {
            cout << itor->first << " = " << itor->second << endl;
         }
      }
      else
      {
         assert(false && "This can not happen.");
      }

      ++line_count;
   }
}

const string cin_arg = "-";

int main (int argc, char** argv) 
{
   sys_info::execname (argv[0]);
   scan_options (argc, argv);

   str_str_map list {};
   int num_inputs = 0;

   for (char** argp = &argv[optind]; argp != &argv[argc]; ++argp) 
   {
      num_inputs++;
      if(*argp == cin_arg) 
      {
         file_help(cin, *argp, list);
      }
      else 
      {
         ifstream infile (*argp);
         if(infile.fail()) 
         {
            complain() << *argp <<
               ": No such file or directory" << endl;
            sys_info::exit_status(1);
         }
         else 
         {
            file_help(infile, *argp, list);
            infile.close();
         }
      }
      
   }

   if(num_inputs == 0) 
   {
      file_help(cin, cin_arg,list);
   }

   str_str_map::iterator itor = list.begin();
   list.erase (itor);
   return sys_info::exit_status();
}

