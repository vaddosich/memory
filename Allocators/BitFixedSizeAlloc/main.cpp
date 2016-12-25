#ifndef _BENCHMARKS
#include <set_bit_alloc.h>
#include <vector>
#include <iostream>
#include <thread>

int main()
{

    typedef std::chrono::high_resolution_clock Clock;
    using std::chrono::duration_cast;
    const size_t min_node_size = 2;
    const size_t node_size = 40;
    const size_t max = 64 * 64  * 64;
    FixedSizeZeroBitStrategy<min_node_size , node_size, max> myalloc;
    volatile void * malloc_allocs[max];
    void * alloc_allocs[max];
    myalloc.clear();

    std::cout << "allocate size : " << max << std::endl;

    std::string mas("massive of string");

    auto start = Clock::now();

    for(size_t i = 0; i < max; i++) {
        void * v = myalloc.allocate(node_size);
        memcpy(v, mas.c_str(), mas.length() + 1);
        alloc_allocs[i] = v;
    }

    for(size_t i = 0; i < max; i++)
        myalloc.deallocate(alloc_allocs[i]);

    auto end = Clock::now();

    auto elapsed = duration_cast<std::chrono::nanoseconds>(end - start);

    std::cout << "My alloc : " << elapsed.count() << '\n';

    start = Clock::now();

    for(size_t i = 0; i < max; i++) {
        volatile void * v = malloc(node_size);
        memcpy((void *)v, mas.c_str(), mas.length() + 1);
        malloc_allocs[i] = v;
    }

    for(size_t i = 0; i < max; i++)
        free((void *)malloc_allocs[i]);

    end = Clock::now();

    elapsed = duration_cast<std::chrono::nanoseconds>(end - start);

    std::cout << "Malloc: " << elapsed.count() << '\n';

    std::vector<void *> vec;
    size_t k = 0;
    for(size_t i = 0; i < max; i++) {
        void * v = myalloc.allocate(node_size);
        if(i % 3)
            alloc_allocs[k++] = v;
    }

    for(size_t i = 0; i < 10; i++) {
        myalloc.deallocate(alloc_allocs[i]);
        vec.pop_back();
    }

    for(size_t i = 0; i < 10; i++) {
        alloc_allocs[i] = myalloc.allocate(node_size);
    }
    return 0;
}

#endif // _BENCHMARKS
