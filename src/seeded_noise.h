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

	bool treeAt(glm::vec2 const& v)
	{
		return glm::simplex(glm::vec3{seed * 1.1f, (12.345f + v.x * 1000), (123.45f + v.y * 1000)}) > 0.8f;
	}

private:
	float seed;
};

#endif // SRC_SEEDED_NOISE_H_
