#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>


#define WINDOW_HEIGHT 900
#define WINDOW_WIDTH 900
#define APPNAME "Trigonometry Demo"

void notifye(const char * error); // Msgbox an error (a sys error if the msg starts with "from:" )
void error(const char * error); // same as notifye but quits the application after the msgbox

void notify(const char * message); // Show a simple msgbox with the message string with no icons
void notifyd(const double d); // Same as above but for integral values

void strlower(const char * str, char * lowered); // Lowercase a string str into a string lowered

void notifyc(char c);

char * laststrchr(char * str, char c);

void post_line_out(char * out); 

