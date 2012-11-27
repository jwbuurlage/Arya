#include <iostream>
#include "../include/Root.h"

using std::cerr;
using std::endl;
using Arya::Root;

int main()
{
    Root* root = new Root;
    if(!(root->init())) {
        cerr << "Could not init *root*, closing" << endl; 
    }

    delete root;

    return 0;
}
