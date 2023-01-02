
#include <gprt.h>

// include for GPRT structs
#include "sharedCode.h"

extern GPRTProgram dev_code;

int main() {

    // create a ray tracing context
    GPRTContext gprt = gprtContextCreate(nullptr, 1);


    GPRTModule module = gprtModuleCreate(gprt, dev_code);


    return 0;


}