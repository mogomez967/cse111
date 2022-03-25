// $Id: ubigint.cpp,v 1.12 2020-10-19 13:14:59-07 - - $
// Marvin Gomez mogomez@ucsc.edu

#include <cctype>
#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
#include <vector>
#include <string>
using namespace std;

#include "debug.h"
#include "relops.h"
#include "ubigint.h"

// constructors
ubigint::ubigint (const int& value)
{
	ubig_value.push_back(value + 48);
}

ubigint::ubigint (const string& that)
{
	string::const_iterator itor = that.cend() - 1;
	while(that.begin() <= itor)
	{
		ubig_value.push_back(*itor);
		itor--;
	}
}

/*
ubigint::ubigint (unsigned long that): ubig_value (that) {
   DEBUGF ('~', this << " -> " << ubig_value)
}

ubigint::ubigint (const string& that): ubig_value(0) {
   DEBUGF ('~', "that = \"" << that << "\"");
   for (char digit: that) {
      if (not isdigit (digit)) {
         throw invalid_argument ("ubigint::ubigint(" + that + ")");
      }
      ubig_value = ubig_value * 10 + digit - '0';
   }
}
*/

//FIX with function outlined by Mackey
ubigint ubigint::operator+ (const ubigint& that) const {
   int add1;
   int add2;
   int result;
   
   ubigint addition;
   
   bool return1 = false;
   bool return2 = false;
   bool carry = false;
   
   ubigvalue_t::const_iterator left = ubig_value.cbegin();
   ubigvalue_t::const_iterator right = that.ubig_value.cbegin();
   
   // iterate through vectors and determine result based off end add1/add2
   // modified by whether they have end flags.
   while(left != ubig_value.cend() || right != that.ubig_value.cend())
   {
	   if (return1 && return2)
	   {
		   break;
	   }
	   if(return1)
	   {
		   add1 = 0;
	   }
	   else
	   {
		   add1 = *left - 48; // lookup the ASCII difference
	   }
	   if(return2)
	   {
		   add2 = 0;
	   }
	   else
	   {
		   add2 = *right - 48;
	   }
	   
	   // auto add 1 to result for carry flag set true
	   result = add1 + add2;
	   if(carry)
	   {
		   result++;
	   }
	   
	   // for results greater than 9
	   if(result > 9)
	   {
		   carry = true;
		   string sum = to_string(result);
		   addition.ubig_value.push_back(sum.back());
	   }
	   else // carry = false
	   {
		   carry = false;
		   string sum41 = to_string(result);
		   addition.ubig_value.push_back(sum41.front());
	   }
	   
	   left++;
	   right++;
	   
	   if(left == ubig_value.cend() &&  right == that.ubig_value.cend())
	   {
		   return1 = true;
		   return2 = true;
	   }
	   else
	   {
		   if(left == ubig_value.cend())
		   {
			   return1 = true;
		   }
		   if(right == that.ubig_value.cend())
		   {
			   return2 = true;
		   }
	   }
   }
   
   if(carry)
   {
	   addition.ubig_value.push_back('1');
   }
   
   return addition;
}

ubigint ubigint::operator- (const ubigint& that) const {
   int add1;
   int add2;
   int result;
   
   ubigint addition;
   
   bool return1 = false;
   bool return2 = false;
   bool carry = false;
   
   ubigvalue_t::const_iterator left = ubig_value.cbegin();
   ubigvalue_t::const_iterator right = that.ubig_value.cbegin();
   
   // iterate through vectors and determine result based off end add1/add2
   // modified by whether they have end flags.
   while(left != ubig_value.cend() || right != that.ubig_value.cend())
   {
	   if (return1 && return2)
	   {
		   break;
	   }
	   if(return1)
	   {
		   add1 = 0;
	   }
	   else
	   {
		   add1 = *left - 48; // lookup the ASCII difference
	   }
	   if(return2)
	   {
		   add2 = 0;
	   }
	   else
	   {
		   add2 = *right - 48;
	   }
	   
	   // auto add 1 to result for carry flag set true
	   result = add1 - add2;
	   if(carry)
	   {
		   result--;
	   }
	   
	   // for results greater than 9
	   if(result < 0)
	   {
		   carry = true;
		   string sum = to_string(result + 10);
		   addition.ubig_value.push_back(sum.front());
	   }
	   else // carry = false
	   {
		   carry = false;
		   string sum41 = to_string(result);
		   addition.ubig_value.push_back(sum41.front());
	   }
	   
	   left++;
	   right++;
	   
	   if(left == ubig_value.cend() &&  right == that.ubig_value.cend())
	   {
		   return1 = true;
		   return2 = true;
	   }
	   else
	   {
		   if(left == ubig_value.cend())
		   {
			   return1 = true;
		   }
		   if(right == that.ubig_value.cend())
		   {
			   return2 = true;
		   }
	   }
   }
	   
   // lose the disgusting zeroes (tantalo way)
   ubigvalue_t::const_iterator zeroes = addition.ubig_value.cend() - 1;
   
   while((*zeroes - 48) == 0)
   {
	   addition.ubig_value.pop_back();
	   zeroes--;
   }
   
   return addition;
}

ubigint ubigint::operator* (const ubigint& that) const {
   ubigint multiplication;
   int result;
   int carry;
   
   // zero case
   if((ubig_value.size() == 1) || (that.ubig_value.size() == 1))
   {
	   if((*ubig_value.cbegin() - 48) == 0)
	   {
		   multiplication.ubig_value.push_back('0');
		   return multiplication;
	   }
	   if((*that.ubig_value.cbegin() - 48 == 0))
	   {
		   multiplication.ubig_value.push_back('0');
		   return multiplication;
	   }
   }
   
   multiplication.ubig_value.resize(ubig_value.size() + that.ubig_value.size(), '0');
   
   // provided in Mackey example
   for (int m = 0; m < static_cast<int>(ubig_value.size()); m++) 
   {
      carry = 0;
      for (int n = 0; n < static_cast<int>(that.ubig_value.size()); n++) 
	  {
         result = (multiplication.ubig_value.at(m + n) - 48) +
         ((ubig_value.at(m) - 48) * (that.ubig_value.at(n)
                                           - 48) + carry);
         multiplication.ubig_value.at(m + n) = result % 10 + 48;
         carry = result / 10;
      }
      multiplication.ubig_value.at(m + that.ubig_value.size()) = carry + 48;
   }
   
    // lose the disgusting zeroes (tantalo way)
   ubigvalue_t::const_iterator zeroes = multiplication.ubig_value.cend() - 1;
   
   while((*zeroes - 48) == 0)
   {
	   multiplication.ubig_value.pop_back();
	   zeroes--;
   }
   
   return multiplication;
}

void ubigint::multiply_by_2() {
   bool carry = false;
   int result;
   ubigvalue_t multiplication;
   ubigvalue_t::const_iterator iterator = ubig_value.cbegin();
   
   while(iterator != ubig_value.cend())
   {
	   result = *iterator - 48;
	   result = result * 2;
	   
	   if(carry)
	   {
		   result++;
	   }
	   
	   if(result > 9)
	   {
		   carry = true;
		   string product = to_string(result);
		   multiplication.push_back(product.back());
	   }
	   else
	   {
		   carry = false;
		   string product2 = to_string(result);
		   multiplication.push_back(product2.front());
	   }
	   
	   iterator++;
   }
   
   if(carry)
   {
	   multiplication.push_back('1');
   }
	   
   ubig_value.clear();
   iterator = multiplication.cbegin();
   while(iterator != multiplication.cend())
   {
	   ubig_value.push_back(*iterator);
	   iterator++;
   }
}

// check division.cpp in Mackey's examples
void ubigint::divide_by_2() {
   int result;
   int review;
   bool carry = false;
   ubigvalue_t::const_iterator iterator = ubig_value.cbegin();
   ubigvalue_t::const_iterator next = ubig_value.cbegin();
   next++;
   ubigvalue_t division;
   
   if(next != ubig_value.cend())
   {
	   review = *next - 48;
	   
	   if((review % 2) != 0)
	   {
		   carry = true;
	   }
   }
   
   while(iterator != ubig_value.cend())
   {
	   result = *iterator - 48;
	   result = result / 2;
	   
	   if(carry)
	   {
		   result += 5;
	   }
	   
	   division.push_back(result + 48);
	   iterator++, next++;
	   
	   if(next != ubig_value.cend())
	   {
		   review = *next - 48;
		   
		   if((review % 2) != 0)
		   {
			   carry = true;
		   }
		   else
		   {
			   carry = false;
		   }
	   }
	   else
	   {
		   carry = false;
	   }
   }
   
    // lose the disgusting zeroes (tantalo way)
   ubigvalue_t::const_iterator zeroes = division.cend() - 1;
   
   while((*zeroes - 48) == 0)
   {
	   division.pop_back();
	   zeroes--;
   }
   
   ubig_value.clear();
   iterator = division.begin();
   while(iterator != division.end())
   {
	   ubig_value.push_back(*iterator);
	   iterator++;
   }   
}


struct quo_rem { ubigint quotient; ubigint remainder; };
quo_rem udivide (const ubigint& dividend, const ubigint& divisor_) {
   // NOTE: udivide is a non-member function.
   ubigint divisor {divisor_};
   ubigint zero {0};
   if (divisor == zero) throw domain_error ("udivide by zero");
   ubigint power_of_2 {1};
   ubigint quotient {0};
   ubigint remainder {dividend}; // left operand, dividend
   while (divisor < remainder) {
      divisor.multiply_by_2();
      power_of_2.multiply_by_2();
   }
   while (power_of_2 > zero) {
      if (divisor <= remainder) {
         remainder = remainder - divisor;
         quotient = quotient + power_of_2;
      }
      divisor.divide_by_2();
      power_of_2.divide_by_2();
   }
   /*
   DEBUGF ('/', "quotient = " << quotient);
   DEBUGF ('/', "remainder = " << remainder);
   */
   return {.quotient = quotient, .remainder = remainder};
}

ubigint ubigint::operator/ (const ubigint& that) const {
   return udivide (*this, that).quotient;
}

ubigint ubigint::operator% (const ubigint& that) const {
   return udivide (*this, that).remainder;
}

bool ubigint::operator== (const ubigint& that) const {
   int l;
   int r;
   ubigvalue_t::const_iterator left = ubig_value.cbegin();
   ubigvalue_t::const_iterator right = that.ubig_value.cbegin();
   
   if(that.ubig_value.size() != ubig_value.size())
   {
	   return false;
   }
   
   while(left != ubig_value.cend())
   {
	   l = *left - 48;
	   r = *right - 48;
	   
	   if(l != r)
	   {
		   return false;
	   }
	   
	   left++, right++;
   }
   
   return true;
}

bool ubigint::operator< (const ubigint& that) const {   
   if(that.ubig_value.size() > ubig_value.size())
   {
	   return true;
   }
   
   else if(that.ubig_value.size() < ubig_value.size())
   {
	   return false;
   }
   
   else
   {
	   int first;
	   int second;
	   ubigvalue_t::const_iterator left = ubig_value.cend() - 1;
	   ubigvalue_t::const_iterator right = that.ubig_value.cend() - 1;
	   
	   while(left >= ubig_value.cbegin())
	   {
		   first = *left - 48;
		   second = *right - 48;
		   
		   if(first < second)
		   {
			   return true;
		   }
		   else if(first > second)
		   {
			   return false;
		   }
		   
		   left--, right--;
	   }
   }
   
   return false;
}

ostream& operator<< (ostream& out, const ubigint& that) { 
   vector<unsigned char>::const_iterator iterator = that.ubig_value.end() - 1;
   int count = 0;
   
   while(iterator >= that.ubig_value.begin())
   {
	   if(count == 69)	// nice
	   {
		   out << "\\" << endl;
		   count = 0;
	   }
	   
	   out << *iterator;
	   iterator--, count++;
   }
   
   return out;
}
