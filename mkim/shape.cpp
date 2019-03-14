// $Id: shape.cpp,v 1.1 2015-07-16 16:47:51-07 - - $

#include <cmath>
#include <typeinfo>
#include <unordered_map>
using namespace std;

#include "shape.h"
#include "util.h"

unordered_map<void*,string> shape::fontname {
   {GLUT_BITMAP_8_BY_13       , "Fixed-8x13"    },
   {GLUT_BITMAP_9_BY_15       , "Fixed-9x15"    },
   {GLUT_BITMAP_HELVETICA_10  , "Helvetica-10"  },
   {GLUT_BITMAP_HELVETICA_12  , "Helvetica-12"  },
   {GLUT_BITMAP_HELVETICA_18  , "Helvetica-18"  },
   {GLUT_BITMAP_TIMES_ROMAN_10, "Times-Roman-10"},
   {GLUT_BITMAP_TIMES_ROMAN_24, "Times-Roman-24"},
};

unordered_map<string,void*> shape::fontcode {
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

vertex_list make_rectangle_vertices(GLfloat width, GLfloat height){
   vertex_list list;
   vertex rect_vertices {width, height};
   list.push_back(rect_vertices);
   return list;
}

vertex_list make_diamond_vertices(GLfloat width, GLfloat height){
   vertex_list list;
   vertex rect_vertices {width, height};
   list.push_back(rect_vertices);
   list.push_back(rect_vertices);
   return list;
}

shape::shape() {
   DEBUGF ('c', this);
}

text::text (void* glut_bitmap_font, const string& textdata):
      glut_bitmap_font(glut_bitmap_font), textdata(textdata) {
   DEBUGF ('c', this);
}

ellipse::ellipse (GLfloat width, GLfloat height):
dimension ({width, height}) {
   DEBUGF ('c', this);
}

circle::circle (GLfloat diameter): ellipse (diameter, diameter) {
   DEBUGF ('c', this);
}


polygon::polygon (const vertex_list& vertices): vertices(vertices) {
   DEBUGF ('c', this);
}

diamond::diamond (const GLfloat width, const GLfloat height):
            polygon(make_diamond_vertices(width, height)) {
   DEBUGF ('c', this << "(" << width << "," << height << ")");
}

rectangle::rectangle (GLfloat width, GLfloat height):
            polygon(make_rectangle_vertices(width, height)) {
   DEBUGF ('c', this << "(" << width << "," << height << ")");
}

square::square (GLfloat width): rectangle (width, width) {
   DEBUGF ('c', this);
}

void number_draw(const vertex& center, const rgbcolor& color,\
                 const size_t& obj_num) {
   string readable = "black";
   if (string (color).compare("0x000000") == 0){
      readable = "white";}
   rgbcolor num_color {readable};
   glColor3ubv (num_color.ubvec);
   glRasterPos2f (center.xpos, center.ypos);
   glutBitmapString (GLUT_BITMAP_TIMES_ROMAN_24,
      reinterpret_cast<const GLubyte*>(to_string(obj_num).c_str()));
}

void text::draw (const vertex& center, const rgbcolor& color,\
                 const bool& selected, const GLfloat& thick,\
                 const string& bcolor, const size_t& obj_num) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");

   //Draw numbers
   string textdata_ = textdata + "(" + to_string(obj_num) + ")";
   auto u_str = reinterpret_cast<const GLubyte*> (textdata_.c_str());
   glColor3ubv (color.ubvec);
   glRasterPos2f (center.xpos, center.ypos);
   glutBitmapString (glut_bitmap_font, u_str);

   if (selected){
      rgbcolor border {bcolor};
      int width = glutBitmapLength (glut_bitmap_font, u_str);
      int height = glutBitmapHeight (glut_bitmap_font);
      glLineWidth (thick);
      glBegin (GL_LINE_LOOP);
      glColor3ubv (border.ubvec);
      glVertex2f (center.xpos, center.ypos);
      glVertex2f (center.xpos + width, center.ypos);
      glVertex2f (center.xpos + width, center.ypos + height);
      glVertex2f (center.xpos, center.ypos + height);
      glEnd();
   }
}

void ellipse::draw (const vertex& center, const rgbcolor& color,\
                    const bool& selected, const GLfloat& thick,\
                    const string& bcolor, const size_t& obj_num)const{
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   //Code taken from ellipses.cpp
   glBegin (GL_POLYGON);
   glEnable (GL_LINE_SMOOTH);
   glColor3ubv (color.ubvec);
   const float delta = 2 * M_PI / 32;
   for (float theta = 0; theta < 2 * M_PI; theta += delta) {
      float xpos = (dimension.xpos/2) * cos (theta) + center.xpos;
      float ypos = (dimension.ypos/2) * sin (theta) + center.ypos;
      glVertex2f (xpos, ypos);
   }
   glEnd();
   //Draw numbers
   number_draw(center, color, obj_num);
   //Draw border if selected
   if (selected){
      rgbcolor border {bcolor};
      glLineWidth (thick);
      glBegin (GL_LINE_LOOP);
      glEnable (GL_LINE_SMOOTH);
      glColor3ubv (border.ubvec);
      const float delta = 2 * M_PI / 32;
      for (float theta = 0; theta < 2 * M_PI; theta += delta) {
         float xpos = (dimension.xpos/2) * cos (theta) + center.xpos;
         float ypos = (dimension.ypos/2) * sin (theta) + center.ypos;
         glVertex2f (xpos, ypos);
      }
      glEnd();
   }
}

void polygon::draw (const vertex& center, const rgbcolor& color,\
                    const bool& selected, const GLfloat& thick,\
                    const string& bcolor, const size_t& obj_num)const{
   DEBUGF ('d', this << "(" << center << "," << color << ")");

   //Handle which type is drawn here!
   if (vertices.size() == 2){
      glBegin(GL_POLYGON);
      glColor3ubv (color.ubvec);
      GLfloat width = vertices.at(0).xpos;
      GLfloat height = vertices.at(0).ypos;
      glVertex2f (center.xpos, center.ypos);
      glVertex2f (center.xpos + (width/2), center.ypos + (height/2));
      glVertex2f (center.xpos + width, center.ypos);
      glVertex2f (center.xpos + (width/2), center.ypos - (height/2));
      glEnd();
      vertex box = {center.xpos+(width/2), center.ypos};
      number_draw(box, color, obj_num);
      if (selected){
         rgbcolor border {bcolor};
         glLineWidth (thick);
         glBegin (GL_LINE_LOOP);
         glColor3ubv (border.ubvec);
         glVertex2f (center.xpos, center.ypos);
         glVertex2f (center.xpos + (width/2), center.ypos + (height/2));
         glVertex2f (center.xpos + width, center.ypos);
         glVertex2f (center.xpos + (width/2), center.ypos - (height/2));
         glEnd();
      }
      return;
   }

   //If box, draw rect/square
   if (vertices.size() == 1){
      //Code taken from square.cpp
      glBegin (GL_POLYGON);
      glColor3ubv (color.ubvec);
      GLfloat width = vertices.at(0).xpos;
      GLfloat height = vertices.at(0).ypos;
      glVertex2f (center.xpos, center.ypos);
      glVertex2f (center.xpos + width, center.ypos);
      glVertex2f (center.xpos + width, center.ypos + height);
      glVertex2f (center.xpos, center.ypos + height);
      glEnd();
      vertex box = {center.xpos+(width/2), center.ypos+(height/2)};
      number_draw(box, color, obj_num);
      if (selected){
         rgbcolor border {bcolor};
         glLineWidth (thick);
         glBegin (GL_LINE_LOOP);
         glColor3ubv (border.ubvec);
         glVertex2f (center.xpos, center.ypos);
         glVertex2f (center.xpos + width, center.ypos);
         glVertex2f (center.xpos + width, center.ypos + height);
         glVertex2f (center.xpos, center.ypos + height);
         glEnd();
      }
      return;  
   }

   //If triangle, draw triangle.
   if (vertices.size() == 3){
      vertex average {0,0};
      //Finding original origin
      for (int i = 0; i < 3; i++){
         average.xpos += vertices.at(i).xpos;
         average.ypos += vertices.at(i).ypos;  
      }
      average.xpos = average.xpos/3;
      average.ypos = average.ypos/3; 
      //Adjusting origin
      average.xpos = (center.xpos - average.xpos);
      average.ypos = (center.ypos - average.ypos);
      glBegin(GL_TRIANGLES);
      glColor3ubv (color.ubvec);
      glVertex2f(vertices.at(0).xpos + average.xpos, 
                 vertices.at(0).ypos + average.ypos);
      glVertex2f(vertices.at(1).xpos + average.xpos, 
                 fabs(vertices.at(1).ypos + average.ypos));
      glVertex2f(fabs(vertices.at(2).xpos + average.xpos), 
                 vertices.at(2).ypos + average.ypos);
      glEnd();
      //Draw number
      number_draw(center, color, obj_num);
      //Draw border if selected
      if (selected){
      rgbcolor border {bcolor};
      glLineWidth (thick);
      glBegin(GL_LINE_LOOP);
      glColor3ubv (border.ubvec);
      glVertex2f(vertices.at(0).xpos + average.xpos, 
                 vertices.at(0).ypos + average.ypos);
      glVertex2f(vertices.at(1).xpos + average.xpos, 
                 fabs(vertices.at(1).ypos + average.ypos));
      glVertex2f(fabs(vertices.at(2).xpos + average.xpos), 
                 vertices.at(2).ypos + average.ypos);
      glEnd();
      }
      return;
   }

   //If none of the above, draw polygon
   glBegin(GL_POLYGON);
   glColor3ubv (color.ubvec);
   vertex average {0,0};
   int size = vertices.size();
   for (int i = 0; i < size; i++){
         average.xpos += vertices.at(i).xpos;
         average.ypos += vertices.at(i).ypos;  
      }
      average.xpos = average.xpos/size;
      average.ypos = average.ypos/size;
      average.xpos = (center.xpos - average.xpos);
      average.ypos = (center.ypos - average.ypos); 
   for (auto& point: vertices){
      glVertex2f(point.xpos+average.xpos, 
                 point.ypos+average.ypos);
   }
   glEnd();
   number_draw(center, color, obj_num);
   if (selected){
      rgbcolor border {bcolor};
      glLineWidth (thick);
      glBegin(GL_LINE_LOOP);
      glColor3ubv (border.ubvec);
      vertex average {0,0};
      int size = vertices.size();
      for (int i = 0; i < size; i++){
         average.xpos += vertices.at(i).xpos;
         average.ypos += vertices.at(i).ypos;  
      }
      average.xpos = average.xpos/size;
      average.ypos = average.ypos/size;
      average.xpos = (center.xpos - average.xpos);
      average.ypos = (center.ypos - average.ypos); 
      for (auto& point: vertices){
         glVertex2f(point.xpos+average.xpos, 
                    point.ypos+average.ypos);
      }
   glEnd();
   }
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

