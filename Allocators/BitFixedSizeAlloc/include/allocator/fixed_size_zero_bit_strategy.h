#pragma once
#include <atomic>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

typedef uintptr_t uptr;
inline uint8_t get_lowest_zero_bit(uint8_t num) {
    for (uint8_t cnt = 0; cnt < 8; ++cnt) {
        if (!(num & (1U << cnt)))
            return cnt;
    }
    return 0;
}

uint16_t get_lowest_bit(uint8_t *bytes)
{
    const uint8_t m8 = static_cast<uint8_t>(-1);
    if (bytes[0] != m8)
        return get_lowest_zero_bit(bytes[0]);
    else if (bytes[1] != m8)
        return get_lowest_zero_bit(bytes[1]) + 8;
    else if (bytes[2] != m8)
        return get_lowest_zero_bit(bytes[2]) + 16;
    else if (bytes[3] != m8)
        return get_lowest_zero_bit(bytes[3]) + 24;
    else if (bytes[4] != m8)
        return get_lowest_zero_bit(bytes[4]) + 32;
    else if (bytes[5] != m8)
        return get_lowest_zero_bit(bytes[5]) + 40;
    else if (bytes[6] != m8)
        return get_lowest_zero_bit(bytes[6]) + 48;
    else if (bytes[7] != m8)
        return get_lowest_zero_bit(bytes[7]) + 56;
    return 0;
}

template < size_t min_handle_size,
           size_t max_handle_size,
           size_t max_total_size >
class FixedSizeZeroBitStrategy
{
public:
    FixedSizeZeroBitStrategy()
    {

    }

    ~FixedSizeZeroBitStrategy()
    {

    }



    void* allocate(std::size_t size) {
        const uint64_t m64 = static_cast<uint64_t>(-1);
        const uint32_t m32 = static_cast<uint32_t>(-1);
        const uint16_t m16 = static_cast<uint16_t>(-1);
        const uint8_t m8 = static_cast<uint8_t>(-1);
        const uint32_t size_in_array = 64;
        while (true) {
            if(curr_size == max_number) {
                //                std::cout << "array_pos " << curr_size << std::endl;
                return malloc(size);
            }
            else {
                uint64_t array_pos = 0;
big_circle:


                for( uint64_t lv3_num = 0; lv3_num < m_size3; ++lv3_num) {

                    if(free_num_lv3[lv3_num].load(std::memory_order_relaxed) != m64) {
                        uint64_t c64_lv3 = free_num_lv3[lv3_num].load(std::memory_order_relaxed);
                        uint64_t bit_lv3_pos = get_lowest_bit(reinterpret_cast<uint8_t *>(&c64_lv3));
                        bit_lv3_pos = get_lowest_bit(reinterpret_cast<uint8_t *>(&c64_lv3));
                        array_pos += bit_lv3_pos * size_in_array * size_in_array;

                        for( uint64_t lv2_num = (lv3_num * size_in_array) + bit_lv3_pos, max = lv2_num + size_in_array; lv2_num < max; ++lv2_num) {

                            if(free_num_lv2[lv2_num].load(std::memory_order_relaxed) != m64) {
                                uint64_t c64_lv2 = free_num_lv2[lv2_num].load(std::memory_order_relaxed);
                                uint64_t bit_lv2_pos = get_lowest_bit(reinterpret_cast<uint8_t *>(&c64_lv2));
                                array_pos += bit_lv2_pos * size_in_array;

                                for( uint64_t lv_num = (lv2_num * size_in_array) + bit_lv2_pos, max = lv_num + size_in_array; lv_num < max; ++lv_num) {

                                    if(free_num[lv_num].load(std::memory_order_relaxed) != m64) {
                                        uint64_t c64 = free_num[lv_num].load(std::memory_order_relaxed);
                                        uint32_t bit_pos = get_lowest_bit(reinterpret_cast<uint8_t *>(&c64));
                                        uint64_t c64t = c64 | ( 1ULL << bit_pos);

                                        if(!free_num[lv_num].compare_exchange_strong(c64, c64t))
                                            goto big_circle;
                                        array_pos += bit_pos;
                                        curr_size++; //TODO: optimaze this

                                        if(free_num[lv_num].load(std::memory_order_relaxed) == m64) {
                                            c64t = c64_lv2 | ( 1ULL << bit_lv2_pos);
                                            free_num_lv2[lv2_num].compare_exchange_strong(c64_lv2, c64t);
//                                            std::cout << "bum " << bit_lv2_pos << std::endl;

                                            if(free_num_lv2[lv2_num].load(std::memory_order_relaxed) == m64) {
                                                c64t = c64_lv3 | ( 1ULL << bit_lv3_pos);
                                                free_num_lv3[lv3_num].compare_exchange_strong(c64_lv3, c64t);
//                                                std::cout << "tadam " << array_pos << std::endl;
                                            }
                                        }
                                        goto end;
                                    }
                                }
                            }
                        }
                        lv3_num = 0;
                    }
                    else {
                        array_pos += size_in_array * size_in_array * size_in_array;
                    }
                }
end:
//                std::cout << "array_pos " << array_pos << std::endl;
                void * ptr = (void*)( reinterpret_cast<uptr>(start) + array_pos * max_handle_size);
                return ptr;
            }
        }
    }

    void deallocate(void* p) {
        if(p >= start && p <= end) {
            uint32_t num = (reinterpret_cast<uptr>(p) - reinterpret_cast<uptr>(start)) / max_handle_size;
            const uint32_t size_in_array = 64;

            uint32_t num_pos = num / size_in_array;
            uint32_t bit_pos = num % size_in_array;

            uint64_t c64 = free_num[num_pos].load(std::memory_order_relaxed);
            uint64_t c64t = c64 & ~( 1ULL << bit_pos);
            free_num[num_pos].compare_exchange_strong(c64, c64t, std::memory_order_relaxed);

            if(free_num[num_pos] == 0) {

                bit_pos = num_pos % size_in_array;
                num_pos /= size_in_array;

                c64 = free_num_lv2[num_pos].load(std::memory_order_relaxed);
                c64t = c64 & ~( 1ULL << bit_pos);
                free_num_lv2[num_pos].compare_exchange_strong(c64, c64t, std::memory_order_relaxed);

                if(free_num_lv2[num_pos] == 0) {

                    num_pos = num_pos / size_in_array;
                    bit_pos = num_pos % size_in_array;
                    c64 = free_num_lv3[num_pos].load(std::memory_order_relaxed);
                    c64t = c64 & ~( 1ULL << bit_pos);
                    free_num_lv3[num_pos].compare_exchange_strong(c64, c64t, std::memory_order_relaxed);
                }
            }
            curr_size--;
        }
        else {
            free(p);
        }
    }
    void clear() {
        memset(start, 0, max_size);
    }


    //protected:
    static const size_t max_size = max_handle_size * max_total_size;
    static uint8_t array[];
    void* start = array;
    void* end = &array[max_size - 1];

    static const size_t m_size = max_total_size;
    static const size_t m_size2 = m_size / 64;
    static const size_t m_size3 = m_size2 / 64;

    static std::atomic_uint_fast64_t free_num[];
    static std::atomic_uint_fast64_t free_num_lv2[];
    static std::atomic_uint_fast64_t free_num_lv3[];

    static std::atomic_uint_fast64_t curr_size;
    const uint64_t max_number = max_total_size;
};

template < size_t min_handle_size,
           size_t max_handle_size,
           size_t max_total_size >
uint8_t FixedSizeZeroBitStrategy<min_handle_size, max_handle_size,  max_total_size>::array[FixedSizeZeroBitStrategy<min_handle_size, max_handle_size,  max_total_size>::max_size];

template < size_t min_handle_size,
           size_t max_handle_size,
           size_t max_total_size >
std::atomic_uint_fast64_t FixedSizeZeroBitStrategy<min_handle_size, max_handle_size, max_total_size>::free_num[FixedSizeZeroBitStrategy<min_handle_size, max_handle_size, max_total_size>::m_size]{};// TODO: check it always 0

template < size_t min_handle_size,
           size_t max_handle_size,
           size_t max_total_size >
std::atomic_uint_fast64_t FixedSizeZeroBitStrategy<min_handle_size, max_handle_size, max_total_size>::free_num_lv2[FixedSizeZeroBitStrategy<min_handle_size, max_handle_size, max_total_size>::m_size2]{};// TODO: check it always 0


template < size_t min_handle_size,
           size_t max_handle_size,
           size_t max_total_size >
std::atomic_uint_fast64_t FixedSizeZeroBitStrategy<min_handle_size, max_handle_size, max_total_size>::free_num_lv3[FixedSizeZeroBitStrategy<min_handle_size, max_handle_size, max_total_size>::m_size3]{};// TODO: check it always 0

template < size_t min_handle_size,
           size_t max_handle_size,
           size_t max_total_size >
std::atomic_uint_fast64_t FixedSizeZeroBitStrategy<min_handle_size, max_handle_size, max_total_size>::curr_size(0);
