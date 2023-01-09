
#include <gprt.h>

// include for GPRT structs
#include "sharedCode.h"

extern GPRTProgram dev_code;


const int image_width = 2048;

const int NUM_SPHERES = 1;
const float3 sphere_org {0.f, 0.f, -1.f};
const float sphere_rad = .5f;

int main() {

  // create a ray tracing context
  GPRTContext gprt = gprtContextCreate(nullptr, 1);

  GPRTModule module = gprtModuleCreate(gprt, dev_code);

  // geometry type setup
  GPRTGeomTypeOf<SphereGeomData> sphereGeomType =
      gprtGeomTypeCreate<SphereGeomData>(gprt, GPRT_AABBS);
  gprtGeomTypeSetClosestHitProg(sphereGeomType, 0, module, "SphereClosestHit");
  gprtGeomTypeSetIntersectionProg(sphereGeomType, 0, module, "SphereIntersection");

  // -------------------------------------------------------
  // set up sphere bounding box compute program
  // -------------------------------------------------------
  GPRTComputeOf<SphereBoundsData> boundsProgram =
      gprtComputeCreate<SphereBoundsData>(gprt, module, "SphereBounds");

  // -------------------------------------------------------
  // set up miss
  // -------------------------------------------------------
  GPRTMissOf<MissProgData> miss = gprtMissCreate<MissProgData>(gprt, module, "miss");

  // -------------------------------------------------------
  // set up ray gen program
  // -------------------------------------------------------
  GPRTRayGenOf<RayGenData> rayGen = gprtRayGenCreate<RayGenData>(gprt, module, "simpleRayGen");

  // Note, we'll need to call this again after creating our acceleration
  // structures, as acceleration structures will introduce new shader
  // binding table records to the pipeline.
  gprtBuildPipeline(gprt);

  // geometry definition
  GPRTBufferOf<float3> vertexBuffer =
      gprtDeviceBufferCreate<float3>(gprt, NUM_SPHERES, &sphere_org);
  GPRTBufferOf<float> radiusBuffer =
      gprtDeviceBufferCreate<float>(gprt, NUM_SPHERES, &sphere_rad);

  // buffer for AABBs
  GPRTBufferOf<float3> aabbPositionsBuffer =
      gprtDeviceBufferCreate<float3>(gprt, 2 * NUM_SPHERES, nullptr);

  // create geometry instances
  GPRTGeomOf<SphereGeomData> aabbGeom = gprtGeomCreate(gprt, sphereGeomType);
  gprtAABBsSetPositions(aabbGeom, aabbPositionsBuffer, NUM_SPHERES);

  // get pointer to device-side geometry data and set values using buffers created above
  SphereGeomData* geomData = gprtGeomGetPointer(aabbGeom);
  geomData->vertex = gprtBufferGetHandle(vertexBuffer);
  geomData->radius = gprtBufferGetHandle(radiusBuffer);

  // get pointer to device-sice AABBs data and set values using buffers created above
  SphereBoundsData *boundsData = gprtComputeGetPointer(boundsProgram);
  boundsData->vertex = gprtBufferGetHandle(vertexBuffer);
  boundsData->radius = gprtBufferGetHandle(radiusBuffer);
  boundsData->aabbs = gprtBufferGetHandle(aabbPositionsBuffer);

  // compute AABBs in parallel with a compute shader
  gprtBuildShaderBindingTable(gprt, GPRT_SBT_COMPUTE);

  // Launch the compute kernel, which will populate our aabbPositionsBuffer and in turn the aabbGeom positions
  gprtComputeLaunch1D(gprt, boundsProgram, NUM_SPHERES);

  // Now that the aabbPositionsBuffer is filled, we can compute our AABB
  // acceleration structure
  GPRTAccel aabbAccel = gprtAABBAccelCreate(gprt, 1, &aabbGeom);
  gprtAccelBuild(gprt, aabbAccel);

  // create an instance acceleration data structure
  GPRTAccel world = gprtInstanceAccelCreate(gprt, 1, &aabbAccel);
  gprtAccelBuild(gprt, world);

  // NOW SETUP TO RAY TRACE

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

  RayGenData *data = gprtRayGenGetPointer(rayGen);
  data->fbPtr = gprtBufferGetHandle(frameBuffer);
  data->fbSize = int2(image_width, image_height);
  data->world = gprtAccelGetHandle(world);
  // set camera properties on the ray gen object
  data->camera.horizontal = horizontal;
  data->camera.vertical = vertical;
  data->camera.pos = origin;
  data->camera.llc = llc;

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