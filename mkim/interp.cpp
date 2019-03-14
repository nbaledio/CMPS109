// $Id: interp.cpp,v 1.2 2015-07-16 16:57:30-07 - - $

#include <memory>
#include <string>
#include <vector>
using namespace std;

#include <GL/freeglut.h>

#include "debug.h"
#include "interp.h"
#include "shape.h"
#include "util.h"

unordered_map<string,interpreter::interpreterfn>
interpreter::interp_map {
   {"define" , &interpreter::do_define },
   {"draw"   , &interpreter::do_draw   },
   {"moveby" , &interpreter::moveby    },
   {"border" , &interpreter::border    },
};

unordered_map<string,interpreter::factoryfn>
interpreter::factory_map {
   {"text"     , &interpreter::make_text     },
   {"ellipse"  , &interpreter::make_ellipse  },
   {"circle"   , &interpreter::make_circle   },
   {"polygon"  , &interpreter::make_polygon  },
   {"diamond"  , &interpreter::make_diamond  },
   {"triangle" , &interpreter::make_polygon  },
   {"equilateral",&interpreter::make_equilat },
   {"rectangle", &interpreter::make_rectangle},
   {"square"   , &interpreter::make_square   },
};

interpreter::shape_map interpreter::objmap;

interpreter::~interpreter() {
   for (const auto& itor: objmap) {
      cout << "objmap[" << itor.first << "] = "
           << *itor.second << endl;
   }
}

//Gets commands from command line and "interprets" by
//finding function associated with the command and 
//passing in the parameters following the command as 
//the arguments to the function. 
void interpreter::interpret (const parameters& params) {
   DEBUGF ('i', params);
   //Make begin an iterator to the beginning 
   //of the params string vector.
   param begin = params.cbegin();                             
   //Get the command from the first element in the vector     
   string command = *begin;                                   
   //Find function in map from associated command.
   auto itor = interp_map.find (command);          
   if (itor == interp_map.end()) throw runtime_error ("syntax error");
   interpreterfn func = itor->second;                         
   //Call function with an iterator at the start and end 
   //of the params vector as arguments. 
   func (++begin, params.cend());
}

void interpreter::arg_check(param begin, param end, int i){
   if (distance(begin, end) != i) throw runtime_error ("syntax error");
}

void interpreter::moveby (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   arg_check(begin, end, 1);
   object::moveby = from_string<GLfloat> (*begin);
   return;
}

void interpreter::border (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   arg_check(begin, end, 2);
   object::bcolor = *begin++;
   object::thick = from_string<GLfloat>(*begin);
   return;
}

void interpreter::do_define (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   if (distance(begin, end) < 2) throw runtime_error ("syntax error");
   string name = *begin;
   //objmap = shape_map = unordered_map<string,shape_ptr>;
   //Put shape name and ptr to the shape in the objmap
   objmap.emplace (name, make_shape (++begin, end));
}


void interpreter::do_draw (param begin, param end) {        
   DEBUGF ('f', range (begin, end));
   if (end - begin != 4) throw runtime_error ("syntax error");
   string name = begin[1];
   //Find the name associated with the shape
   shape_map::const_iterator itor = objmap.find (name);
   if (itor == objmap.end()) {
      cerr << name << ": no such shape" <<endl;
      return;
   }
   rgbcolor color {begin[0]};
   //struct vertex {GLfloat xpos; GLfloat ypos; };
   //Change the xcenter and ycenter values from string to GLfloat.
   vertex where {from_string<GLfloat> (begin[2]),
                 from_string<GLfloat> (begin[3])};
   //Using the shape_ptr in objmap, call the draw function.
   //itor->second->draw (where, color);
   //Create new object? or maybe in shape?
   object new_object;
   //Passing the shape_ptr created in do_define() 
   //to the newly created object. 
   new_object.pshape = itor->second;
   new_object.center = where;
   new_object.color = color;  
   //If the first object, make selected.
   if (window::select_copy == 0){
      new_object.is_selected = true;
   }
   new_object.obj_num = window::select_copy++;
   //Push the new object into the objects vector which will
   //be looped through, drawing each object in objects, 
   //in graphics.cpp when window::main() is called.
   window::objects.push_back(new_object);

}

shape_ptr interpreter::make_shape (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   string type = *begin++;
   //Find the shape in the factory_map
   auto itor = factory_map.find(type);
   if (itor == factory_map.end()) {
      throw runtime_error (type + ": no such shape");
   }
   //Get the function to make the shape
   factoryfn func = itor->second;
   //returns shape_ptr of shape type which is returned
   //to do_define, and put in shape_map objmap. 
   return func (begin, end);
}

//Remember when using make_shared: constructs a shape/text object
//pointed to by the shared_ptr and uses it's args as the parameter 
//list for the constructor of the object. 

shape_ptr interpreter::make_text (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   //Calls text ctor and makes shared_ptr to text object created.
   string words;
   auto font = shape::fontcode.find(*begin);
   if (font == shape::fontcode.end()){
      throw runtime_error (*begin + ": no such font");
   }
   ++begin;
   while (begin != end){
      words += *begin++ + " ";
   }
   return make_shared<text> (font->second, words);
}

shape_ptr interpreter::make_ellipse (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   arg_check(begin, end, 2);
   //Creates an ellipse and a shared_ptr that points to it. 
   GLfloat width = from_string<GLfloat>(*begin++);
   GLfloat height = from_string<GLfloat>(*begin);
   return make_shared<ellipse> (width, height);
}

shape_ptr interpreter::make_circle (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   arg_check(begin, end, 1);
   GLfloat diameter = from_string<GLfloat>(*begin);
   return make_shared<circle> (diameter);
}

shape_ptr interpreter::make_polygon (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   vertex points;
   vertex_list verticies;
   while (begin != end){
      points.xpos = from_string<GLfloat>(*begin++);
      points.ypos = from_string<GLfloat>(*begin++);
      verticies.push_back(points);
   }
   return make_shared<polygon> (verticies);
}

shape_ptr interpreter::make_equilat (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   arg_check(begin, end, 1);
   parameters cat;
   cat.push_back("0");
   cat.push_back("0");
   int point = stoi(*begin);
   point /= 2;
   cat.push_back(to_string(point));
   cat.push_back(*begin);
   cat.push_back(*begin);
   cat.push_back("0");
   param begin_ = cat.cbegin();
   return make_polygon(begin_, cat.cend());
}

shape_ptr interpreter::make_diamond (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   arg_check(begin, end, 2);
   GLfloat width = from_string<GLfloat>(*begin++);
   GLfloat height = from_string<GLfloat>(*begin);
   return make_shared<diamond> (width, height);
}

shape_ptr interpreter::make_rectangle (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   arg_check(begin, end, 2);
   GLfloat width = from_string<GLfloat>(*begin++);
   GLfloat height = from_string<GLfloat>(*begin);
   return make_shared<rectangle> (width, height);
}

shape_ptr interpreter::make_square (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   arg_check(begin, end, 1);
   GLfloat width = from_string<GLfloat>(*begin);
   return make_shared<square> (width);
}

