#include <vector>
#include <iostream>
#include <fstream>
#include "./sort_tester.h"

int main() {
    SortTester tester;

    std::ofstream fout("results.csv");

    tester.run_all(fout);

    fout.close();

    return 0;
}
