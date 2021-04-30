#ifndef SRC_PERLIN_H_
#define SRC_PERLIN_H_

#include <cmath>

float dotGridGradient(float x, float z, float gridX, float gridZ, int seed)
{
	// FIXME do a proper generation?
	auto rand = ((float)seed + 2920.f) * std::sin(gridX * 21942.f + gridZ * 171324.f + 8912.f) * std::cos(gridX * 23157.f * gridZ * 217832.f + 9758.f);
	auto randX = std::cos(rand);
	auto randZ = std::sin(rand);
	return (x - gridX) * randX + (z - gridZ) * randZ;
}

float seeded2dPerlin(int seed, float x, float z)
{
	auto ix = std::floor(x);
	auto fx = x - ix;
	auto iz = std::floor(z);
	auto fz = z - iz;

	auto n0 = dotGridGradient(x, z, ix, iz, seed);
	auto n1 = dotGridGradient(x, z, ix + 1, iz, seed);
	auto ix0 = std::lerp(n0, n1, fx);

	n0 = dotGridGradient(x, z, ix, iz + 1, seed);
	n1 = dotGridGradient(x, z, ix + 1, iz + 1, seed);
	auto ix1 = std::lerp(n0, n1, fx);

	return std::lerp(ix0, ix1, fz);
}

#endif // SRC_PERLIN_H_
