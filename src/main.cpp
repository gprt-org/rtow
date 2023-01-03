
#include <gprt.h>

// include for GPRT structs
#include "sharedCode.h"

extern GPRTProgram dev_code;


const int image_width = 800;

const float3 sphere_org {0.f, 0.f, -1.f};
const float sphere_rad = .5f;

int main() {

  // create a ray tracing context
  GPRTContext gprt = gprtContextCreate(nullptr, 1);

  GPRTModule module = gprtModuleCreate(gprt, dev_code);

  // compute some camera values
  const float aspect_ratio = 16.0 / 9.0;
  const int image_height = static_cast<int>(image_width / aspect_ratio);

  // create a frame buffer used to view/write images
  GPRTBufferOf<uint32_t> frameBuffer =
    gprtDeviceBufferCreate<uint32_t>(gprt, image_width * image_height);

  const float focal_length = 1.0;

  float viewport_height = 2.0;
  float viewport_width = aspect_ratio * viewport_height;

  const auto origin = float3(0.f, 0.f, 0.f);
  const auto horizontal = float3(viewport_width, 0.f, 0.f);
  const auto vertical = float3(0.f, viewport_height, 0.f);

  const auto llc = origin - horizontal / 2 - vertical / 2 - float3(0.f, 0.f, focal_length);

  // create a ray generation program and set common data
  GPRTRayGenOf<RayGenData> rayGen =
    gprtRayGenCreate<RayGenData>(gprt, module, "simpleRayGen");

  RayGenData *data = gprtRayGenGetPointer(rayGen);
  data->fbPtr = gprtBufferGetHandle(frameBuffer);
  data->fbSize = int2(image_width, image_height);

  // set camera properties on the ray gen object
  data->camera.horizontal = horizontal;
  data->camera.vertical = vertical;
  data->camera.pos = origin;
  data->camera.llc = llc;

  data->sphere.center = sphere_org;
  data->sphere.radius = sphere_rad;

  gprtBuildPipeline(gprt);

  // setup shader binding table
  gprtBuildShaderBindingTable(gprt, GPRT_SBT_RAYGEN);

  // populate the frame buffer
  gprtRayGenLaunch2D(gprt, rayGen, image_width, image_height);

  // save the current frame buffer to file
  gprtBufferSaveImage(frameBuffer, image_width, image_height, "test.png");

  gprtBufferDestroy(frameBuffer);
  gprtRayGenDestroy(rayGen);
  gprtModuleDestroy(module);
  gprtContextDestroy(gprt);

  return 0;
}