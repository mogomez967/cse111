// $Id: commands.cpp,v 1.20 2021-01-11 15:52:17-08 - - $

#include "commands.h"
#include "debug.h"
#include <algorithm>

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
   {"rmr"   , fn_rmr   },
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
   int status = exec::status();
   cout << exec::execname() << ": exit(" << status << ")" << endl;
   return status;
}

void fn_cat (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   wordvec w_vector = words;
   w_vector.erase(w_vector.begin());
   string new_string;
   for(auto const& s : w_vector) {new_string += s;}

   string result = "cat: " + new_string + ": No such file or directory";
   map<string, string>::iterator my_map = state.file_content.find(new_string);

   if(my_map != state.file_content.end())
   {
      result = my_map->second;
   }

   cout << result << endl;
}

void fn_cd (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_echo (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   cout << word_range (words.cbegin() + 1, words.cend()) << endl;
}


void fn_exit (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   throw ysh_exit();
}

inode_ptr find_value(inode_state& state, string param)
{
   inode_ptr value;

   for(auto &i : state.file_name)
   {
      if(param == i.first)
      {
         value = i.second;
         break;
      }
   }

   return value;
}

void fn_ls (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   wordvec w_vector = words;
   w_vector.erase(w_vector.begin());
   string new_string;
   for(auto const& s : w_vector) {new_string += s;}

   if(words.size() == 1)
   {
      string result;
      auto compare = state.file_name;
      for(auto i = compare.begin(); i != compare.end(); ++i)
      {
         if(state.get_cwd() != i->second)
         {
            continue;
         }
         else
         {
            result = result + " " + i->first;
         }
      }

      cout <<result << endl;
   }
   else if(new_string[0] == '/')
   {
      inode_ptr string_root = find_value(state, ". ..");
      string result;
      auto compare = state.file_name;
      for(auto i = compare.begin(); i != compare.end(); ++i)
      {
         if(string_root != i->second)
         {
            continue;
         }
         else
         {
            result = result + " " + i->first;
         }
      }

      cout << result << endl;
   }
   else
   {
      wordvec second_vector = words;
      second_vector.erase(second_vector.begin());
      string second_string;
      for(auto const& s : second_vector) {second_string += s;}

      if(second_string[second_string.size() - 1] == '/')
      {
         second_string.pop_back();
      }

      string the_file;

      for(int i = second_string.size() - 1; i >= 0; --i)
      {
         if(second_string[i] == '/')
         {
            break;
         }
         else
         {
            the_file = the_file + second_string[i];
         }
      }

      reverse(the_file.begin(), the_file.end());

      inode_ptr new_ptr = find_value(state, the_file);

      string result;
      auto compare = state.file_name;
      for(auto i = compare.begin(); i != compare.end(); ++i)
      {
         if(i->second == new_ptr)
         {
            result = result + " " + i->first;
         }
         else
         {
            continue;
         }
      }

      cout << result << endl;
   } 
}

void fn_lsr (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_make (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   if(words.size() > 1)
   {
      string result;
      for(long unsigned int i = 2; i < words.size(); ++i)
      {
         if( i == words.size() - 1)
         {
            result = result + words[i];
         }
         else
         {
            result = result + words[i] + " ";
         }
      }

      state.file_content.insert({words[1], result});
      state.file_name.insert({words[1], state.get_cwd()});
   }
   else
   {
      throw command_error("make: *** No targets specified. \n");
   }
}

void fn_mkdir (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   if(words.size() > 1)
   {
      wordvec w_vector = words;
      w_vector.erase(w_vector.begin());
      string new_string;
      for(auto const& s : w_vector) {new_string += s;}

      inode_ptr my_inode = make_shared<inode>(file_type::DIRECTORY_TYPE);
      state.directories.insert({new_string, my_inode});

      state.setCwd(my_inode);
      state.filepath = state.filepath + new_string + "/";
   }
   else
   {
      throw command_error("mkdir: *** No targets specified. \n");
   }
}

void fn_prompt (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   wordvec my_prompt = words;

   wordvec w_vector = my_prompt;
   w_vector.erase(w_vector.begin());
   string new_string;
   for(auto const& s : w_vector) {new_string += s;}

   state.prompt(new_string);
}

void fn_pwd (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   cout << state.filepath << endl;
}

void fn_rm (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   wordvec w_vector = words;
   w_vector.erase(w_vector.begin());
   string new_string;
   for(auto const& s : w_vector) {new_string += s;}

   state.file_name.erase(new_string);
   
   inode_ptr my_file = find_value(state, new_string);
   state.directories.erase(new_string);
   state.file_content.erase(new_string);

   auto compare = state.directories;
   for(auto i = compare.begin(); i != compare.end(); ++i)
   {
      if(i->second == my_file)
      {
         state.directories.erase(i->first);
      }
      else
      {
         continue;
      }
   }
}

void fn_rmr (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}
