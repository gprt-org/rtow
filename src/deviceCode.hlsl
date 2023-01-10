// MIT License

// Copyright (c) 2022 Nathan V. Morrical

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "sharedCode.h"
#include "rng.h"

struct Payload {
  float3 color;
};

struct Attribute {
  float3 position;
};

GPRT_COMPUTE_PROGRAM(SphereBounds, (SphereBoundsData, record)) {
  int primID = DispatchThreadID.x;
  float3 position = gprt::load<float3>(record.vertex, primID);
  float radius = gprt::load<float>(record.radius, primID);
  float3 aabbMin = position - float3(radius, radius, radius);
  float3 aabbMax = position + float3(radius, radius, radius);
  gprt::store(record.aabbs, 2 * primID, aabbMin);
  gprt::store(record.aabbs, 2 * primID + 1, aabbMax);
}

// The first parameter here is the name of our entry point.
//
// The second is the type and name of the shader record. A shader record
// can be thought of as the parameters passed to this kernel.
GPRT_RAYGEN_PROGRAM(simpleRayGen, (RayGenData, record)) {
  uint2 pixelID = DispatchRaysIndex().xy;

  if (pixelID.x == 0 && pixelID.y == 0) {
    printf("Hello from your first raygen program!\n");
  }

  Payload payload;
  payload.color = float3(0.f, 0.f, 0.f);

  RayDesc ray;
  ray.Origin = record.camera.pos;
  ray.TMin = 0.0;
  ray.TMax = 1000;

  float3 color = float3(0.f, 0.f, 0.f);

  const int samples_per_pixel = 100;

  RaytracingAccelerationStructure world = gprt::getAccelHandle(record.world);

  int i = 0;
  while (i < samples_per_pixel) {

    LCGRand rng = get_rng(i, pixelID, record.fbSize);

    float u = (float(pixelID.x) + lcg_randomf(rng)) / float(record.fbSize.x - 1);
    float v = (float(record.fbSize.y - pixelID.y) + lcg_randomf(rng)) / float(record.fbSize.y - 1);

    float3 dir = normalize(record.camera.llc + u * record.camera.horizontal + v * record.camera.vertical - record.camera.pos);
    ray.Direction = dir;

    TraceRay(world,
            RAY_FLAG_FORCE_OPAQUE,
            0xff,
            0,
            1,
            0,
            ray,
            payload);

    color = payload.color + color;
    i++;
  }

  // find the frame buffer location (x + width*y) and put the result there
  const int fbOfs = pixelID.x + record.fbSize.x * pixelID.y;
  gprt::store(record.fbPtr, fbOfs, gprt::make_bgra(color / samples_per_pixel));
}

GPRT_INTERSECTION_PROGRAM(SphereIntersection, (SphereGeomData, record)) {
  uint primID = PrimitiveIndex();

  float3 position = gprt::load<float3>(record.vertex, primID);
  float radius = gprt::load<float>(record.radius, primID);

  float3 ro = ObjectRayOrigin();
  float3 rd = ObjectRayDirection();

  float3 oc = ro - position;
  float b = dot(oc, rd);
  float c = dot(oc, oc) - radius * radius;
  float h = b * b - c;

  if (h < 0.0)
    return;
  float tHit = -b - sqrt(h);

  Attribute attr;
  attr.position = position;
  ReportHit(tHit, /*hitKind*/ 0, attr);
}

GPRT_CLOSEST_HIT_PROGRAM(SphereClosestHit, (SphereGeomData, record),
                         (Payload, payload), (Attribute, attribute)) {
  float3 origin = attribute.position;
  float3 hitPos = ObjectRayOrigin() + RayTCurrent() * ObjectRayDirection();
  float3 normal = normalize(hitPos - origin);
  payload.color = 0.5 * (1.f + normal);
}

GPRT_MISS_PROGRAM(miss, (MissProgData, record), (Payload, payload)) {
  float t = 0.5f * WorldRayDirection().y + 1.0;
  payload.color = (1.0 - t)*float3(1.f, 1.f, 1.f) + t*float3(0.5f, 0.7f, 1.0f);
}