#ifdef _BENCHMARKS
#include <benchmark/benchmark.h>
#include <allocator/fixed_size_zero_bit_strategy.h>
#include <allocator/fixed_size_mpmc_queue_strategy.h>


std::string mas("massive of string");
const size_t min_node_size = 2;
const size_t node_size = 40;
const size_t max = 64 * 64 * 64 ;
const size_t threads_num = 14;
fixed_size_zero_bit_strategy<min_node_size , node_size, max> bit_alloc;
fixed_size_mpmc_queue_strategy<min_node_size , node_size, max> mpmc_alloc;

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
        bit_alloc.clear();
        state.ResumeTiming();
        for(size_t i = 0; i < max; i++) {
            void * v = bit_alloc.allocate(node_size);
            memcpy((void *)v, mas.c_str(), mas.length() + 1);
            bit_alloc.deallocate(v);
        }
    }
}

BENCHMARK(BM_MyAlloc)->Unit(benchmark::kMillisecond);


// Define another benchmark
static void BM_MpmcAlloc(benchmark::State& state) {
    while (state.KeepRunning()) {
        for(size_t i = 0; i < max; i++) {
            void * v = mpmc_alloc.allocate(node_size);
            memcpy((void *)v, mas.c_str(), mas.length() + 1);
            mpmc_alloc.deallocate(v);
        }
    }
}

BENCHMARK(BM_MpmcAlloc)->Unit(benchmark::kMillisecond);



static void BM_MultiThreadedMalloc(benchmark::State& state) {
    if (state.thread_index == 0) {
        // Setup code here.
    }
    while (state.KeepRunning()) {
        // Run the test as normal.
        for(size_t i = 0; i < max; i++) {
            volatile void * v;
            benchmark::DoNotOptimize(v = malloc(node_size));
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
            void * v = bit_alloc.allocate(node_size);            
            memcpy((void *)v, mas.c_str(), mas.length() + 1);
            bit_alloc.deallocate(v);
        }
        // Run the test as normal.
    }
    if (state.thread_index == 0) {
        // Teardown code here.
    }
}

BENCHMARK(BM_MultiThreadedMyAlloc)->Threads(threads_num)->Unit(benchmark::kMillisecond);



static void BM_MultiThreadedMyAlloc2(benchmark::State& state) {
    if (state.thread_index == 0) {
        // Setup code here.
    }
    while (state.KeepRunning()) {
        for(size_t i = 0; i < max; i++) {
            void * v = mpmc_alloc.allocate(node_size);
            memcpy((void *)v, mas.c_str(), mas.length() + 1);
            mpmc_alloc.deallocate(v);
        }
        // Run the test as normal.
    }
    if (state.thread_index == 0) {
        // Teardown code here.
    }
}

BENCHMARK(BM_MultiThreadedMyAlloc2)->Threads(threads_num)->Unit(benchmark::kMillisecond);

//BENCHMARK_MAIN()

int main(int argc, char** argv) {
    mpmc_alloc.init();
  ::benchmark::Initialize(&argc, argv);
  ::benchmark::RunSpecifiedBenchmarks();
}

#endif // _BENCHMARKS
