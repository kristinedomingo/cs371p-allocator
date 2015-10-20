// ------------------------------------
// projects/allocator/TestAllocator1.c++
// Copyright (C) 2015
// Glenn P. Downing
// ------------------------------------

// --------
// includes
// --------

#include <algorithm> // count
#include <memory>    // allocator

#include "gtest/gtest.h"

#include "Allocator.h"

// --------------
// TestAllocator1
// --------------

template <typename A>
struct TestAllocator1 : testing::Test
{
    // --------
    // typedefs
    // --------

    typedef          A                  allocator_type;
    typedef typename A::value_type      value_type;
    typedef typename A::difference_type difference_type;
    typedef typename A::pointer         pointer;
};

typedef testing::Types<
            std::allocator<int>,
            std::allocator<double>,
            Allocator<int,    100>,
            Allocator<double, 100> >
        my_types_1;

TYPED_TEST_CASE(TestAllocator1, my_types_1);

TYPED_TEST(TestAllocator1, test_1)
{
    typedef typename TestFixture::allocator_type  allocator_type;
    typedef typename TestFixture::value_type      value_type;
    typedef typename TestFixture::difference_type difference_type;
    typedef typename TestFixture::pointer         pointer;

          allocator_type  x;
    const difference_type s = 1;
    const value_type      v = 2;
    const pointer         p = x.allocate(s);
    if (p != nullptr)
    {
        x.construct(p, v);
        ASSERT_EQ(v, *p);
        x.destroy(p);
        x.deallocate(p, s);
    }
}

TYPED_TEST(TestAllocator1, test_10)
{
    typedef typename TestFixture::allocator_type  allocator_type;
    typedef typename TestFixture::value_type      value_type;
    typedef typename TestFixture::difference_type difference_type;
    typedef typename TestFixture::pointer         pointer;

          allocator_type  x;
    const difference_type s = 10;
    const value_type      v = 2;
    const pointer         b = x.allocate(s);
    if (b != nullptr)
    {
        pointer e = b + s;
        pointer p = b;
        try
        {
            while (p != e)
            {
                x.construct(p, v);
                ++p;
            }
        }
        catch (...)
        {
            while (b != p)
            {
                --p;
                x.destroy(p);
            }
            x.deallocate(b, s);
            throw;
        }
        ASSERT_EQ(s, std::count(b, e, v));
        while (b != e)
        {
            --e;
            x.destroy(e);
        }
        x.deallocate(b, s);
    }
}

// --------------
// Index operator
// --------------

/**
 * Tests const indexing operator
 * @param TestIndexOperator a fixture
 * @param const_index test name
 */
TEST(TestIndexOperator, const_index)
{
    const Allocator<int, 100> x;
    ASSERT_EQ(x[0], 92);
}

/**
 * Tests indexing operator
 * @param TestIndexOperator a fixture
 * @param index_1 test name
 */
TEST(TestIndexOperator, index_1)
{
    Allocator<int, 100> x;
    ASSERT_EQ(x[0], 92);
}

/**
 * Tests indexing operator
 * @param TestIndexOperator a fixture
 * @param index2 test name
 */
TEST(TestIndexOperator, index_2)
{
    Allocator<int, 12> x;
    ASSERT_EQ(x[0], 4);
}

// -----------
// Constructor
// -----------

/**
 * Tests that constructor throws bad_alloc
 * @param TestConstructor a fixture
 * @param exception_is_thrown_1 test name
 */
TEST(TestConstructor, exception_is_thrown_1)
{
    bool exception_thrown = false;
    try
    {
        Allocator<int, 1> x;
    }
    catch(std::bad_alloc& e)
    {
        exception_thrown = true;
    }
    ASSERT_TRUE(exception_thrown);
}

/**
 * Tests that constructor throws bad_alloc
 * @param TestConstructor a fixture
 * @param exception_is_thrown2 test name
 */
TEST(TestConstructor, exception_is_thrown2)
{
    bool exception_thrown = false;
    try
    {
        Allocator<int, 12> x;
    }
    catch(std::bad_alloc& e)
    {
        exception_thrown = true;
    }
    ASSERT_FALSE(exception_thrown);
}

/**
 * Tests that constructor throws bad_alloc
 * @param TestConstructor a fixture
 * @param exception_is_thrown3 test name
 */
TEST(TestConstructor, exception_is_thrown3)
{
    bool exception_thrown = false;
    try
    {
        Allocator<int, 11> x;
    }
    catch(std::bad_alloc& e)
    {
        exception_thrown = true;
    }
    ASSERT_TRUE(exception_thrown);
}

// ----------------------
// bytes_to_next_sentinel
// ----------------------

/**
 * Tests helper function that moves to index of next sentinel
 * @param TestBytesToNextSentinel a fixture
 * @param bytes_to_next_sentinel_1 test name
 */
TEST(TestBytesToNextSentinel, bytes_to_next_sentinel_1)
{
    Allocator <int, 100> a;
    ASSERT_EQ (a.bytes_to_next_sentinel(92), 96);
}

/**
 * Tests that helper function moves to index of next sentinel
 * @param TestBytesToNextSentinel a fixture
 * @param bytes_to_next_sentinel_2 test name
 */
TEST(TestBytesToNextSentinel, bytes_to_next_sentinel_2)
{
    Allocator <int, 100> a;
    ASSERT_EQ (a.bytes_to_next_sentinel(-92), 96);
}

// --------
// allocate
// --------

/**
 * Tests allocate
 * @param TestAllocate a fixture
 * @param allocate_1 test name
 */
TEST(TestAllocate, allocate_1)
{
    Allocator<int, 50> x;
    const size_t s = 1;
    x.allocate(s);
    
    ASSERT_EQ(x[0], -4);
    ASSERT_EQ(x[8], -4);
    ASSERT_EQ(x[12], 30);
    ASSERT_EQ(x[46], 30);
}

/**
 * Tests allocate
 * @param TestAllocate a fixture
 * @param allocate_1 test name
 */
TEST(TestAllocate, allocate_2)
{
    Allocator<double, 100> x;
    const size_t s = 4;
    x.allocate(s);
    
    ASSERT_EQ(x[0], -32);
    ASSERT_EQ(x[36], -32);
    ASSERT_EQ(x[40], 52);
    ASSERT_EQ(x[96], 52);
}

/**
 * Tests allocate
 * @param TestAllocate a fixture
 * @param allocate_1 test name
 */
TEST(TestAllocate, allocate_3)
{
    Allocator<int, 100> x;
    ASSERT_EQ(x[0], 92);
    ASSERT_EQ(x[96], 92);

    const size_t s = 1;

    x.allocate(s);
    ASSERT_EQ(x[0], -4);
    ASSERT_EQ(x[8], -4);
    ASSERT_EQ(x[12], 80);
    ASSERT_EQ(x[96], 80);

    x.allocate(s);
    ASSERT_EQ(x[0], -4);
    ASSERT_EQ(x[8], -4);
    ASSERT_EQ(x[12], -4);
    ASSERT_EQ(x[20], -4);
    ASSERT_EQ(x[24], 68);
    ASSERT_EQ(x[96], 68);
}

/**
 * Tests allocate
 * @param TestAllocate a fixture
 * @param allocate_1 test name
 */
TEST(TestAllocate, allocate_4)
{
    bool exception_thrown = false;
    Allocator<double, 100> x;
    const size_t s = 13;
    try
    {
        x.allocate(s);
    }
    catch(std::bad_alloc& e)
    {
        exception_thrown = true;
    }

    ASSERT_TRUE(exception_thrown);
}

/**
 * Tests allocate
 * @param TestAllocate a fixture
 * @param allocate_5 test name
 */
TEST(TestAllocate, allocate_5)
{
    Allocator<int, 24> x;
    const size_t s = 1;
    x.allocate(s);

    ASSERT_EQ(x[0], -4);
    ASSERT_EQ(x[8], -4);
    ASSERT_EQ(x[12], 4);
    ASSERT_EQ(x[20], 4);
}

/**
 * Tests allocate
 * @param TestAllocate a fixture
 * @param allocate_6 test name
 */
TEST(TestAllocate, allocate_6)
{
    Allocator<int, 24> x;
    const size_t s = 1;
    x.allocate(s);

    ASSERT_EQ(x[0], -4);
    ASSERT_EQ(x[8], -4);
    ASSERT_EQ(x[12], 4);
    ASSERT_EQ(x[20], 4);

    x.allocate(s);
    ASSERT_EQ(x[0],  -4);
    ASSERT_EQ(x[8],  -4);
    ASSERT_EQ(x[12], -4);
    ASSERT_EQ(x[20], -4);
}

/**
 * Tests allocate
 * @param TestAllocate a fixture
 * @param allocate_7 test name
 */
TEST(TestAllocate, allocate_7)
{
    Allocator<int, 20> x;
    const size_t s = 1;
    x.allocate(s);

    ASSERT_EQ(x[0],  -12);
    ASSERT_EQ(x[16], -12);

    bool exception_thrown = false;
    try
    {
        x.allocate(s);
    }
    catch(std::bad_alloc& e)
    {
        exception_thrown = true;
    }
    ASSERT_TRUE(exception_thrown);
}

// ----------
// deallocate
// ----------

/**
 * Tests deallocate (deallocating more than once)
 * @param TestDeallocate a fixture
 * @param deallocate_1 test name
 */
TEST(TestDeallocate, deallocate_1)
{
    Allocator<int, 100> x;
    const size_t s = 1;

    int* p = x.allocate(s);
    int* p2 = x.allocate(s);

    x.deallocate(p, s);
    x.deallocate(p2, s);
    ASSERT_EQ(x[0], 92);
    ASSERT_EQ(x[96], 92);
}

/**
 * Tests deallocate (deallocating once)
 * @param TestDeallocate a fixture
 * @param deallocate_1 test name
 */
TEST(TestDeallocate, deallocate_2)
{
    Allocator<int, 100> x;
    const size_t s = 1;

    int* p = x.allocate(s);

    x.deallocate(p, s);
    ASSERT_EQ(x[0], 92);
    ASSERT_EQ(x[96], 92);
}

/**
 * Tests deallocate (deallocating the middle block)
 * @param TestDeallocate a fixture
 * @param deallocate_3 test name
 */
TEST(TestDeallocate, deallocate_3)
{
    Allocator<int, 100> x;
    const size_t s = 4;
    int* p = x.allocate(s);

    const size_t s2 = 2;
    int* p2 = x.allocate(s2);

    const size_t s3 = 13;
    int* p3 = x.allocate(s3);

    x.deallocate(p2, s2);
    ASSERT_EQ(x[0], -16);
    ASSERT_EQ(x[20], -16);
    ASSERT_EQ(x[24], 8);
    ASSERT_EQ(x[36], 8);
    ASSERT_EQ(x[40], -52);
    ASSERT_EQ(x[96], -52);

    x.deallocate(p, s);
    x.deallocate(p3, s3);
    ASSERT_EQ(x[0], 92);
    ASSERT_EQ(x[96], 92);
}

/**
 * Tests deallocate (deallocating with an invalid pointer)
 * @param TestDeallocate a fixture
 * @param deallocate_4 test name
 */
TEST(TestDeallocate, deallocate_4)
{
    Allocator<int, 100> x;
    const size_t s = 4;
    x.allocate(s);

    int* p2 = x.allocate(s);

    bool exception_thrown = false;
    try
    {
        x.deallocate(p2 - 1, s);
    }
    catch(std::invalid_argument &e)
    {
        exception_thrown = true;
    }
    ASSERT_TRUE(exception_thrown);
}

/**
 * Tests deallocate (a lot of allocation and deallocation)
 * @param TestDeallocate a fixture
 * @param deallocate_5 test name
 */
TEST(TestDeallocate, deallocate_5)
{
    Allocator<int, 100> x;
    const size_t s = 1;
    int *p1 = x.allocate(s);
    int *p2 = x.allocate(s);
    int *p3 = x.allocate(s);
    int *p4 = x.allocate(s);
    int *p5 = x.allocate(s);
    int *p6 = x.allocate(s);
    int *p7 = x.allocate(s);
    int *p8 = x.allocate(s);

    x.deallocate(p1, s);
    x.deallocate(p2, s);
    x.deallocate(p3, s);
    x.deallocate(p4, s);
    x.deallocate(p5, s);
    x.deallocate(p6, s);
    x.deallocate(p7, s);
    x.deallocate(p8, s);

    ASSERT_EQ(x[0], 92);
    ASSERT_EQ(x[96], 92);
}

// --------------
// struct used for testing allocator
// --------------
struct test_struct
{
    int a;
    double b;
};

/**
 * Tests deallocate (structs as T)
 * @param TestDeallocate a fixture
 * @param deallocate_6 test name
 */
TEST(TestDeallocate, deallocate_6)
{
    Allocator<test_struct, 200> x;
    const size_t s1 = 3;
    const size_t s2 = 1;
    
    ASSERT_EQ(x[0], 192);
    ASSERT_EQ(x[196], 192);

    test_struct* p1 = x.allocate(s1);
    test_struct* p2 = x.allocate(s2);

    ASSERT_EQ(x[0], -48);
    ASSERT_EQ(x[52], -48);
    ASSERT_EQ(x[56], -16);
    ASSERT_EQ(x[76], -16);
    ASSERT_EQ(x[80], 112);
    ASSERT_EQ(x[196], 112);

    x.deallocate(p2, s2);
    ASSERT_EQ(x[56], 136);
    ASSERT_EQ(x[196], 136);

    x.deallocate(p1, s1);
    ASSERT_EQ(x[0], 192);
    ASSERT_EQ(x[196], 192);
}

// --------------
// TestAllocator3
// --------------

template <typename A>
struct TestAllocator3 : testing::Test
{
    // --------
    // typedefs
    // --------

    typedef          A                  allocator_type;
    typedef typename A::value_type      value_type;
    typedef typename A::difference_type difference_type;
    typedef typename A::pointer         pointer;
};

typedef testing::Types<
            Allocator<int,    100>,
            Allocator<double, 100> >
        my_types_2;

TYPED_TEST_CASE(TestAllocator3, my_types_2);

TYPED_TEST(TestAllocator3, test_1)
{
    typedef typename TestFixture::allocator_type  allocator_type;
    typedef typename TestFixture::value_type      value_type;
    typedef typename TestFixture::difference_type difference_type;
    typedef typename TestFixture::pointer         pointer;

          allocator_type  x;
    const difference_type s = 1;
    const value_type      v = 2;
    const pointer         p = x.allocate(s);
    if (p != nullptr)
    {
        x.construct(p, v);
        ASSERT_EQ(v, *p);
        x.destroy(p);
        x.deallocate(p, s);
    }
}

TYPED_TEST(TestAllocator3, test_10)
{
    typedef typename TestFixture::allocator_type  allocator_type;
    typedef typename TestFixture::value_type      value_type;
    typedef typename TestFixture::difference_type difference_type;
    typedef typename TestFixture::pointer         pointer;

          allocator_type  x;
    const difference_type s = 10;
    const value_type      v = 2;
    const pointer         b = x.allocate(s);
    if (b != nullptr)
    {
        pointer e = b + s;
        pointer p = b;
        try
        {
            while (p != e)
            {
                x.construct(p, v);
                ++p;
            }
        }
        catch (...)
        {
            while (b != p)
            {
                --p;
                x.destroy(p);
            }
            x.deallocate(b, s);
            throw;
        }
        ASSERT_EQ(s, std::count(b, e, v));
        while (b != e)
        {
            --e;
            x.destroy(e);
        }
        x.deallocate(b, s);
    }
}
