#ifdef _GTEST
#include <gtest/gtest.h>
#include <thread>
#include <allocator/fixed_size_mpmc_queue_strategy.h>

template <class Tp>
inline void do_not_optimize(Tp const& value) {
    asm volatile("" : : "g"(value) : "memory");
}

class mpmc_bounded_queue : public ::testing::Test
{
protected:
    static void SetUpTestCase() {

    }

    static void TearDownTestCase() {

    }

    void SetUp()
    {
        mpmc_alloc.init();
    }

    void TearDown()
    {
    }

    static const size_t min_node_size = 2;
    static const size_t node_size = 40;
    static const size_t max = 64 * 64 * 64 ;
    static const size_t threads_num = 14;
    static fixed_size_mpmc_queue_strategy<min_node_size , node_size, max> mpmc_alloc;

};

fixed_size_mpmc_queue_strategy<mpmc_bounded_queue::min_node_size , mpmc_bounded_queue::node_size, mpmc_bounded_queue::max> mpmc_bounded_queue::mpmc_alloc;

TEST_F(mpmc_bounded_queue, all_address_in_range_one_thread) {

    for(size_t i = 0; i < max; i++) {
        void * v = mpmc_alloc.allocate(node_size);
        EXPECT_TRUE(mpmc_alloc.begin <= v && v <= mpmc_alloc.end);
    }
}

TEST_F(mpmc_bounded_queue, all_address_in_range_eight_threads) {

    const size_t num = 8;
    std::atomic_bool start(false);
    std::vector<std::thread> threads;
    for (size_t n = 0; n < num; ++n) {
        threads.emplace_back([&]() {
            while(!start);
            for(size_t i = 0; i < max / num; i++) {
                void * v = mpmc_alloc.allocate(node_size);
                EXPECT_TRUE(mpmc_alloc.begin <= v && v <= mpmc_alloc.end);
            }
        });
    }
    start.store(true);

    for (auto &th : threads) {
        th.join();
    }
}


template < size_t min_handle_size,
           size_t max_handle_size,
           size_t max_numbers >
void multithreading_test(uint16_t threads_num)
{
    std::cout << std::endl << "node_size - " << max_handle_size << ", threads num - " << threads_num << std::endl;
    fixed_size_mpmc_queue_strategy<min_handle_size , max_handle_size, max_numbers> mpmc_alloc;

    typedef std::chrono::high_resolution_clock Clock;
    using std::chrono::duration_cast;

    const size_t th_num = threads_num;
    const size_t iteration_num = threads_num * 100;
    const size_t size = max_handle_size;
    std::atomic_bool start(false);
    std::vector<std::thread> threads;
    std::chrono::milliseconds ss;
    std::atomic<std::chrono::milliseconds> malloc_result;
    std::atomic<std::chrono::milliseconds> mpmc_alloc_result;
    malloc_result.store(ss.zero());
    mpmc_alloc_result.store(ss.zero());

    for (uint32_t n = 0; n < th_num; ++n) {
        threads.push_back(std::thread([n, &start, &malloc_result, &mpmc_alloc_result, &mpmc_alloc, th_num, iteration_num, size]() {
            for (uint16_t k = 0; k < iteration_num; ++k) {
                std::string mas("massive of chars");
                while(!start);
                auto start_time = Clock::now();
                for(size_t i = 0; i < max_numbers / th_num; i++) {
                    volatile void * v;
                    do_not_optimize( v = malloc(size));
                    do_not_optimize(memcpy((void *)v, mas.c_str(), mas.length() + 1));
                    free((void *)v);
                }
                auto end_time = Clock::now();
                malloc_result.store(malloc_result.load() + duration_cast<std::chrono::milliseconds>(end_time - start_time));

                start_time = Clock::now();
                for(size_t i = 0; i < max_numbers / th_num; i++) {
                    void * v = mpmc_alloc.allocate(size);
                    memcpy((void *)v, mas.c_str(), mas.length() + 1);
                    mpmc_alloc.deallocate(v);
                }
                end_time = Clock::now();
                mpmc_alloc_result.store(mpmc_alloc_result.load() + duration_cast<std::chrono::milliseconds>(end_time - start_time));
            }
        }));
    }
    start.store(true);

    for (auto &th : threads) {
        th.join();
    }

    std::cout << "malloc time: " << malloc_result.load().count() << '\n';
    std::cout << "mpmc alloc time: " << mpmc_alloc_result.load().count() << '\n';
    if (malloc_result.load() > mpmc_alloc_result.load())
        std::cout << "mpmc alloc is faster than alloc !!!" << std::endl;
    else if (malloc_result.load() == mpmc_alloc_result.load())
        std::cout << "mpmc alloc  time equal alloc  time." << std::endl;
    else
        std::cout << "mpmc alloc is slower than alloc (((" << std::endl;
}

TEST_F(mpmc_bounded_queue, check_concurrency_speed_vs_malloc_middle_size) {

    size_t max_threads = 4;
    multithreading_test<min_node_size , node_size, max>(max_threads);

//    mpmc_alloc.init();
    max_threads = 8;
    multithreading_test<min_node_size , node_size, max>(max_threads);

    max_threads = 12;
    multithreading_test<min_node_size , node_size, max>(max_threads);
}



TEST_F(mpmc_bounded_queue, check_concurrency_speed_vs_malloc_small_size) {

    const size_t min_node_size = 2;
    const size_t node_size = 20;
    const size_t max = 64 * 64 * 64 ;
    size_t max_threads = 4;
    multithreading_test<min_node_size , node_size, max>(max_threads);

    max_threads = 8;
    multithreading_test<min_node_size , node_size, max>(max_threads);

    max_threads = 12;
    multithreading_test<min_node_size , node_size, max>(max_threads);
}



TEST_F(mpmc_bounded_queue, check_concurrency_speed_vs_malloc_big_size) {

    const size_t min_node_size = 2;
    const size_t node_size = 400;
    const size_t max = 64 * 64 * 64 ;
    size_t max_threads = 4;
    multithreading_test<min_node_size , node_size, max>(max_threads);

    max_threads = 8;
    multithreading_test<min_node_size , node_size, max>(max_threads);

    max_threads = 12;
    multithreading_test<min_node_size , node_size, max>(max_threads);
}



TEST_F(mpmc_bounded_queue, check_concurrency_speed_vs_malloc_huge_size) {

    const size_t min_node_size = 2;
    const size_t node_size = 2000;
    const size_t max = 64 * 64 * 64 ;
    const size_t threads_num = 14;
    size_t max_threads = 4;
    multithreading_test<min_node_size , node_size, max>(max_threads);

    max_threads = 8;
    multithreading_test<min_node_size , node_size, max>(max_threads);

    max_threads = 12;
    multithreading_test<min_node_size , node_size, max>(max_threads);
}


#endif // _BENCHMARKS
