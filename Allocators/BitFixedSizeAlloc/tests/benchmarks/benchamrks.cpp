#ifdef _BENCHMARKS
#include <benchmark/benchmark.h>

std::string mas("massive of string");
const size_t min_node_size = 2;
const size_t node_size = 40;
//const size_t max = 64 * 64 * 64 ;
const size_t max = 64 * 64 * 65 ;
StaticSizeAllocator<min_node_size , node_size, max> myalloc;

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
BENCHMARK(BM_Malloc);

// Define another benchmark
static void BM_MyAlloc(benchmark::State& state) {
    //    myalloc.clear();
    while (state.KeepRunning()) {
                for(size_t i = 0; i < max; i++) {
        void * v = myalloc.allocate(19);
        memcpy((void *)v, mas.c_str(), mas.length() + 1);
        myalloc.deallocate(v);
                }
    }
}
BENCHMARK(BM_MyAlloc);

BENCHMARK_MAIN()

#endif // _BENCHMARKS
