/// @file
/// Unit tests for cetl/pf17/optional.hpp
///
/// @copyright
/// Copyright (C) OpenCyphal Development Team  <opencyphal.org>
/// Copyright Amazon.com Inc. or its affiliates.
/// SPDX-License-Identifier: MIT

// +******************************************************************************************************************+
// * WARNING:
// *    This test can take a _very_ long time to compile. No, the build isn't stuck. Ninja is purposefully very
// * quiet and the time is all spent doing some really-big-O operations when processing all the template code.
// * This is a complex set of test templates used to test a complex set of templates (optional).
// *
// * For example: on a virt one user saw a 4 minute compilation time for this one file:
// *    [driver] Build completed: 00:04:14.217
// *
// * Be patient. Sorry.
// +******************************************************************************************************************+

#include <cetl/pf17/optional.hpp>
#include <cetlvast/typelist.hpp>
#include <cetlvast/smf_policies.hpp>
#include <gtest/gtest.h>

// NOLINTBEGIN(*-use-after-move)

using namespace cetlvast::smf_policies;

using policy_combinations = cetlvast::typelist::cartesian_product<  //
    std::tuple<copy_ctor_policy<policy_deleted>,                    //
               copy_ctor_policy<policy_trivial>,
               copy_ctor_policy<policy_nontrivial>>,
    std::tuple<move_ctor_policy<policy_deleted>,  //
               move_ctor_policy<policy_trivial>,
               move_ctor_policy<policy_nontrivial>>,
    std::tuple<copy_assignment_policy<policy_deleted>,  //
               copy_assignment_policy<policy_trivial>,
               copy_assignment_policy<policy_nontrivial>>,
    std::tuple<move_assignment_policy<policy_deleted>,  //
               move_assignment_policy<policy_trivial>,
               move_assignment_policy<policy_nontrivial>>,
    std::tuple<dtor_policy<policy_trivial>,  //
               dtor_policy<policy_nontrivial>>>;

/// This is a long list of all the possible combinations of special function policies.
/// Derive from each type to test all possible policies.
using testing_types =
    cetlvast::typelist::into<::testing::Types>::from<cetlvast::typelist::map<combine_bases, policy_combinations>::type>;

/// TESTS -----------------------------------------------------------------------------------------------------------

using cetl::pf17::optional;
using cetl::pf17::nullopt;
using cetl::pf17::in_place;

static_assert(std::is_same<optional<bool>::value_type, bool>::value, "");
static_assert(std::is_same<optional<long>::value_type, long>::value, "");

static_assert(std::is_trivially_copy_constructible<optional<bool>>::value, "");
static_assert(std::is_trivially_move_constructible<optional<bool>>::value, "");
static_assert(std::is_trivially_copy_assignable<optional<bool>>::value, "");
static_assert(std::is_trivially_move_assignable<optional<bool>>::value, "");
static_assert(std::is_trivially_destructible<optional<bool>>::value, "");

template <typename>
class test_optional_combinations : public ::testing::Test
{};

TYPED_TEST_SUITE(test_optional_combinations, testing_types, );

struct copyable_t
{};
struct noncopyable_t
{
    noncopyable_t()                                = default;
    noncopyable_t(const noncopyable_t&)            = delete;
    noncopyable_t(noncopyable_t&&)                 = delete;
    noncopyable_t& operator=(const noncopyable_t&) = delete;
    noncopyable_t& operator=(noncopyable_t&&)      = delete;
    ~noncopyable_t()                               = default;
};

/// A simple pair of types for testing where foo is implicitly convertible to bar but not vice versa.
template <typename>
struct bar;
template <typename Base>
struct foo final : Base
{
    foo()
        : value{0}
    {
    }
    explicit foo(const std::int64_t val) noexcept
        : value{val}
    {
    }
    explicit foo(const bar<Base>& val) noexcept;
    explicit foo(bar<Base>&& val) noexcept;
    foo(const std::initializer_list<std::int64_t> il)
        : value{static_cast<std::int64_t>(il.size())}
    {
    }
    std::int64_t value;
};
template <typename Base>
struct bar final : Base
{
    bar(const std::int64_t val) noexcept  // NOLINT(*-explicit-constructor)
        : value{val}
    {
    }
    bar(const foo<Base>& other) noexcept  // NOLINT(*-explicit-constructor)
        : value{other.value}
    {
    }
    bar(foo<Base>&& other) noexcept  // NOLINT(*-explicit-constructor)
        : value{other.value}
    {
        other.value = 0;  // Moving zeroes the source.
    }
    std::int64_t value;
};
template <typename Base>
foo<Base>::foo(const bar<Base>& val) noexcept
    : value{val.value}
{
}
template <typename Base>
foo<Base>::foo(bar<Base>&& val) noexcept
    : value{val.value}
{
    val.value = 0;  // Moving zeroes the source.
}

// COPYABLE CASE
// Check implicit conversions.
static_assert(std::is_convertible<foo<copyable_t>, bar<copyable_t>>::value, "");
static_assert(!std::is_convertible<bar<copyable_t>, foo<copyable_t>>::value, "");
static_assert(std::is_convertible<optional<foo<copyable_t>>, optional<bar<copyable_t>>>::value, "");
static_assert(!std::is_convertible<optional<bar<copyable_t>>, optional<foo<copyable_t>>>::value, "");
// Check explicit conversions.
static_assert(std::is_constructible<bar<copyable_t>, foo<copyable_t>>::value, "");
static_assert(std::is_constructible<foo<copyable_t>, bar<copyable_t>>::value, "");
static_assert(std::is_constructible<optional<bar<copyable_t>>, optional<foo<copyable_t>>>::value, "");
static_assert(std::is_constructible<optional<foo<copyable_t>>, optional<bar<copyable_t>>>::value, "");
// Check triviality of foo.
static_assert(std::is_trivially_copy_constructible<optional<foo<copyable_t>>>::value, "");
static_assert(std::is_trivially_move_constructible<optional<foo<copyable_t>>>::value, "");
static_assert(std::is_trivially_copy_assignable<optional<foo<copyable_t>>>::value, "");
static_assert(std::is_trivially_move_assignable<optional<foo<copyable_t>>>::value, "");
static_assert(std::is_trivially_destructible<optional<foo<copyable_t>>>::value, "");
// Check triviality of bar.
static_assert(std::is_trivially_copy_constructible<optional<bar<copyable_t>>>::value, "");
static_assert(std::is_trivially_move_constructible<optional<bar<copyable_t>>>::value, "");
static_assert(std::is_trivially_copy_assignable<optional<bar<copyable_t>>>::value, "");
static_assert(std::is_trivially_move_assignable<optional<bar<copyable_t>>>::value, "");
static_assert(std::is_trivially_destructible<optional<bar<copyable_t>>>::value, "");
// NONCOPYABLE CASE
// Check implicit conversions.
// CAVEAT: in C++14, std::is_convertible<F, T> is not true if T is not copyable, even if F is convertible to T,
// so we use std::is_convertible<F, T&&> instead.
static_assert(std::is_convertible<foo<noncopyable_t>, bar<noncopyable_t>&&>::value, "");
static_assert(!std::is_convertible<bar<noncopyable_t>, foo<noncopyable_t>&&>::value, "");
static_assert(std::is_convertible<optional<foo<noncopyable_t>>, optional<bar<noncopyable_t>>&&>::value, "");
static_assert(!std::is_convertible<optional<bar<noncopyable_t>>, optional<foo<noncopyable_t>>&&>::value, "");
// Check explicit conversions.
static_assert(std::is_constructible<bar<noncopyable_t>, foo<noncopyable_t>>::value, "");
static_assert(std::is_constructible<foo<noncopyable_t>, bar<noncopyable_t>>::value, "");
static_assert(std::is_constructible<optional<bar<noncopyable_t>>, optional<foo<noncopyable_t>>>::value, "");
static_assert(std::is_constructible<optional<foo<noncopyable_t>>, optional<bar<noncopyable_t>>>::value, "");
// Check triviality of foo.
static_assert(!std::is_copy_constructible<optional<foo<noncopyable_t>>>::value, "");
static_assert(!std::is_move_constructible<optional<foo<noncopyable_t>>>::value, "");
static_assert(!std::is_copy_assignable<optional<foo<noncopyable_t>>>::value, "");
static_assert(!std::is_move_assignable<optional<foo<noncopyable_t>>>::value, "");
static_assert(std::is_trivially_destructible<optional<foo<noncopyable_t>>>::value, "");
// Check triviality of bar.
static_assert(!std::is_copy_constructible<optional<bar<noncopyable_t>>>::value, "");
static_assert(!std::is_move_constructible<optional<bar<noncopyable_t>>>::value, "");
static_assert(!std::is_copy_assignable<optional<bar<noncopyable_t>>>::value, "");
static_assert(!std::is_move_assignable<optional<bar<noncopyable_t>>>::value, "");
static_assert(std::is_trivially_destructible<optional<bar<noncopyable_t>>>::value, "");

/// ------------------------------------------------------------------------------------------------

/// This test checks common behaviors that are independent of the copy/move policies.
TYPED_TEST(test_optional_combinations, common)
{
    // Ensure trivial copy/move policies are correctly inherited from the value type.
    static_assert(std::is_trivially_copy_constructible<TypeParam>::value ==
                      std::is_trivially_copy_constructible<optional<TypeParam>>::value,
                  "");
    static_assert(std::is_trivially_move_constructible<TypeParam>::value ==
                      std::is_trivially_move_constructible<optional<TypeParam>>::value,
                  "");
    static_assert((std::is_trivially_copy_assignable<TypeParam>::value &&
                   std::is_trivially_copy_constructible<TypeParam>::value &&
                   std::is_trivially_destructible<TypeParam>::value) ==
                      std::is_trivially_copy_assignable<optional<TypeParam>>::value,
                  "");
    static_assert((std::is_trivially_move_assignable<TypeParam>::value &&
                   std::is_trivially_move_constructible<TypeParam>::value &&
                   std::is_trivially_destructible<TypeParam>::value) ==
                      std::is_trivially_move_assignable<optional<TypeParam>>::value,
                  "");
    static_assert(std::is_trivially_destructible<TypeParam>::value ==
                      std::is_trivially_destructible<optional<TypeParam>>::value,
                  "");

    // Ensure implicit convertibility is inherited from the value type.
    // Note that these checks behave differently in C++14 vs. newer standards because in C++14,
    // std::is_convertible<F, T> is not true if T is not copyable, even if F is convertible to T.
    static_assert(std::is_convertible<foo<TypeParam>, bar<TypeParam>>::value ==
                      std::is_convertible<optional<foo<TypeParam>>, optional<bar<TypeParam>>>::value,
                  "");
    static_assert(std::is_convertible<bar<TypeParam>, foo<TypeParam>>::value ==
                      std::is_convertible<optional<bar<TypeParam>>, optional<foo<TypeParam>>>::value,
                  "");

    // Ensure explicit convertibility is inherited from the value type.
    static_assert(std::is_constructible<bar<TypeParam>, foo<TypeParam>>::value ==
                      std::is_constructible<optional<bar<TypeParam>>, optional<foo<TypeParam>>>::value,
                  "");
    static_assert(std::is_constructible<foo<TypeParam>, bar<TypeParam>>::value ==
                      std::is_constructible<optional<foo<TypeParam>>, optional<bar<TypeParam>>>::value,
                  "");

    // Runtime tests.
    std::uint32_t            destruction_count = 0;
    optional<foo<TypeParam>> opt;
    EXPECT_FALSE(opt.has_value());
    EXPECT_FALSE(opt);
    opt.emplace(12345).configure_destruction_counter(&destruction_count);
    EXPECT_TRUE(opt.has_value());
    EXPECT_TRUE(opt);
    EXPECT_EQ(0, destruction_count);
    EXPECT_EQ(12345, opt->value);
    EXPECT_EQ(12345, (*opt).value);
    EXPECT_EQ(12345, (*std::move(opt)).value);
    EXPECT_EQ(12345, opt.value().value);
    EXPECT_EQ(12345, std::move(opt).value().value);
    {
        const auto& copt = opt;
        EXPECT_EQ(12345, copt->value);
        EXPECT_EQ(12345, (*copt).value);
        EXPECT_EQ(12345, (*std::move(copt)).value);
        EXPECT_EQ(12345, copt.value().value);
        EXPECT_EQ(12345, std::move(copt).value().value);
    }
    EXPECT_EQ(0, opt->get_copy_ctor_count());
    EXPECT_EQ(0, opt->get_move_ctor_count());
    EXPECT_EQ(0, opt->get_copy_assignment_count());
    EXPECT_EQ(0, opt->get_move_assignment_count());
    EXPECT_EQ(0, destruction_count);
    opt = cetl::pf17::nullopt;
    EXPECT_FALSE(opt);
    EXPECT_EQ((TypeParam::dtor_policy_value == policy_nontrivial) ? 1 : 0, destruction_count);
    opt.emplace(std::initializer_list<std::int64_t>{1, 2, 3, 4, 5}).configure_destruction_counter(&destruction_count);
    EXPECT_TRUE(opt);
    EXPECT_EQ(5, opt->value);
    EXPECT_EQ((TypeParam::dtor_policy_value == policy_nontrivial) ? 1 : 0, destruction_count);
    EXPECT_EQ(0, opt->get_copy_ctor_count());
    EXPECT_EQ(0, opt->get_move_ctor_count());
    EXPECT_EQ(0, opt->get_copy_assignment_count());
    EXPECT_EQ(0, opt->get_move_assignment_count());
    opt.reset();
    EXPECT_EQ((TypeParam::dtor_policy_value == policy_nontrivial) ? 2 : 0, destruction_count);
}

/// ------------------------------------------------------------------------------------------------

#if defined(__cpp_exceptions)
TYPED_TEST(test_optional_combinations, exceptions)
{
    optional<TypeParam> opt;
    const auto sink = [](auto&&) {};  // Workaround for GCC bug https://gcc.gnu.org/bugzilla/show_bug.cgi?id=66425
    EXPECT_THROW(sink(opt.value()), cetl::pf17::bad_optional_access);
    EXPECT_THROW(sink(std::move(opt).value()), cetl::pf17::bad_optional_access);
    opt.emplace();
    EXPECT_NO_THROW(sink(opt.value()));
    EXPECT_NO_THROW(sink(std::move(opt).value()));
}
#endif

/// ------------------------------------------------------------------------------------------------

TYPED_TEST(test_optional_combinations, ctor_1)
{
    optional<TypeParam> opt1;
    EXPECT_FALSE(opt1.has_value());
    optional<TypeParam> opt2(nullopt);
    EXPECT_FALSE(opt2.has_value());
}

/// ------------------------------------------------------------------------------------------------

template <typename T, std::uint8_t CopyCtorPolicy = T::copy_ctor_policy_value>
struct test_ctor_2
{
    static void test()
    {
        std::uint32_t destructed = 0;
        optional<T>   opt;
        opt.emplace().configure_destruction_counter(&destructed);
        {
            optional<T> opt2 = opt;
            EXPECT_EQ((T::copy_ctor_policy_value == policy_nontrivial) ? 1 : 0, opt2->get_copy_ctor_count());
            EXPECT_EQ(0U, opt2->get_move_ctor_count());
            EXPECT_EQ(0U, opt2->get_copy_assignment_count());
            EXPECT_EQ(0U, opt2->get_move_assignment_count());
            EXPECT_EQ(0U, destructed);
            EXPECT_EQ(0U, opt->get_copy_ctor_count());
            EXPECT_EQ(0U, opt->get_move_ctor_count());
            EXPECT_EQ(0U, opt->get_copy_assignment_count());
            EXPECT_EQ(0U, opt->get_move_assignment_count());
            opt.reset();
            EXPECT_EQ((T::dtor_policy_value == policy_nontrivial) ? 1 : 0, destructed);
        }
        EXPECT_EQ((T::dtor_policy_value == policy_nontrivial) ? 2 : 0, destructed);
    }
};
template <typename T>
struct test_ctor_2<T, policy_deleted>
{
    static_assert(!std::is_copy_constructible<T>::value, "");
    static_assert(!std::is_copy_constructible<optional<T>>::value, "");
    static void test() {}
};

TYPED_TEST(test_optional_combinations, ctor_2)
{
    test_ctor_2<TypeParam>::test();
}

/// ------------------------------------------------------------------------------------------------

// Caveat: types without a move constructor but with a copy constructor that accepts const T& arguments,
// satisfy std::is_move_constructible.
template <typename T,
          std::uint8_t CopyCtorPolicy = T::copy_ctor_policy_value,
          std::uint8_t MoveCtorPolicy = T::move_ctor_policy_value>
struct test_ctor_3
{
    static void test()
    {
        std::uint32_t destructed = 0;
        optional<T>   opt;
        opt.emplace().configure_destruction_counter(&destructed);
        {
            optional<T> opt2 = std::move(opt);
            EXPECT_EQ(((T::move_ctor_policy_value == policy_deleted) &&
                       (T::copy_ctor_policy_value == policy_nontrivial))
                          ? 1
                          : 0,
                      opt2->get_copy_ctor_count());
            EXPECT_EQ((T::move_ctor_policy_value == policy_nontrivial) ? 1 : 0, opt2->get_move_ctor_count());
            EXPECT_EQ(0U, opt2->get_copy_assignment_count());
            EXPECT_EQ(0U, opt2->get_move_assignment_count());
            EXPECT_EQ(0U, destructed);
            EXPECT_EQ(0U, opt->get_copy_ctor_count());
            EXPECT_EQ(0U, opt->get_move_ctor_count());
            EXPECT_EQ(0U, opt->get_copy_assignment_count());
            EXPECT_EQ(0U, opt->get_move_assignment_count());
            opt.reset();
            EXPECT_EQ((T::dtor_policy_value == policy_nontrivial) ? 1 : 0, destructed);
        }
        EXPECT_EQ((T::dtor_policy_value == policy_nontrivial) ? 2 : 0, destructed);
    }
};
template <typename T>
struct test_ctor_3<T, policy_deleted, policy_deleted>
{
    // Caveat: types without a move constructor but with a copy constructor that accepts const T& arguments,
    // satisfy std::is_move_constructible.
    static_assert(!std::is_move_constructible<T>::value, "");
    static_assert(!std::is_move_constructible<optional<T>>::value, "");
    static_assert(!std::is_copy_constructible<T>::value, "");
    static_assert(!std::is_copy_constructible<optional<T>>::value, "");
    static void test() {}
};

TYPED_TEST(test_optional_combinations, ctor_3)
{
    test_ctor_3<TypeParam>::test();
}

/// ------------------------------------------------------------------------------------------------

TYPED_TEST(test_optional_combinations, ctor_4)
{
    using F = foo<TypeParam>;
    using B = bar<TypeParam>;
    static_assert(std::is_constructible<F, B>::value, "");
    static_assert(std::is_constructible<B, F>::value, "");
    static_assert(std::is_constructible<optional<F>, optional<B>>::value, "");
    static_assert(std::is_constructible<optional<B>, optional<F>>::value, "");
    std::uint32_t f_dtor = 0;
    std::uint32_t b_dtor = 0;
    optional<F>   f1;
    f1.emplace(12345).configure_destruction_counter(&f_dtor);
    optional<B> b1(f1);  // Use implicit constructor because foo is implicitly convertible to bar
    b1.value().configure_destruction_counter(&b_dtor);
    {
        optional<F> f2(b1);  // Use explicit constructor because bar is not implicitly convertible to foo
        f2.value().configure_destruction_counter(&f_dtor);
        EXPECT_EQ(12345, f1.value().value);
        EXPECT_EQ(12345, b1.value().value);
        EXPECT_EQ(12345, f2.value().value);
        // Ensure no copy/move of the base took place.
        EXPECT_EQ(0, f1->get_copy_ctor_count());
        EXPECT_EQ(0, f1->get_move_ctor_count());
        EXPECT_EQ(0, f1->get_copy_assignment_count());
        EXPECT_EQ(0, f1->get_move_assignment_count());
        EXPECT_EQ(0, b1->get_copy_ctor_count());
        EXPECT_EQ(0, b1->get_move_ctor_count());
        EXPECT_EQ(0, b1->get_copy_assignment_count());
        EXPECT_EQ(0, b1->get_move_assignment_count());
        EXPECT_EQ(0, f2->get_copy_ctor_count());
        EXPECT_EQ(0, f2->get_move_ctor_count());
        EXPECT_EQ(0, f2->get_copy_assignment_count());
        EXPECT_EQ(0, f2->get_move_assignment_count());
        EXPECT_EQ(0, f_dtor);
        EXPECT_EQ(0, b_dtor);
    }
    EXPECT_EQ((TypeParam::dtor_policy_value == policy_nontrivial) ? 1 : 0, f_dtor);
    EXPECT_EQ(0, b_dtor);
    b1.reset();
    EXPECT_EQ((TypeParam::dtor_policy_value == policy_nontrivial) ? 1 : 0, f_dtor);
    EXPECT_EQ((TypeParam::dtor_policy_value == policy_nontrivial) ? 1 : 0, b_dtor);
    f1.reset();
    EXPECT_EQ((TypeParam::dtor_policy_value == policy_nontrivial) ? 2 : 0, f_dtor);
    EXPECT_EQ((TypeParam::dtor_policy_value == policy_nontrivial) ? 1 : 0, b_dtor);
    // Test empty copy.
    optional<B> b2(f1);
    EXPECT_FALSE(b2);
}

/// ------------------------------------------------------------------------------------------------

TYPED_TEST(test_optional_combinations, ctor_5)
{
    using F = foo<TypeParam>;
    using B = bar<TypeParam>;
    static_assert(std::is_constructible<F, B>::value, "");
    static_assert(std::is_constructible<B, F>::value, "");
    static_assert(std::is_constructible<optional<F>, optional<B>>::value, "");
    static_assert(std::is_constructible<optional<B>, optional<F>>::value, "");
    std::uint32_t f_dtor = 0;
    std::uint32_t b_dtor = 0;
    optional<F>   f1;
    f1.emplace(12345).configure_destruction_counter(&f_dtor);
    optional<B> b1(std::move(f1));   // Use implicit constructor because foo is implicitly convertible to bar
    EXPECT_EQ(0, f1.value().value);  // Moving zeroes the source.
    EXPECT_EQ(12345, b1.value().value);
    b1.value().configure_destruction_counter(&b_dtor);
    {
        optional<F> f2(std::move(b1));  // Use explicit constructor because bar is not implicitly convertible to foo
        f2.value().configure_destruction_counter(&f_dtor);
        EXPECT_EQ(0, f1.value().value);
        EXPECT_EQ(0, b1.value().value);  // Moving zeroes the source.
        EXPECT_EQ(12345, f2.value().value);
        // Ensure no copy/move of the base took place.
        EXPECT_EQ(0, f1->get_copy_ctor_count());
        EXPECT_EQ(0, f1->get_move_ctor_count());
        EXPECT_EQ(0, f1->get_copy_assignment_count());
        EXPECT_EQ(0, f1->get_move_assignment_count());
        EXPECT_EQ(0, b1->get_copy_ctor_count());
        EXPECT_EQ(0, b1->get_move_ctor_count());
        EXPECT_EQ(0, b1->get_copy_assignment_count());
        EXPECT_EQ(0, b1->get_move_assignment_count());
        EXPECT_EQ(0, f2->get_copy_ctor_count());
        EXPECT_EQ(0, f2->get_move_ctor_count());
        EXPECT_EQ(0, f2->get_copy_assignment_count());
        EXPECT_EQ(0, f2->get_move_assignment_count());
        EXPECT_EQ(0, f_dtor);
        EXPECT_EQ(0, b_dtor);
    }
    EXPECT_EQ((TypeParam::dtor_policy_value == policy_nontrivial) ? 1 : 0, f_dtor);
    EXPECT_EQ(0, b_dtor);
    b1.reset();
    EXPECT_EQ((TypeParam::dtor_policy_value == policy_nontrivial) ? 1 : 0, f_dtor);
    EXPECT_EQ((TypeParam::dtor_policy_value == policy_nontrivial) ? 1 : 0, b_dtor);
    f1.reset();
    EXPECT_EQ((TypeParam::dtor_policy_value == policy_nontrivial) ? 2 : 0, f_dtor);
    EXPECT_EQ((TypeParam::dtor_policy_value == policy_nontrivial) ? 1 : 0, b_dtor);
    // Test empty copy.
    optional<B> b2(std::move(f1));
    EXPECT_FALSE(b2);
}

/// ------------------------------------------------------------------------------------------------

TYPED_TEST(test_optional_combinations, ctor_6)
{
    std::uint32_t            f_dtor = 0;
    optional<foo<TypeParam>> f1(in_place, 12345);
    f1.value().configure_destruction_counter(&f_dtor);
    EXPECT_TRUE(f1.has_value());
    EXPECT_EQ(12345, f1.value().value);
    // Ensure no copy/move of the base took place.
    EXPECT_EQ(0, f1->get_copy_ctor_count());
    EXPECT_EQ(0, f1->get_move_ctor_count());
    EXPECT_EQ(0, f1->get_copy_assignment_count());
    EXPECT_EQ(0, f1->get_move_assignment_count());
    EXPECT_EQ(0, f_dtor);
    f1 = nullopt;
    EXPECT_FALSE(f1);
    EXPECT_EQ((TypeParam::dtor_policy_value == policy_nontrivial) ? 1 : 0, f_dtor);
}

/// ------------------------------------------------------------------------------------------------

TYPED_TEST(test_optional_combinations, ctor_7)
{
    std::uint32_t            f_dtor = 0;
    optional<foo<TypeParam>> f1(in_place, std::initializer_list<std::int64_t>{1, 2, 3, 4, 5});
    f1.value().configure_destruction_counter(&f_dtor);
    EXPECT_TRUE(f1.has_value());
    EXPECT_EQ(5, f1.value().value);
    // Ensure no copy/move of the base took place.
    EXPECT_EQ(0, f1->get_copy_ctor_count());
    EXPECT_EQ(0, f1->get_move_ctor_count());
    EXPECT_EQ(0, f1->get_copy_assignment_count());
    EXPECT_EQ(0, f1->get_move_assignment_count());
    EXPECT_EQ(0, f_dtor);
    f1 = nullopt;
    EXPECT_FALSE(f1);
    EXPECT_EQ((TypeParam::dtor_policy_value == policy_nontrivial) ? 1 : 0, f_dtor);
}

/// ------------------------------------------------------------------------------------------------

static_assert(cetl::pf17::detail::opt::enable_ctor8<bar<noncopyable_t>, foo<noncopyable_t>, false>, "");
static_assert(cetl::pf17::detail::opt::enable_ctor8<foo<noncopyable_t>, bar<noncopyable_t>, true>, "");

TYPED_TEST(test_optional_combinations, ctor_8)
{
    std::uint32_t            f_dtor = 0;
    std::uint32_t            b_dtor = 0;
    optional<foo<TypeParam>> f1(12345);  // Use explicit constructor.
    optional<bar<TypeParam>> b1(23456);  // Use implicit constructor.
    f1.value().configure_destruction_counter(&f_dtor);
    b1.value().configure_destruction_counter(&b_dtor);
    EXPECT_TRUE(f1.has_value());
    EXPECT_EQ(12345, f1.value().value);
    EXPECT_TRUE(b1.has_value());
    EXPECT_EQ(23456, b1.value().value);
    // Ensure no copy/move of the base took place.
    EXPECT_EQ(0, f1->get_copy_ctor_count());
    EXPECT_EQ(0, f1->get_move_ctor_count());
    EXPECT_EQ(0, f1->get_copy_assignment_count());
    EXPECT_EQ(0, f1->get_move_assignment_count());
    EXPECT_EQ(0, f_dtor);
    EXPECT_EQ(0, b1->get_copy_ctor_count());
    EXPECT_EQ(0, b1->get_move_ctor_count());
    EXPECT_EQ(0, b1->get_copy_assignment_count());
    EXPECT_EQ(0, b1->get_move_assignment_count());
    EXPECT_EQ(0, b_dtor);
    f1 = nullopt;
    b1 = nullopt;
    EXPECT_FALSE(f1);
    EXPECT_FALSE(b1);
    EXPECT_EQ((TypeParam::dtor_policy_value == policy_nontrivial) ? 1 : 0, f_dtor);
    EXPECT_EQ((TypeParam::dtor_policy_value == policy_nontrivial) ? 1 : 0, b_dtor);
}

/// ------------------------------------------------------------------------------------------------

TYPED_TEST(test_optional_combinations, assignment_1)
{
    optional<TypeParam> opt1(in_place);
    EXPECT_TRUE(opt1.has_value());
    opt1 = nullopt;
    EXPECT_FALSE(opt1.has_value());
}

/// ------------------------------------------------------------------------------------------------

/// For the copy assignment to work, T shall be both copy-constructible and copy-assignable.
template <typename T,
          std::uint8_t CopyCtorPolicy       = T::copy_ctor_policy_value,
          std::uint8_t CopyAssignmentPolicy = T::copy_assignment_policy_value>
struct test_assignment_2
{
    static void test()
    {
        std::uint32_t destructed = 0;
        optional<T>   opt1;
        optional<T>   opt2;
        EXPECT_FALSE(opt1);
        EXPECT_FALSE(opt2);
        // Empty to empty.
        opt1 = opt2;
        EXPECT_FALSE(opt1);
        EXPECT_FALSE(opt2);
        // Non-empty to empty. A copy ctor is invoked.
        opt1.emplace().configure_destruction_counter(&destructed);
        EXPECT_TRUE(opt1);
        EXPECT_FALSE(opt2);
        opt2 = opt1;
        EXPECT_TRUE(opt1);
        EXPECT_TRUE(opt2);
        EXPECT_EQ(0U, opt1->get_copy_ctor_count());
        EXPECT_EQ(0U, opt1->get_move_ctor_count());
        EXPECT_EQ(0U, opt1->get_copy_assignment_count());
        EXPECT_EQ(0U, opt1->get_move_assignment_count());
        EXPECT_EQ(0U, destructed);
        EXPECT_EQ((T::copy_ctor_policy_value == policy_nontrivial) ? 1 : 0, opt2->get_copy_ctor_count());
        EXPECT_EQ(0U, opt2->get_move_ctor_count());
        EXPECT_EQ(0U, opt2->get_copy_assignment_count());
        EXPECT_EQ(0U, opt2->get_move_assignment_count());
        EXPECT_EQ(0U, destructed);
        // Non-empty to non-empty. A copy assignment is invoked.
        opt1 = opt2;
        EXPECT_TRUE(opt1);
        EXPECT_TRUE(opt2);
        // The copy ctor count is copied over from opt2!
        EXPECT_EQ((T::copy_ctor_policy_value == policy_nontrivial) ? 1 : 0, opt1->get_copy_ctor_count());
        EXPECT_EQ(0U, opt1->get_move_ctor_count());
        EXPECT_EQ((T::copy_assignment_policy_value == policy_nontrivial) ? 1 : 0, opt1->get_copy_assignment_count());
        EXPECT_EQ(0U, opt1->get_move_assignment_count());
        EXPECT_EQ(0U, destructed);
        EXPECT_EQ((T::copy_ctor_policy_value == policy_nontrivial) ? 1 : 0, opt2->get_copy_ctor_count());
        EXPECT_EQ(0U, opt2->get_move_ctor_count());
        EXPECT_EQ(0U, opt2->get_copy_assignment_count());
        EXPECT_EQ(0U, opt2->get_move_assignment_count());
        EXPECT_EQ(0U, destructed);
        // Empty to non-empty. Destructor is invoked.
        opt1 = nullopt;
        EXPECT_FALSE(opt1);
        EXPECT_TRUE(opt2);
        EXPECT_EQ((T::dtor_policy_value == policy_nontrivial) ? 1 : 0, destructed);
        opt2 = opt1;
        EXPECT_FALSE(opt1);
        EXPECT_FALSE(opt2);
        EXPECT_EQ((T::dtor_policy_value == policy_nontrivial) ? 2 : 0, destructed);
    }
};
template <typename T, std::uint8_t CopyCtorPolicy>
struct test_assignment_2<T, CopyCtorPolicy, policy_deleted>
{
    static_assert(!std::is_copy_assignable<T>::value, "");
    static_assert(!std::is_copy_assignable<optional<T>>::value, "");
    static void test() {}
};
template <typename T, std::uint8_t CopyAssignmentPolicy>
struct test_assignment_2<T, policy_deleted, CopyAssignmentPolicy>
{
    static_assert(!std::is_copy_constructible<T>::value, "");
    static_assert(!std::is_copy_constructible<optional<T>>::value, "");
    static_assert(!std::is_copy_assignable<optional<T>>::value, "");
    static void test() {}
};
template <typename T>
struct test_assignment_2<T, policy_deleted, policy_deleted>  // This is to avoid the specialization ambiguity.
{
    static_assert(!std::is_copy_assignable<T>::value, "");
    static_assert(!std::is_copy_constructible<T>::value, "");
    static_assert(!std::is_copy_constructible<optional<T>>::value, "");
    static_assert(!std::is_copy_assignable<optional<T>>::value, "");
    static void test() {}
};

TYPED_TEST(test_optional_combinations, assignment_2)
{
    test_assignment_2<TypeParam>::test();
}

/// ------------------------------------------------------------------------------------------------

/// For the move assignment to work, T shall be both
/// (move-constructible or copy-constructible) and (move-assignable or copy-assignable).
/// Notes:
/// - A type does not have to implement move assignment operator in order to satisfy MoveAssignable:
///   a copy assignment operator that takes its parameter by value or as a const Type&, will bind to rvalue argument.
/// - A type does not have to implement a move constructor to satisfy MoveConstructible:
///   a copy constructor that takes a const T& argument can bind rvalue expressions.
template <typename T,
          std::uint8_t CopyCtorPolicy       = T::copy_ctor_policy_value,
          std::uint8_t CopyAssignmentPolicy = T::copy_assignment_policy_value,
          std::uint8_t MoveCtorPolicy       = T::move_ctor_policy_value,
          std::uint8_t MoveAssignmentPolicy = T::move_assignment_policy_value>
struct test_assignment_3
{
    static void test()
    {
        std::uint32_t destructed = 0;
        optional<T>   opt1;
        optional<T>   opt2;
        EXPECT_FALSE(opt1);
        EXPECT_FALSE(opt2);
        // Empty to empty.
        opt1 = std::move(opt2);
        EXPECT_FALSE(opt1);
        EXPECT_FALSE(opt2);
        // Non-empty to empty. A copy/move ctor is invoked.
        opt1.emplace().configure_destruction_counter(&destructed);
        EXPECT_TRUE(opt1);
        EXPECT_FALSE(opt2);
        opt2 = std::move(opt1);
        EXPECT_TRUE(opt1);
        EXPECT_TRUE(opt2);
        // Check opt1 counters.
        EXPECT_EQ(0U, opt1->get_copy_ctor_count());
        EXPECT_EQ(0U, opt1->get_move_ctor_count());
        EXPECT_EQ(0U, opt1->get_copy_assignment_count());
        EXPECT_EQ(0U, opt1->get_move_assignment_count());
        // Check opt2 counters.
        EXPECT_EQ(((T::copy_ctor_policy_value == policy_nontrivial) && (T::move_ctor_policy_value == policy_deleted))
                      ? 1
                      : 0,
                  opt2->get_copy_ctor_count());
        EXPECT_EQ((T::move_ctor_policy_value == policy_nontrivial) ? 1 : 0, opt2->get_move_ctor_count());
        EXPECT_EQ(0U, opt2->get_copy_assignment_count());
        EXPECT_EQ(0U, opt2->get_move_assignment_count());
        EXPECT_EQ(0U, destructed);
        // Non-empty to non-empty. A copy/move assignment is invoked.
        opt1 = std::move(opt2);
        EXPECT_TRUE(opt1);
        EXPECT_TRUE(opt2);
        // Check opt1 counters. The copy/move ctor count is copied over from opt2!
        EXPECT_EQ(((T::copy_ctor_policy_value == policy_nontrivial) && (T::move_ctor_policy_value == policy_deleted))
                      ? 1
                      : 0,
                  opt1->get_copy_ctor_count());
        EXPECT_EQ((T::move_ctor_policy_value == policy_nontrivial) ? 1 : 0, opt1->get_move_ctor_count());
        EXPECT_EQ(((T::copy_assignment_policy_value == policy_nontrivial) &&
                   (T::move_assignment_policy_value == policy_deleted))
                      ? 1
                      : 0,
                  opt1->get_copy_assignment_count());
        EXPECT_EQ((T::move_assignment_policy_value == policy_nontrivial) ? 1 : 0, opt1->get_move_assignment_count());
        // Check opt2 counters.
        EXPECT_EQ(((T::copy_ctor_policy_value == policy_nontrivial) && (T::move_ctor_policy_value == policy_deleted))
                      ? 1
                      : 0,
                  opt2->get_copy_ctor_count());
        EXPECT_EQ((T::move_ctor_policy_value == policy_nontrivial) ? 1 : 0, opt2->get_move_ctor_count());
        EXPECT_EQ(0U, opt2->get_copy_assignment_count());
        EXPECT_EQ(0U, opt2->get_move_assignment_count());
        EXPECT_EQ(0U, destructed);
        // Empty to non-empty. Destructor is invoked.
        opt1 = nullopt;
        EXPECT_FALSE(opt1);
        EXPECT_TRUE(opt2);
        EXPECT_EQ((T::dtor_policy_value == policy_nontrivial) ? 1 : 0, destructed);
        opt2 = std::move(opt1);
        EXPECT_FALSE(opt1);
        EXPECT_FALSE(opt2);
        EXPECT_EQ((T::dtor_policy_value == policy_nontrivial) ? 2 : 0, destructed);
    }
};
template <typename T, std::uint8_t CopyAssignmentPolicy, std::uint8_t MoveAssignmentPolicy>
struct test_assignment_3<T, policy_deleted, CopyAssignmentPolicy, policy_deleted, MoveAssignmentPolicy>
{
    static_assert(std::is_copy_assignable<T>::value == (CopyAssignmentPolicy != policy_deleted), "");
    static_assert(!std::is_copy_constructible<T>::value, "");
    static_assert(std::is_move_assignable<T>::value, "");  // requires either copy or move assignment
    static_assert(!std::is_move_constructible<T>::value, "");
    static_assert(!std::is_copy_constructible<optional<T>>::value, "");
    static_assert(!std::is_copy_assignable<optional<T>>::value, "");
    static_assert(!std::is_move_constructible<optional<T>>::value, "");
    static_assert(!std::is_move_assignable<optional<T>>::value, "");
    static void test() {}
};
template <typename T, std::uint8_t CopyCtorPolicy, std::uint8_t MoveCtorPolicy>
struct test_assignment_3<T, CopyCtorPolicy, policy_deleted, MoveCtorPolicy, policy_deleted>
{
    static_assert(!std::is_copy_assignable<T>::value, "");
    static_assert(std::is_copy_constructible<T>::value == (CopyCtorPolicy != policy_deleted), "");
    static_assert(!std::is_move_assignable<T>::value, "");
    static_assert(std::is_move_constructible<T>::value, "");  // requires either copy or move ctor
    static_assert(std::is_copy_constructible<optional<T>>::value == (CopyCtorPolicy != policy_deleted), "");
    static_assert(!std::is_copy_assignable<optional<T>>::value, "");
    static_assert(std::is_move_constructible<optional<T>>::value, "");  // requires either copy or move ctor
    static_assert(!std::is_move_assignable<optional<T>>::value, "");
    static void test() {}
};
template <typename T>
struct test_assignment_3<T, policy_deleted, policy_deleted, policy_deleted, policy_deleted>
{
    static_assert(!std::is_copy_assignable<T>::value, "");
    static_assert(!std::is_copy_constructible<T>::value, "");
    static_assert(!std::is_move_assignable<T>::value, "");
    static_assert(!std::is_move_constructible<T>::value, "");
    static_assert(!std::is_copy_constructible<optional<T>>::value, "");
    static_assert(!std::is_copy_assignable<optional<T>>::value, "");
    static_assert(!std::is_move_constructible<optional<T>>::value, "");
    static_assert(!std::is_move_assignable<optional<T>>::value, "");
    static void test() {}
};

TYPED_TEST(test_optional_combinations, assignment_3)
{
    test_assignment_3<TypeParam>::test();
}

/// ------------------------------------------------------------------------------------------------

TYPED_TEST(test_optional_combinations, assignment_4)
{
    struct value_type final : TypeParam
    {
        explicit value_type(const std::int64_t val) noexcept
            : value{val}
        {
        }
        value_type& operator=(const std::int64_t val) noexcept
        {
            value = val;
            return *this;
        }
        std::int64_t value;
    };
    std::uint32_t        dtor = 0;
    optional<value_type> v1;
    // Assign empty. Copy or move ctor is invoked.
    v1 = 12345;
    v1.value().configure_destruction_counter(&dtor);
    EXPECT_TRUE(v1.has_value());
    EXPECT_EQ(12345, v1.value().value);
    EXPECT_EQ(0, v1->get_copy_ctor_count());
    EXPECT_EQ(0, v1->get_move_ctor_count());
    EXPECT_EQ(0, v1->get_copy_assignment_count());
    EXPECT_EQ(0, v1->get_move_assignment_count());
    EXPECT_EQ(0, dtor);
    // Assign non-empty.
    v1 = 23456;
    EXPECT_TRUE(v1);
    EXPECT_EQ(23456, v1.value().value);
    EXPECT_EQ(0, v1->get_copy_ctor_count());
    EXPECT_EQ(0, v1->get_move_ctor_count());
    EXPECT_EQ(0, v1->get_copy_assignment_count());
    EXPECT_EQ(0, v1->get_move_assignment_count());
    EXPECT_EQ(0, dtor);
    // Drop.
    v1 = nullopt;
    EXPECT_FALSE(v1);
    EXPECT_EQ((TypeParam::dtor_policy_value == policy_nontrivial) ? 1 : 0, dtor);
}

/// ------------------------------------------------------------------------------------------------

TYPED_TEST(test_optional_combinations, assignment_5)
{
    struct From final : TypeParam
    {
        explicit From(const std::int64_t val) noexcept
            : value{val}
        {
        }
        std::int64_t value;
    };
    struct To final : TypeParam
    {
        explicit To(const std::int64_t val) noexcept
            : value{val}
        {
        }
        explicit To(const From& val) noexcept
            : value{val.value}
        {
        }
        To& operator=(const From& val) noexcept
        {
            value = val.value;
            return *this;
        }
        std::int64_t value;
    };
    static_assert(std::is_assignable<To, From>::value, "");
    static_assert(std::is_assignable<optional<To>, optional<From>>::value, "");
    std::uint32_t  a_dtor = 0;
    std::uint32_t  b_dtor = 0;
    optional<To>   a;
    optional<From> b;
    EXPECT_FALSE(a);
    EXPECT_FALSE(b);
    // Assign empty to empty.
    a = b;
    EXPECT_FALSE(a);
    EXPECT_FALSE(b);
    // Non-empty to empty.
    b.emplace(12345).configure_destruction_counter(&b_dtor);
    EXPECT_TRUE(b);
    EXPECT_FALSE(a);
    a = b;
    EXPECT_TRUE(a);
    EXPECT_TRUE(b);
    a.value().configure_destruction_counter(&a_dtor);
    EXPECT_EQ(12345, a.value().value);
    EXPECT_EQ(12345, b.value().value);
    EXPECT_EQ(0, a->get_copy_ctor_count());
    EXPECT_EQ(0, a->get_move_ctor_count());
    EXPECT_EQ(0, a->get_copy_assignment_count());
    EXPECT_EQ(0, a->get_move_assignment_count());
    EXPECT_EQ(0, a_dtor);
    EXPECT_EQ(0, b->get_copy_ctor_count());
    EXPECT_EQ(0, b->get_move_ctor_count());
    EXPECT_EQ(0, b->get_copy_assignment_count());
    EXPECT_EQ(0, b->get_move_assignment_count());
    EXPECT_EQ(0, b_dtor);
    // Non-empty to non-empty.
    b.value().value = 23456;
    EXPECT_EQ(12345, a.value().value);
    EXPECT_EQ(23456, b.value().value);
    a = b;
    EXPECT_TRUE(a);
    EXPECT_TRUE(b);
    EXPECT_EQ(23456, a.value().value);
    EXPECT_EQ(23456, b.value().value);
    EXPECT_EQ(0, a->get_copy_ctor_count());
    EXPECT_EQ(0, a->get_move_ctor_count());
    EXPECT_EQ(0, a->get_copy_assignment_count());
    EXPECT_EQ(0, a->get_move_assignment_count());
    EXPECT_EQ(0, a_dtor);
    EXPECT_EQ(0, b->get_copy_ctor_count());
    EXPECT_EQ(0, b->get_move_ctor_count());
    EXPECT_EQ(0, b->get_copy_assignment_count());
    EXPECT_EQ(0, b->get_move_assignment_count());
    EXPECT_EQ(0, b_dtor);
    // Empty to non-empty.
    b = nullopt;
    EXPECT_TRUE(a);
    EXPECT_FALSE(b);
    EXPECT_EQ((TypeParam::dtor_policy_value == policy_nontrivial) ? 1 : 0, b_dtor);
    a = b;
    EXPECT_FALSE(a);
    EXPECT_FALSE(b);
    EXPECT_EQ((TypeParam::dtor_policy_value == policy_nontrivial) ? 1 : 0, a_dtor);
    EXPECT_EQ((TypeParam::dtor_policy_value == policy_nontrivial) ? 1 : 0, b_dtor);
}

/// ------------------------------------------------------------------------------------------------

TYPED_TEST(test_optional_combinations, assignment_6)
{
    struct From final : TypeParam
    {
        explicit From(const std::int64_t val) noexcept
            : value{val}
        {
        }
        std::int64_t value;
    };
    struct To final : TypeParam
    {
        explicit To(const std::int64_t val) noexcept
            : value{val}
        {
        }
        explicit To(const From& other) noexcept;  // Intentionally not implemented.
        explicit To(From&& other) noexcept
            : value{other.value}
        {
            other.value = 0;
        }
        To& operator=(const From&) noexcept;  // Intentionally not implemented.
        To& operator=(From&& other) noexcept
        {
            value       = other.value;
            other.value = 0;
            return *this;
        }
        std::int64_t value;
    };
    std::uint32_t  a_dtor = 0;
    std::uint32_t  b_dtor = 0;
    optional<To>   a;
    optional<From> b;
    EXPECT_FALSE(a);
    EXPECT_FALSE(b);
    // Assign empty to empty.
    a = std::move(b);
    EXPECT_FALSE(a);
    EXPECT_FALSE(b);
    // Non-empty to empty.
    b.emplace(12345).configure_destruction_counter(&b_dtor);
    EXPECT_TRUE(b);
    EXPECT_FALSE(a);
    a = std::move(b);
    EXPECT_TRUE(a);
    EXPECT_TRUE(b);
    a.value().configure_destruction_counter(&a_dtor);
    EXPECT_EQ(12345, a.value().value);
    EXPECT_EQ(0, b.value().value);  // Moving zeroes the source.
    EXPECT_EQ(0, a->get_copy_ctor_count());
    EXPECT_EQ(0, a->get_move_ctor_count());
    EXPECT_EQ(0, a->get_copy_assignment_count());
    EXPECT_EQ(0, a->get_move_assignment_count());
    EXPECT_EQ(0, a_dtor);
    EXPECT_EQ(0, b->get_copy_ctor_count());
    EXPECT_EQ(0, b->get_move_ctor_count());
    EXPECT_EQ(0, b->get_copy_assignment_count());
    EXPECT_EQ(0, b->get_move_assignment_count());
    EXPECT_EQ(0, b_dtor);
    // Non-empty to non-empty.
    b.value().value = 23456;
    EXPECT_EQ(12345, a.value().value);
    EXPECT_EQ(23456, b.value().value);
    a = std::move(b);
    EXPECT_TRUE(a);
    EXPECT_TRUE(b);
    EXPECT_EQ(23456, a.value().value);
    EXPECT_EQ(0, b.value().value);  // Moving zeroes the source.
    EXPECT_EQ(0, a->get_copy_ctor_count());
    EXPECT_EQ(0, a->get_move_ctor_count());
    EXPECT_EQ(0, a->get_copy_assignment_count());
    EXPECT_EQ(0, a->get_move_assignment_count());
    EXPECT_EQ(0, a_dtor);
    EXPECT_EQ(0, b->get_copy_ctor_count());
    EXPECT_EQ(0, b->get_move_ctor_count());
    EXPECT_EQ(0, b->get_copy_assignment_count());
    EXPECT_EQ(0, b->get_move_assignment_count());
    EXPECT_EQ(0, b_dtor);
    // Empty to non-empty.
    b = nullopt;
    EXPECT_TRUE(a);
    EXPECT_FALSE(b);
    EXPECT_EQ((TypeParam::dtor_policy_value == policy_nontrivial) ? 1 : 0, b_dtor);
    a = std::move(b);
    EXPECT_FALSE(a);
    EXPECT_FALSE(b);
    EXPECT_EQ((TypeParam::dtor_policy_value == policy_nontrivial) ? 1 : 0, a_dtor);
    EXPECT_EQ((TypeParam::dtor_policy_value == policy_nontrivial) ? 1 : 0, b_dtor);
}

/// ------------------------------------------------------------------------------------------------

TEST(test_optional, swap)
{
    optional<std::int64_t> a(in_place, 12345);
    optional<std::int64_t> b(in_place, 23456);
    // Non-empty with non-empty.
    a.swap(b);
    EXPECT_TRUE(a);
    EXPECT_TRUE(b);
    EXPECT_EQ(23456, a.value());
    EXPECT_EQ(12345, b.value());
    // Empty with non-empty.
    a = nullopt;
    a.swap(b);
    EXPECT_TRUE(a);
    EXPECT_FALSE(b);
    EXPECT_EQ(12345, a.value());
    a.swap(b);
    EXPECT_FALSE(a);
    EXPECT_TRUE(b);
    EXPECT_EQ(12345, b.value());
    // Empty with empty.
    b = nullopt;
    a.swap(b);
    EXPECT_FALSE(a);
    EXPECT_FALSE(b);
}

/// ------------------------------------------------------------------------------------------------

TEST(test_optional, make_optional_1)
{
    const std::int64_t           value = 123456;
    const optional<std::int64_t> a     = cetl::pf17::make_optional(static_cast<const std::int64_t&>(value));
    EXPECT_TRUE(a);
    EXPECT_EQ(123456, a.value());
}

TEST(test_optional, make_optional_2)
{
    struct value_type
    {
        value_type(const std::int64_t a, const std::int64_t b)
            : a(a)
            , b(b)
        {
        }
        std::int64_t a;
        std::int64_t b;
    };
    const optional<value_type> opt = cetl::pf17::make_optional<value_type>(123, 456);
    EXPECT_TRUE(opt);
    EXPECT_EQ(123, opt.value().a);
    EXPECT_EQ(456, opt.value().b);
}

TEST(test_optional, make_optional_3)
{
    struct value_type
    {
        value_type(const std::initializer_list<std::int64_t> list, const std::int64_t b)
            : a(list.size())
            , b(b)
        {
        }
        std::size_t  a;
        std::int64_t b;
    };
    const optional<value_type> opt =
        cetl::pf17::make_optional<value_type>(std::initializer_list<std::int64_t>{1, 2, 3, 4, 5}, 123);
    EXPECT_TRUE(opt);
    EXPECT_EQ(5, opt.value().a);
    EXPECT_EQ(123, opt.value().b);
}

/// ------------------------------------------------------------------------------------------------

TEST(test_optional, value_or)
{
    optional<std::int64_t> a = 12345;
    EXPECT_EQ(12345, a.value_or(23456));
    a = nullopt;
    EXPECT_EQ(23456, a.value_or(23456));
    EXPECT_EQ(12345, (optional<std::int64_t>{12345}).value_or(23456));
    EXPECT_EQ(23456, (optional<std::int64_t>{}).value_or(23456));
}

/// ------------------------------------------------------------------------------------------------

/// This wrapper is used to test the comparison operators. It is made non-copyable and non-movable to ensure that the
/// comparison operators are not using the copy/move constructors or assignment operators.
template <typename T>
struct comparable final
{
    explicit constexpr comparable(const T& value)
        : value(value)
    {
    }
    comparable(const comparable&)            = delete;
    comparable(comparable&&)                 = delete;
    comparable& operator=(const comparable&) = delete;
    comparable& operator=(comparable&&)      = delete;
    ~comparable()                            = default;
    T value;
};

template <typename T, typename U>
constexpr bool operator==(const comparable<T>& lhs, const comparable<U>& rhs)
{
    return lhs.value == rhs.value;
}
template <typename T, typename U>
constexpr bool operator!=(const comparable<T>& lhs, const comparable<U>& rhs)
{
    return lhs.value != rhs.value;
}
template <typename T, typename U>
constexpr bool operator<(const comparable<T>& lhs, const comparable<U>& rhs)
{
    return lhs.value < rhs.value;
}
template <typename T, typename U>
constexpr bool operator<=(const comparable<T>& lhs, const comparable<U>& rhs)
{
    return lhs.value <= rhs.value;
}
template <typename T, typename U>
constexpr bool operator>(const comparable<T>& lhs, const comparable<U>& rhs)
{
    return lhs.value > rhs.value;
}
template <typename T, typename U>
constexpr bool operator>=(const comparable<T>& lhs, const comparable<U>& rhs)
{
    return lhs.value >= rhs.value;
}

TEST(test_optional, comparison_optional_to_optional)
{
    using A = optional<comparable<std::int64_t>>;
    using B = optional<comparable<std::int32_t>>;
    // ==
    EXPECT_TRUE(A{} == B{});
    EXPECT_TRUE(A{10} == B{10});
    EXPECT_FALSE(A{} == B{10});
    EXPECT_FALSE(A{10} == B{});
    // !=
    EXPECT_FALSE(A{} != B{});
    EXPECT_FALSE(A{10} != B{10});
    EXPECT_TRUE(A{} != B{10});
    EXPECT_TRUE(A{10} != B{});
    // <
    EXPECT_FALSE(A{} < B{});
    EXPECT_FALSE(A{10} < B{10});
    EXPECT_TRUE(A{} < B{10});
    EXPECT_FALSE(A{10} < B{});
    // <=
    EXPECT_TRUE(A{} <= B{});
    EXPECT_TRUE(A{10} <= B{10});
    EXPECT_TRUE(A{} <= B{10});
    EXPECT_FALSE(A{10} <= B{});
    // >
    EXPECT_FALSE(A{} > B{});
    EXPECT_FALSE(A{10} > B{10});
    EXPECT_FALSE(A{} > B{10});
    EXPECT_TRUE(A{10} > B{});
    // >=
    EXPECT_TRUE(A{} >= B{});
    EXPECT_TRUE(A{10} >= B{10});
    EXPECT_FALSE(A{} >= B{10});
    EXPECT_TRUE(A{10} >= B{});
}

TEST(test_optional, comparison_optional_to_nullopt)
{
    using A = optional<comparable<std::int64_t>>;
    // ==
    EXPECT_TRUE(A{} == nullopt);
    EXPECT_TRUE(nullopt == A{});
    EXPECT_FALSE(A{10} == nullopt);
    EXPECT_FALSE(nullopt == A{10});
    // !=
    EXPECT_FALSE(A{} != nullopt);
    EXPECT_FALSE(nullopt != A{});
    EXPECT_TRUE(A{10} != nullopt);
    EXPECT_TRUE(nullopt != A{10});
    // <
    EXPECT_FALSE(A{} < nullopt);
    EXPECT_FALSE(nullopt < A{});
    EXPECT_FALSE(A{10} < nullopt);
    EXPECT_TRUE(nullopt < A{10});
    // <=
    EXPECT_TRUE(A{} <= nullopt);
    EXPECT_TRUE(nullopt <= A{});
    EXPECT_FALSE(A{10} <= nullopt);
    EXPECT_TRUE(nullopt <= A{10});
    // >
    EXPECT_FALSE(A{} > nullopt);
    EXPECT_FALSE(nullopt > A{});
    EXPECT_TRUE(A{10} > nullopt);
    EXPECT_FALSE(nullopt > A{10});
    // >=
    EXPECT_TRUE(A{} >= nullopt);
    EXPECT_TRUE(nullopt >= A{});
    EXPECT_TRUE(A{10} >= nullopt);
    EXPECT_FALSE(nullopt >= A{10});
}

TEST(test_optional, comparison_optional_to_value)
{
    using A = optional<std::int64_t>;
    // ==
    EXPECT_FALSE(A{} == 10);
    EXPECT_FALSE(10 == A{});
    EXPECT_TRUE(A{10} == 10);
    EXPECT_TRUE(10 == A{10});
    EXPECT_FALSE(A{10} == 0);
    EXPECT_FALSE(0 == A{10});
    // !=
    EXPECT_TRUE(A{} != 10);
    EXPECT_TRUE(10 != A{});
    EXPECT_FALSE(A{10} != 10);
    EXPECT_FALSE(10 != A{10});
    EXPECT_TRUE(A{10} != 0);
    EXPECT_TRUE(0 != A{10});
    // <
    EXPECT_TRUE(A{} < 10);
    EXPECT_FALSE(10 < A{});
    EXPECT_FALSE(A{10} < 10);
    EXPECT_FALSE(10 < A{10});
    EXPECT_FALSE(A{10} < 0);
    EXPECT_TRUE(0 < A{10});
    // <=
    EXPECT_TRUE(A{} <= 10);
    EXPECT_FALSE(10 <= A{});
    EXPECT_TRUE(A{10} <= 10);
    EXPECT_TRUE(10 <= A{10});
    EXPECT_FALSE(A{10} <= 0);
    EXPECT_TRUE(0 <= A{10});
    // >
    EXPECT_FALSE(A{} > 10);
    EXPECT_TRUE(10 > A{});
    EXPECT_FALSE(A{10} > 10);
    EXPECT_FALSE(10 > A{10});
    EXPECT_TRUE(A{10} > 0);
    EXPECT_FALSE(0 > A{10});
    // >=
    EXPECT_FALSE(A{} >= 10);
    EXPECT_TRUE(10 >= A{});
    EXPECT_TRUE(A{10} >= 10);
    EXPECT_TRUE(10 >= A{10});
    EXPECT_TRUE(A{10} >= 0);
    EXPECT_FALSE(0 >= A{10});
}

// NOLINTEND(*-use-after-move)
