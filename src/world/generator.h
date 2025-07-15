#pragma once
// ---------------------------------------------------------------------------
//  Beta 1.7.3‑style Perlin noise (temperature / rainfall) – public‑domain code
// ---------------------------------------------------------------------------
#include <array>
#include <cstdint>
#include <random>
#include <vector>
#include <cmath>
#include <algorithm>

#include <cstdint>
#include <vector>

/**
 * @brief Beta 1.7.3–style 2D Perlin noise and FBM (fractal Brownian motion) generator.
 *
 * The permutation table is initialized identically to Minecraft Beta 1.7.3,
 * using a 48-bit Java LCG seeded with the world seed.
 */
class Beta173Perlin {
public:
    Beta173Perlin(std::uint64_t worldSeed, int octaves = 6);
    double noise(double x, double z) const;
    double fbm(double x, double z,
               double baseFreq = 0.05,
               double lacunarity = 2.0,
               double gain = 0.5) const;
private:
    int m_octaves;
    std::vector<int> m_perm;

    static int fastFloor(double x)        { return x < 0 ? int(x) - 1 : int(x); }
    static double fade(double t)          { return t * t * t * (t * (t * 6 - 15) + 10); }
    static double lerp(double a, double b, double t) { return a + t * (b - a); }
    static double grad(int h, double x, double z) {
        switch (h & 3) {                 // 2‑D gradient hash
            case 0: return  x + z;
            case 1: return -x + z;
            case 2: return  x - z;
            default:return -x - z;
        }
    }
};