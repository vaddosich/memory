#ifndef FIXED_SIZE_ZERO_BIT_STRATEGY
#define FIXED_SIZE_ZERO_BIT_STRATEGY

#include <atomic>
#include <bit_manip/bit_manip.h>
#include <stddef.h>
#include <string.h>
#include <iostream>

typedef uintptr_t uptr;

template < size_t min_handle_size,
           size_t max_handle_size,
           size_t max_numbers >
class fixed_size_zero_bit_strategy
{
public:
    fixed_size_zero_bit_strategy()
    {

    }

    ~fixed_size_zero_bit_strategy()
    {

    }



    inline void* allocate(std::size_t size) {

        while (true) {
start:
            if(curr_size == max_number) {
#ifdef _DEBUG
                std::cout << "current size -  " << curr_size << std::endl;
#endif
                return malloc(size);
            }
            else {
                uint64_t array_pos = 0;

                for( uint32_t count_lv3 = 0; count_lv3 < max_numbers_lv3;
                     ++count_lv3, array_pos += size_in_array * size_in_array * size_in_array) {

                    uint64_t expected_num_lv3 = free_num_lv3[count_lv3].load(std::memory_order_relaxed);
                    if(expected_num_lv3 != all_bits_are_used) {

                        uint64_t bit_pos_lv3 = get_lowest_bit(expected_num_lv3);
                        array_pos += bit_pos_lv3 * size_in_array * size_in_array;

                        for( uint32_t lv2_num = (count_lv3 * size_in_array) + bit_pos_lv3, max = lv2_num + size_in_array; lv2_num < max; ++lv2_num) {

                            uint64_t expected_num_lv2 = free_num_lv2[lv2_num].load(std::memory_order_relaxed);
                            if(expected_num_lv2 != all_bits_are_used) {
                                uint32_t bit_pos_lv2 = get_lowest_bit(expected_num_lv2);
                                array_pos += bit_pos_lv2 * size_in_array;

                                for( uint32_t lv1_num = (lv2_num * size_in_array) + bit_pos_lv2, max = lv1_num + size_in_array; lv1_num < max; ++lv1_num) {
                                    uint64_t expected_num_lv1 = free_num_lv1[lv1_num].load(std::memory_order_relaxed);
                                    if(expected_num_lv1 != all_bits_are_used) {
                                        uint32_t bit_pos_lv1 = get_lowest_bit(expected_num_lv1);
                                        uint64_t set_num = expected_num_lv1 | ( 1ULL << bit_pos_lv1);

                                        if(!free_num_lv1[lv1_num].compare_exchange_strong(expected_num_lv1, set_num)) {
                                            if(free_num_lv1[lv1_num].load(std::memory_order_relaxed) == all_bits_are_used)
                                                goto start;
                                            continue;
                                        }
                                        array_pos += bit_pos_lv1;
                                        curr_size++;

                                        if(free_num_lv1[lv1_num].load(std::memory_order_relaxed) == all_bits_are_used) {
                                            set_num = expected_num_lv2 | ( 1ULL << bit_pos_lv2);
                                            free_num_lv2[lv2_num].compare_exchange_weak(expected_num_lv2, set_num, std::memory_order_relaxed);

                                            if(free_num_lv2[lv2_num].load(std::memory_order_relaxed) == all_bits_are_used) {
                                                set_num = expected_num_lv3 | ( 1ULL << bit_pos_lv3);
                                                free_num_lv3[count_lv3].compare_exchange_weak(expected_num_lv3, set_num, std::memory_order_relaxed);
                                            }
                                        }
                                        return get_address_in_array(array_pos);
                                    }
                                }
                                goto start;
                            }
                        }
                        goto start;
                    }
                }
                goto start;
            }
        }
    }

    void deallocate(void* p) {
        if(p >= begin && p <= end) {
            uint32_t array_pos = (reinterpret_cast<uptr>(p) - reinterpret_cast<uptr>(begin)) / max_handle_size;

#ifdef _DEBUG
        std::cout << "deallocate array_pos - " << array_pos << " , curr_size - " << curr_size << std::endl;
#endif
            uint32_t num_pos = array_pos / size_in_array;
            uint32_t bit_pos = array_pos % size_in_array;
            free_num_lv1[num_pos] &= ~( 1ULL << bit_pos);

#ifdef _DEBUG
        std::cout << "deallocate lv1. pos - " << num_pos << " , bit_pos - " << bit_pos << std::endl;
#endif
            if(free_num_lv1[num_pos].load(std::memory_order_relaxed) == all_bits_are_empty) {

                bit_pos = num_pos % size_in_array;
                num_pos /= size_in_array;
                free_num_lv2[num_pos] &= ~( 1ULL << bit_pos);

#ifdef _DEBUG
        std::cout << "deallocate lv2. pos - " << num_pos << " , bit_pos - " << bit_pos << std::endl;
#endif
                if(free_num_lv2[num_pos].load(std::memory_order_relaxed) == all_bits_are_empty) {

                    bit_pos = num_pos % size_in_array;
                    num_pos /= size_in_array;
                    free_num_lv3[num_pos] &= ~( 1ULL << bit_pos);
#ifdef _DEBUG
        std::cout << "deallocate lv2. pos - " << num_pos << " , bit_pos - " << bit_pos << std::endl;
#endif
                }
            }
            curr_size--;
        }
        else {
            free(p);
        }
    }

    void clear() {
        curr_size = 0;
    }


    void clear_all() {
        clear();
        memset(begin, 0, array_size);
    }

private:

    inline void* get_address_in_array(uint64_t array_pos)
    {
#ifdef _DEBUG
        std::cout << "allocate array_pos - " << array_pos  << " , curr_size - " << curr_size << std::endl;
#endif
        void * ptr = (void*)( reinterpret_cast<uptr>(begin) + array_pos * max_handle_size);
        return ptr;
    }

    static const size_t array_size = max_handle_size * max_numbers;
    static uint8_t array[];
    static void* begin;
    static void* end;

    static const uint64_t all_bits_are_used = static_cast<uint64_t>(-1);
    static const uint64_t all_bits_are_empty = 0;
    static const uint32_t size_in_array = 64;

    static const size_t max_numbers_lv1 = max_numbers  / size_in_array + (max_numbers % size_in_array  ? 1 : 0 );
    static const size_t max_numbers_lv2 = max_numbers_lv1 / size_in_array + (max_numbers_lv1 % size_in_array  ? 1 : 0 );
    static const size_t max_numbers_lv3 = max_numbers_lv2 / size_in_array + (max_numbers_lv2 % size_in_array  ? 1 : 0 );

    static std::atomic_uint_fast64_t free_num_lv1[];
    static std::atomic_uint_fast64_t free_num_lv2[];
    static std::atomic_uint_fast64_t free_num_lv3[];

    static std::atomic_uint_fast64_t curr_size;
    const uint64_t max_number = max_numbers;
};

template < size_t min_handle_size,
           size_t max_handle_size,
           size_t max_numbers >
uint8_t fixed_size_zero_bit_strategy<min_handle_size, max_handle_size,  max_numbers>::array[fixed_size_zero_bit_strategy<min_handle_size, max_handle_size,  max_numbers>::array_size];

template < size_t min_handle_size,
           size_t max_handle_size,
           size_t max_numbers >
void *fixed_size_zero_bit_strategy<min_handle_size, max_handle_size,  max_numbers>::begin = fixed_size_zero_bit_strategy<min_handle_size, max_handle_size,  max_numbers>::array;

template < size_t min_handle_size,
           size_t max_handle_size,
           size_t max_numbers >
void *fixed_size_zero_bit_strategy<min_handle_size, max_handle_size,  max_numbers>::end = &fixed_size_zero_bit_strategy<min_handle_size, max_handle_size,  max_numbers>::array[array_size - 1];


template < size_t min_handle_size,
           size_t max_handle_size,
           size_t max_numbers >
std::atomic_uint_fast64_t fixed_size_zero_bit_strategy<min_handle_size, max_handle_size, max_numbers>::free_num_lv1[fixed_size_zero_bit_strategy<min_handle_size, max_handle_size, max_numbers>::max_numbers_lv1]{};// TODO: check it always 0

template < size_t min_handle_size,
           size_t max_handle_size,
           size_t max_numbers >
std::atomic_uint_fast64_t fixed_size_zero_bit_strategy<min_handle_size, max_handle_size, max_numbers>::free_num_lv2[fixed_size_zero_bit_strategy<min_handle_size, max_handle_size, max_numbers>::max_numbers_lv2]{};// TODO: check it always 0


template < size_t min_handle_size,
           size_t max_handle_size,
           size_t max_numbers >
std::atomic_uint_fast64_t fixed_size_zero_bit_strategy<min_handle_size, max_handle_size, max_numbers>::free_num_lv3[fixed_size_zero_bit_strategy<min_handle_size, max_handle_size, max_numbers>::max_numbers_lv3]{};// TODO: check it always 0

template < size_t min_handle_size,
           size_t max_handle_size,
           size_t max_numbers >
std::atomic_uint_fast64_t fixed_size_zero_bit_strategy<min_handle_size, max_handle_size, max_numbers>::curr_size(0);


#endif // FIXED_SIZE_ZERO_BIT_STRATEGY
