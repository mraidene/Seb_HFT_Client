#include <iostream>
#include <charconv>
#include <random>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <vector>

using namespace std;
using namespace std::chrono;

inline int char_id_to_int(const char* id_num){
    auto result = 0;
    from_chars(id_num, id_num+4, result);
    return result;
}

string generate_random_float_string() {
    static mt19937 rng(42);
    static uniform_real_distribution<float> dist(0.0f, 10000.0f);

    float value = dist(rng);
    ostringstream oss;
    oss << fixed << std::setprecision(6) << value;

    string str = oss.str();
    str.erase(str.find_last_not_of('0') + 1);
    if (str.back() == '.') str.pop_back();
    str.push_back('\n');
    return str;
}

inline int char_id_to_int_first(const char* id_num)
{
    int ticker_id = (id_num[0] - '0') * 1000 + (id_num[1] - '0') * 100 + (id_num[2] - '0') * 10 + (id_num[3] - '0');    
    return ticker_id;
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
        if (char_id_to_int_first(ptrs_a[i])) ++count1;
    }
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start);
    cout << "\nBenchmark (" << iterations << " comparisons):\n";
    cout << "char_id_to_int_first:       " << duration.count() << " ms (true count = " << count1 << ")\n";

    int count2 = 0;
    start = high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        if (char_id_to_int(ptrs_a[i])) ++count2;
    }
    end = high_resolution_clock::now();
    duration = duration_cast<milliseconds>(end - start);
    cout << "char_id_to_int: " << duration.count() << " ms (true count = " << count2 << ")\n";
}

int main() {
    benchmark();
    return 0;
}