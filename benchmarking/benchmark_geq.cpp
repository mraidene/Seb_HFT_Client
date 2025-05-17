#include <iostream>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <charconv>
#include <random>
#include <vector>
#include <string>
#include <iomanip>
#include <sstream>
#include "..\\compare_char_bid_a_geq_b.h"

using namespace std;
using namespace chrono;

// Conversion-based comparison: copies strings up to '|' and uses atof
bool compare_char_bid_a_geq_b_float(const char* a, const char* b) {
    float fa, fb;

    auto end_a = strchr(a, '|');
    auto end_b = strchr(b, '|');

    if (!end_a || !end_b) return false; // invalid input

    std::from_chars(a, end_a, fa);
    std::from_chars(b, end_b, fb);

    return fa >= fb;
}

// Tests with no trailing zeros (exchange-style)
void run_tests() {
    struct TestCase {
        const char* a;
        const char* b;
        bool expected;
    };

    TestCase tests[] = {
        {"123.45|", "123.46|", false},
        {"123.45|", "123.44|", true},
        {"123.5|",  "123.5|",  true},
        {"999.999|", "1000|",  false},
        {"1000|",   "999.999|", true},
        {"500|",    "500|",    true},
        {"1|","2|", false},
        {"2|","2|", true},
        {"2|","1|", true},
        {"0.1|",    "0.09|",   true},
        {"0.123|",  "0.1234|", false},
        {"0.1234|", "0.123|",  true},
    };

    cout << "Running test cases:\n";
    for (auto& test : tests) {
        bool result_custom = compare_char_bid_a_geq_b(test.a, test.b);
        bool result_float  = compare_char_bid_a_geq_b_float(test.a, test.b);
        cout << "a = " << test.a << ", b = " << test.b
             << " | expected: " << test.expected
             << ", char*: " << result_custom
             << ", float: " << result_float
             << (result_custom != test.expected ? " [FAIL]" : "")
             << '\n';
    }
}

std::string generate_random_float_string() {
    static std::mt19937 rng(42);
    static std::uniform_real_distribution<float> dist(0.0f, 10000.0f);

    float value = dist(rng);
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(6) << value;

    std::string str = oss.str();
    str.erase(str.find_last_not_of('0') + 1);
    if (str.back() == '.') str.pop_back();
    str.push_back('|');

    return str;
}

void benchmark() {
    const int iterations = 1'000'000;

    std::vector<std::string> inputs_a, inputs_b;
    std::vector<const char*> ptrs_a, ptrs_b;

    inputs_a.reserve(iterations);
    inputs_b.reserve(iterations);
    ptrs_a.reserve(iterations);
    ptrs_b.reserve(iterations);

    // Generate input strings
    for (int i = 0; i < iterations; ++i) {
        inputs_a.emplace_back(generate_random_float_string());
        inputs_b.emplace_back(generate_random_float_string());
        ptrs_a.push_back(inputs_a.back().c_str());
        ptrs_b.push_back(inputs_b.back().c_str());
    }

    // Benchmark char* comparison
    int count1 = 0;
    auto start = high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        if (compare_char_bid_a_geq_b(ptrs_a[i], ptrs_b[i])) ++count1;
    }
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start);
    cout << "\nBenchmarking with random inputs (" << iterations << " iterations):\n";
    cout << "compare_char_bid_a_geq_b:       " << duration.count() << " ms (true count = " << count1 << ")\n";

    // Benchmark float-based comparison
    int count2 = 0;
    start = high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        if (compare_char_bid_a_geq_b_float(ptrs_a[i], ptrs_b[i])) ++count2;
    }
    end = high_resolution_clock::now();
    duration = duration_cast<milliseconds>(end - start);
    cout << "compare_char_bid_a_geq_b_float: " << duration.count() << " ms (true count = " << count2 << ")\n";
}

int main() {
    run_tests();
    benchmark();
    return 0;
}