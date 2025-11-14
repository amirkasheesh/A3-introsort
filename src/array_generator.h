#pragma once

#include <vector>
#include <random>
#include <algorithm>
#include <functional>
#include <utility>
#include <cmath>
#include <string>
#include <sstream>

class ArrayGenerator {
    int n_max = 100000;
    int value_min = 0;
    int value_max = 6000;
    double swap_const = 0.005;
    int num_swaps = 1;
    uint64_t seed = 123456;
    std::mt19937_64 prng;
    std::vector<int> base_random;
    std::vector<int> base_reverse;
    std::vector<int> base_almost;

 public:
    enum class Category { Random, Reverse, Almost };

    ArrayGenerator() : prng(seed), base_random(n_max), base_reverse(n_max), base_almost(n_max) {}

    void build() {
        prng.seed(seed);
        std::uniform_int_distribution<int> value_dist(value_min, value_max);

        // Построим массив из рандомных значений
        for (int i = 0; i < n_max; ++i) {
            base_random[i] = value_dist(prng);
        }

        // Построим временный отсортированный массив
        std::vector<int> temp_sorted(base_random);
        std::sort(temp_sorted.begin(), temp_sorted.end());

        // Построим строго невозрастающий массив
        for (int i = 0; i < n_max; ++i) {
            base_reverse[i] = temp_sorted[n_max - i - 1];
        }

        // Построим почти отсортированный
        base_almost = temp_sorted;
        num_swaps = std::max(1, std::min(n_max - 1, static_cast<int>(floor(n_max*swap_const))));

        if (n_max >= 2 && num_swaps > 0) {
            std::uniform_int_distribution<int> index_dist(0, n_max - 2);
            for (int i = 0; i < num_swaps; ++i) {
                int idx = index_dist(prng);
                std::swap(base_almost[idx], base_almost[idx + 1]);
            }
        } else {
            num_swaps = 0;
        }
    }

    void copy_prefix(Category cat, size_t m, std::vector<int>& out) const {
        if (static_cast<int>(m) > n_max) {
            m = static_cast<size_t>(n_max);
        }

        out.resize(m);
        if (cat == Category::Random) {
            for (size_t i = 0; i < m; ++i) {
                out[i] = base_random[i];
            }
        } else if (cat == Category::Almost) {
            for (size_t i = 0; i < m; ++i) {
                out[i] = base_almost[i];
            }
        } else if (cat == Category::Reverse) {
            for (size_t i = 0; i < m; ++i) {
                out[i] = base_reverse[i];
            }
        }
    }

    std::string describe() const {
        std::ostringstream ss;
        ss << "n_max = " << n_max << "; "  << "value_min = " << value_min << "; " << "value_max = " << value_max << "; " << "swap_const = " << swap_const << "; " << "seed = " << seed << "; " << "num_swaps = " << num_swaps;
        return ss.str();
    }
};
