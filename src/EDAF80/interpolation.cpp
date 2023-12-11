#include "interpolation.hpp"

glm::vec3
interpolation::evalLERP(glm::vec3 const& p0, glm::vec3 const& p1, float const x)
{
	//! \todo Implement this function
	glm::vec3 interpolatedPoint;
	interpolatedPoint = glm::mix(p0, p1, x);
	return interpolatedPoint;
}

glm::vec3
interpolation::evalCatmullRom(glm::vec3 const& p0, glm::vec3 const& p1,
                              glm::vec3 const& p2, glm::vec3 const& p3,
                              float const t, float const x)
{
	//! \todo Implement this function
	glm::vec3 interpolatedPoint;
	glm::vec4 X{1.0f, x, x * x, x * x * x};

	glm::mat4x4 T2{ 0, -t, 2 * t, -t,
					1, 0, t - 3, 2 - t,
					0, t, 3 - 2 * t, t - 2,
					0, 0, -t, t
	};
	glm::vec4 vt = X * T2;
	glm::mat<4, 3, float> pointMat{p0.x, p0.y, p0.z,
								   p1.x, p1.y, p1.z,
								   p2.x, p2.y, p2.z,
								   p3.x, p3.y, p3.z};

	//interpolatedPoint = X * T * pointMat;
	glm::vec3 interpolatedPoint2 = vt[0] * p0 + vt[1] * p1 + vt[2] * p2 + vt[3] * p3;
	return interpolatedPoint2;
}
