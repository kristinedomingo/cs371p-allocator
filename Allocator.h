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

        bool valid () const
        {
            size_t bytes_read = 0;
            bool last_was_pos = (*this)[0] > 0 ? true : false;
            int current_sentinel = (*this)[0];

            // Sentinels cannot be 0
            if (current_sentinel == 0) return false;
            
            // Increment bytes_read to the next sentinel
            if (current_sentinel < 0)
            {
                bytes_read += (current_sentinel * -1) + 4;
            }
            else
            {
                bytes_read += current_sentinel + 4;
            }
            
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
                // Increment bytes_read to the next sentinel
                if (current_sentinel < 0)
                {
                    bytes_read += (current_sentinel * -1) + 4;
                }
                else
                {
                    bytes_read += current_sentinel + 4;
                }

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
            // <your code>
            assert(valid());
            return nullptr; // replace
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
