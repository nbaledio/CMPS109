// $Id: bigint.cpp,v 1.76 2016-06-14 16:34:24-07 - - $

#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
using namespace std;

#include "bigint.h"
#include "debug.h"
#include "relops.h"

bigint::bigint (long that): uvalue (that), is_negative (that < 0) {
   DEBUGF ('~', this << " -> " << uvalue)
}

bigint::bigint (const ubigint& uvalue, bool is_negative):
                uvalue(uvalue), is_negative(is_negative) {
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
//Addition operator. Calls ubigint's +
bigint bigint::operator+ (const bigint& that) const {
   bigint result;
   //Checks if signs are the same
   if(this->is_negative == that.is_negative){
        result.uvalue = this->uvalue + that.uvalue;
        result.is_negative = this->is_negative;
   }else{
        //Checks for which sign to use. Also sets sign to
        //positive if result is zero
        if(this->uvalue < that.uvalue){
                result.uvalue = that.uvalue - this->uvalue;
                result.is_negative = that.is_negative;
        }else if(that.uvalue < this->uvalue){
                result.uvalue = this->uvalue - that.uvalue;
                result.is_negative = this->is_negative;
        }else if (this->uvalue == that.uvalue){
                result.uvalue = this->uvalue - that.uvalue;
                result.is_negative = false;
        }
   }
   return result;
}

//Subtraction operator. Uses ubigint's -
bigint bigint::operator- (const bigint& that) const {
   bigint result;
   bigint checkzero(0);
   //Checks for which sign to use. Also sets sign to
   //positive if result is zero
   if(this->is_negative != that.is_negative){
        result.uvalue = this->uvalue + that.uvalue;
        result.is_negative = this->is_negative;
   }else{
        //If signs are both negative
        if(this->is_negative == true){
                if(that.uvalue > this->uvalue){
                       result.uvalue = that.uvalue - this->uvalue;
                       result.is_negative = false; 
                }else{
                       result.uvalue = this->uvalue - that.uvalue;
                       result.is_negative = true;
                }
        //If signs are both positive
        }else if(this->is_negative == false){
                if(that.uvalue > this->uvalue){
                        result.uvalue = that.uvalue - this->uvalue;
                        result.is_negative = true;
                }else{
                        result.uvalue = this->uvalue - that.uvalue;
                        result.is_negative = false;
                }
        }
   }
   if(this->uvalue == that.uvalue&&this->is_negative==that.is_negative){
        result.is_negative = false;
   }
   return result;
}

//Multiplication operator. Uses ubigint's *
bigint bigint::operator* (const bigint& that) const {
   bigint result;
   bigint checkzero(0);
   result.uvalue = this->uvalue * that.uvalue;
   //Checks for which sign to use. Also sets sign to
   //positive if result is zero
   if(this->is_negative != that.is_negative){
        result.is_negative = true;
   }else{
        result.is_negative = false;
   }
   if(result == checkzero){
        result.is_negative = false;
   }
return result;
}

//Division operator. Uses ubigint's /
bigint bigint::operator/ (const bigint& that) const {
   bigint result;
   bigint checkzero(0);
   result.uvalue = this->uvalue / that.uvalue;
   //Checks for which sign to use. Also sets sign to
   //positive if result is zero
   if(this->is_negative != that.is_negative){
        result.is_negative = true;
   }else{
        result.is_negative = false;
   }
   if(result == checkzero){
        result.is_negative = false;
   }
return result;
}

bigint bigint::operator% (const bigint& that) const {
   bigint result = uvalue % that.uvalue;
   return result;
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
   return out <<  (that.is_negative ? "-" : "") << that.uvalue;
}

