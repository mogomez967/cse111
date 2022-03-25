// $Id: file_sys.cpp,v 1.9 2020-10-26 21:32:08-07 - - $

#include <cassert>
#include <iostream>
#include <stdexcept>

using namespace std;

#include "debug.h"
#include "file_sys.h"

size_t inode::next_inode_nr {1};

map<string, inode_ptr> inode_state::directories;
map<string, string> inode_state::file_content;
map<string, inode_ptr> inode_state::file_name;

string inode_state::filepath;

ostream& operator<< (ostream& out, file_type type) {
   switch (type) {
      case file_type::PLAIN_TYPE: cout << "PLAIN_TYPE"; break;
      case file_type::DIRECTORY_TYPE: cout << "DIRECTORY_TYPE"; break;
      default: assert (false);
   };
   return out;
}

inode_state::inode_state() {
   /*
   DEBUGF ('i', "root = " << root << ", cwd = " << cwd
          << ", prompt = \"" << prompt() << "\"");
   */
  inode new_inode(file_type::DIRECTORY_TYPE);
  root = make_shared<inode>(file_type::DIRECTORY_TYPE);
  cwd = root;

  // create directory and insert the root with appropriate periods
  // insert the file path, content, and name into global maps
  directory my_direc;
  my_direc.get_dir().insert({".", root});
  my_direc.get_dir().insert({"..", root});
  directories.insert({"/", root});
  file_name.insert({". ..", root});
  file_content.insert({". ..", ". .."});
  filepath = "/";
}

const string& inode_state::prompt() const { return prompt_; }
// Added function for ease to assist with fn_prompt()
void inode_state::prompt(string prompt) {prompt_ = prompt + " ";}

ostream& operator<< (ostream& out, const inode_state& state) {
   out << "inode_state: root = " << state.root
       << ", cwd = " << state.cwd;
   return out;
}

inode::inode(file_type type): inode_nr (next_inode_nr++) {
   switch (type) {
      case file_type::PLAIN_TYPE:
           contents = make_shared<plain_file>();
           break;
      case file_type::DIRECTORY_TYPE:
           contents = make_shared<directory>();
           break;
   }
   DEBUGF ('i', "inode " << inode_nr << ", type = " << type);
}

size_t inode::get_inode_nr() const {
   DEBUGF ('i', "inode = " << inode_nr);
   return inode_nr;
}


file_error::file_error (const string& what):
            runtime_error (what) {
}

const wordvec& base_file::readfile() const {
   throw file_error ("is a " + error_file_type());
}

void base_file::writefile (const wordvec&) {
   throw file_error ("is a " + error_file_type());
}

void base_file::remove (const string&) {
   throw file_error ("is a " + error_file_type());
}

inode_ptr base_file::mkdir (const string&) {
   throw file_error ("is a " + error_file_type());
}

inode_ptr base_file::mkfile (const string&) {
   throw file_error ("is a " + error_file_type());
}


size_t plain_file::size() const {
   size_t size {0};
   DEBUGF ('i', "size = " << size);
   return size;
}

const wordvec& plain_file::readfile() const {
   DEBUGF ('i', data);
   return data;
}

void plain_file::writefile (const wordvec& words) {
   DEBUGF ('i', words);
}

size_t directory::size() const {
   size_t size {0};
   DEBUGF ('i', "size = " << size);
   return size;
}

void directory::remove (const string& filename) {
   DEBUGF ('i', filename);

   if(!(dirents.empty()))
   {
    map<string, inode_ptr>::iterator my_iterator;
    if(dirents.count(filename) == 0)
    {
      throw file_error("is a " + error_file_type());
    }
    else
    {
      my_iterator = dirents.find(filename);
      dirents.erase(my_iterator);
      cout << filename << " has been removed." << endl;
    }
   }
   else
   {
    throw file_error ("is a " + error_file_type());
   }
}

inode_ptr directory::mkdir (const string& dirname) {
   DEBUGF ('i', dirname);
   return nullptr;
}

inode_ptr directory::mkfile (const string& filename) {
   DEBUGF ('i', filename);
   return nullptr;
}
