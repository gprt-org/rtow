
#ifndef _RNG_H
#define _RNG_H

static const float MY_PI = 3.14159265f;

struct LCGRand {
    uint32_t state;
};


uint32_t murmur_hash3_mix(uint32_t hash, uint32_t k)
{
    const uint32_t c1 = 0xcc9e2d51;
    const uint32_t c2 = 0x1b873593;
    const uint32_t r1 = 15;
    const uint32_t r2 = 13;
    const uint32_t m = 5;
    const uint32_t n = 0xe6546b64;

    k *= c1;
    k = (k << r1) | (k >> (32 - r1));
    k *= c2;

    hash ^= k;
    hash = ((hash << r2) | (hash >> (32 - r2))) * m + n;

    return hash;
}

uint32_t murmur_hash3_finalize(uint32_t hash)
{
    hash ^= hash >> 16;
    hash *= 0x85ebca6b;
    hash ^= hash >> 13;
    hash *= 0xc2b2ae35;
    hash ^= hash >> 16;

    return hash;
}

uint32_t lcg_random(inout LCGRand rng)
{
    const uint32_t m = 1664525;
    const uint32_t n = 1013904223;
    rng.state = rng.state * m + n;
    return rng.state;
}

float lcg_randomf(inout LCGRand rng)
{
    return ldexp((float)lcg_random(rng), -32);
}

float lcg_uniformf(inout LCGRand rng, float min, float max) {
    return (max - min) * lcg_randomf(rng) + min;
}

float3 lcg_random_vec3(inout LCGRand rng) {
    // produce vector relative to the origin from these values
    float theta = 2 * MY_PI * lcg_randomf(rng);
    float phi = acos(1. - 2.*lcg_randomf(rng));

    return normalize(float3(sin(phi)*cos(theta),
                  sin(phi)*sin(theta),
                  cos(phi)));
}

float3 random_in_hemisphere(inout LCGRand rng, float3 dir) {
    float3 new_dir;
    // get a random vector in the unit sphere
    new_dir = lcg_random_vec3(rng);
    if (dot(dir, new_dir) > 0) { return new_dir; }
    else return -new_dir;
}

LCGRand get_rng(int frame_id, uint2 pixel, uint2 dims)
{
    LCGRand rng;
    rng.state = murmur_hash3_mix(0, pixel.x + pixel.y * dims.x);
    rng.state = murmur_hash3_mix(rng.state, frame_id);
    rng.state = murmur_hash3_finalize(rng.state);
    return rng;
}

#endif