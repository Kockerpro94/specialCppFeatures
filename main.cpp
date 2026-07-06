// =====================================================================
// SUPER SPECIAL FEATURES AND GOOD THINGS - ULTIMATE C++ SOURCE FILE
// This file demonstrates a staggering number of advanced C++ techniques
// all working together in perfect harmony to achieve absolutely 
// magnificent and unquestionably useful results.
// Compile with: g++ -std=c++20 -O2 super_special_features.cpp -o super
// =====================================================================

#include <iostream>
#include <type_traits>
#include <tuple>
#include <variant>
#include <functional>
#include <memory>
#include <optional>
#include <array>
#include <string_view>
#include <utility>
#include <coroutine>
#include <cstddef>
#include <concepts>

// =====================================================================
// SECTION 1: COMPILE-TIME MATHEMATICAL MARVELS
// =====================================================================
namespace SuperMath {

    // Constexpr factorial – because runtime is too mainstream
    template<std::size_t N>
    struct Factorial {
        static constexpr std::size_t value = N * Factorial<N - 1>::value;
    };

    template<>
    struct Factorial<0> {
        static constexpr std::size_t value = 1;
    };

    template<std::size_t N>
    inline constexpr std::size_t factorial_v = Factorial<N>::value;

    // Constexpr Fibonacci – now with O(1) compile-time lookup (just kidding, O(N))
    template<std::size_t N>
    struct Fibonacci {
        static constexpr std::size_t value = Fibonacci<N - 1>::value + Fibonacci<N - 2>::value;
    };

    template<>
    struct Fibonacci<0> { static constexpr std::size_t value = 0; };

    template<>
    struct Fibonacci<1> { static constexpr std::size_t value = 1; };

    template<std::size_t N>
    inline constexpr std::size_t fibonacci_v = Fibonacci<N>::value;

    // Constexpr GCD (Euclid's algorithm) – invaluable when you need GCD at compile time.
    constexpr std::size_t gcd(std::size_t a, std::size_t b) {
        while (b != 0) {
            auto t = b;
            b = a % b;
            a = t;
        }
        return a;
    }

    // Constexpr prime check – for those truly sophisticated compile-time decisions.
    constexpr bool is_prime(std::size_t n) {
        if (n < 2) return false;
        for (std::size_t i = 2; i * i <= n; ++i)
            if (n % i == 0) return false;
        return true;
    }

    // Because the answer to everything must be available at compile time.
    constexpr std::size_t the_answer = 42;
    static_assert(is_prime(the_answer) == false, "42 is not prime, but that's okay.");
}

// =====================================================================
// SECTION 2: TYPE LIST AND META-ALGORITHMS (THE TYPELIST MONAD)
// =====================================================================
namespace TypeListMagic {

    // The fundamental type list – a classic variadic template.
    template<typename... Ts>
    struct TypeList {};

    // Length of a type list.
    template<typename List>
    struct Length;

    template<typename... Ts>
    struct Length<TypeList<Ts...>> : std::integral_constant<std::size_t, sizeof...(Ts)> {};

    template<typename List>
    inline constexpr std::size_t length_v = Length<List>::value;

    // Indexed access (type at I).
    template<std::size_t I, typename List>
    struct At;

    template<std::size_t I, typename T, typename... Ts>
    struct At<I, TypeList<T, Ts...>> : At<I - 1, TypeList<Ts...>> {};

    template<typename T, typename... Ts>
    struct At<0, TypeList<T, Ts...>> {
        using type = T;
    };

    template<std::size_t I, typename List>
    using at_t = typename At<I, List>::type;

    // Concatenate two type lists.
    template<typename L1, typename L2>
    struct Concat;

    template<typename... Ts, typename... Us>
    struct Concat<TypeList<Ts...>, TypeList<Us...>> {
        using type = TypeList<Ts..., Us...>;
    };

    template<typename L1, typename L2>
    using concat_t = typename Concat<L1, L2>::type;

    // Push back a type (because push_front is too simple).
    template<typename List, typename T>
    struct PushBack;

    template<typename... Ts, typename T>
    struct PushBack<TypeList<Ts...>, T> {
        using type = TypeList<Ts..., T>;
    };

    template<typename List, typename T>
    using push_back_t = typename PushBack<List, T>::type;

    // Filter: keep types that satisfy a predicate.
    template<template<typename> class Pred, typename List>
    struct Filter;

    template<template<typename> class Pred, typename... Ts>
    struct Filter<Pred, TypeList<Ts...>> {
        using type = TypeList<>;
        // Just kidding – we need a real implementation using recursion.
    };

    // Better Filter implementation
    template<template<typename> class Pred, typename List, typename = void>
    struct FilterImpl;

    template<template<typename> class Pred>
    struct FilterImpl<Pred, TypeList<>, void> {
        using type = TypeList<>;
    };

    template<template<typename> class Pred, typename T, typename... Rest>
    struct FilterImpl<Pred, TypeList<T, Rest...>, std::enable_if_t<Pred<T>::value>> {
        using type = push_back_t<typename FilterImpl<Pred, TypeList<Rest...>>::type, T>;
    };

    template<template<typename> class Pred, typename T, typename... Rest>
    struct FilterImpl<Pred, TypeList<T, Rest...>, std::enable_if_t<!Pred<T>::value>> {
        using type = typename FilterImpl<Pred, TypeList<Rest...>>::type;
    };

    template<template<typename> class Pred, typename List>
    using filter_t = typename FilterImpl<Pred, List>::type;

    // A trivial predicate: is_integral
    template<typename T>
    using is_integral_pred = std::is_integral<T>;

    // Let's test filtering: only integral types from a mix.
    using MixedList = TypeList<int, double, char, float, long, std::string>;
    using IntegralOnly = filter_t<is_integral_pred, MixedList>;
    static_assert(length_v<IntegralOnly> == 3, "Filter works like magic!");

    // Map: transform a type list with a meta-function.
    template<template<typename> class Func, typename List>
    struct Map;

    template<template<typename> class Func, typename... Ts>
    struct Map<Func, TypeList<Ts...>> {
        using type = TypeList<typename Func<Ts>::type...>;
    };

    template<template<typename> class Func, typename List>
    using map_t = typename Map<Func, List>::type;

    // A meta-function to add const.
    template<typename T>
    struct AddConst { using type = const T; };

    using ConstList = map_t<AddConst, IntegralOnly>;
    static_assert(std::is_same_v<at_t<0, ConstList>, const int>, "Map works!");
}

// =====================================================================
// SECTION 3: CRTP – CURIOUSLY RECURRING TEMPLATE PATTERN FOR GOODNESS
// =====================================================================
namespace CRTPMagic {

    // Base class that injects operator== and operator!= using CRTP.
    template<typename Derived>
    class EqualityComparable {
    public:
        friend bool operator==(const Derived& lhs, const Derived& rhs) {
            return lhs.equals(rhs);
        }
        friend bool operator!=(const Derived& lhs, const Derived& rhs) {
            return !(lhs == rhs);
        }
    };

    // A concrete class that becomes comparable.
    class SuperInt : public EqualityComparable<SuperInt> {
        int val;
    public:
        explicit SuperInt(int v) : val(v) {}
        bool equals(const SuperInt& other) const {
            return val == other.val;
        }
        int get() const { return val; }
    };

    // Cloneable CRTP – creates a clone() method.
    template<typename Derived>
    class Cloneable {
    public:
        std::unique_ptr<Derived> clone() const {
            return std::make_unique<Derived>(static_cast<const Derived&>(*this));
        }
    };

    class ClonableThing : public Cloneable<ClonableThing> {
        std::string name;
    public:
        explicit ClonableThing(std::string n) : name(std::move(n)) {}
        const std::string& getName() const { return name; }
    };
}

// =====================================================================
// SECTION 4: POLICY-BASED SMART POINTER (ULTIMATE CONTROL)
// =====================================================================
namespace PolicyBased {

    // Deletion policies.
    struct DeleteByDelete {
        template<typename T>
        void operator()(T* ptr) const { delete ptr; }
    };

    struct DeleteByFree {
        template<typename T>
        void operator()(T* ptr) const { free(ptr); }
    };

    template<typename T, typename DeletionPolicy = DeleteByDelete>
    class UltimatePtr {
        T* ptr;
        DeletionPolicy deleter;
    public:
        explicit UltimatePtr(T* p = nullptr) : ptr(p) {}
        ~UltimatePtr() { if (ptr) deleter(ptr); }
        T* operator->() const { return ptr; }
        T& operator*() const { return *ptr; }
        // Non-copyable, movable...
        UltimatePtr(const UltimatePtr&) = delete;
        UltimatePtr& operator=(const UltimatePtr&) = delete;
        UltimatePtr(UltimatePtr&& other) noexcept : ptr(other.ptr) {
            other.ptr = nullptr;
        }
        // etc.
    };
}

// =====================================================================
// SECTION 5: THE SINGLETON – THE ONE AND ONLY
// =====================================================================
namespace TheOne {

    template<typename T>
    class Singleton {
    public:
        static T& instance() {
            static T inst;
            return inst;
        }
    protected:
        Singleton() = default;
    };

    // A concrete singleton with super powers.
    class UltimateManager : public Singleton<UltimateManager> {
        friend class Singleton<UltimateManager>;
        int secret;
        UltimateManager() : secret(42) {}
    public:
        int getSecret() const { return secret; }
        void doSomethingSpecial() {
            std::cout << "UltimateManager doing something special...\n";
        }
    };
}

// =====================================================================
// SECTION 6: VARIANT VISITOR PATTERN WITH LAMBDAS
// =====================================================================
namespace VariantVisitor {

    using SuperVariant = std::variant<int, double, std::string, char>;

    void visitVariant(const SuperVariant& v) {
        std::visit([](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, int>)
                std::cout << "It's an int: " << val << '\n';
            else if constexpr (std::is_same_v<T, double>)
                std::cout << "It's a double: " << val << '\n';
            else if constexpr (std::is_same_v<T, std::string>)
                std::cout << "It's a string: " << val << '\n';
            else if constexpr (std::is_same_v<T, char>)
                std::cout << "It's a char: " << val << '\n';
        }, v);
    }
}

// =====================================================================
// SECTION 7: CUSTOM OPTIONAL (BECAUSE STD::OPTIONAL IS TOO MAINSTREAM)
// =====================================================================
namespace MyOptional {

    template<typename T>
    class Optional {
        alignas(T) unsigned char data[sizeof(T)];
        bool has_value = false;
    public:
        Optional() = default;
        Optional(const T& value) {
            new (data) T(value);
            has_value = true;
        }
        Optional(Optional&& other) noexcept : has_value(other.has_value) {
            if (other.has_value) {
                new (data) T(std::move(*other));
                other.reset();
            }
        }
        ~Optional() { reset(); }
        void reset() {
            if (has_value) {
                reinterpret_cast<T*>(data)->~T();
                has_value = false;
            }
        }
        T& value() {
            if (!has_value) throw std::runtime_error("Bad optional access");
            return *reinterpret_cast<T*>(data);
        }
        explicit operator bool() const { return has_value; }
    };
}

// =====================================================================
// SECTION 8: COMPILE-TIME STRING HASHING AND FIZZBUZZ GENERATION
// =====================================================================
namespace CompileTimeStr {

    // Compile-time string hasher (FNV-1a) – extremely necessary.
    constexpr std::size_t fnv1a_hash(const char* s, std::size_t h = 0xcbf29ce484222325ULL) {
        return (*s == '\0') ? h : fnv1a_hash(s + 1, (h ^ static_cast<std::size_t>(*s)) * 0x100000001b3ULL);
    }

    // Compile-time FizzBuzz stored in std::array for runtime display.
    template<std::size_t N>
    constexpr auto generate_fizzbuzz() {
        std::array<std::string_view, N> result{};
        for (std::size_t i = 1; i <= N; ++i) {
            if (i % 15 == 0) result[i-1] = "FizzBuzz";
            else if (i % 3 == 0) result[i-1] = "Fizz";
            else if (i % 5 == 0) result[i-1] = "Buzz";
            else result[i-1] = "number"; // we'll format later
        }
        return result;
    }

    constexpr auto fizzbuzz20 = generate_fizzbuzz<20>();
}

// =====================================================================
// SECTION 9: COROUTINE THAT YIELDS ENDLESS GOODNESS
// =====================================================================
namespace CoroutineGen {

    struct Generator {
        struct promise_type {
            int current_value;
            std::suspend_always yield_value(int value) {
                current_value = value;
                return {};
            }
            std::suspend_always initial_suspend() { return {}; }
            std::suspend_always final_suspend() noexcept { return {}; }
            Generator get_return_object() { return Generator{*this}; }
            void unhandled_exception() { std::terminate(); }
            void return_void() {}
        };

        using handle_type = std::coroutine_handle<promise_type>;
        handle_type coro;

        Generator(promise_type& p) : coro(handle_type::from_promise(p)) {}
        Generator(Generator&& other) noexcept : coro(other.coro) { other.coro = nullptr; }
        ~Generator() { if (coro) coro.destroy(); }

        int next() {
            coro.resume();
            return coro.promise().current_value;
        }
        bool done() const {
            return coro.done();
        }
    };

    Generator infiniteSequence() {
        int i = 0;
        while (true) {
            co_yield i++;
        }
    }
}

// =====================================================================
// SECTION 10: MAIN – WHERE EVERYTHING GLORIOUSLY COMES TOGETHER
// =====================================================================
int main() {
    std::cout << "===== SUPER SPECIAL FEATURES EXECUTION START =====\n\n";

    // Compile-time math showcase
    constexpr std::size_t fact10 = SuperMath::factorial_v<10>;
    constexpr std::size_t fib30  = SuperMath::fibonacci_v<30>;
    constexpr std::size_t gcd_val = SuperMath::gcd(1071, 462);
    constexpr bool prime_97 = SuperMath::is_prime(97);

    std::cout << "Compile-time Factorial(10)   = " << fact10 << '\n';
    std::cout << "Compile-time Fibonacci(30)   = " << fib30 << '\n';
    std::cout << "Compile-time GCD(1071,462)   = " << gcd_val << '\n';
    std::cout << "Compile-time Is 97 prime?    = " << (prime_97 ? "Yes" : "No") << '\n';
    std::cout << "The answer to everything     = " << SuperMath::the_answer << "\n\n";

    // Type list magic
    using MyList = TypeListMagic::TypeList<int, double, char, float, long>;
    std::cout << "Type list length: " << TypeListMagic::length_v<MyList> << '\n';
    using FirstType = TypeListMagic::at_t<0, MyList>;
    std::cout << "First type is int: " << std::is_same_v<FirstType, int> << '\n';
    std::cout << "Integral-only list size: " << TypeListMagic::length_v<TypeListMagic::IntegralOnly> << "\n\n";

    // CRTP
    CRTPMagic::SuperInt a(10), b(10), c(20);
    std::cout << "CRTP equality: a == b = " << (a == b) << ", a != c = " << (a != c) << '\n';
    auto cloneable = std::make_unique<CRTPMagic::ClonableThing>("Original");
    auto clone = cloneable->clone();
    std::cout << "Cloned name: " << clone->getName() << "\n\n";

    // Policy-based pointer
    PolicyBased::UltimatePtr<int> ptr(new int(100));
    std::cout << "UltimatePtr holds: " << *ptr << "\n\n";

    // Singleton
    TheOne::UltimateManager::instance().doSomethingSpecial();
    std::cout << "Singleton secret: " << TheOne::UltimateManager::instance().getSecret() << "\n\n";

    // Variant visitor
    VariantVisitor::SuperVariant v1 = 3.14;
    VariantVisitor::SuperVariant v2 = std::string("Hello, Variant!");
    VariantVisitor::visitVariant(v1);
    VariantVisitor::visitVariant(v2);

    // MyOptional
    MyOptional::Optional<int> opt;
    if (!opt) std::cout << "Optional is empty.\n";
    opt = 42;
    if (opt) std::cout << "Optional now has value: " << opt.value() << "\n\n";

    // Compile-time string hash
    constexpr auto hash = CompileTimeStr::fnv1a_hash("SuperSpecial");
    std::cout << "Compile-time hash of 'SuperSpecial': " << hash << '\n';

    // Compile-time FizzBuzz
    std::cout << "FizzBuzz up to 20 (compile-time generated):\n";
    for (std::size_t i = 0; i < 20; ++i) {
        auto s = CompileTimeStr::fizzbuzz20[i];
        std::cout << (i+1) << ": ";
        if (s == "number") std::cout << (i+1);
        else std::cout << s;
        std::cout << '\n';
    }

    // Coroutine demonstration (only first 5 values)
    auto gen = CoroutineGen::infiniteSequence();
    std::cout << "\nCoroutine infinite sequence (first 5): ";
    for (int i = 0; i < 5; ++i) {
        std::cout << gen.next() << ' ';
    }
    std::cout << "\n\n";

    // A ridiculous SFINAE + fold expression just because we can
    auto superFold = [](auto&&... args) {
        return (... + args); // fold expression
    };
    std::cout << "Super fold (1,2,3,4,5): " << superFold(1,2,3,4,5) << '\n';

    // Concepts (C++20) – ensure our fold works only with arithmetic types
    auto arithmeticFold = []<typename... Ts>(Ts... args) requires (std::is_arithmetic_v<Ts> && ...) {
        return (... + args);
    };
    std::cout << "Arithmetic fold (1.1, 2.2, 3.3): " << arithmeticFold(1.1, 2.2, 3.3) << '\n';

    std::cout << "\n===== ALL SUPER SPECIAL FEATURES EXECUTED SUCCESSFULLY =====" << std::endl;
    std::cout << "Everything is good. The world is a better place now." << std::endl;

    return 0;
}
