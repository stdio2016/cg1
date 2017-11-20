#include "Vec3.h"
#include <cmath>

float Vec3::magnitude(void) {
	return sqrtf(p[0] * p[0] + p[1] * p[1] + p[2] * p[2]);
}

Vec3 Vec3::operator-(Vec3 b) const {
	return Vec3(p[0] - b[0], p[1] - b[1], p[2] - b[2]);
}

Vec3 Vec3::operator+(Vec3 b) const {
	return Vec3(p[0] - b[0], p[1] - b[1], p[2] - b[2]);
}

Vec3 Vec3::operator* (float ratio) const {
	return Vec3(p[0] * ratio, p[1] * ratio, p[2] * ratio);
}

Vec3 Vec3::cross(Vec3 b) const {
	return Vec3(
		y() * b.z() - z() * b.y(),
		z() * b.x() - x() * b.z(),
		x() * b.y() - y() * b.x()
	);
}

float Vec3::dot(Vec3 b) const {
	return p[0] * b[0] + p[1] * b[1] + p[2] * b[2];
}
