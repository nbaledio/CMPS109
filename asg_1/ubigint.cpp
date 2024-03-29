// $Id: ubigint.cpp,v 1.14 2016-06-23 17:21:26-07 - - $
// Nathan Baledio
// ID: 157454
// NOTES:
// -multiplication formula derived from pdf
// -trimming of higher order zeros has condition of while size > 1
//  instead of size > 0, as any calculation resulting in 0 will
//  return an empty vector
//  -division formula derived from 
//  https://en.wikipedia.org/wiki/Division_by_two

#include <cctype>
#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
#include <cmath>
using namespace std;

#include "ubigint.h"
#include "debug.h"

ubigint::ubigint (unsigned long that) {
  //Stores unsigned integer as a vector of characters.
  //Lowest digit stored first
  int remainder;
  unsigned char ch;
  while (that > 0){
        remainder = that%10;
        ch = remainder + '0';
        ubig_value.push_back(ch);
        that /= 10;
  }
}

ubigint::ubigint (const string& that) {
   DEBUGF ('~', "that = \"" << that << "\"");
   for (int i = that.length() - 1; i >= 0; i--) {
      if (not isdigit (that.at(i))) {
        throw invalid_argument ("ubigint::ubigint(" + that + ")");
      }
      ubig_value.push_back(that.at(i));
   }
}

ubigint ubigint::operator+ (const ubigint& that) const {
  ubigint answer;
  unsigned char value;
  int longerlength = 0;
  int remainder = 0;
  int carryvalue = 0;
  int v1 = 0;
  int v2 = 0;
  int thissize = this->ubig_value.size();
  int thatsize = that.ubig_value.size();
  //Detmermines which of the two numbers has the shorter length
  if(that.ubig_value.size() < this->ubig_value.size()){
        longerlength = this->ubig_value.size();
  }else if(this->ubig_value.size() < that.ubig_value.size()){
        longerlength = that.ubig_value.size();
  }else {
        longerlength = this->ubig_value.size();
  }
  //Iterates through vectors, adding lower order digits first
  for(int i = 0; i < longerlength; i++){
        //Converts current char index to an int so it can be calculated
        //Checks to see if current iteration is out of bounds
        //Places a 0 as a placeholder for smaller ubigint
        if( i >= thissize){
                v1 = 0;
                v2 = that.ubig_value[i] - '0';
        }else if ( i >= thatsize){
                v1 = this->ubig_value[i] - '0';
                v2 = 0;
        }else{
                v1 = this->ubig_value[i] - '0';
                v2 = that.ubig_value[i] - '0';
        }
        //Adds digit pairs together
        int sum = v1 + v2 + carryvalue;
        //Sets remainder/carry value is sum is >= 10
        if (sum >= 10){
                remainder = sum%10;
                value = remainder + '0';
                answer.ubig_value.push_back(value);
                carryvalue = 1;
                //Checks if current digit pair is highest order
                //Appends carry value if it is since it is 
                //the last iteration
                //Ex:209 + 800 = 1009 
                if(i == longerlength - 1){
                        value = carryvalue + '0';
                        answer.ubig_value.push_back(value);
                }
        //Pushes sum if it's less than 10
        }else{
                value = sum + '0';
                answer.ubig_value.push_back(value);
                remainder = 0;
                carryvalue = 0;
        }
  }

  //Trims higher order zeros
  while(answer.ubig_value.size() > 1&& answer.ubig_value.back()=='0'){
        answer.ubig_value.pop_back();
   }
        return answer;
}

ubigint ubigint::operator- (const ubigint& that) const {
   //Checks if this < that and throws an error if true since unsigned
   //integers cannot handle negative numbers
//   if (*this < that) throw domain_error ("ubigint::operator-(a<b)");
   ubigint answer;
   unsigned char value;
   int thisvalue = 0;
   int thatvalue = 0;
   int looplength = this->ubig_value.size();
   bool carry = false;
   int difference = 0;
   int thatlength = that.ubig_value.size();
   for(int i = 0; i < looplength; i++){
        //Checks if previous iteration carried a 1
        if(carry == true){
                thisvalue = (this->ubig_value[i] - '0') - 1;
                //Checks if iteration is out of bounds for that value
                if(i >= thatlength){
                        thatvalue = 0;
                }else{
                        thatvalue = that.ubig_value[i] - '0';
                }
                carry = false;
        }else{
                thisvalue = this->ubig_value[i] - '0';
                //Checks if iteration is out of bounds for that value
                if(i >= thatlength){
                        thatvalue = 0;
                }else{
                        thatvalue = that.ubig_value[i] - '0';
                }
        }
       //Checks if thisvalue is less than thatvalue and carries a
       //1 before calculating. Sets carry to true so it can subtract
       //1 from the next iteration's value
       if(thisvalue < thatvalue){
                thisvalue += 10;
                carry = true;
       }
       difference = thisvalue - thatvalue;
       value = difference + '0';
       answer.ubig_value.push_back(value);
   }
   //Trims higher order zeroes
   while(answer.ubig_value.size() > 1 && answer.ubig_value.back()=='0'){
        answer.ubig_value.pop_back();
   }   
   return answer;
}

ubigint ubigint::operator* (const ubigint& that) const {
  ubigint answer;
  unsigned char value;
  int c = 0;
  int d = 0;
  int thislength = this->ubig_value.size();
  int thatlength = that.ubig_value.size();
  vector<unsigned char> p(thislength + thatlength,'0');
  //Formula for multiplication
  for(int i = 0; i < thislength; i++){
      c = 0;
      for(int j = 0; j < thatlength; j++){
                d = (p[i+j] - '0') + 
                ((this->ubig_value[i] - '0')*(that.ubig_value[j] - '0'))
                + c;
                p[i+j] = ((d%10) + '0');
                c = floor(d/10);
      }
      value = c + '0';
      p[i + thatlength] = value;
  }
int plength = p.size();
  //Appends answer to ubigint answer
  for(int k = 0; k < plength; k++){
        answer.ubig_value.push_back(p[k]);
  }

  //Trims higher order zeros
  while(answer.ubig_value.size() > 1 && answer.ubig_value.back() == 0){
        answer.ubig_value.pop_back();
   }
  return answer;
}

void ubigint::multiply_by_2() {
   ubigint answer;
   int size = this->ubig_value.size();
   for(int i = 0; i < size; i++){
        answer.ubig_value.push_back(this->ubig_value[i]);
   }
   answer = answer + answer;
   int size2 = answer.ubig_value.size();
   this->ubig_value.clear();
   for(int i = 0; i < size2; i++){
        this->ubig_value.push_back(answer.ubig_value[i]);
   }

   //Trims higher order zeros
   while(this->ubig_value.size() > 1 && this->ubig_value.back() == '0'){
         this->ubig_value.pop_back();
   }
}

void ubigint::divide_by_2() {
  int firstdigit = 0;
  int seconddigit = 0;
  ubigint result;
  ubigint resultreversed;
  //Appends a zero to the start so it can be compared properly
  this->ubig_value.push_back('0');
  //Algorithm that compares digits and appends them to a ubigint
  //in reversed order
  for(int i = ubig_value.size() - 1; i > 0; i--){
        firstdigit = ubig_value[i] - '0';
        seconddigit = ubig_value[i-1] - '0';
        if(firstdigit%2 == 0){
                if(seconddigit == 0 || seconddigit == 1){
                        result.ubig_value.push_back('0');
                }
                if(seconddigit == 2 || seconddigit == 3){
                        result.ubig_value.push_back('1');
                }
                if(seconddigit == 4 || seconddigit == 5){
                        result.ubig_value.push_back('2');
                }
                if(seconddigit == 6 || seconddigit == 7){
                        result.ubig_value.push_back('3');
                }
                if(seconddigit == 8 || seconddigit == 9){
                        result.ubig_value.push_back('4');
                }
        }else{
                if(seconddigit == 0 || seconddigit == 1){
                        result.ubig_value.push_back('5');
                }
                if(seconddigit == 2 || seconddigit == 3){
                        result.ubig_value.push_back('6');
                }
                if(seconddigit == 4 || seconddigit == 5){
                        result.ubig_value.push_back('7');
                }
                if(seconddigit == 6 || seconddigit == 7){
                        result.ubig_value.push_back('8');
                }
                if(seconddigit == 8 || seconddigit == 9){
                        result.ubig_value.push_back('9');
                }
        }
  }
  int length = result.ubig_value.size();
  //Takes result ubigint from above and puts it in the right order
  for(int i = length - 1; i >=0; i--){
        resultreversed.ubig_value.push_back(result.ubig_value[i]);
  }
  int length2 = resultreversed.ubig_value.size();
  this->ubig_value.clear();
  for(int i = 0; i < length2; i++){
        this->ubig_value.push_back(resultreversed.ubig_value[i]);
  }
  //Trims higher order zeros
  while(this->ubig_value.size() > 1 && this->ubig_value.back() == '0'){
        this->ubig_value.pop_back();
  }
}

void ubigint::append(unsigned char ch){
        this->ubig_value.push_back(ch);
}


struct quo_rem { ubigint quotient; ubigint remainder; };
quo_rem udivide (const ubigint& dividend, ubigint divisor) {

   // Note: divisor is modified so pass by value (copy).
   ubigint zero;
   zero.append('0');
   if (divisor == zero) throw domain_error ("udivide by zero");
   ubigint power_of_2;
   power_of_2.append('1');
   ubigint quotient;
   quotient.append('0');
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
   return {.quotient = quotient, .remainder = remainder};
}

ubigint ubigint::operator/ (const ubigint& that) const {
   return udivide (*this, that).quotient;
}

ubigint ubigint::operator% (const ubigint& that) const {
   return udivide (*this, that).remainder;
}

bool ubigint::operator== (const ubigint& that) const {
int size = this->ubig_value.size();
   //Checks to see if this and that are the same size
   //Returns false if they are not
   if(this->ubig_value.size() != that.ubig_value.size()){
        return false;
   }
   //Iterates through both vectors and checks if current iteration
   //value is the same on both. Returns false if they are not
   for(int i = 0; i < size; i++){
        if(this->ubig_value[i] != that.ubig_value[i]){
                return false;
        }
   }
   //If all checks are passed, return true
   return true;
}

bool ubigint::operator< (const ubigint& that) const {
   int thissize = this->ubig_value.size();
   int thatsize = that.ubig_value.size();
   int thisvalue = 0;
   int thatvalue = 0;
   //Compares length of this and that. Returns true if this < that
   //and false otherwise. Moves on if they are the same size
   if( thissize < thatsize){
        return true;
   }else if( thissize > thatsize){
        return false;
   }
   //Compares digits if they are the same length. Starts at the
   //highest order first, then scrolls to the lowest order
   for(int i = thissize - 1; i >= 0; i--){
        thisvalue = this->ubig_value[i] - '0';
        thatvalue = that.ubig_value[i] - '0';
        if(thisvalue < thatvalue){
                return true;
        }else if (thatvalue < thisvalue){
                return false;
        }
   }
   //Returns false if they are the same number
   return false;
}

ostream& operator<< (ostream& out, const ubigint& that) {
   //New ubigint since that is a constant (can't be changed)
   ubigint answer = that;
   int counter = 0;
   //Trims high order zeros 
   while(answer.ubig_value.size() > 1 &&answer.ubig_value.back()=='0'){
        answer.ubig_value.pop_back();
   }
   int length = answer.ubig_value.size();
   //Appends digits to output, from high order to low order
   for(int i = length - 1; i >= 0; i--){
        if(counter == 69){
                counter = 0;
                char line = 92;
                char newline = 10;
                out << line;
                out << newline;
        }
        out << answer.ubig_value[i];
        counter++;
   }
   return out;
}

