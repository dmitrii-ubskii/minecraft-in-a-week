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
		for (auto scale = 16.f; scale > 1.f; scale /= 2.f)
			altitude += glm::simplex(glm::vec3{seed / scale, (12.345f + v.x) / scale, (123.45f + v.y) / scale}) * scale * 3.f;
		return (int)altitude + 60;
	}

private:
	float seed;
};

#endif // SRC_SEEDED_NOISE_H_
