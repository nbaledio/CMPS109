// $Id: xless.h,v 1.3 2014-04-24 18:02:55-07 - - $
// Nathan Baledio
// ID: 157454
#ifndef __XLESS_H__
#define __XLESS_H__

//
// We assume that the type type_t has an operator< function.
//

template <typename Type>
struct xless {
   bool operator() (const Type& left, const Type& right) const {
      return left < right;
   }
};

#endif

