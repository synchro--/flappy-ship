#include <functional>
#include <GL/gl.h>
#include <GL/glu.h>

//accepts a lambda to be performed between between push and pop
//Saves time and helps against bugs
void scope(const std::function <void (void)>& lambda){
    glPushMatrix(); 
    lambda(); 
    glPopMatrix(); 
}