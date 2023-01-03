
#include <gprt.h>

// include for GPRT structs
#include "sharedCode.h"

extern GPRTProgram dev_code;


const int image_width = 1600;
const int image_height = 900;

int main() {

  // create a ray tracing context
  GPRTContext gprt = gprtContextCreate(nullptr, 1);

  GPRTModule module = gprtModuleCreate(gprt, dev_code);

  GPRTRayGenOf<RayGenData> rayGen =
    gprtRayGenCreate<RayGenData>(gprt, module, "simpleRayGen");

  gprtBuildPipeline(gprt);

  GPRTBufferOf<uint32_t> frameBuffer =
    gprtDeviceBufferCreate<uint32_t>(gprt, image_width * image_height);

  // compute some camera values

  const float aspect_ratio = 16.0 / 9.0;
  const int image_width = 400;
  const int image_height = static_cast<int>(image_width / aspect_ratio);
  const float focal_length = 1.0;

  const auto origin = float3(0.f, 0.f, 0.f);
  const auto horizontal = float3(image_width, 0.f, 0.f);
  const auto vertical = float3(0.f, image_height, 0.f);

  const auto llc = origin - horizontal / 2 - vertical / 2 - float3(0.f, 0.f, focal_length);

  RayGenData *data = gprtRayGenGetPointer(rayGen);
  data->fbPtr = gprtBufferGetHandle(frameBuffer);
  data->fbSize = int2(image_width, image_height);

  // set camera properties on the ray gen object
  data->camera.horizontal = horizontal;
  data->camera.vertical = vertical;
  data->camera.pos = origin;
  data->camera.llc = llc;

  gprtBuildShaderBindingTable(gprt, GPRT_SBT_RAYGEN);

  gprtRayGenLaunch2D(gprt, rayGen, image_width, image_height);

  gprtBufferSaveImage(frameBuffer, image_width, image_height, "test.png");

  gprtBufferDestroy(frameBuffer);
  gprtRayGenDestroy(rayGen);
  gprtModuleDestroy(module);
  gprtContextDestroy(gprt);

  return 0;
}