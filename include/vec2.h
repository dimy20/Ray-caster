#pragma once

#include <cmath>
namespace rc{
	template<typename T>
	struct Vec2{
		Vec2() : x(0), y(0) {};
		Vec2(T _x, T _y) : x(_x), y(_y) {};

		inline double length() const { return sqrt(x*x + y*y); };
		Vec2& operator += (const Vec2& other){
			x += other.x;
			y += other.y;
			return *this;
		}

		public:
			T x, y;
	};

	template<typename T>
	inline Vec2<T> operator + (const Vec2<T>& a, const Vec2<T>& b){
		return Vec2<T>(a.x + b.x, a.y + b.y);
	}

	template<typename T>
	inline Vec2<T> operator - (const Vec2<T>& a, const Vec2<T>& b){
		return Vec2<T>(a.x - b.x, a.y - b.y);
	}

	template<typename T>
	inline Vec2<T> operator * (const Vec2<T>& a, double t){
		return Vec2<T>(a.x * t, a.y * t);
	}

	typedef Vec2<double> Vec2f;
	typedef Vec2<int> Vec2i;
}
