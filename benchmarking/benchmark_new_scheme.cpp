#include <iostream>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <charconv>
#include <random>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <sstream>
#include <boost/lexical_cast.hpp>



bool compare_char_bid_a_geq_b(const char* a, const char* b){
    if (b == nullptr) return true;

    // returns True if a >= b
    const char* int_a = a;
    const char* int_b = b;

    const char* dot_a = a;
    const char* dot_b = b;
    while (*dot_a != '.' && *dot_a != '\0') dot_a++;
    while (*dot_b != '.' && *dot_b != '\0') dot_b++;

    // If the int representation of a is longer than the int rep of b, return True
    // If the int representation of a is shorter than the int rep of b, return False
    unsigned int len_a = dot_a - int_a;
    unsigned int len_b = dot_b - int_b;

    if (len_a > len_b) return true;
    if (len_a < len_b) return false;

    for (int i=0; i< len_a; i++) {
        if (*int_a > *int_b) return true;
        if (*int_a < *int_b) return false;
        int_a++;
        int_b++;
    }

    if (*dot_b == '\0') return true;
    if (*dot_a == '\0') return false;
    
    dot_a++;
    dot_b++;

    while (*dot_a != '\0' && *dot_b != '\0') {
        if (*dot_a > *dot_b) return true;
        if (*dot_a < *dot_b) return false;
        dot_a++;
        dot_b++;
    }

    if (*dot_b != '\0') return false;
    return true;
}

// Conversion-based comparison: changes std::string using stod and compares
bool compare_char_bid_a_geq_b_float(const char* a, const char* b)
{
    if (b == nullptr) return true;

    while((*a != '.') && (*b != '.'))
    {
        if (*a > *b) return true;
        if (*a < *b) return false;
        a++;
        b++;
    }
    
    if((*a != '.') && (*b == '.'))
        return true;
    else if((*a == '.') && (*b != '.'))
        return false;
    
    a++;
    b++;

    while (*a != '\0' && *b != '\0') {
        if (*a > *b) return true;
        if (*a < *b) return false;
        a++;
        b++;
    }

    if (*b != '\0') return false;
    return true;
}

// Tests with no trailing zeros (exchange-style)
void run_tests() {
    struct TestCase {
        const char* a;
        const char* b;
        bool expected;
    };

    TestCase tests[] = {
        {"123.45", "123.46", false},
        {"123.45", "123.44", true},
        {"123.5",  "123.5",  true},
        // {"999.999", "1000",  false},
        {"1000.000",   "999.999", true},
        // {"500",    "500",    true},
        // {"1","2", false},
        // {"2","2", true},
        // {"2","1", true},
        {"0.1",    "0.09",   true},
        {"0.123",  "0.1234", false},
        {"0.1234", "0.123",  true},
    };

    std::cout << "Running test cases:\n";
    for (auto& test : tests) {
        bool result_custom = compare_char_bid_a_geq_b(test.a, test.b);
        bool result_float  = compare_char_bid_a_geq_b_float(test.a, test.b);
        std::cout << "a = " << test.a << ", b = " << test.b
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

    // Generate input std::strings
    for (int i = 0; i < iterations; ++i) {
        inputs_a.emplace_back(generate_random_float_string());
        inputs_b.emplace_back(generate_random_float_string());
        ptrs_a.push_back(inputs_a.back().c_str());
        ptrs_b.push_back(inputs_b.back().c_str());
    }

    // Benchmark char* comparison
    int count1 = 0;
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        if (compare_char_bid_a_geq_b(ptrs_a[i], ptrs_b[i])) ++count1;
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "\nBenchmarking with random inputs (" << iterations << " iterations):\n";
    std::cout << "compare_char_bid_a_geq_b:       " << duration.count() << " ms (true count = " << count1 << ")\n";

    // Benchmark float-based comparison
    int count2 = 0;
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        if (compare_char_bid_a_geq_b_float(ptrs_a[i], ptrs_b[i])) ++count2;
    }
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "compare_char_bid_a_geq_b_float: " << duration.count() << " ms (true count = " << count2 << ")\n";
}

int main() {
    run_tests();
    benchmark();
    return 0;
}