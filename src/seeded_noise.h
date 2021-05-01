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
		constexpr auto scale = 8.f;
		auto altitude = std::floor((1 + glm::simplex(glm::vec3{seed, v})) * scale);
		return (int)altitude;
	}

private:
	float seed;
};

#endif // SRC_SEEDED_NOISE_H_
