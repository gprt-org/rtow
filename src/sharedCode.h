// MIT License

// Copyright (c) 2022 Nathan V. Morrical

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "gprt.h"

/* variables for the triangle mesh geometry */
struct DPTriangleData
{
  /*! array/buffer of vertex indices */
  alignas(16) gprt::Buffer index; // vec3f*
  /*! array/buffer of vertex positions */
  alignas(16) gprt::Buffer vertex; // float *
  /*! array/buffer of AABBs */
  alignas(16) gprt::Buffer aabbs;
  /*! array/buffer of double precision rays */
  alignas(16) gprt::Buffer dpRays;
};

struct RayGenData
{
  alignas(4)  int frameId;
  alignas(16) gprt::Buffer dpRays;
  alignas(16) gprt::Accel world;
  alignas(16) gprt::Buffer distances;
};

/* variables for the miss program */
struct MissProgData
{
  alignas(4) int temp;
};
