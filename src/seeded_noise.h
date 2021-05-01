#ifndef SRC_SEEDED_NOISE_H_
#define SRC_SEEDED_NOISE_H_

#include <glm/gtc/noise.hpp>

class Noise
{
public:
	Noise(int seed_)
		: seed{(float)seed_}
	{}

	int at(glm::vec2 const& v)
	{
		auto altitude = 0.f;
		for (auto scale = 1.f; scale < 64.f; scale *= 2.f)
			altitude += (1 + glm::simplex(glm::vec3{seed / scale, 0.12f + v.x / scale, 0.45f + v.y / scale})) * scale;
		return (int)altitude;
	}

private:
	float seed;
};

#endif // SRC_SEEDED_NOISE_H_
