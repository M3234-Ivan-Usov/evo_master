#include<iostream>
#include<vector>
#include<algorithm>
#include<chrono>
#include<functional>

using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::milliseconds;

int WARMUPS = 1;
int TEST_RUNS = 5;

template<typename T>
std::vector<T> generate(size_t size) {
    std::vector<T> v(size);
    for (size_t i = 0; i < size; ++i) {
        v[i] = static_cast<T>(std::rand());
    }
    return v;
}

template<typename T>
void heapify(std::vector<T>& v, size_t n, size_t i) {
    size_t largest = i;   
    size_t l = 2 * i + 1;
    size_t r = 2 * i + 2;

    if (l < n && v[l] > v[largest])
        largest = l;
    if (r < n && v[r] > v[largest])
        largest = r;

    if (largest != i) {
        std::swap(v[i], v[largest]);
        heapify(v, n, largest);
    }
}

template<typename T>
void simple_heapsort(std::vector<T>& v) {
    size_t n = v.size();
    for (int i = v.size() / 2 - 1; i >= 0; --i)
        heapify(v, n, i);

    for (int i = n - 1; i >= 0; --i) {
        std::swap(v[0], v[i]);
        heapify(v, i, 0);
    }
}

template<typename T>
void std_heapsort(std::vector<T>& v) {
    std::make_heap(v.begin(), v.end());
    std::sort_heap(v.begin(), v.end());
}

template <typename T>
void verify(std::vector<T>& v) {
    for (size_t i = 1; i < v.size(); ++i) {
        if (v[i] < v[i - 1]) throw "Verification failed";
    }
}

template<typename T>
int with_time(std::vector<T>& v, std::function<void(std::vector<T>&)> fun) {
    auto start = high_resolution_clock::now();
    fun(v);
    auto end = high_resolution_clock::now();
    verify(v);
    return duration_cast<milliseconds>(end - start).count();
}

template<typename T>
int estimate(size_t size, std::function<void(std::vector<T>&)> fun) {
    for (int warmup = 0; warmup < WARMUPS; ++warmup) {
        std::vector<T> v = generate<T>(size);
        fun(v);
    }
    int elapsed = 0;
    for (int test = 0; test < TEST_RUNS; ++test) {
        std::vector<T> v = generate<T>(size);
        elapsed += with_time(v, fun);
    }
    return elapsed / TEST_RUNS;
}


template<typename T>
std::pair<int, int> run_test(size_t size) {
    int elapsed_simple = estimate<T>(size, &simple_heapsort<T>);
    int elapsed_std = estimate<T>(size, &std_heapsort<T>);
    return {elapsed_simple, elapsed_std};
}

void log_result(std::pair<int, int>& p, std::string type) {
    std::cout << "-- Type: " << type << "\n";
    std::cout << "---- Simple: " << p.first << "ms\n";
    std::cout << "---- Std: " << p.second << "ms\n";
}

int main() {
    for (size_t size: {5.0e5, 7.5e5, 1.0e6, 2.5e6, 5.0e6}) {
        std::cout << "Running tests with size " << size << ":\n";
        auto int_test = run_test<int>(size);
        log_result(int_test, "int");
        auto float_test = run_test<float>(size);
        log_result(float_test, "float");
        auto double_test = run_test<double>(size);
        log_result(double_test, "double");
    }
}


/*
Running tests with size 500000:
-- Type: int
---- Simple: 91ms
---- Std: 79ms
-- Type: float
---- Simple: 94ms
---- Std: 81ms
-- Type: double
---- Simple: 119ms
---- Std: 87ms
Running tests with size 750000:
-- Type: int
---- Simple: 144ms
---- Std: 126ms
-- Type: float
---- Simple: 151ms
---- Std: 130ms
-- Type: double
---- Simple: 190ms
---- Std: 145ms
Running tests with size 1000000:
-- Type: int
---- Simple: 206ms
---- Std: 170ms
-- Type: float
---- Simple: 216ms
---- Std: 181ms
-- Type: double
---- Simple: 272ms
---- Std: 204ms
Running tests with size 2500000:
-- Type: int
---- Simple: 624ms
---- Std: 527ms
-- Type: float
---- Simple: 649ms
---- Std: 552ms
-- Type: double
---- Simple: 855ms
---- Std: 667ms
Running tests with size 5000000:
-- Type: int
---- Simple: 1497ms
---- Std: 2020ms
-- Type: float
---- Simple: 6440ms
---- Std: 5643ms
-- Type: double
---- Simple: 8171ms
---- Std: 6612ms
*/