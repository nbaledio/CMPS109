// $Id: graphics.cpp,v 1.1 2015-07-16 16:47:51-07 - - $

#include <iostream>
using namespace std;

#include <GL/freeglut.h>

#include "graphics.h"
#include "util.h"

int window::width = 640; // in pixels
int window::height = 480; // in pixels
vector<object> window::objects;
size_t window::selected_obj = 0;
size_t window::select_copy = 0;
string object::bcolor = "red";
GLfloat object::moveby = 4;
GLfloat object::thick = 4;
mouse window::mus;

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

void window::select_object (size_t obj) {
   bool found {false};
   for (auto& object: window::objects) {
      //Turn off is_selected for previous
      object.is_selected = false;
      if (obj == object.obj_num) {
         object.is_selected = true;
         window::selected_obj = obj;
         found = true;
      }
   }
   if (found) return;
   for (auto& reset: window::objects) {
      if (reset.obj_num == window::selected_obj) {
         reset.is_selected = true;
      }
   }
   cout << "No object " << obj << endl;
}

void window::move_selected_object (int x, int y) {
   for (auto& object: window::objects){
      if (object.obj_num == window::selected_obj){
         object.move(x,y);
      }
   }
}

// Called to display the objects in the window.
void window::display() {
   //Clears color buffer
   glClear (GL_COLOR_BUFFER_BIT);
   //Loop through objects in objects vector and draw them
   for (auto& object: window::objects) object.draw();
   cout << "selected: " << window::selected_obj << endl; //debug
   //Draw mouse & mouse info
   mus.draw();
   glutSwapBuffers();
}

// Called when window is opened and when resized.
void window::reshape (int width, int height) {
   DEBUGF ('g', "width=" << width << ", height=" << height);
   window::width = width;
   window::height = height;
   //Sets up what we want to see in scene
   glMatrixMode (GL_PROJECTION);
   //Sets the current matrix to the identity matrix
   glLoadIdentity();
   //Sets the COODINATES within the viewport
   //Resizes objects in window
   gluOrtho2D (0, window::width, 0, window::height);
   //Used to make transformations to the objects
   //Only have to define object once and use this to scale it
   glMatrixMode (GL_MODELVIEW);
   //Resizes what you see in window itself.
   //If gluortho was not also changed, 
   //images in the window would be stretched. 
   glViewport (0, 0, window::width, window::height);
   //Re-sets the background color? Grey?
   glClearColor (0.25, 0.25, 0.25, 1.0);
   glutPostRedisplay();
}


// Executed when a regular keyboard key is pressed.
enum {BS=8, TAB=9, ESC=27, SPACE=32, DEL=127};
void window::keyboard (GLubyte key, int x, int y) {
   DEBUGF ('g', "key=" << unsigned (key) << ", x=" << x << ", y=" << y);
   window::mus.set (x, y);
   switch (key) {
      case 'Q': case 'q': case ESC:
         window::close();
         break;
      case 'H': case 'h':
         move_selected_object (-1, 0);
         break;
      case 'J': case 'j':
         move_selected_object (0, -1);
         break;
      case 'K': case 'k':
         move_selected_object (0, +1);
         break;
      case 'L': case 'l':
         move_selected_object (+1, 0);
         break;
      case 'N': case 'n': case SPACE: case TAB:
         select_copy = window::selected_obj;
         select_object (++select_copy);
         break;
      case 'P': case 'p': case BS:
         select_copy = window::selected_obj;
         select_object (--select_copy);
         break;
      case '0'...'9':
         select_object (key - '0');
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
      case GLUT_KEY_LEFT: move_selected_object (-1, 0); break;
      case GLUT_KEY_DOWN: move_selected_object (0, -1); break;
      case GLUT_KEY_UP: move_selected_object (0, +1); break;
      case GLUT_KEY_RIGHT: move_selected_object (+1, 0); break;
      case GLUT_KEY_F1: select_object (1); break;
      case GLUT_KEY_F2: select_object (2); break;
      case GLUT_KEY_F3: select_object (3); break;
      case GLUT_KEY_F4: select_object (4); break;
      case GLUT_KEY_F5: select_object (5); break;
      case GLUT_KEY_F6: select_object (6); break;
      case GLUT_KEY_F7: select_object (7); break;
      case GLUT_KEY_F8: select_object (8); break;
      case GLUT_KEY_F9: select_object (9); break;
      case GLUT_KEY_F10: select_object (10); break;
      case GLUT_KEY_F11: select_object (11); break;
      case GLUT_KEY_F12: select_object (12); break;
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
      //Gets the font as a void pointer.
      void* font = GLUT_BITMAP_HELVETICA_18;
      //Sets the current color from an already 
      //existing array of color values.
      glColor3ubv (color.ubvec);
      //Sets the raster position, setting the position 
      //for glutBitmapString to draw the text.
      glRasterPos2i (10, 10);
      //Cast to a unsigned char *string so text 
      //can be printed out using glutBitmapString.
      auto ubytes = reinterpret_cast<const GLubyte*>
                    (text.str().c_str());
      //Draws a string of bitmapped characters 
      //to the current raster position.
      glutBitmapString (font, ubytes);
   }
}

