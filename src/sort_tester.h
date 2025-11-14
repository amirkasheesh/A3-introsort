#pragma once

#include <vector>
#include <iostream>
#include <sstream>
#include <chrono>
#include <fstream>
#include <cstdint>
#include <algorithm>
#include <tuple>
#include <utility>
#include <cmath>
#include <random>
#include "./array_generator.h"

class SortTester {
    ArrayGenerator creater;
    std::vector<int> sizes;
    std::mt19937 rng;
    std::vector<ArrayGenerator::Category> cat_list = {ArrayGenerator::Category::Random, ArrayGenerator::Category::Almost, ArrayGenerator::Category::Reverse};
    int R = 5;
    void InsertionSort(int left, int right, std::vector<int>& vec) {
        for (int i = left + 1; i <= right; ++i) {
            int current_el = vec[i];
            int j = i - 1;
            while (j >= left && current_el < vec[j]) {
                vec[j + 1] = vec[j];
                --j;
            }
            vec[j + 1] = current_el;
        }
    }




    void heapifyRange(std::vector<int>& vec, int l, int heap_size, int i) {
        int largest = i;
        int left  = 2 * i + 1;
        int right = 2 * i + 2;

        if (left < heap_size && vec[l + left] > vec[l + largest]) {
            largest = left;
        }
        if (right < heap_size && vec[l + right] > vec[l + largest]) {
            largest = right;
        }

        if (largest != i) {
            std::swap(vec[l + i], vec[l + largest]);
            heapifyRange(vec, l, heap_size, largest);
        }
    }

    void heapSortRange(std::vector<int>& vec, int l, int r) {
        int heap_size = r - l + 1;
        if (heap_size <= 1) {
            return;
        }

        for (int i = (heap_size - 2) / 2; i >= 0; --i) {
            heapifyRange(vec, l, heap_size, i);
        }

        for (int end = heap_size - 1; end > 0; --end) {
            std::swap(vec[l], vec[l + end]);
            heapifyRange(vec, l, end, 0);
        }
    }


    int partition(std::vector<int>& vec, int l, int r) {
        std::uniform_int_distribution<int> dist(l, r);
        int pivotIdx = dist(rng);
        std::swap(vec[pivotIdx], vec[r]);

        int pivot_el = vec[r];
        int i = l - 1;

        for (int j = l; j < r; ++j) {
            if (vec[j] < pivot_el) {
                ++i;
                std::swap(vec[i], vec[j]);
            }
        }
        std::swap(vec[i + 1], vec[r]);
        return i + 1;
    }

    void introSort(std::vector<int>& vec, int l, int r, int depth) {
        if (r - l + 1 < 16) {
            InsertionSort(l, r, vec);
            return;
        }
        if (depth == 0) {
            heapSortRange(vec, l, r);
            return;
        }

        int p = partition(vec, l, r);
        introSort(vec, l, p - 1, depth - 1);
        introSort(vec, p + 1, r, depth - 1);
    }

    void quickSort(std::vector<int>& vec, int l, int r) {
        if (r - l + 1 <= 1) {
            return;
        }
        int p = partition(vec, l, r);
        quickSort(vec, l, p - 1);
        quickSort(vec, p + 1, r);
    }

 public:
    enum class Algorithm { QuickSort, IntroSort };
    std::vector<Algorithm> algs = {Algorithm::QuickSort, Algorithm::IntroSort};


    SortTester() : sizes(996), rng(123456) {
        int val = 500;
        for (int i = 0; i < 996; ++i) {
            sizes[i] = val;
            val += 100;
        }
        creater.build();
    }

    static const char* algo_name(Algorithm a) {
        switch (a) {
            case Algorithm::QuickSort:
                return "QuickSort";
            case Algorithm::IntroSort:
                return "IntroSort";
        }
        return "?";
    }

    static const char* cat_name(ArrayGenerator::Category cat) {
        switch (cat) {
            case ArrayGenerator::Category::Almost:
                return "Almost";
            case ArrayGenerator::Category::Random:
                return "Random";
            case ArrayGenerator::Category::Reverse:
                return "Reverse";
        }
        return "?";
    }

    void write_csv_header(std::ostream& out) {
        out << "Algorithm,category,size,trial,time_us\n";
    }

    void write_csv_row(std::ostream& out, Algorithm algo, ArrayGenerator::Category cat, int size, int trial, int64_t time_us) {
        std::ostringstream ss;
        ss << algo_name(algo) << ',' << cat_name(cat) << ',' << size << ',' << trial << ',' << time_us << '\n';
        out << ss.str();
    }

    int64_t time_calc(std::vector<int>& vec, int l, int r, Algorithm algo) {
        auto start = std::chrono::high_resolution_clock::now();
        if (algo == Algorithm::QuickSort) {
            quickSort(vec, l, r);
        } else if (algo == Algorithm::IntroSort) {
            introSort(vec, l, r, 2 * static_cast<int>(std::log2(r - l + 1)));
        }
        auto elapsed = std::chrono::high_resolution_clock::now() - start;
        int64_t msec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
        return msec;
    }

    void run_all(std::ostream& out) {
        write_csv_header(out);

        std::vector<int> a0;
        for (auto cat : cat_list) {
            for (auto sz : sizes) {
                creater.copy_prefix(cat, sz, a0);
                for (auto alg : algs) {
                    for (int i = 1; i <= R; ++i) {
                        std::vector<int> a = a0;
                        auto tm = time_calc(a, 0, sz - 1, alg);
                        if (!std::is_sorted(a.begin(), a.end())) {
                            return;
                        }
                        write_csv_row(out, alg, cat, sz, i, tm);
                    }
                }
            }
        }
    }
};
