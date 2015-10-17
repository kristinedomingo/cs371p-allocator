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
// TestAllocator2
// --------------

TEST(TestAllocator2, const_index)
{
    const Allocator<int, 100> x;
    ASSERT_EQ(x[0], 92);
}

TEST(TestAllocator2, index1)
{
    Allocator<int, 100> x;
    ASSERT_EQ(x[0], 92);
}

TEST(TestAllocator2, index2)
{
    Allocator<int, 12> x;
    ASSERT_EQ(x[0], 4);
}

TEST(TestAllocator2, exception_is_thrown1)
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

TEST(TestAllocator2, exception_is_thrown2)
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

TEST(TestAllocator2, exception_is_thrown3)
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

TEST(TestAllocator2, bytes_to_next_sentinel_1)
{
    Allocator <int, 100> a;
    ASSERT_EQ (a.bytes_to_next_sentinel(92), 96);
}
TEST(TestAllocator2, bytes_to_next_sentinel_2)
{
    Allocator <int, 100> a;
    ASSERT_EQ (a.bytes_to_next_sentinel(-92), 96);
}
TEST(TestAllocator2, allocate_1) {
    Allocator<int, 50> x;
    const size_t s = 1;
    x.allocate(s);  
    
    const int zero = x[0];
    const int eight = x[8];

    ASSERT_EQ(zero, -4);
    ASSERT_EQ(eight, -4);
    
    
    //ASSERT_EQ(x[0], -4);
    //ASSERT_EQ(x[8], -4);
    //ASSERT_EQ(x[12], 30);
    //ASSERT_EQ(x[46], 30);
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
