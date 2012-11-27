#include <iostream>
#include "../include/Root.h"

using std::cerr;
using std::endl;
using Arya::Root;

Root* root;

class Closer : public Arya::InputListener
{
    bool keyDown(int key, bool keyDown){ if( keyDown && key == GLFW_KEY_ESC ){ root->stopRendering(); } return false; };
};

Closer closer;

int main()
{
    root = new Root;
    if(!(root->init())) {
        cerr << "Could not init *root*, closing" << endl; 
    }
    else
    {
        root->addInputListener(&closer);
        root->startRendering();
    }

    delete root;

    return 0;
}
