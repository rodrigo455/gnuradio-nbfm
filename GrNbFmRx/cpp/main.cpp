#include <iostream>
#include "ossie/ossieSupport.h"

#include "GrNbFmRx.h"
int main(int argc, char* argv[])
{
    GrNbFmRx_i* GrNbFmRx_servant;
    Component::start_component(GrNbFmRx_servant, argc, argv);
    return 0;
}

