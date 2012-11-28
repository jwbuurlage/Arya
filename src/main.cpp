#include <iostream>
#include "../include/Root.h"

using std::cerr;
using std::endl;
using Arya::Root;

Root* root;

class Closer : public Arya::InputListener
{
    bool keyDown(int key, bool keyDown)
    {
        if( keyDown )
        {
            if( key == GLFW_KEY_ESC )
                root->stopRendering();
            else if( key == GLFW_KEY_F11 )
                root->setFullscreen(!root->getFullscreen());
        }
        return false;
    }
};

Closer closer;

int main()
{
    root = new Root;
    if(!(root->init(false))) {
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
