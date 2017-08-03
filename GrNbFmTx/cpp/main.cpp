#include <iostream>
#include "ossie/ossieSupport.h"

#include "GrNbFmTx.h"
int main(int argc, char* argv[])
{
    GrNbFmTx_i* GrNbFmTx_servant;
    Component::start_component(GrNbFmTx_servant, argc, argv);
    return 0;
}

