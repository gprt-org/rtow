
#include <gprt.h>

// include for GPRT structs
#include "sharedCode.h"

extern GPRTProgram dev_code;


const int image_width = 256;
const int image_height = 256;

int main() {

  // create a ray tracing context
  GPRTContext gprt = gprtContextCreate(nullptr, 1);

  GPRTModule module = gprtModuleCreate(gprt, dev_code);

  GPRTRayGenOf<RayGenData> rayGen =
    gprtRayGenCreate<RayGenData>(gprt, module, "simpleRayGen");

  gprtBuildPipeline(gprt);

  GPRTBufferOf<uint32_t> frameBuffer =
    gprtDeviceBufferCreate<uint32_t>(gprt, image_width * image_height);

  RayGenData *data = gprtRayGenGetPointer(rayGen);
  data->fbPtr = gprtBufferGetHandle(frameBuffer);
  data->fbSize = int2(image_width, image_height);

  gprtBuildShaderBindingTable(gprt, GPRT_SBT_RAYGEN);

  gprtRayGenLaunch2D(gprt, rayGen, image_width, image_height);

  gprtBufferSaveImage(frameBuffer, image_width, image_height, "test.png");

  gprtBufferDestroy(frameBuffer);
  gprtRayGenDestroy(rayGen);
  gprtModuleDestroy(module);
  gprtContextDestroy(gprt);

  return 0;
}