// $Id: graphics.h,v 1.1 2015-07-16 16:47:51-07 - - $

#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include <memory>
#include <vector>
using namespace std;

#include <GL/freeglut.h>

#include "rgbcolor.h"
#include "shape.h"

class object {
   private:
      shared_ptr<shape> pshape; //shape
      //Making them public to make it quicker
      //to work with vs making setters/getters
      //rgbcolor color;
      //string name;
   public:
      rgbcolor color;
      string name;
      vertex center;
      //Constructor for new objects in display
      object (const shared_ptr<shape>& shape, 
      vertex& cent, rgbcolor colour){
         pshape = shape; 
         center = cent;
         color = colour;
      }
      // Default copiers, movers, dtor all OK.
      void draw() { pshape->draw (center, color); }
      void move (GLfloat delta_x, GLfloat delta_y) {
         center.xpos += delta_x;
         center.ypos += delta_y;
      }
      void drawborder(rgbcolor col, GLfloat thickness){
         pshape->drawborder(center, col, thickness);
      }
      void drawnumber(string number){pshape->drawnumber(center, 
      number);}
      //Methods to print object number. Setter and printer
      //Uses a text object and shape's print method
};

class mouse {
      friend class window;
   private:
      int xpos {0};
      int ypos {0};
      int entered {GLUT_LEFT};
      int left_state {GLUT_UP};
      int middle_state {GLUT_UP};
      int right_state {GLUT_UP};
   private:
      void set (int x, int y) { xpos = x; ypos = y; }
      void state (int button, int state);
      void draw();
};


class window {
      friend class mouse;
   private:
      static int width;         // in pixels
      static int height;        // in pixels
      static vector<object> objects;
      static size_t selected_obj;
      static mouse mus;
   private:
      static void close();
      static void entry (int mouse_entered);
      static void display();
      static void reshape (int width, int height);
      static void keyboard (GLubyte key, int, int);
      static void special (int key, int, int);
      static void motion (int x, int y);
      static void passivemotion (int x, int y);
      static void mousefn (int button, int state, int x, int y);
   public:
      static GLfloat pixels;
      static rgbcolor bordercolor;
      static GLfloat thickness;
      static void set_new_center(string obj_name, vertex v);
      static void set_color(string obj_name, const rgbcolor& color);
      static void push_back (const object& obj) {
                  objects.push_back (obj); }
      static void setwidth (int width_) { width = width_; }
      static void setheight (int height_) { height = height_; }
      static void main();
};

#endif

