#include "generator.h"

Beta173Perlin::Beta173Perlin(std::uint64_t worldSeed, int octaves)
    : m_octaves(octaves),
    m_perm(512)                                // 2 × 256 for fast wrap
{
    /*
        * Minecraft Beta calls Java’s java.util.Random with the world seed,
        * then initialises a 256‑entry permutation table using a Fisher‑Yates
        * shuffle.  Java Random is a 48‑bit LCG:  (seed * 0x5DEECE66D + 11) & 0xFFFF_FFFFFFFF.
        */
    auto nextInt = [seed = (worldSeed ^ 0x5DEECE66DL) & ((1LL << 48) - 1)]() mutable {
        seed = (seed * 0x5DEECE66DL + 0xB) & ((1LL << 48) - 1);
        return static_cast<int>(seed >> 17);
    };

    std::array<int, 256> p{};
    for (int i = 0; i < 256; ++i) p[i] = i;

    for (int i = 255; i >= 0; --i) {
        int j = nextInt() % (i + 1);
        std::swap(p[i], p[j]);
        m_perm[i] = m_perm[i + 256] = p[i];
    }
}

// 2‑D noise sample; identical to Beta 1.7.3
double Beta173Perlin::noise(double x, double z) const {
    const int X = fastFloor(x) & 255;
    const int Z = fastFloor(z) & 255;
    x -= fastFloor(x);
    z -= fastFloor(z);
    const double u = fade(x);
    const double v = fade(z);

    const int A  = m_perm[X] + Z;
    const int B  = m_perm[X + 1] + Z;

    return lerp(v,
                lerp(u, grad(m_perm[A  ], x    , z    ),
                            grad(m_perm[B  ], x - 1, z    )),
                lerp(u, grad(m_perm[A+1], x    , z - 1),
                            grad(m_perm[B+1], x - 1, z - 1)));
}

// Full FBM stack (6 octaves, identical gain/frequency)
double Beta173Perlin::fbm(double x, double z,
            double baseFreq,   // 1/20 blocks – official value
            double lacunarity,
            double gain) const
{
    double amp = 1.0, freq = baseFreq, sum = 0.0;
    for (int i = 0; i < m_octaves; ++i) {
        sum += amp * noise(x * freq, z * freq);
        freq *= lacunarity;
        amp  *= gain;
    }
    // Java Perlin returns in [‑1,1]; Beta rescales to [0,1]
    return 0.5 * (sum + 1.0);
}