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

        // -----
        // valid
        // -----

        /**
         * O(1) in space
         * O(n) in time
         * <your documentation>
         */
        FRIEND_TEST(TestAllocator2, bytes_to_next_sentinel_1);
        FRIEND_TEST(TestAllocator2, bytes_to_next_sentinel_2);
        size_t bytes_to_next_sentinel (int current_sentinel) const 
        {
            // Increment bytes_read to the next sentinel
            if (current_sentinel < 0)
            {
                return (current_sentinel * -1) + 4;
            }
            return current_sentinel + 4;
        }

        bool valid () const
        {
            size_t bytes_read = 0;
            bool last_was_pos = (*this)[0] > 0 ? true : false;
            int current_sentinel = (*this)[0];

            // Sentinels cannot be 0
            if (current_sentinel == 0) return false;
            
            bytes_read += bytes_to_next_sentinel (current_sentinel);
            
            // If sentinel pairs do not match, return false
            if ((*this)[bytes_read] != current_sentinel)
            {
                return false;
            }

            // Increments bytes_read to the next sentinel
            bytes_read += sizeof(int);

            // Check the rest of the sentinels for validity
            while (bytes_read < N)
            {
                current_sentinel = (*this)[bytes_read];
                
                // Sentinels cannot be 0
                if (current_sentinel == 0) return false;
                
                // if new current sentinel not compatible with last was pos
                if ( (last_was_pos && current_sentinel > 0) ||
                     (!last_was_pos && current_sentinel < 0))
                {
                        return false;
                }
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

        /**
         * O(1) in space
         * O(1) in time
         * <your documentation>
         * https://code.google.com/p/googletest/wiki/
         *     AdvancedGuide#Private_Class_Members
         */
        FRIEND_TEST(TestAllocator2, index1); 
        FRIEND_TEST(TestAllocator2, index2);
        int& operator [] (int i)
        {
            return *reinterpret_cast<int*>(&a[i]);
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

        /* Helper method for allocate
         * Check if a given block should be allocated and/or split into two smaller blocks
         * Return pointer to allocated area if appropriate, null pointer otherwise
        */
        pointer allocate_if_possible(size_t bytes_read, int current_sentinel, size_t bytes_needed, size_t n){
            //if enough space here, return
            if (current_sentinel >= bytes_needed && current_sentinel > 0)
            {
                int remaining_space = current_sentinel + (2 * sizeof(int)) - (2 * sizeof(int)) + (n * sizeof(T));
               
                // If there is enough space to allocate n Ts AND another "smallest allowable block":
                if (remaining_space >= (2 * sizeof(int) + sizeof(T)))
                {
                    // Create pointer to beginning of allocated space
                    bytes_read += 4;
                    pointer p = reinterpret_cast<const T*>(a + bytes_read);
                    // Modify first sentinel to new value
                    a[bytes_read - sizeof(int)] = bytes_needed * -1;
                    // Go to second sentinel
                    bytes_read += bytes_needed;
                    // Create an end sentinel
                    a[bytes_read] = bytes_needed * -1;
                    // Go to first in second group of sentinels
                    bytes_read += 4;
                    // Create first sentinel in second group
                    a[bytes_read] = remaining_space - 2 * sizeof(int);
                    // Create second sentinel in second group
                    bytes_read += remaining_space - 4;
                    a[bytes_read] = remaining_space - 2 * sizeof(int);
                    return p;
                }
                // If there's not enough space for another T and 2 sentinels, just fill in this block
                else
                {
                    pointer p = reinterpret_cast<const T*>(a + bytes_read + 4);
                    return p;
                }
            }
            // Return null pointer if this block was not suitable
            return nullptr;
        }

        // --------
        // allocate
        // --------

        /**
         * O(1) in space
         * O(n) in time
         * after allocation there must be enough space left for a valid block
         * the smallest allowable block is sizeof(T) + (2 * sizeof(int))
         * choose the first block that fits
         * throw a bad_alloc exception, if n is invalid
         */
        pointer allocate (size_type n)
        {
            assert (n > 0);

            size_t bytes_read = 0;
            int current_sentinel = (*this)[0];
            size_t bytes_needed = n * sizeof(T);

            pointer p = allocate_if_possible(bytes_read, current_sentinel, bytes_needed, n);
            if (p != NULL) 
            {
                return p;
            }

            bytes_read += bytes_to_next_sentinel (current_sentinel) + sizeof(int);

            // Check the rest of the sentinels for possible allocation
            while (bytes_read < N)
            {
                current_sentinel = (*this)[bytes_read];
                pointer p = allocate_if_possible(bytes_read, current_sentinel, bytes_needed, n);
                
                if (p != NULL)
                {
                    return p;
                }

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

        /**
         * O(1) in space
         * O(1) in time
         * after deallocation adjacent free blocks must be coalesced
         * throw an invalid_argument exception, if p is invalid
         * <your documentation>
         */
        void deallocate (pointer p, size_type)
        {
            // <your code>
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
         * <your documentation>
         */
        const int& operator [] (int i) const
        {
            return *reinterpret_cast<const int*>(&a[i]);
        }
    };

#endif // Allocator_h
