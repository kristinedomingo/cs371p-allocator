/** @file Allocator.h
 * @brief Contains an implementation of a heap memory allocator
 */

// ------------------------------
// projects/allocator/Allocator.h
// Copyright (C) 2015
// Glenn P. Downing
// ------------------------------

#ifndef Allocator_h
#define Allocator_h

// --------
// includes
// --------

#include <cassert>   // assert
#include <cstddef>   // ptrdiff_t, size_t
#include <new>       // bad_alloc, new
#include <stdexcept> // invalid_argument

// ---------
// Allocator
// ---------

template <typename T, std::size_t N>
class Allocator
{
    public:
        // --------
        // typedefs
        // --------

        typedef T                 value_type;

        typedef std::size_t       size_type;
        typedef std::ptrdiff_t    difference_type;

        typedef       value_type*       pointer;
        typedef const value_type* const_pointer;

        typedef       value_type&       reference;
        typedef const value_type& const_reference;

    public:
        // -----------
        // operator ==
        // -----------

        friend bool operator == (const Allocator&, const Allocator&)
        {
            return true; // this is correct
        }

        // -----------
        // operator !=
        // -----------

        friend bool operator != (const Allocator& lhs, const Allocator& rhs)
        {
            return !(lhs == rhs);
        }

    private:
        // ----
        // data
        // ----

        char a[N];

        // ----------------------
        // bytes_to_next_sentinel
        // ----------------------

        /**
         * O(1) in space
         * O(1) in time
         * A helper function for valid() and allocate(). Returns the absolute
         * value of current_sentinel plus the size of an integer (4), which is
         * used to move an index from a beginning sentinel of a block to the
         * end sentinel.
         * @param current_sentinel an integer representing a sentinel value
         * @return a size_t of |current_sentinel| + 4
         */
        FRIEND_TEST(TestBytesToNextSentinel, bytes_to_next_sentinel_1);
        FRIEND_TEST(TestBytesToNextSentinel, bytes_to_next_sentinel_2);
        size_t bytes_to_next_sentinel (int current_sentinel) const 
        {
            if (current_sentinel < 0)
            {
                return (current_sentinel * -1) + sizeof(int);
            }
            return current_sentinel + sizeof(int);
        }

        // -----
        // valid
        // -----

        /**
         * O(1) in space
         * O(n) in time
         * Class invariant used to check whether a given Allocator is in a valid
         * state after construction or a call to allocate or deallocate.
         * Checks that sentinel pairs match, there are not uncoalesced blocks.
         * @return a bool value representing whether or not a[] is a valid heap
         */
        bool valid () const
        {

            // To avoid dealing with checking the validity of the first pair
            // of sentinels (a special case) check the first pair outside
            // of the while loop, and then check the rest of sentinel pairs.
            size_t bytes_read = 0;
            bool last_was_pos = (*this)[0] > 0 ? true : false;
            int current_sentinel = (*this)[0];

            // Sentinels cannot be 0
            if (current_sentinel == 0)
            {
                return false;
            }
            
            // Increment bytes_read to the next sentinel
            bytes_read += bytes_to_next_sentinel(current_sentinel);
            
            // If sentinel pairs do not match, return false
            if ((*this)[bytes_read] != current_sentinel)
            {
                return false;
            }

            // Increment bytes_read to the next sentinel
            bytes_read += sizeof(int);

            // Check the rest of the sentinels for validity
            while (bytes_read < N)
            {
                current_sentinel = (*this)[bytes_read];
            
                // Sentinels cannot be 0
                if (current_sentinel == 0)
                {
                    return false;
                }
                
                // There cannot be two free blocks beside each other
                if (last_was_pos && current_sentinel > 0)
                {
                    return false;
                }

                // Update last_was_pos to the current sentinel
                last_was_pos = (*this)[bytes_read] > 0 ? true : false;

                // Increment bytes_read to the next sentinel
                bytes_read += bytes_to_next_sentinel (current_sentinel);

                // If sentinel pairs do not match, return false
                if ((*this)[bytes_read] != current_sentinel)
                {
                    return false;
                }

                // Increments bytes_read to the next sentinel
                bytes_read += sizeof(int);
            }
            return true;
        }

        // -----------
        // operator []
        // -----------

        /**
         * O(1) in space
         * O(1) in time
         * Read a sentinel from a[], cast value to an int as a[] is a char
         * array, and dereference it
         * https://code.google.com/p/googletest/wiki/
         *     AdvancedGuide#Private_Class_Members
         */
        FRIEND_TEST(TestIndexOperator, index_1); 
        FRIEND_TEST(TestIndexOperator, index_2);
        int& operator [] (int i)
        {
            return *reinterpret_cast<int*>(&a[i]);
        }

        /**
         * Helper method for allocate that checks if a given block should be 
         * allocated and/or split into two smaller blocks. Returns a pointer to 
         * the allocated area if appropriate, and a null pointer otherwise.
         * @param bytes_read the number of bytes read in a[] at this point
         * @param current_sentinel the value of the block's sentinel that this
         *                         function is trying to allocate room in
         * @param bytes_needed the number of bytes needed for allocation
         * @param n the number of Ts to allocate
         */
        pointer allocate_if_possible(size_t bytes_read, int current_sentinel, 
                                     int bytes_needed, size_t n)
        {            
            // If there is enough space in this block, allocate:
            if (current_sentinel >= bytes_needed && current_sentinel > 0)
            {
                // Calculate the remaining space there would be in this block if
                // n Ts have been allocated
                int remaining_space = current_sentinel + (2 * sizeof(int)) - 
                                      ((2 * sizeof(int)) + (n * sizeof(T)));

                // If there is enough space to allocate n Ts AND another block
                // that is AT LEAST bigger than the "smallest allowable block",
                // create two more sentinels to designate the latter
                if (remaining_space >= (2 * sizeof(int) + sizeof(T)))
                {
                    // Create pointer to beginning of allocated space
                    bytes_read += sizeof(int);
                    pointer p = reinterpret_cast<pointer>(&a[bytes_read]);

                    // Modify first sentinel to new value
                    (*this)[bytes_read - sizeof(int)] = bytes_needed * -1;

                    // Go to second sentinel
                    bytes_read += bytes_needed;
                    
                    // Create an end sentinel
                    (*this)[bytes_read] = bytes_needed * -1;
                   
                    // Go to first in second group of sentinels
                    bytes_read += sizeof(int);
                    
                    // Create first sentinel in second group
                    (*this)[bytes_read] = remaining_space - 2 * sizeof(int);
                    
                    // Create second sentinel in second group
                    bytes_read += remaining_space - sizeof(int);
                    
                    (*this)[bytes_read] = remaining_space - 2 * sizeof(int);
                    
                    assert(valid());
                    return p;
                }
                // If there's not enough space for another T and 2 sentinels, 
                // just fill in this block
                else
                {
                    pointer p = reinterpret_cast<pointer>(&a[bytes_read + sizeof(int)]);

                    // Mark current pair of sentinels as "used"
                    (*this)[bytes_read] *= -1;
                    (*this)[bytes_read + current_sentinel + sizeof(int)] *= -1;

                    assert(valid());
                    return p;
                }
            }

            // Return null pointer if this block was not suitable
            return nullptr;
        }

    public:
        // ------------
        // constructors
        // ------------

        /**
         * O(1) in space
         * O(1) in time
         * throw a bad_alloc exception, if N is less than sizeof(T) + 
         * (2 * sizeof(int))
         */
        Allocator ()
        {
            size_t sentinels_size = 2 * sizeof(int);

            if (N < sizeof(T) + sentinels_size)
            {
                std::bad_alloc exception;
                throw exception;
            }

            // Set sentinels
            (*this)[0] = N - sentinels_size;
            (*this)[N - sizeof(int)] = N - sentinels_size;

            assert(valid());
        }

        // Default copy, destructor, and copy assignment
        // Allocator  (const Allocator&);
        // ~Allocator ();
        // Allocator& operator = (const Allocator&);

        // --------
        // allocate
        // --------

        FRIEND_TEST(TestAllocate, allocate_1);
        FRIEND_TEST(TestAllocate, allocate_2);
        FRIEND_TEST(TestAllocate, allocate_3);
        FRIEND_TEST(TestAllocate, allocate_4);
        FRIEND_TEST(TestAllocate, allocate_5);
        FRIEND_TEST(TestAllocate, allocate_6);
        FRIEND_TEST(TestAllocate, allocate_7);

        /**
         * O(1) in space
         * O(n) in time
         * Allocates at least enough space for n T's when requested
         * After allocation there must be enough space left for a valid block
         * the smallest allowable block is sizeof(T) + (2 * sizeof(int))
         * choose the first block that fits
         * Throw a bad_alloc exception, if n is invalid
         */
        pointer allocate (const size_type& n)
        {
            assert (n > 0);

            // Check if n Ts can be allocated in the first block
            size_t bytes_read = 0;
            int current_sentinel = (*this)[0];
            size_t bytes_needed = n * sizeof(T);
            pointer p = allocate_if_possible(bytes_read, current_sentinel, bytes_needed, n);

            if (p != NULL)
            {
                return p;
            }

            // If n Ts cannot be allocated in the first block, move bytes_read
            // to the next pair of sentinels
            bytes_read += bytes_to_next_sentinel (current_sentinel) + sizeof(int);

            // Then, check the rest of the sentinels for possible allocation
            while (bytes_read < N)
            {
                current_sentinel = (*this)[bytes_read];
                pointer p = allocate_if_possible(bytes_read, current_sentinel, bytes_needed, n);
                
                if (p != NULL)
                {
                    return p;
                }

                // If n Ts cannot be allocated in the first block, move
                // bytes_read to the next pair of sentinels
                bytes_read += bytes_to_next_sentinel (current_sentinel) + sizeof(int);
            }

            // If there is no more space, throw bad_alloc
            std::bad_alloc e;
            throw e;

            assert(valid());
            return nullptr; // Will never reach here
        }

        // ---------
        // construct
        // ---------

        /**
         * O(1) in space
         * O(1) in time
         */
        void construct (pointer p, const_reference v)
        {
            new (p) T(v);    // this is correct and exempt
            assert(valid()); // from the prohibition of new
        }

        // ----------
        // deallocate
        // ----------

        FRIEND_TEST(TestDeallocate, deallocate_1);
        FRIEND_TEST(TestDeallocate, deallocate_2);
        FRIEND_TEST(TestDeallocate, deallocate_3);
        FRIEND_TEST(TestDeallocate, deallocate_4);
        FRIEND_TEST(TestDeallocate, deallocate_5);
        FRIEND_TEST(TestDeallocate, deallocate_6);
        
        /**
         * O(1) in space
         * O(1) in time
         * Takes in pointer to previously allocated area,
         * deallocates the area
         * After deallocation adjacent free blocks must be coalesced
         * throw an invalid_argument exception, if p is invalid
         * 
         */
        void deallocate (pointer p, size_type)
        {
            if (p == 0)
            {
                throw std::invalid_argument("Invalid p pointer");
            }

            // Get the value of the sentinel of the block that p is pointing to
            int* sentinel_pointer = reinterpret_cast<int*>(p) - 1;
            int sentinel_value = *(sentinel_pointer);

            // Find the beginning and end of a
            int* beginning_of_a = reinterpret_cast<int*>(&a);
            int* end_of_a = reinterpret_cast<int*>(&a[sizeof(a) / sizeof(a[0])]);

            // Find the last valid location that a sentinel can be at
            char* end_minus_block = reinterpret_cast<char*>(end_of_a) - 
                                        (2 * sizeof(int) - sizeof(T));
            int* last_valid_location = reinterpret_cast<int*>(end_minus_block);

            // Make sentinel_value positive
            if(sentinel_value < 0)
            {
                sentinel_value *= -1;
            }

            // Mark the "beginning sentinel" in this block
            int* first_sentinel = sentinel_pointer;
            char* reader = reinterpret_cast<char*>(first_sentinel);

            // Mark the "end sentinel" in this block
            char* second_reader = reinterpret_cast<char*>(first_sentinel) +
                                  sentinel_value + sizeof(int);
            int* end_sentinel = reinterpret_cast<int*>(second_reader);

            // Check for validity before you begin actual deallocation
            if(*(end_sentinel) > 0 ||
               (*(end_sentinel) * -1) != sentinel_value ||
               reinterpret_cast<int*>(p) < beginning_of_a ||
               reinterpret_cast<int*>(p) > last_valid_location)
            {
                throw std::invalid_argument("Invalid p pointer");
            }

            // Check for free blocks BEFORE this block
            if(first_sentinel - 1 > beginning_of_a)
            {
                // Check if previous block is free
                int previous_sentinel_value = *(reinterpret_cast<int*>(reader - sizeof(int)));
                if(previous_sentinel_value > 0)
                {

                    // Set the first_sentinel to the previous block's first sentinel
                    first_sentinel = reinterpret_cast<int*>(reader - previous_sentinel_value - (2 * sizeof(int)));

                    // Accumulate sentinel value
                    sentinel_value += previous_sentinel_value + (2 * sizeof(int));
                }
            }

            // Check for free blocks AFTER this block
            if(end_sentinel + 1 < end_of_a)
            {
                // Check if next block is free
                int next_sentinel_value = *(reinterpret_cast<int*>(second_reader + sizeof(int)));

                if(next_sentinel_value > 0)
                {
                    // Set the end_sentinel to the next block's end sentinel
                    end_sentinel = reinterpret_cast<int*>(second_reader + next_sentinel_value + (2 * sizeof(int)));

                    // Accumulate sentinel value
                    sentinel_value += next_sentinel_value + (2 * sizeof(int));
                }
            }


            // Set the values of the new sentinels
            *(first_sentinel) = sentinel_value;
            *(end_sentinel) = sentinel_value;

            assert(valid());
        }

        // -------
        // destroy
        // -------

        /**
         * O(1) in space
         * O(1) in time
         */
        void destroy (pointer p)
        {
            p->~T(); // this is correct
            assert(valid());
        }

        /**
         * O(1) in space
         * O(1) in time
         * Read a sentinel from a[], cast value to an int as a[] is a char
         * array, and dereference it
         * Returns a read only reference so it can be used safely as a public
         * method
         */
        const int& operator [] (int i) const
        {
            return *reinterpret_cast<const int*>(&a[i]);
        }
    };

#endif // Allocator_h
