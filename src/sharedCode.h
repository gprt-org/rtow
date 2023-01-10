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

#include "gprt.h"

struct SphereBoundsData {
  /*! array/buffer of vertex indices */
  alignas(16) gprt::Buffer vertex; // vec3f*
  /*! array/buffer of vertex positions */
  alignas(16) gprt::Buffer radius; // float *
  /*! array/buffer of AABBs */
  alignas(16) gprt::Buffer aabbs;
};

/* variables for the triangle mesh geometry */
struct SphereGeomData {
  /*! array/buffer of vertex indices */
  alignas(16) gprt::Buffer vertex; // vec3f*
  /*! array/buffer of vertex positions */
  alignas(16) gprt::Buffer radius; // float *
};

// note! HLSL aligns to float4 boundaries!
struct RayGenData {
  // pointers are represented using uint64_t
  alignas(16) gprt::Buffer fbPtr;

  alignas(8) int2 fbSize;

  alignas(16) gprt::Accel world;

  alignas(16) float rand;

  alignas(4) int frames;

  struct {
    alignas(16) float3 horizontal; // horizontal field of view
    alignas(16) float3 vertical;   // vertical field of view
    alignas(16) float3 pos;    // camera position
    alignas(16) float3 llc;    // lower-left corner of visiable space
  } camera;
};

struct MissProgData {};
