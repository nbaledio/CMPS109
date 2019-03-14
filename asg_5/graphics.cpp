// $Id: graphics.cpp,v 1.4 2019-02-28 15:24:20-08 - - $

#include <iostream>
#include <string>
using namespace std;

#include <GL/freeglut.h>

#include "graphics.h"
#include "util.h"

int window::width = 640; // in pixels
int window::height = 480; // in pixels
vector<object> window::objects;
size_t window::selected_obj = 0;
mouse window::mus;
GLfloat window::thickness = 4.0;
rgbcolor window::bordercolor = rgbcolor{255,0,0};
GLfloat window::pixels = 4.0;

// Executed when window system signals to shut down.
void window::close() {
   DEBUGF ('g', sys_info::execname() << ": exit ("
           << sys_info::exit_status() << ")");
   exit (sys_info::exit_status());
}

// Executed when mouse enters or leaves window.
void window::entry (int mouse_entered) {
   DEBUGF ('g', "mouse_entered=" << mouse_entered);
   window::mus.entered = mouse_entered;
   if (window::mus.entered == GLUT_ENTERED) {
      DEBUGF ('g', sys_info::execname() << ": width=" << window::width
           << ", height=" << window::height);
   }
   glutPostRedisplay();
}


//ADD HERE 
// Called to display the objects in the window.
void window::display() {
   glClear (GL_COLOR_BUFFER_BIT);
   int x = 0;
   for(auto& object: window::objects){
      object.draw();
      std::string numb = std::to_string(x);
      object.drawnumber(numb);
      //object.drawborder(window::bordercolor, window::thickness);
      //Check for oob resetting
      if(object.center.ypos > window::height){
         object.center.ypos = 0;
      }
      if(object.center.ypos < 0){
         object.center.ypos = window::height;
      }
      if(object.center.xpos > window::width){
         object.center.xpos = 0;
      }
      if(object.center.xpos < 0){
         object.center.xpos = window::width;
      }
      x++;
   }
   //Draws border of selected objetct
   objects[selected_obj].drawborder(window::bordercolor,
   window::thickness);
   mus.draw();
   glutSwapBuffers();
}

// Called when window is opened and when resized.
void window::reshape (int width, int height) {
   DEBUGF ('g', "width=" << width << ", height=" << height);
   window::width = width;
   window::height = height;
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity();
   gluOrtho2D (0, window::width, 0, window::height);
   glMatrixMode (GL_MODELVIEW);
   glViewport (0, 0, window::width, window::height);
   glClearColor (0.25, 0.25, 0.25, 1.0);
   glutPostRedisplay();
}


// Executed when a regular keyboard key is pressed.
void window::keyboard (GLubyte key, int x, int y) {
   enum {BS = 8, TAB = 9, ESC = 27, SPACE = 32, DEL = 127};
   DEBUGF ('g', "key=" << unsigned (key) << ", x=" << x << ", y=" << y);
   window::mus.set (x, y);
   switch (key) {
      case 'Q': case 'q': case ESC:
         window::close();
         break;
      case 'H': case 'h':
         objects[selected_obj].move(-pixels,0);
         //move_selected_object (
         break;
      case 'J': case 'j':
         objects[selected_obj].move(0,-pixels);
         //move_selected_object (
         break;
      case 'K': case 'k':
         objects[selected_obj].move(0,pixels);
         //move_selected_object (
         break;
      case 'L': case 'l':
         objects[selected_obj].move(pixels,0);
         //move_selected_object (
         break;
      case 'N': case 'n': case SPACE: case TAB:
         selected_obj+=1;
         //Reset to first obj if oob
         if(selected_obj == objects.size()){
            selected_obj = 0;
         }
         break;
      case 'P': case 'p': case BS:
         selected_obj-=1;
         //Reset to last obj if oob
         if(selected_obj > objects.size()){
           selected_obj = objects.size() - 1;
         }
         break;
      case '0': case '1': case '2': case '3': case '4':
      case '5': case '6': case '7': case '8': case '9':
         if(unsigned(key - '0') >= objects.size()){
            cerr << "Invalid index" << endl;
            break;
         }
         selected_obj = (key - '0');
         break;
      default:
         cerr << unsigned (key) << ": invalid keystroke" << endl;
         break;
   }
   glutPostRedisplay();
}


// Executed when a special function key is pressed.
void window::special (int key, int x, int y) {
   DEBUGF ('g', "key=" << key << ", x=" << x << ", y=" << y);
   window::mus.set (x, y);
   switch (key) {
      case GLUT_KEY_LEFT: objects[selected_obj].move(-pixels,0); break;
      case GLUT_KEY_DOWN: objects[selected_obj].move(0,-pixels); break;
      case GLUT_KEY_UP: objects[selected_obj].move(0,pixels); break;
      case GLUT_KEY_RIGHT: objects[selected_obj].move(pixels,0); break;
      case GLUT_KEY_F1:
      case GLUT_KEY_F2: 
      case GLUT_KEY_F3:
      case GLUT_KEY_F4: 
      case GLUT_KEY_F5:
      case GLUT_KEY_F6:
      case GLUT_KEY_F7: 
      case GLUT_KEY_F8:
      case GLUT_KEY_F9:
      case GLUT_KEY_F10:
      case GLUT_KEY_F11:
      case GLUT_KEY_F12:
         if(unsigned(key-1) >= objects.size()){
               cerr << "Invalid index" << endl;
               break;
         }
         selected_obj = (key-1);
         break;
      default:
         cerr << unsigned (key) << ": invalid function key" << endl;
         break;
   }
   glutPostRedisplay();
}


void window::motion (int x, int y) {
   DEBUGF ('g', "x=" << x << ", y=" << y);
   window::mus.set (x, y);
   glutPostRedisplay();
}

void window::passivemotion (int x, int y) {
   DEBUGF ('g', "x=" << x << ", y=" << y);
   window::mus.set (x, y);
   glutPostRedisplay();
}

void window::mousefn (int button, int state, int x, int y) {
   DEBUGF ('g', "button=" << button << ", state=" << state
           << ", x=" << x << ", y=" << y);
   window::mus.state (button, state);
   window::mus.set (x, y);
   glutPostRedisplay();
}

void window::main () {
   static int argc = 0;
   glutInit (&argc, nullptr);
   glutInitDisplayMode (GLUT_RGBA | GLUT_DOUBLE);
   glutInitWindowSize (window::width, window::height);
   glutInitWindowPosition (128, 128);
   DEBUGF ('g', "glutCreateWindow (" << sys_info::execname() << ")");
   glutCreateWindow (sys_info::execname().c_str());
   glutCloseFunc (window::close);
   glutEntryFunc (window::entry);
   glutDisplayFunc (window::display);
   glutReshapeFunc (window::reshape);
   glutKeyboardFunc (window::keyboard);
   glutSpecialFunc (window::special);
   glutMotionFunc (window::motion);
   glutPassiveMotionFunc (window::passivemotion);
   glutMouseFunc (window::mousefn);
   DEBUGF ('g', "Calling glutMainLoop()");
   glutMainLoop();
}


void mouse::state (int button, int state) {
   switch (button) {
      case GLUT_LEFT_BUTTON: left_state = state; break;
      case GLUT_MIDDLE_BUTTON: middle_state = state; break;
      case GLUT_RIGHT_BUTTON: right_state = state; break;
   }
}

void mouse::draw() {
   static rgbcolor color ("green");
   ostringstream text;
   text << "(" << xpos << "," << window::height - ypos << ")";
   if (left_state == GLUT_DOWN) text << "L"; 
   if (middle_state == GLUT_DOWN) text << "M"; 
   if (right_state == GLUT_DOWN) text << "R"; 
   if (entered == GLUT_ENTERED) {
      void* font = GLUT_BITMAP_HELVETICA_18;
      glColor3ubv (color.ubvec);
      glRasterPos2i (10, 10);
      auto ubytes = reinterpret_cast<const GLubyte*>
                    (text.str().c_str());
      glutBitmapString (font, ubytes);
   }
}

