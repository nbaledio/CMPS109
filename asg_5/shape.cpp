// $Id: shape.cpp,v 1.2 2019-02-28 15:24:20-08 - - $

#include <typeinfo>
#include <unordered_map>
#include <cmath>
using namespace std;

#include "shape.h"
#include "util.h"

static unordered_map<void*,string> fontname {
   {GLUT_BITMAP_8_BY_13       , "Fixed-8x13"    },
   {GLUT_BITMAP_9_BY_15       , "Fixed-9x15"    },
   {GLUT_BITMAP_HELVETICA_10  , "Helvetica-10"  },
   {GLUT_BITMAP_HELVETICA_12  , "Helvetica-12"  },
   {GLUT_BITMAP_HELVETICA_18  , "Helvetica-18"  },
   {GLUT_BITMAP_TIMES_ROMAN_10, "Times-Roman-10"},
   {GLUT_BITMAP_TIMES_ROMAN_24, "Times-Roman-24"},
};

static unordered_map<string,void*> fontcode {
   {"Fixed-8x13"    , GLUT_BITMAP_8_BY_13       },
   {"Fixed-9x15"    , GLUT_BITMAP_9_BY_15       },
   {"Helvetica-10"  , GLUT_BITMAP_HELVETICA_10  },
   {"Helvetica-12"  , GLUT_BITMAP_HELVETICA_12  },
   {"Helvetica-18"  , GLUT_BITMAP_HELVETICA_18  },
   {"Times-Roman-10", GLUT_BITMAP_TIMES_ROMAN_10},
   {"Times-Roman-24", GLUT_BITMAP_TIMES_ROMAN_24},
};

ostream& operator<< (ostream& out, const vertex& where) {
   out << "(" << where.xpos << "," << where.ypos << ")";
   return out;
}
//ALL SHAPES CREATED AT ORIGIN (0,0) THEY ARE RE-CENTERED
//ONCE THEIR DRAW FUNCTION IS CALLED (which simply
//adds to each vertice)

shape::shape() {
   DEBUGF ('c', this);
}

text::text (void* glut_bitmap_font_, const string& textdata_):
      glut_bitmap_font(glut_bitmap_font_), textdata(textdata_) {
   DEBUGF ('c', this);
}

ellipse::ellipse (GLfloat width, GLfloat height):
dimension ({width, height}) {
   DEBUGF ('c', this);
}

circle::circle (GLfloat diameter): ellipse (diameter, diameter) {
   DEBUGF ('c', this);
}


polygon::polygon (const vertex_list& vertices_): vertices(vertices_) {
   DEBUGF ('c', this);
}

rectangle::rectangle (GLfloat width, GLfloat height):
polygon({{-width,-height},{-width,height},{width,height},
{width,-height}}) {
   DEBUGF ('c', this << "(" << width << "," << height << ")");
}

square::square (GLfloat width): rectangle (width, width) {
   DEBUGF ('c', this);
}

diamond::diamond (const GLfloat width, const GLfloat height):polygon
({{-width/2,0},{0,height/2},{width/2,0},{0,-height/2}}){
   DEBUGF ('c', this);
}

triangle::triangle(const vertex_list& vertices_):polygon(vertices_){
   DEBUGF ('c', this);
}

//Rough estimates to create an equilateral
equilateral::equilateral(const GLfloat width):triangle({
{-width/2,.865f*.333f*-width},{0,.865f*.666f*width},
{width/2,.865f*.333f*-width}}){
   DEBUGF ('c', this);
}

void text::draw (const vertex& center, const rgbcolor& color) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   glColor3ubv(color.ubvec); //Set text color
   glRasterPos2f(center.xpos,center.ypos); //Set position
   glutBitmapString (glut_bitmap_font, //Set Font/text
   reinterpret_cast<const unsigned char*>(textdata.c_str()));
}

void ellipse::draw (const vertex& center, const rgbcolor& color) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
// CODE DOESN"T WORK. USE ELLIPSE CODE IN EXAMPLES
//   glBegin(GL_POLYGON);
//   glEnable(GL_LINE_SMOOTH);
//   glColor3ubv(color.ubvec);
//   int conversion = M_PI/180;
//   for(int i = 0; i < 360; i++){
//      float rad = i*conversion;
//      glVertex2f(cos(rad)*center.xpos,sin(rad)*center.ypos);
//   }
//   glEnd();

   //PROVIDED FROM EXAMPLES
   glBegin (GL_POLYGON);
   glEnable(GL_LINE_SMOOTH);
   glColor3ubv (color.ubvec);
   const float delta = 2 * M_PI / 32;
//   float width = window.width / 3 * scale;
//   float height = window.width / 3 * scale;
   for (float theta = 0; theta < 2 * M_PI; theta += delta) {
      //Re-locate to new center
      float xpos = dimension.xpos * cos (theta) + center.xpos;
      float ypos = dimension.ypos * sin (theta) + center.ypos;
      glVertex2f (xpos, ypos);
   }
   glEnd();
}

void polygon::draw (const vertex& center, const rgbcolor& color) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   glBegin(GL_POLYGON);
   glColor3ubv(color.ubvec);
   for(unsigned int i = 0; i < vertices.size(); i++){
        //Re-locate to new center
        glVertex2f(vertices[i].xpos+center.xpos,
        vertices[i].ypos+center.ypos);
   }
   glEnd();
}

//Borders redraw shape, but behind and bigger (based on input)

void text::drawborder(const vertex&, const rgbcolor&) const {

}

void ellipse::drawborder(const vertex&, const rgbcolor&) const {
   
}
 
void polygon::drawborder(const vertex&, const rgbcolor&) const {
   
}

void shape::show (ostream& out) const {
   out << this << "->" << demangle (*this) << ": ";
}

void text::show (ostream& out) const {
   shape::show (out);
   out << glut_bitmap_font << "(" << fontname[glut_bitmap_font]
       << ") \"" << textdata << "\"";
}

void ellipse::show (ostream& out) const {
   shape::show (out);
   out << "{" << dimension << "}";
}

void polygon::show (ostream& out) const {
   shape::show (out);
   out << "{" << vertices << "}";
}

ostream& operator<< (ostream& out, const shape& obj) {
   obj.show (out);
   return out;
}

