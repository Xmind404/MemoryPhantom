#ifndef VECTORS_H
#define VECTORS_H

#include <cmath>
#include <iostream>

class Vector3 {
    public:
        float x, y, z;

        Vector3() {
            this->x = 0;
            this->y = 0;
            this->z = 0;
        }

        Vector3(float x, float y, float z){
            this->x = x;
            this->y = y;
            this->z = z;
        }

        const std::string to_string() {
            return "(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")";
        }

        static Vector3 Add(const Vector3& a, const Vector3& b) {
            return Vector3(a.x + b.x, a.y + b.y, a.z + b.z);
        }

        static Vector3 Subtract(const Vector3& a, const Vector3& b) {
            return Vector3(a.x - b.x, a.y - b.y, a.z - b.z);
        }

        static float Distance(const Vector3& a, const Vector3& b) {
            return std::sqrt(
                (b.x - a.x) * (b.x - a.x) +
                (b.y - a.y) * (b.y - a.y) +
                (b.z - a.z) * (b.z - a.z)
            );
        }
};

class Vector2 {
    public:
        float x, y;

        Vector2() {
            this->x = 0;
            this->y = 0;
        }

        Vector2(float x, float y) {
            this->x = x;
            this->y = y;
        }

        std::string to_string() const {
            return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
        }

        static Vector2 Add(const Vector2& a, const Vector2& b) {
            return Vector2(a.x + b.x, a.y + b.y);
        }

        static Vector2 Subtract(const Vector2& a, const Vector2& b) {
            return Vector2(a.x - b.x, a.y - b.y);
        }

        static float Distance(const Vector2& a, const Vector2& b) {
            return std::sqrt(
                (b.x - a.x) * (b.x - a.x) +
                (b.y - a.y) * (b.y - a.y)
            );
        }
};
#endif