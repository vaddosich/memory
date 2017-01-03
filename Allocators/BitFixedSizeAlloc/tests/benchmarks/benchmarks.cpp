#ifdef _BENCHMARKS
#include <benchmark/benchmark.h>
#include <allocator/fixed_size_zero_bit_strategy.h>

std::string mas("massive of string");
const size_t min_node_size = 2;
const size_t node_size = 600;
const size_t max = 64 * 64 * 66 ;
const size_t threads_num = 2;
fixed_size_zero_bit_strategy<min_node_size , node_size, max> myalloc;

static void BM_Malloc(benchmark::State& state) {
    while (state.KeepRunning()) {
        for(size_t i = 0; i < max; i++) {
            volatile void * v = malloc(node_size);
            memcpy((void *)v, mas.c_str(), mas.length() + 1);
            free((void *)v);
        }
    }
}
// Register the function as a benchmark

BENCHMARK(BM_Malloc)->Unit(benchmark::kMillisecond);

// Define another benchmark
static void BM_MyAlloc(benchmark::State& state) {
    //    myalloc.clear();
    while (state.KeepRunning()) {
        state.PauseTiming();
        myalloc.clear();
        state.ResumeTiming();
        for(size_t i = 0; i < max; i++) {
            void * v = myalloc.allocate(node_size);
            memcpy((void *)v, mas.c_str(), mas.length() + 1);
            myalloc.deallocate(v);
        }
    }
}

BENCHMARK(BM_MyAlloc)->Unit(benchmark::kMillisecond);


static void BM_MultiThreadedMalloc(benchmark::State& state) {
    if (state.thread_index == 0) {
        // Setup code here.
    }
    while (state.KeepRunning()) {
        // Run the test as normal.
        for(size_t i = 0; i < max; i++) {
            volatile void * v = malloc(node_size);
            memcpy((void *)v, mas.c_str(), mas.length() + 1);
            free((void *)v);
        }
    }
    if (state.thread_index == 0) {
        // Teardown code here.
    }
}

BENCHMARK(BM_MultiThreadedMalloc)->Threads(threads_num)->Unit(benchmark::kMillisecond);


static void BM_MultiThreadedMyAlloc(benchmark::State& state) {
    if (state.thread_index == 0) {
        // Setup code here.
    }
    while (state.KeepRunning()) {
        for(size_t i = 0; i < max; i++) {
            void * v = myalloc.allocate(node_size);
            memcpy((void *)v, mas.c_str(), mas.length() + 1);
            myalloc.deallocate(v);
        }
        // Run the test as normal.
    }
    if (state.thread_index == 0) {
        // Teardown code here.
    }
}

BENCHMARK(BM_MultiThreadedMyAlloc)->Threads(threads_num)->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN()

#endif // _BENCHMARKS
