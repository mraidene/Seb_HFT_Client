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
#include "../compare_char_ask_a_leq_b.h"

using namespace std;
using namespace chrono;

// Float-based baseline comparison using from_chars
bool compare_char_bid_a_leq_b_float(const char* a, const char* b) {
    float fa, fb;
    auto end_a = strchr(a, '\n');
    auto end_b = strchr(b, '\n');
    if (!end_a || !end_b) return false;

    std::from_chars(a, end_a, fa);
    std::from_chars(b, end_b, fb);
    return fa <= fb;
}

// Generate random float string ending in '\n'
std::string generate_random_float_string() {
    static std::mt19937 rng(42);
    static std::uniform_real_distribution<float> dist(0.0f, 10000.0f);

    float value = dist(rng);
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(6) << value;

    std::string str = oss.str();
    str.erase(str.find_last_not_of('0') + 1);
    if (str.back() == '.') str.pop_back();
    str.push_back('\n');
    return str;
}

void run_tests() {
    struct TestCase {
        const char* a;
        const char* b;
        bool expected;
    };

    TestCase tests[] = {
        {"123.45\n", "123.46\n", true},
        {"123.45\n", "123.44\n", false},
        {"123.5\n",  "123.5\n",  true},
        {"999.999\n", "1000\n",  true},
        {"1000\n",   "999.999\n", false},
        {"1\n",      "2\n", true},
        {"2\n",      "2\n", true},
        {"2\n",      "1\n", false},
        {"0.1\n",    "0.09\n", false},
        {"0.123\n",  "0.1234\n", true},
        {"0.1234\n", "0.123\n", false},
    };

    cout << "Running test cases:\n";
    for (const auto& test : tests) {
        bool result_custom = compare_char_bid_a_leq_b(test.a, test.b);
        bool result_float  = compare_char_bid_a_leq_b_float(test.a, test.b);
        cout << "a = " << test.a << ", b = " << test.b
             << " | expected: " << test.expected
             << ", char*: " << result_custom
             << ", float: " << result_float
             << (result_custom != test.expected ? " [FAIL]" : "")
             << '\n';
    }
}

void benchmark() {
    const int iterations = 1'000'000;
    vector<string> inputs_a, inputs_b;
    vector<const char*> ptrs_a, ptrs_b;

    inputs_a.reserve(iterations);
    inputs_b.reserve(iterations);
    ptrs_a.reserve(iterations);
    ptrs_b.reserve(iterations);

    for (int i = 0; i < iterations; ++i) {
        inputs_a.emplace_back(generate_random_float_string());
        inputs_b.emplace_back(generate_random_float_string());
        ptrs_a.push_back(inputs_a.back().c_str());
        ptrs_b.push_back(inputs_b.back().c_str());
    }

    int count1 = 0;
    auto start = high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        if (compare_char_bid_a_leq_b(ptrs_a[i], ptrs_b[i])) ++count1;
    }
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start);
    cout << "\nBenchmark (" << iterations << " comparisons):\n";
    cout << "compare_char_bid_a_leq_b:       " << duration.count() << " ms (true count = " << count1 << ")\n";

    int count2 = 0;
    start = high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        if (compare_char_bid_a_leq_b_float(ptrs_a[i], ptrs_b[i])) ++count2;
    }
    end = high_resolution_clock::now();
    duration = duration_cast<milliseconds>(end - start);
    cout << "compare_char_bid_a_leq_b_float: " << duration.count() << " ms (true count = " << count2 << ")\n";
}

int main() {
    run_tests();
    benchmark();
    return 0;
}