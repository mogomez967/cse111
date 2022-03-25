// $Id: bigint.cpp,v 1.3 2020-10-11 12:47:51-07 - - $
// Marvin Gomez mogomez@ucsc.edu

#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
using namespace std;

#include "bigint.h"
#include "relops.h"
#include "debug.h"

bigint::bigint (long that): uvalue (that), is_negative (that < 0) {
   DEBUGF ('~', this << " -> " << uvalue)
}

bigint::bigint (const ubigint& uvalue_, bool is_negative_):
                uvalue(uvalue_), is_negative(is_negative_) {
}

bigint::bigint (const string& that) {
   is_negative = that.size() > 0 and that[0] == '_';
   uvalue = ubigint (that.substr (is_negative ? 1 : 0));
}

bigint bigint::operator+ () const {
   return *this;
}

bigint bigint::operator- () const {
   return {uvalue, not is_negative};
}

bigint bigint::operator+ (const bigint& that) const {
   ubigint result;
   bool negative = false;
   
   if(that.is_negative == is_negative)
   {
	   if(is_negative)
	   {
		   negative = true;
	   }
	   else
	   {
		   negative = false;
	   }
	   
       result = uvalue + that.uvalue;
   }
   
   else if(uvalue < that.uvalue)
   {
	   if(that.is_negative)
	   {
		   negative = true;
	   }
	   else
	   {
		   negative = false;
	   }
	   
	   result = that.uvalue - uvalue;
   }
   
   else if(uvalue > that.uvalue)
   {
	   if(is_negative)
	   {
		   negative = true;
	   }
	   else
	   {
		   negative = false;
	   }
	   
	   result = uvalue - that.uvalue;
   }
   
   return {result, negative};
}

bigint bigint::operator- (const bigint& that) const {
   ubigint result;
   bool negative = false;
   
   if(that.is_negative != is_negative)
   {
	   if(is_negative)
	   {
		   negative = true;
	   }
	   
       result = uvalue + that.uvalue;
   }
   
   else if(uvalue < that.uvalue)
   {
	   if(is_negative == false)
	   {
		   negative = true;
	   }
	   
	   result = that.uvalue - uvalue;
   }
   
   else if(uvalue > that.uvalue)
   {
	   if(is_negative)
	   {
		   negative = true;
	   }
	   
	   result = uvalue - that.uvalue;
   }
   
   return {result, negative};
}


bigint bigint::operator* (const bigint& that) const {
   ubigint result;
   bool negative = false;
   
   if(that.is_negative || is_negative)
   {
	   negative = true;
   }
   
   result = that.uvalue * uvalue;
   return {result, negative};
}

bigint bigint::operator/ (const bigint& that) const {
   ubigint result;
   bool negative = false;
   
   if(that.is_negative || is_negative)
   {
	   negative = true;
   }
   
   result = uvalue / that.uvalue;
   return {result, negative};
}

bigint bigint::operator% (const bigint& that) const {
   ubigint result;
   bool negative = false;
   
   if(that.is_negative || is_negative)
   {
	   negative = true;
   }
   
   result = uvalue % that.uvalue;
   return {result, negative};
}

bool bigint::operator== (const bigint& that) const {
   return is_negative == that.is_negative and uvalue == that.uvalue;
}

bool bigint::operator< (const bigint& that) const {
   if (is_negative != that.is_negative) return is_negative;
   return is_negative ? uvalue > that.uvalue
                      : uvalue < that.uvalue;
}

ostream& operator<< (ostream& out, const bigint& that) {
   return out << (that.is_negative ? "-" : "")
              << that.uvalue;
}

