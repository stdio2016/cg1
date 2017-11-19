#pragma once
struct Vec3 // stores (x,y,z)
{
public:
	float p[3];
	inline float operator[] (size_t i) const { return p[i]; }
	inline float& operator[] (size_t i) { return p[i]; }
	Vec3() {
		p[0] = p[1] = p[2] = 0.0f;
	}
	Vec3(float v[3]) {
		for (size_t i = 0; i < 3; i++) {
			p[i] = v[i];
		}
	}
	Vec3(float x, float y, float z) {
		p[0] = x, p[1] = y, p[2] = z;
	}
	float magnitude(void);
	Vec3 operator+(Vec3 b) const;
	Vec3 operator-(Vec3 b) const;
};
