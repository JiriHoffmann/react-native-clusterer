#pragma once

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <memory>
#include <sstream>
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <cassert>
#include <cstddef>   // size_t
#include <new>       // operator new
#include <stdexcept> // runtime_error
#include <tuple>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <functional>

#ifdef DEBUG_TIMER
#include <chrono>
#include <iostream>
#endif

namespace mapbox
{
    namespace util
    {

        template <typename T>
        class recursive_wrapper
        {

            T *p_;

            void assign(T const &rhs)
            {
                this->get() = rhs;
            }

        public:
            using type = T;

            /**
             * Default constructor default initializes the internally stored value.
             * For POD types this means nothing is done and the storage is
             * uninitialized.
             *
             * @throws std::bad_alloc if there is insufficient memory for an object
             *         of type T.
             * @throws any exception thrown by the default constructur of T.
             */
            recursive_wrapper()
                : p_(new T) {}

            ~recursive_wrapper() noexcept { delete p_; }

            recursive_wrapper(recursive_wrapper const &operand)
                : p_(new T(operand.get())) {}

            recursive_wrapper(T const &operand)
                : p_(new T(operand)) {}

            recursive_wrapper(recursive_wrapper &&operand)
                : p_(new T(std::move(operand.get()))) {}

            recursive_wrapper(T &&operand)
                : p_(new T(std::move(operand))) {}

            inline recursive_wrapper &operator=(recursive_wrapper const &rhs)
            {
                assign(rhs.get());
                return *this;
            }

            inline recursive_wrapper &operator=(T const &rhs)
            {
                assign(rhs);
                return *this;
            }

            inline void swap(recursive_wrapper &operand) noexcept
            {
                T *temp = operand.p_;
                operand.p_ = p_;
                p_ = temp;
            }

            recursive_wrapper &operator=(recursive_wrapper &&rhs) noexcept
            {
                swap(rhs);
                return *this;
            }

            recursive_wrapper &operator=(T &&rhs)
            {
                get() = std::move(rhs);
                return *this;
            }

            T &get()
            {
                assert(p_);
                return *get_pointer();
            }

            T const &get() const
            {
                assert(p_);
                return *get_pointer();
            }

            T *get_pointer() { return p_; }

            const T *get_pointer() const { return p_; }

            operator T const &() const { return this->get(); }

            operator T &() { return this->get(); }

        }; // class recursive_wrapper

        template <typename T>
        inline void swap(recursive_wrapper<T> &lhs, recursive_wrapper<T> &rhs) noexcept
        {
            lhs.swap(rhs);
        }
    } // namespace util
} // namespace mapbox

namespace mapbox
{
    namespace util
    {

        template <typename... Fns>
        struct visitor;

        template <typename Fn>
        struct visitor<Fn> : Fn
        {
            using type = Fn;
            using Fn::operator();

            visitor(Fn fn) : Fn(fn) {}
        };

        template <typename Fn, typename... Fns>
        struct visitor<Fn, Fns...> : Fn, visitor<Fns...>
        {
            using type = visitor;
            using Fn::operator();
            using visitor<Fns...>::operator();

            visitor(Fn fn, Fns... fns) : Fn(fn), visitor<Fns...>(fns...) {}
        };

        template <typename... Fns>
        visitor<Fns...> make_visitor(Fns... fns)
        {
            return visitor<Fns...>(fns...);
        }

    } // namespace util
} // namespace mapbox

// clang-format off
// [[deprecated]] is only available in C++14, use this for the time being
#if __cplusplus <= 201103L
# ifdef __GNUC__
#  define MAPBOX_VARIANT_DEPRECATED __attribute__((deprecated))
# elif defined(_MSC_VER)
#  define MAPBOX_VARIANT_DEPRECATED __declspec(deprecated)
# else
#  define MAPBOX_VARIANT_DEPRECATED
# endif
#else
#  define MAPBOX_VARIANT_DEPRECATED [[deprecated]]
#endif


#ifdef _MSC_VER
// https://msdn.microsoft.com/en-us/library/bw1hbe6y.aspx
# ifdef NDEBUG
#  define VARIANT_INLINE __forceinline
# else
#  define VARIANT_INLINE //__declspec(noinline)
# endif
#else
# ifdef NDEBUG
#  define VARIANT_INLINE //inline __attribute__((always_inline))
# else
#  define VARIANT_INLINE __attribute__((noinline))
# endif
#endif
// clang-format on

// Exceptions
#if defined(__EXCEPTIONS) || defined(_MSC_VER)
#define HAS_EXCEPTIONS
#endif

#define VARIANT_MAJOR_VERSION 1
#define VARIANT_MINOR_VERSION 1
#define VARIANT_PATCH_VERSION 0

#define VARIANT_VERSION (VARIANT_MAJOR_VERSION * 100000) + (VARIANT_MINOR_VERSION * 100) + (VARIANT_PATCH_VERSION)

namespace mapbox
{
    namespace util
    {

        // XXX This should derive from std::logic_error instead of std::runtime_error.
        //     See https://github.com/mapbox/variant/issues/48 for details.
        class bad_variant_access : public std::runtime_error
        {

        public:
            explicit bad_variant_access(const std::string &what_arg)
                : runtime_error(what_arg) {}

            explicit bad_variant_access(const char *what_arg)
                : runtime_error(what_arg) {}

        }; // class bad_variant_access

        template <typename R = void>
        struct MAPBOX_VARIANT_DEPRECATED static_visitor
        {
            using result_type = R;

        protected:
            static_visitor() {}
            ~static_visitor() {}
        };

        namespace detail
        {

            static constexpr std::size_t invalid_value = std::size_t(-1);

            template <typename T, typename... Types>
            struct direct_type;

            template <typename T, typename First, typename... Types>
            struct direct_type<T, First, Types...>
            {
                static constexpr std::size_t index = std::is_same<T, First>::value
                                                         ? sizeof...(Types)
                                                         : direct_type<T, Types...>::index;
            };

            template <typename T>
            struct direct_type<T>
            {
                static constexpr std::size_t index = invalid_value;
            };

#if __cpp_lib_logical_traits >= 201510L

            using std::conjunction;
            using std::disjunction;

#else

            template <typename...>
            struct conjunction : std::true_type
            {
            };

            template <typename B1>
            struct conjunction<B1> : B1
            {
            };

            template <typename B1, typename B2>
            struct conjunction<B1, B2> : std::conditional<B1::value, B2, B1>::type
            {
            };

            template <typename B1, typename... Bs>
            struct conjunction<B1, Bs...> : std::conditional<B1::value, conjunction<Bs...>, B1>::type
            {
            };

            template <typename...>
            struct disjunction : std::false_type
            {
            };

            template <typename B1>
            struct disjunction<B1> : B1
            {
            };

            template <typename B1, typename B2>
            struct disjunction<B1, B2> : std::conditional<B1::value, B1, B2>::type
            {
            };

            template <typename B1, typename... Bs>
            struct disjunction<B1, Bs...> : std::conditional<B1::value, B1, disjunction<Bs...>>::type
            {
            };

#endif

            template <typename T, typename... Types>
            struct convertible_type;

            template <typename T, typename First, typename... Types>
            struct convertible_type<T, First, Types...>
            {
                static constexpr std::size_t index = std::is_convertible<T, First>::value
                                                         ? disjunction<std::is_convertible<T, Types>...>::value ? invalid_value : sizeof...(Types)
                                                         : convertible_type<T, Types...>::index;
            };

            template <typename T>
            struct convertible_type<T>
            {
                static constexpr std::size_t index = invalid_value;
            };

            template <typename T, typename... Types>
            struct value_traits
            {
                using value_type = typename std::remove_const<typename std::remove_reference<T>::type>::type;
                static constexpr std::size_t direct_index = direct_type<value_type, Types...>::index;
                static constexpr bool is_direct = direct_index != invalid_value;
                static constexpr std::size_t index = is_direct ? direct_index : convertible_type<value_type, Types...>::index;
                static constexpr bool is_valid = index != invalid_value;
                static constexpr std::size_t tindex = is_valid ? sizeof...(Types) - index : 0;
                using target_type = typename std::tuple_element<tindex, std::tuple<void, Types...>>::type;
            };

            template <typename T, typename R = void>
            struct enable_if_type
            {
                using type = R;
            };

            template <typename F, typename V, typename Enable = void>
            struct result_of_unary_visit
            {
                using type = typename std::result_of<F(V &)>::type;
            };

            template <typename F, typename V>
            struct result_of_unary_visit<F, V, typename enable_if_type<typename F::result_type>::type>
            {
                using type = typename F::result_type;
            };

            template <typename F, typename V, typename Enable = void>
            struct result_of_binary_visit
            {
                using type = typename std::result_of<F(V &, V &)>::type;
            };

            template <typename F, typename V>
            struct result_of_binary_visit<F, V, typename enable_if_type<typename F::result_type>::type>
            {
                using type = typename F::result_type;
            };

            template <std::size_t arg1, std::size_t... others>
            struct static_max;

            template <std::size_t arg>
            struct static_max<arg>
            {
                static const std::size_t value = arg;
            };

            template <std::size_t arg1, std::size_t arg2, std::size_t... others>
            struct static_max<arg1, arg2, others...>
            {
                static const std::size_t value = arg1 >= arg2 ? static_max<arg1, others...>::value : static_max<arg2, others...>::value;
            };

            template <typename... Types>
            struct variant_helper;

            template <typename T, typename... Types>
            struct variant_helper<T, Types...>
            {
                VARIANT_INLINE static void destroy(const std::size_t type_index, void *data)
                {
                    if (type_index == sizeof...(Types))
                    {
                        reinterpret_cast<T *>(data)->~T();
                    }
                    else
                    {
                        variant_helper<Types...>::destroy(type_index, data);
                    }
                }

                VARIANT_INLINE static void move(const std::size_t old_type_index, void *old_value, void *new_value)
                {
                    if (old_type_index == sizeof...(Types))
                    {
                        new (new_value) T(std::move(*reinterpret_cast<T *>(old_value)));
                    }
                    else
                    {
                        variant_helper<Types...>::move(old_type_index, old_value, new_value);
                    }
                }

                VARIANT_INLINE static void copy(const std::size_t old_type_index, const void *old_value, void *new_value)
                {
                    if (old_type_index == sizeof...(Types))
                    {
                        new (new_value) T(*reinterpret_cast<const T *>(old_value));
                    }
                    else
                    {
                        variant_helper<Types...>::copy(old_type_index, old_value, new_value);
                    }
                }
            };

            template <>
            struct variant_helper<>
            {
                VARIANT_INLINE static void destroy(const std::size_t, void *) {}
                VARIANT_INLINE static void move(const std::size_t, void *, void *) {}
                VARIANT_INLINE static void copy(const std::size_t, const void *, void *) {}
            };

            template <typename T>
            struct unwrapper
            {
                static T const &apply_const(T const &obj) { return obj; }
                static T &apply(T &obj) { return obj; }
            };

            template <typename T>
            struct unwrapper<recursive_wrapper<T>>
            {
                static auto apply_const(recursive_wrapper<T> const &obj)
                    -> typename recursive_wrapper<T>::type const &
                {
                    return obj.get();
                }
                static auto apply(recursive_wrapper<T> &obj)
                    -> typename recursive_wrapper<T>::type &
                {
                    return obj.get();
                }
            };

            template <typename T>
            struct unwrapper<std::reference_wrapper<T>>
            {
                static auto apply_const(std::reference_wrapper<T> const &obj)
                    -> typename std::reference_wrapper<T>::type const &
                {
                    return obj.get();
                }
                static auto apply(std::reference_wrapper<T> &obj)
                    -> typename std::reference_wrapper<T>::type &
                {
                    return obj.get();
                }
            };

            template <typename F, typename V, typename R, typename... Types>
            struct dispatcher;

            template <typename F, typename V, typename R, typename T, typename... Types>
            struct dispatcher<F, V, R, T, Types...>
            {
                VARIANT_INLINE static R apply_const(V const &v, F &&f)
                {
                    if (v.template is<T>())
                    {
                        return f(unwrapper<T>::apply_const(v.template get_unchecked<T>()));
                    }
                    else
                    {
                        return dispatcher<F, V, R, Types...>::apply_const(v, std::forward<F>(f));
                    }
                }

                VARIANT_INLINE static R apply(V &v, F &&f)
                {
                    if (v.template is<T>())
                    {
                        return f(unwrapper<T>::apply(v.template get_unchecked<T>()));
                    }
                    else
                    {
                        return dispatcher<F, V, R, Types...>::apply(v, std::forward<F>(f));
                    }
                }
            };

            template <typename F, typename V, typename R, typename T>
            struct dispatcher<F, V, R, T>
            {
                VARIANT_INLINE static R apply_const(V const &v, F &&f)
                {
                    return f(unwrapper<T>::apply_const(v.template get_unchecked<T>()));
                }

                VARIANT_INLINE static R apply(V &v, F &&f)
                {
                    return f(unwrapper<T>::apply(v.template get_unchecked<T>()));
                }
            };

            template <typename F, typename V, typename R, typename T, typename... Types>
            struct binary_dispatcher_rhs;

            template <typename F, typename V, typename R, typename T0, typename T1, typename... Types>
            struct binary_dispatcher_rhs<F, V, R, T0, T1, Types...>
            {
                VARIANT_INLINE static R apply_const(V const &lhs, V const &rhs, F &&f)
                {
                    if (rhs.template is<T1>()) // call binary functor
                    {
                        return f(unwrapper<T0>::apply_const(lhs.template get_unchecked<T0>()),
                                 unwrapper<T1>::apply_const(rhs.template get_unchecked<T1>()));
                    }
                    else
                    {
                        return binary_dispatcher_rhs<F, V, R, T0, Types...>::apply_const(lhs, rhs, std::forward<F>(f));
                    }
                }

                VARIANT_INLINE static R apply(V &lhs, V &rhs, F &&f)
                {
                    if (rhs.template is<T1>()) // call binary functor
                    {
                        return f(unwrapper<T0>::apply(lhs.template get_unchecked<T0>()),
                                 unwrapper<T1>::apply(rhs.template get_unchecked<T1>()));
                    }
                    else
                    {
                        return binary_dispatcher_rhs<F, V, R, T0, Types...>::apply(lhs, rhs, std::forward<F>(f));
                    }
                }
            };

            template <typename F, typename V, typename R, typename T0, typename T1>
            struct binary_dispatcher_rhs<F, V, R, T0, T1>
            {
                VARIANT_INLINE static R apply_const(V const &lhs, V const &rhs, F &&f)
                {
                    return f(unwrapper<T0>::apply_const(lhs.template get_unchecked<T0>()),
                             unwrapper<T1>::apply_const(rhs.template get_unchecked<T1>()));
                }

                VARIANT_INLINE static R apply(V &lhs, V &rhs, F &&f)
                {
                    return f(unwrapper<T0>::apply(lhs.template get_unchecked<T0>()),
                             unwrapper<T1>::apply(rhs.template get_unchecked<T1>()));
                }
            };

            template <typename F, typename V, typename R, typename T, typename... Types>
            struct binary_dispatcher_lhs;

            template <typename F, typename V, typename R, typename T0, typename T1, typename... Types>
            struct binary_dispatcher_lhs<F, V, R, T0, T1, Types...>
            {
                VARIANT_INLINE static R apply_const(V const &lhs, V const &rhs, F &&f)
                {
                    if (lhs.template is<T1>()) // call binary functor
                    {
                        return f(unwrapper<T1>::apply_const(lhs.template get_unchecked<T1>()),
                                 unwrapper<T0>::apply_const(rhs.template get_unchecked<T0>()));
                    }
                    else
                    {
                        return binary_dispatcher_lhs<F, V, R, T0, Types...>::apply_const(lhs, rhs, std::forward<F>(f));
                    }
                }

                VARIANT_INLINE static R apply(V &lhs, V &rhs, F &&f)
                {
                    if (lhs.template is<T1>()) // call binary functor
                    {
                        return f(unwrapper<T1>::apply(lhs.template get_unchecked<T1>()),
                                 unwrapper<T0>::apply(rhs.template get_unchecked<T0>()));
                    }
                    else
                    {
                        return binary_dispatcher_lhs<F, V, R, T0, Types...>::apply(lhs, rhs, std::forward<F>(f));
                    }
                }
            };

            template <typename F, typename V, typename R, typename T0, typename T1>
            struct binary_dispatcher_lhs<F, V, R, T0, T1>
            {
                VARIANT_INLINE static R apply_const(V const &lhs, V const &rhs, F &&f)
                {
                    return f(unwrapper<T1>::apply_const(lhs.template get_unchecked<T1>()),
                             unwrapper<T0>::apply_const(rhs.template get_unchecked<T0>()));
                }

                VARIANT_INLINE static R apply(V &lhs, V &rhs, F &&f)
                {
                    return f(unwrapper<T1>::apply(lhs.template get_unchecked<T1>()),
                             unwrapper<T0>::apply(rhs.template get_unchecked<T0>()));
                }
            };

            template <typename F, typename V, typename R, typename... Types>
            struct binary_dispatcher;

            template <typename F, typename V, typename R, typename T, typename... Types>
            struct binary_dispatcher<F, V, R, T, Types...>
            {
                VARIANT_INLINE static R apply_const(V const &v0, V const &v1, F &&f)
                {
                    if (v0.template is<T>())
                    {
                        if (v1.template is<T>())
                        {
                            return f(unwrapper<T>::apply_const(v0.template get_unchecked<T>()),
                                     unwrapper<T>::apply_const(v1.template get_unchecked<T>())); // call binary functor
                        }
                        else
                        {
                            return binary_dispatcher_rhs<F, V, R, T, Types...>::apply_const(v0, v1, std::forward<F>(f));
                        }
                    }
                    else if (v1.template is<T>())
                    {
                        return binary_dispatcher_lhs<F, V, R, T, Types...>::apply_const(v0, v1, std::forward<F>(f));
                    }
                    return binary_dispatcher<F, V, R, Types...>::apply_const(v0, v1, std::forward<F>(f));
                }

                VARIANT_INLINE static R apply(V &v0, V &v1, F &&f)
                {
                    if (v0.template is<T>())
                    {
                        if (v1.template is<T>())
                        {
                            return f(unwrapper<T>::apply(v0.template get_unchecked<T>()),
                                     unwrapper<T>::apply(v1.template get_unchecked<T>())); // call binary functor
                        }
                        else
                        {
                            return binary_dispatcher_rhs<F, V, R, T, Types...>::apply(v0, v1, std::forward<F>(f));
                        }
                    }
                    else if (v1.template is<T>())
                    {
                        return binary_dispatcher_lhs<F, V, R, T, Types...>::apply(v0, v1, std::forward<F>(f));
                    }
                    return binary_dispatcher<F, V, R, Types...>::apply(v0, v1, std::forward<F>(f));
                }
            };

            template <typename F, typename V, typename R, typename T>
            struct binary_dispatcher<F, V, R, T>
            {
                VARIANT_INLINE static R apply_const(V const &v0, V const &v1, F &&f)
                {
                    return f(unwrapper<T>::apply_const(v0.template get_unchecked<T>()),
                             unwrapper<T>::apply_const(v1.template get_unchecked<T>())); // call binary functor
                }

                VARIANT_INLINE static R apply(V &v0, V &v1, F &&f)
                {
                    return f(unwrapper<T>::apply(v0.template get_unchecked<T>()),
                             unwrapper<T>::apply(v1.template get_unchecked<T>())); // call binary functor
                }
            };

            // comparator functors
            struct equal_comp
            {
                template <typename T>
                bool operator()(T const &lhs, T const &rhs) const
                {
                    return lhs == rhs;
                }
            };

            struct less_comp
            {
                template <typename T>
                bool operator()(T const &lhs, T const &rhs) const
                {
                    return lhs < rhs;
                }
            };

            template <typename Variant, typename Comp>
            class comparer
            {
            public:
                explicit comparer(Variant const &lhs) noexcept
                    : lhs_(lhs) {}
                comparer &operator=(comparer const &) = delete;
                // visitor
                template <typename T>
                bool operator()(T const &rhs_content) const
                {
                    T const &lhs_content = lhs_.template get_unchecked<T>();
                    return Comp()(lhs_content, rhs_content);
                }

            private:
                Variant const &lhs_;
            };

            // hashing visitor
            struct hasher
            {
                template <typename T>
                std::size_t operator()(const T &hashable) const
                {
                    return std::hash<T>{}(hashable);
                }
            };

        } // namespace detail

        struct no_init
        {
        };

        template <typename... Types>
        class variant
        {
            static_assert(sizeof...(Types) > 0, "Template parameter type list of variant can not be empty");
            static_assert(!detail::disjunction<std::is_reference<Types>...>::value, "Variant can not hold reference types. Maybe use std::reference_wrapper?");

        private:
            static const std::size_t data_size = detail::static_max<sizeof(Types)...>::value;
            static const std::size_t data_align = detail::static_max<alignof(Types)...>::value;

        public:
            struct adapted_variant_tag;
            using types = std::tuple<Types...>;

        private:
            using first_type = typename std::tuple_element<0, types>::type;
            using data_type = typename std::aligned_storage<data_size, data_align>::type;
            using helper_type = detail::variant_helper<Types...>;

            std::size_t type_index;
            data_type data;

        public:
            VARIANT_INLINE variant() noexcept(std::is_nothrow_default_constructible<first_type>::value)
                : type_index(sizeof...(Types) - 1)
            {
                static_assert(std::is_default_constructible<first_type>::value, "First type in variant must be default constructible to allow default construction of variant");
                new (&data) first_type();
            }

            VARIANT_INLINE variant(no_init) noexcept
                : type_index(detail::invalid_value) {}

            // http://isocpp.org/blog/2012/11/universal-references-in-c11-scott-meyers
            template <typename T, typename Traits = detail::value_traits<T, Types...>,
                      typename Enable = typename std::enable_if<Traits::is_valid && !std::is_same<variant<Types...>, typename Traits::value_type>::value>::type>
            VARIANT_INLINE variant(T &&val) noexcept(std::is_nothrow_constructible<typename Traits::target_type, T &&>::value)
                : type_index(Traits::index)
            {
                new (&data) typename Traits::target_type(std::forward<T>(val));
            }

            VARIANT_INLINE variant(variant<Types...> const &old)
                : type_index(old.type_index)
            {
                helper_type::copy(old.type_index, &old.data, &data);
            }

            VARIANT_INLINE variant(variant<Types...> &&old) noexcept(detail::conjunction<std::is_nothrow_move_constructible<Types>...>::value)
                : type_index(old.type_index)
            {
                helper_type::move(old.type_index, &old.data, &data);
            }

        private:
            VARIANT_INLINE void copy_assign(variant<Types...> const &rhs)
            {
                helper_type::destroy(type_index, &data);
                type_index = detail::invalid_value;
                helper_type::copy(rhs.type_index, &rhs.data, &data);
                type_index = rhs.type_index;
            }

            VARIANT_INLINE void move_assign(variant<Types...> &&rhs)
            {
                helper_type::destroy(type_index, &data);
                type_index = detail::invalid_value;
                helper_type::move(rhs.type_index, &rhs.data, &data);
                type_index = rhs.type_index;
            }

        public:
            VARIANT_INLINE variant<Types...> &operator=(variant<Types...> &&other)
            {
                move_assign(std::move(other));
                return *this;
            }

            VARIANT_INLINE variant<Types...> &operator=(variant<Types...> const &other)
            {
                copy_assign(other);
                return *this;
            }

            // conversions
            // move-assign
            template <typename T>
            VARIANT_INLINE variant<Types...> &operator=(T &&rhs) noexcept
            {
                variant<Types...> temp(std::forward<T>(rhs));
                move_assign(std::move(temp));
                return *this;
            }

            // copy-assign
            template <typename T>
            VARIANT_INLINE variant<Types...> &operator=(T const &rhs)
            {
                variant<Types...> temp(rhs);
                copy_assign(temp);
                return *this;
            }

            template <typename T, typename std::enable_if<
                                      (detail::direct_type<T, Types...>::index != detail::invalid_value)>::type * = nullptr>
            VARIANT_INLINE bool is() const
            {
                return type_index == detail::direct_type<T, Types...>::index;
            }

            template <typename T, typename std::enable_if<
                                      (detail::direct_type<recursive_wrapper<T>, Types...>::index != detail::invalid_value)>::type * = nullptr>
            VARIANT_INLINE bool is() const
            {
                return type_index == detail::direct_type<recursive_wrapper<T>, Types...>::index;
            }

            VARIANT_INLINE bool valid() const
            {
                return type_index != detail::invalid_value;
            }

            template <typename T, typename... Args>
            VARIANT_INLINE void set(Args &&...args)
            {
                helper_type::destroy(type_index, &data);
                type_index = detail::invalid_value;
                new (&data) T(std::forward<Args>(args)...);
                type_index = detail::direct_type<T, Types...>::index;
            }

            // get_unchecked<T>()
            template <typename T, typename std::enable_if<
                                      (detail::direct_type<T, Types...>::index != detail::invalid_value)>::type * = nullptr>
            VARIANT_INLINE T &get_unchecked()
            {
                return *reinterpret_cast<T *>(&data);
            }

#ifdef HAS_EXCEPTIONS
            // get<T>()
            template <typename T, typename std::enable_if<
                                      (detail::direct_type<T, Types...>::index != detail::invalid_value)>::type * = nullptr>
            VARIANT_INLINE T &get()
            {
                if (type_index == detail::direct_type<T, Types...>::index)
                {
                    return *reinterpret_cast<T *>(&data);
                }
                else
                {
                    throw bad_variant_access("in get<T>()");
                }
            }
#endif

            template <typename T, typename std::enable_if<
                                      (detail::direct_type<T, Types...>::index != detail::invalid_value)>::type * = nullptr>
            VARIANT_INLINE T const &get_unchecked() const
            {
                return *reinterpret_cast<T const *>(&data);
            }

#ifdef HAS_EXCEPTIONS
            template <typename T, typename std::enable_if<
                                      (detail::direct_type<T, Types...>::index != detail::invalid_value)>::type * = nullptr>
            VARIANT_INLINE T const &get() const
            {
                if (type_index == detail::direct_type<T, Types...>::index)
                {
                    return *reinterpret_cast<T const *>(&data);
                }
                else
                {
                    throw bad_variant_access("in get<T>()");
                }
            }
#endif

            // get_unchecked<T>() - T stored as recursive_wrapper<T>
            template <typename T, typename std::enable_if<
                                      (detail::direct_type<recursive_wrapper<T>, Types...>::index != detail::invalid_value)>::type * = nullptr>
            VARIANT_INLINE T &get_unchecked()
            {
                return (*reinterpret_cast<recursive_wrapper<T> *>(&data)).get();
            }

#ifdef HAS_EXCEPTIONS
            // get<T>() - T stored as recursive_wrapper<T>
            template <typename T, typename std::enable_if<
                                      (detail::direct_type<recursive_wrapper<T>, Types...>::index != detail::invalid_value)>::type * = nullptr>
            VARIANT_INLINE T &get()
            {
                if (type_index == detail::direct_type<recursive_wrapper<T>, Types...>::index)
                {
                    return (*reinterpret_cast<recursive_wrapper<T> *>(&data)).get();
                }
                else
                {
                    throw bad_variant_access("in get<T>()");
                }
            }
#endif

            template <typename T, typename std::enable_if<
                                      (detail::direct_type<recursive_wrapper<T>, Types...>::index != detail::invalid_value)>::type * = nullptr>
            VARIANT_INLINE T const &get_unchecked() const
            {
                return (*reinterpret_cast<recursive_wrapper<T> const *>(&data)).get();
            }

#ifdef HAS_EXCEPTIONS
            template <typename T, typename std::enable_if<
                                      (detail::direct_type<recursive_wrapper<T>, Types...>::index != detail::invalid_value)>::type * = nullptr>
            VARIANT_INLINE T const &get() const
            {
                if (type_index == detail::direct_type<recursive_wrapper<T>, Types...>::index)
                {
                    return (*reinterpret_cast<recursive_wrapper<T> const *>(&data)).get();
                }
                else
                {
                    throw bad_variant_access("in get<T>()");
                }
            }
#endif

            // get_unchecked<T>() - T stored as std::reference_wrapper<T>
            template <typename T, typename std::enable_if<
                                      (detail::direct_type<std::reference_wrapper<T>, Types...>::index != detail::invalid_value)>::type * = nullptr>
            VARIANT_INLINE T &get_unchecked()
            {
                return (*reinterpret_cast<std::reference_wrapper<T> *>(&data)).get();
            }

#ifdef HAS_EXCEPTIONS
            // get<T>() - T stored as std::reference_wrapper<T>
            template <typename T, typename std::enable_if<
                                      (detail::direct_type<std::reference_wrapper<T>, Types...>::index != detail::invalid_value)>::type * = nullptr>
            VARIANT_INLINE T &get()
            {
                if (type_index == detail::direct_type<std::reference_wrapper<T>, Types...>::index)
                {
                    return (*reinterpret_cast<std::reference_wrapper<T> *>(&data)).get();
                }
                else
                {
                    throw bad_variant_access("in get<T>()");
                }
            }
#endif

            template <typename T, typename std::enable_if<
                                      (detail::direct_type<std::reference_wrapper<T const>, Types...>::index != detail::invalid_value)>::type * = nullptr>
            VARIANT_INLINE T const &get_unchecked() const
            {
                return (*reinterpret_cast<std::reference_wrapper<T const> const *>(&data)).get();
            }

#ifdef HAS_EXCEPTIONS
            template <typename T, typename std::enable_if<
                                      (detail::direct_type<std::reference_wrapper<T const>, Types...>::index != detail::invalid_value)>::type * = nullptr>
            VARIANT_INLINE T const &get() const
            {
                if (type_index == detail::direct_type<std::reference_wrapper<T const>, Types...>::index)
                {
                    return (*reinterpret_cast<std::reference_wrapper<T const> const *>(&data)).get();
                }
                else
                {
                    throw bad_variant_access("in get<T>()");
                }
            }
#endif

            // This function is deprecated because it returns an internal index field.
            // Use which() instead.
            MAPBOX_VARIANT_DEPRECATED VARIANT_INLINE std::size_t get_type_index() const
            {
                return type_index;
            }

            VARIANT_INLINE int which() const noexcept
            {
                return static_cast<int>(sizeof...(Types) - type_index - 1);
            }

            template <typename T, typename std::enable_if<
                                      (detail::direct_type<T, Types...>::index != detail::invalid_value)>::type * = nullptr>
            VARIANT_INLINE static constexpr int which() noexcept
            {
                return static_cast<int>(sizeof...(Types) - detail::direct_type<T, Types...>::index - 1);
            }

            // visitor
            // unary
            template <typename F, typename V, typename R = typename detail::result_of_unary_visit<F, first_type>::type>
            auto VARIANT_INLINE static visit(V const &v, F &&f)
                -> decltype(detail::dispatcher<F, V, R, Types...>::apply_const(v, std::forward<F>(f)))
            {
                return detail::dispatcher<F, V, R, Types...>::apply_const(v, std::forward<F>(f));
            }
            // non-const
            template <typename F, typename V, typename R = typename detail::result_of_unary_visit<F, first_type>::type>
            auto VARIANT_INLINE static visit(V &v, F &&f)
                -> decltype(detail::dispatcher<F, V, R, Types...>::apply(v, std::forward<F>(f)))
            {
                return detail::dispatcher<F, V, R, Types...>::apply(v, std::forward<F>(f));
            }

            // binary
            // const
            template <typename F, typename V, typename R = typename detail::result_of_binary_visit<F, first_type>::type>
            auto VARIANT_INLINE static binary_visit(V const &v0, V const &v1, F &&f)
                -> decltype(detail::binary_dispatcher<F, V, R, Types...>::apply_const(v0, v1, std::forward<F>(f)))
            {
                return detail::binary_dispatcher<F, V, R, Types...>::apply_const(v0, v1, std::forward<F>(f));
            }
            // non-const
            template <typename F, typename V, typename R = typename detail::result_of_binary_visit<F, first_type>::type>
            auto VARIANT_INLINE static binary_visit(V &v0, V &v1, F &&f)
                -> decltype(detail::binary_dispatcher<F, V, R, Types...>::apply(v0, v1, std::forward<F>(f)))
            {
                return detail::binary_dispatcher<F, V, R, Types...>::apply(v0, v1, std::forward<F>(f));
            }

            // match
            // unary
            template <typename... Fs>
            auto VARIANT_INLINE match(Fs &&...fs) const
                -> decltype(variant::visit(*this, ::mapbox::util::make_visitor(std::forward<Fs>(fs)...)))
            {
                return variant::visit(*this, ::mapbox::util::make_visitor(std::forward<Fs>(fs)...));
            }
            // non-const
            template <typename... Fs>
            auto VARIANT_INLINE match(Fs &&...fs)
                -> decltype(variant::visit(*this, ::mapbox::util::make_visitor(std::forward<Fs>(fs)...)))
            {
                return variant::visit(*this, ::mapbox::util::make_visitor(std::forward<Fs>(fs)...));
            }

            ~variant() noexcept // no-throw destructor
            {
                helper_type::destroy(type_index, &data);
            }

            // comparison operators
            // equality
            VARIANT_INLINE bool operator==(variant const &rhs) const
            {
                assert(valid() && rhs.valid());
                if (this->which() != rhs.which())
                {
                    return false;
                }
                detail::comparer<variant, detail::equal_comp> visitor(*this);
                return visit(rhs, visitor);
            }

            VARIANT_INLINE bool operator!=(variant const &rhs) const
            {
                return !(*this == rhs);
            }

            // less than
            VARIANT_INLINE bool operator<(variant const &rhs) const
            {
                assert(valid() && rhs.valid());
                if (this->which() != rhs.which())
                {
                    return this->which() < rhs.which();
                }
                detail::comparer<variant, detail::less_comp> visitor(*this);
                return visit(rhs, visitor);
            }
            VARIANT_INLINE bool operator>(variant const &rhs) const
            {
                return rhs < *this;
            }
            VARIANT_INLINE bool operator<=(variant const &rhs) const
            {
                return !(*this > rhs);
            }
            VARIANT_INLINE bool operator>=(variant const &rhs) const
            {
                return !(*this < rhs);
            }
        };

        // unary visitor interface
        // const
        template <typename F, typename V>
        auto VARIANT_INLINE apply_visitor(F &&f, V const &v) -> decltype(V::visit(v, std::forward<F>(f)))
        {
            return V::visit(v, std::forward<F>(f));
        }

        // non-const
        template <typename F, typename V>
        auto VARIANT_INLINE apply_visitor(F &&f, V &v) -> decltype(V::visit(v, std::forward<F>(f)))
        {
            return V::visit(v, std::forward<F>(f));
        }

        // binary visitor interface
        // const
        template <typename F, typename V>
        auto VARIANT_INLINE apply_visitor(F &&f, V const &v0, V const &v1) -> decltype(V::binary_visit(v0, v1, std::forward<F>(f)))
        {
            return V::binary_visit(v0, v1, std::forward<F>(f));
        }

        // non-const
        template <typename F, typename V>
        auto VARIANT_INLINE apply_visitor(F &&f, V &v0, V &v1) -> decltype(V::binary_visit(v0, v1, std::forward<F>(f)))
        {
            return V::binary_visit(v0, v1, std::forward<F>(f));
        }

        // getter interface

#ifdef HAS_EXCEPTIONS
        template <typename ResultType, typename T>
        auto get(T &var) -> decltype(var.template get<ResultType>())
        {
            return var.template get<ResultType>();
        }
#endif

        template <typename ResultType, typename T>
        ResultType &get_unchecked(T &var)
        {
            return var.template get_unchecked<ResultType>();
        }

#ifdef HAS_EXCEPTIONS
        template <typename ResultType, typename T>
        auto get(T const &var) -> decltype(var.template get<ResultType>())
        {
            return var.template get<ResultType>();
        }
#endif

        template <typename ResultType, typename T>
        ResultType const &get_unchecked(T const &var)
        {
            return var.template get_unchecked<ResultType>();
        }
    } // namespace util
} // namespace mapbox

// hashable iff underlying types are hashable
namespace std
{
    template <typename... Types>
    struct hash<::mapbox::util::variant<Types...>>
    {
        std::size_t operator()(const ::mapbox::util::variant<Types...> &v) const noexcept
        {
            return ::mapbox::util::apply_visitor(::mapbox::util::detail::hasher{}, v);
        }
    };
}

namespace mapbox
{
    namespace geometry
    {

        struct empty
        {
        }; //  this Geometry type represents the empty point set, ∅, for the coordinate space (OGC Simple Features).

        constexpr bool operator==(empty, empty) { return true; }
        constexpr bool operator!=(empty, empty) { return false; }
        constexpr bool operator<(empty, empty) { return false; }
        constexpr bool operator>(empty, empty) { return false; }
        constexpr bool operator<=(empty, empty) { return true; }
        constexpr bool operator>=(empty, empty) { return true; }

    } // namespace geometry
} // namespace mapbox

namespace mapbox
{
    namespace geometry
    {

        template <typename T>
        struct point
        {
            using coordinate_type = T;

            constexpr point()
                : x(), y()
            {
            }
            constexpr point(T x_, T y_)
                : x(x_), y(y_)
            {
            }

            T x;
            T y;
        };

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal"

        template <typename T>
        constexpr bool operator==(point<T> const &lhs, point<T> const &rhs)
        {
            return lhs.x == rhs.x && lhs.y == rhs.y;
        }

#pragma GCC diagnostic pop

        template <typename T>
        constexpr bool operator!=(point<T> const &lhs, point<T> const &rhs)
        {
            return !(lhs == rhs);
        }

    } // namespace geometry
} // namespace mapbox

namespace mapbox
{
    namespace geometry
    {

        template <typename T, template <typename...> class Cont = std::vector>
        struct multi_point : Cont<point<T>>
        {
            using coordinate_type = T;
            using point_type = point<T>;
            using container_type = Cont<point_type>;
            using size_type = typename container_type::size_type;

            template <class... Args>
            multi_point(Args &&...args) : container_type(std::forward<Args>(args)...)
            {
            }
            multi_point(std::initializer_list<point_type> args)
                : container_type(std::move(args)) {}
        };

    } // namespace geometry
} // namespace mapbox

namespace mapbox
{
    namespace geometry
    {

        template <typename T, template <typename...> class Cont = std::vector>
        struct geometry_collection;

        template <typename T, template <typename...> class Cont = std::vector>
        using geometry_base = mapbox::util::variant<empty,
                                                    point<T>,
                                                    multi_point<T, Cont>,
                                                    geometry_collection<T, Cont>>;

        template <typename T, template <typename...> class Cont = std::vector>
        struct geometry : geometry_base<T, Cont>
        {
            using coordinate_type = T;
            using geometry_base<T>::geometry_base;
        };

        template <typename T, template <typename...> class Cont>
        struct geometry_collection : Cont<geometry<T>>
        {
            using coordinate_type = T;
            using geometry_type = geometry<T>;
            using container_type = Cont<geometry_type>;
            using size_type = typename container_type::size_type;

            template <class... Args>
            geometry_collection(Args &&...args) : container_type(std::forward<Args>(args)...)
            {
            }
            geometry_collection(std::initializer_list<geometry_type> args)
                : container_type(std::move(args)) {}
        };

    } // namespace geometry
} // namespace mapbox

namespace mapbox
{
    namespace feature
    {

        struct value;

        struct null_value_t
        {
        };

        constexpr bool operator==(const null_value_t &, const null_value_t &) { return true; }
        constexpr bool operator!=(const null_value_t &, const null_value_t &) { return false; }
        constexpr bool operator<(const null_value_t &, const null_value_t &) { return false; }

        constexpr null_value_t null_value = null_value_t();

        // Multiple numeric types (uint64_t, int64_t, double) are present in order to support
        // the widest possible range of JSON numbers, which do not have a maximum range.
        // Implementations that produce `value`s should use that order for type preference,
        // using uint64_t for positive integers, int64_t for negative integers, and double
        // for non-integers and integers outside the range of 64 bits.
        using value_base = mapbox::util::variant<null_value_t, bool, uint64_t, int64_t, double, std::string,
                                                 mapbox::util::recursive_wrapper<std::vector<value>>,
                                                 mapbox::util::recursive_wrapper<std::unordered_map<std::string, value>>>;

        struct value : value_base
        {
            using value_base::value_base;
        };

        using property_map = std::unordered_map<std::string, value>;

        // The same considerations and requirement for numeric types apply as for `value_base`.
        using identifier = mapbox::util::variant<null_value_t, uint64_t, int64_t, double, std::string>;

        template <class T>
        struct feature
        {
            using coordinate_type = T;
            using geometry_type = mapbox::geometry::geometry<T>; // Fully qualified to avoid GCC -fpermissive error.

            geometry_type geometry;
            property_map properties;
            identifier id;

            feature()
                : geometry(),
                  properties(),
                  id() {}
            feature(geometry_type const &geom_)
                : geometry(geom_),
                  properties(),
                  id() {}
            feature(geometry_type &&geom_)
                : geometry(std::move(geom_)),
                  properties(),
                  id() {}
            feature(geometry_type const &geom_, property_map const &prop_)
                : geometry(geom_), properties(prop_), id() {}
            feature(geometry_type &&geom_, property_map &&prop_)
                : geometry(std::move(geom_)),
                  properties(std::move(prop_)),
                  id() {}
            feature(geometry_type const &geom_, property_map const &prop_, identifier const &id_)
                : geometry(geom_),
                  properties(prop_),
                  id(id_) {}
            feature(geometry_type &&geom_, property_map &&prop_, identifier &&id_)
                : geometry(std::move(geom_)),
                  properties(std::move(prop_)),
                  id(std::move(id_)) {}
        };

        template <class T>
        constexpr bool operator==(feature<T> const &lhs, feature<T> const &rhs)
        {
            return lhs.id == rhs.id && lhs.geometry == rhs.geometry && lhs.properties == rhs.properties;
        }

        template <class T>
        constexpr bool operator!=(feature<T> const &lhs, feature<T> const &rhs)
        {
            return !(lhs == rhs);
        }

        template <class T, template <typename...> class Cont = std::vector>
        struct feature_collection : Cont<feature<T>>
        {
            using coordinate_type = T;
            using feature_type = feature<T>;
            using container_type = Cont<feature_type>;
            using size_type = typename container_type::size_type;

            template <class... Args>
            feature_collection(Args &&...args) : container_type(std::forward<Args>(args)...)
            {
            }
            feature_collection(std::initializer_list<feature_type> args)
                : container_type(std::move(args)) {}
        };

    } // namespace feature
} // namespace mapbox

namespace mapbox
{
    namespace geometry
    {

        template <typename T>
        point<T> operator+(point<T> const &lhs, point<T> const &rhs)
        {
            return point<T>(lhs.x + rhs.x, lhs.y + rhs.y);
        }

        template <typename T>
        point<T> operator+(point<T> const &lhs, T const &rhs)
        {
            return point<T>(lhs.x + rhs, lhs.y + rhs);
        }

        template <typename T>
        point<T> operator-(point<T> const &lhs, point<T> const &rhs)
        {
            return point<T>(lhs.x - rhs.x, lhs.y - rhs.y);
        }

        template <typename T>
        point<T> operator-(point<T> const &lhs, T const &rhs)
        {
            return point<T>(lhs.x - rhs, lhs.y - rhs);
        }

        template <typename T>
        point<T> operator*(point<T> const &lhs, point<T> const &rhs)
        {
            return point<T>(lhs.x * rhs.x, lhs.y * rhs.y);
        }

        template <typename T>
        point<T> operator*(point<T> const &lhs, T const &rhs)
        {
            return point<T>(lhs.x * rhs, lhs.y * rhs);
        }

        template <typename T>
        point<T> operator/(point<T> const &lhs, point<T> const &rhs)
        {
            return point<T>(lhs.x / rhs.x, lhs.y / rhs.y);
        }

        template <typename T>
        point<T> operator/(point<T> const &lhs, T const &rhs)
        {
            return point<T>(lhs.x / rhs, lhs.y / rhs);
        }

        template <typename T>
        point<T> &operator+=(point<T> &lhs, point<T> const &rhs)
        {
            lhs.x += rhs.x;
            lhs.y += rhs.y;
            return lhs;
        }

        template <typename T>
        point<T> &operator+=(point<T> &lhs, T const &rhs)
        {
            lhs.x += rhs;
            lhs.y += rhs;
            return lhs;
        }

        template <typename T>
        point<T> &operator-=(point<T> &lhs, point<T> const &rhs)
        {
            lhs.x -= rhs.x;
            lhs.y -= rhs.y;
            return lhs;
        }

        template <typename T>
        point<T> &operator-=(point<T> &lhs, T const &rhs)
        {
            lhs.x -= rhs;
            lhs.y -= rhs;
            return lhs;
        }

        template <typename T>
        point<T> &operator*=(point<T> &lhs, point<T> const &rhs)
        {
            lhs.x *= rhs.x;
            lhs.y *= rhs.y;
            return lhs;
        }

        template <typename T>
        point<T> &operator*=(point<T> &lhs, T const &rhs)
        {
            lhs.x *= rhs;
            lhs.y *= rhs;
            return lhs;
        }

        template <typename T>
        point<T> &operator/=(point<T> &lhs, point<T> const &rhs)
        {
            lhs.x /= rhs.x;
            lhs.y /= rhs.y;
            return lhs;
        }

        template <typename T>
        point<T> &operator/=(point<T> &lhs, T const &rhs)
        {
            lhs.x /= rhs;
            lhs.y /= rhs;
            return lhs;
        }

    } // namespace geometry
} // namespace mapbox

namespace mapbox
{
    namespace supercluster
    {

        using namespace mapbox::geometry;
        using namespace mapbox::feature;

        class Cluster
        {
        public:
            const point<double> pos;
            const std::uint32_t num_points;
            std::uint32_t id;
            std::uint32_t parent_id = 0;
            bool visited = false;
            std::unique_ptr<property_map> properties{nullptr};

            Cluster(const point<double> &pos_, const std::uint32_t num_points_, const std::uint32_t id_)
                : pos(pos_), num_points(num_points_), id(id_)
            {
            }

            Cluster(const point<double> &pos_,
                    const std::uint32_t num_points_,
                    const std::uint32_t id_,
                    const property_map &properties_)
                : pos(pos_), num_points(num_points_), id(id_)
            {
                if (!properties_.empty())
                {
                    properties = std::make_unique<property_map>(properties_);
                }
            }

            mapbox::feature::feature<double> toGeoJSON() const
            {
                const double x = (pos.x - 0.5) * 360.0;
                const double y =
                    360.0 * std::atan(std::exp((180.0 - pos.y * 360.0) * M_PI / 180)) / M_PI - 90.0;
                return {point<double>{x, y}, getProperties(),
                        identifier(static_cast<std::uint64_t>(id))};
            }

            property_map getProperties() const
            {
                property_map result{{"cluster", true},
                                    {"cluster_id", static_cast<std::uint64_t>(id)},
                                    {"point_count", static_cast<std::uint64_t>(num_points)}};
                std::stringstream ss;
                if (num_points >= 1000)
                {
                    ss << std::fixed;
                    if (num_points < 10000)
                    {
                        ss << std::setprecision(1);
                    }
                    ss << double(num_points) / 1000 << "k";
                }
                else
                {
                    ss << num_points;
                }
                result.emplace("point_count_abbreviated", ss.str());
                if (properties)
                {
                    for (const auto &property : *properties)
                    {
                        result.emplace(property);
                    }
                }
                return result;
            }
        };

    } // namespace supercluster
} // namespace mapbox

namespace kdbush
{
    template <std::uint8_t I, typename T>
    struct nth
    {
        inline static typename std::tuple_element<I, T>::type get(const T &t)
        {
            return std::get<I>(t);
        }
    };

    template <typename TPoint, typename TIndex = std::size_t>
    class KDBush
    {

    public:
        using TNumber = decltype(nth<0, TPoint>::get(std::declval<TPoint>()));
        static_assert(
            std::is_same<TNumber, decltype(nth<1, TPoint>::get(std::declval<TPoint>()))>::value,
            "point component types must be identical");

        static const std::uint8_t defaultNodeSize = 64;

        KDBush(const std::uint8_t nodeSize_ = defaultNodeSize) : nodeSize(nodeSize_)
        {
        }

        KDBush(const std::vector<TPoint> &points_, const std::uint8_t nodeSize_ = defaultNodeSize)
            : KDBush(std::begin(points_), std::end(points_), nodeSize_)
        {
        }

        template <typename TPointIter>
        KDBush(const TPointIter &points_begin,
               const TPointIter &points_end,
               const std::uint8_t nodeSize_ = defaultNodeSize)
            : nodeSize(nodeSize_)
        {
            fill(points_begin, points_end);
        }

        void fill(const std::vector<TPoint> &points_)
        {
            fill(std::begin(points_), std::end(points_));
        }

        template <typename TPointIter>
        void fill(const TPointIter &points_begin, const TPointIter &points_end)
        {
            assert(points.empty());
            const TIndex size = static_cast<TIndex>(std::distance(points_begin, points_end));

            if (size == 0)
                return;

            points.reserve(size);
            ids.reserve(size);

            TIndex i = 0;
            for (auto p = points_begin; p != points_end; p++)
            {
                points.emplace_back(nth<0, TPoint>::get(*p), nth<1, TPoint>::get(*p));
                ids.push_back(i++);
            }

            sortKD(0, size - 1, 0);
        }

        template <typename TVisitor>
        void range(const TNumber minX,
                   const TNumber minY,
                   const TNumber maxX,
                   const TNumber maxY,
                   const TVisitor &visitor) const
        {
            range(minX, minY, maxX, maxY, visitor, 0, static_cast<TIndex>(ids.size() - 1), 0);
        }

        template <typename TVisitor>
        void within(const TNumber qx, const TNumber qy, const TNumber r, const TVisitor &visitor) const
        {
            within(qx, qy, r, visitor, 0, static_cast<TIndex>(ids.size() - 1), 0);
        }

    protected:
        std::vector<TIndex> ids;
        std::vector<std::pair<TNumber, TNumber>> points;

    private:
        const std::uint8_t nodeSize;

        template <typename TVisitor>
        void range(const TNumber minX,
                   const TNumber minY,
                   const TNumber maxX,
                   const TNumber maxY,
                   const TVisitor &visitor,
                   const TIndex left,
                   const TIndex right,
                   const std::uint8_t axis) const
        {

            if (points.empty())
                return;

            if (right - left <= nodeSize)
            {
                for (auto i = left; i <= right; i++)
                {
                    const TNumber x = std::get<0>(points[i]);
                    const TNumber y = std::get<1>(points[i]);
                    if (x >= minX && x <= maxX && y >= minY && y <= maxY)
                        visitor(ids[i]);
                }
                return;
            }

            const TIndex m = (left + right) >> 1;
            const TNumber x = std::get<0>(points[m]);
            const TNumber y = std::get<1>(points[m]);

            if (x >= minX && x <= maxX && y >= minY && y <= maxY)
                visitor(ids[m]);

            if (axis == 0 ? minX <= x : minY <= y)
                range(minX, minY, maxX, maxY, visitor, left, m - 1, (axis + 1) % 2);

            if (axis == 0 ? maxX >= x : maxY >= y)
                range(minX, minY, maxX, maxY, visitor, m + 1, right, (axis + 1) % 2);
        }

        template <typename TVisitor>
        void within(const TNumber qx,
                    const TNumber qy,
                    const TNumber r,
                    const TVisitor &visitor,
                    const TIndex left,
                    const TIndex right,
                    const std::uint8_t axis) const
        {

            if (points.empty())
                return;

            const TNumber r2 = r * r;

            if (right - left <= nodeSize)
            {
                for (auto i = left; i <= right; i++)
                {
                    const TNumber x = std::get<0>(points[i]);
                    const TNumber y = std::get<1>(points[i]);
                    if (sqDist(x, y, qx, qy) <= r2)
                        visitor(ids[i]);
                }
                return;
            }

            const TIndex m = (left + right) >> 1;
            const TNumber x = std::get<0>(points[m]);
            const TNumber y = std::get<1>(points[m]);

            if (sqDist(x, y, qx, qy) <= r2)
                visitor(ids[m]);

            if (axis == 0 ? qx - r <= x : qy - r <= y)
                within(qx, qy, r, visitor, left, m - 1, (axis + 1) % 2);

            if (axis == 0 ? qx + r >= x : qy + r >= y)
                within(qx, qy, r, visitor, m + 1, right, (axis + 1) % 2);
        }

        void sortKD(const TIndex left, const TIndex right, const std::uint8_t axis)
        {
            if (right - left <= nodeSize)
                return;
            const TIndex m = (left + right) >> 1;
            if (axis == 0)
            {
                select<0>(m, left, right);
            }
            else
            {
                select<1>(m, left, right);
            }
            sortKD(left, m - 1, (axis + 1) % 2);
            sortKD(m + 1, right, (axis + 1) % 2);
        }

        template <std::uint8_t I>
        void select(const TIndex k, TIndex left, TIndex right)
        {

            while (right > left)
            {
                if (right - left > 600)
                {
                    const double n = right - left + 1;
                    const double m = k - left + 1;
                    const double z = std::log(n);
                    const double s = 0.5 * std::exp(2 * z / 3);
                    const double r =
                        k - m * s / n + 0.5 * std::sqrt(z * s * (1 - s / n)) * (2 * m < n ? -1 : 1);
                    select<I>(k, std::max(left, TIndex(r)), std::min(right, TIndex(r + s)));
                }

                const TNumber t = std::get<I>(points[k]);
                TIndex i = left;
                TIndex j = right;

                swapItem(left, k);
                if (std::get<I>(points[right]) > t)
                    swapItem(left, right);

                while (i < j)
                {
                    swapItem(i++, j--);
                    while (std::get<I>(points[i]) < t)
                        i++;
                    while (std::get<I>(points[j]) > t)
                        j--;
                }

                if (std::get<I>(points[left]) == t)
                    swapItem(left, j);
                else
                {
                    swapItem(++j, right);
                }

                if (j <= k)
                    left = j + 1;
                if (k <= j)
                    right = j - 1;
            }
        }

        void swapItem(const TIndex i, const TIndex j)
        {
            std::iter_swap(ids.begin() + static_cast<std::int32_t>(i), ids.begin() + static_cast<std::int32_t>(j));
            std::iter_swap(points.begin() + static_cast<std::int32_t>(i), points.begin() + static_cast<std::int32_t>(j));
        }

        TNumber sqDist(const TNumber ax, const TNumber ay, const TNumber bx, const TNumber by) const
        {
            auto dx = ax - bx;
            auto dy = ay - by;
            return dx * dx + dy * dy;
        }
    };
    using Cluster = mapbox::supercluster::Cluster;

    template <>
    struct nth<0, Cluster>
    {
        inline static double get(const Cluster &c)
        {
            return c.pos.x;
        };
    };
    template <>
    struct nth<1, Cluster>
    {
        inline static double get(const Cluster &c)
        {
            return c.pos.y;
        };
    };

} // namespace kdbush

namespace mapbox
{
    namespace supercluster
    {

#ifdef DEBUG_TIMER
        class Timer
        {
        public:
            std::chrono::high_resolution_clock::time_point started;
            Timer()
            {
                started = std::chrono::high_resolution_clock::now();
            }
            void operator()(std::string msg)
            {
                const auto now = std::chrono::high_resolution_clock::now();
                const auto ms = std::chrono::duration_cast<std::chrono::microseconds>(now - started);
                std::cerr << msg << ": " << double(ms.count()) / 1000 << "ms\n";
                started = now;
            }
        };
#endif

        struct Options
        {
            std::uint8_t minZoom = 0;   // min zoom to generate clusters on
            std::uint8_t maxZoom = 16;  // max zoom level to cluster the points on
            std::uint16_t radius = 40;  // cluster radius in pixels
            std::uint16_t extent = 512; // tile extent (radius is calculated relative to it)
            std::size_t minPoints = 2;  // minimum points to form a cluster
            bool generateId = false;    // whether to generate numeric ids for input features (in vector tiles)

            std::function<property_map(const property_map &)> map =
                [](const property_map &p) -> property_map
            { return p; };
            std::function<void(property_map &, const property_map &)> reduce{nullptr};
        };

        class Supercluster
        {
            using GeoJSONPoint = point<double>;
            using GeoJSONFeature = mapbox::feature::feature<double>;
            using GeoJSONFeatures = feature_collection<double>;

            using TilePoint = point<std::int16_t>;
            using TileFeature = mapbox::feature::feature<std::int16_t>;
            using TileFeatures = feature_collection<std::int16_t>;

        public:
            const GeoJSONFeatures features;
            const Options options;

            Supercluster(const GeoJSONFeatures &features_, Options options_ = Options())
                : features(features_), options(std::move(options_))
            {

#ifdef DEBUG_TIMER
                Timer timer;
#endif
                // convert and index initial points
                zooms.emplace(options.maxZoom + 1, Zoom(features, options));
#ifdef DEBUG_TIMER
                timer(std::to_string(features.size()) + " initial points");
#endif
                for (int z = options.maxZoom; z >= options.minZoom; z--)
                {
                    // cluster points from the previous zoom level
                    const double r = options.radius / (options.extent * std::pow(2, z));
                    zooms.emplace(z, Zoom(zooms[z + 1], r, z, options));
#ifdef DEBUG_TIMER
                    timer(std::to_string(zooms[z].clusters.size()) + " clusters");
#endif
                }
            }

            TileFeatures
            getTile(const std::uint8_t z, const std::uint32_t x_, const std::uint32_t y) const
            {
                TileFeatures result;

                const auto zoom_iter = zooms.find(limitZoom(z));
                assert(zoom_iter != zooms.end());
                const auto &zoom = zoom_iter->second;

                std::uint32_t z2 = std::pow(2, z);
                const double r = static_cast<double>(options.radius) / options.extent;
                std::int32_t x = x_;

                const auto visitor = [&, this](const auto &id)
                {
                    assert(id < zoom.clusters.size());
                    const auto &c = zoom.clusters[id];

                    const TilePoint point(::round(this->options.extent * (c.pos.x * z2 - x)),
                                          ::round(this->options.extent * (c.pos.y * z2 - y)));

                    if (c.num_points == 1)
                    {
                        const auto &original_feature = this->features[c.id];
                        // Generate feature id if options.generateId is set.
                        auto featureId = options.generateId ? identifier{static_cast<std::uint64_t>(c.id)} : original_feature.id;
                        result.emplace_back(point, original_feature.properties, std::move(featureId));
                    }
                    else
                    {
                        result.emplace_back(point, c.getProperties(),
                                            identifier(static_cast<std::uint64_t>(c.id)));
                    }
                };

                const double top = (y - r) / z2;
                const double bottom = (y + 1 + r) / z2;

                zoom.tree.range((x - r) / z2, top, (x + 1 + r) / z2, bottom, visitor);

                if (x_ == 0)
                {
                    x = z2;
                    zoom.tree.range(1 - r / z2, top, 1, bottom, visitor);
                }
                if (x_ == z2 - 1)
                {
                    x = -1;
                    zoom.tree.range(0, top, r / z2, bottom, visitor);
                }

                return result;
            }

            GeoJSONFeatures getClusters(double bbox[4], std::uint8_t zoomArg) {
                GeoJSONFeatures result;

                double minLng = std::fmod(std::fmod((bbox[0] + 180.0), 360.0) + 360.0, 360) - 180;
                const double minLat = std::max(-90.0, std::min(90.0, bbox[1]));
                double maxLng = bbox[2] == 180 ? 180 : std::fmod(std::fmod(bbox[2] + 180.0, 360.0) + 360.0, 360) - 180;
                const double maxLat = std::max(-90.0, std::min(90.0, bbox[3]));

                if (bbox[2] - bbox[0] >= 360) {
                    minLng = -180;
                    maxLng = 180;
                } else if (minLng > maxLng) {
                    double eastBbox[4] = { minLng, minLat, 180, maxLat };
                    double westBbox[4] = { -180, minLat, maxLng, maxLat };
                    const GeoJSONFeatures easternHem = getClusters(eastBbox, zoomArg);
                    const GeoJSONFeatures westernHem = getClusters(westBbox, zoomArg);
                    result.insert(result.end(), easternHem.begin(), easternHem.end());
                    result.insert(result.end(), westernHem.begin(), westernHem.end());
                    return result;
                }

                const auto zoom_iter = zooms.find(limitZoom(zoomArg));
                assert(zoom_iter != zooms.end());
                const auto &zoom = zoom_iter->second;

                const auto visitor = [&, this](const auto &id) {
                    assert(id < zoom.clusters.size());
                    const mapbox::supercluster::Cluster &c = zoom.clusters[id];

                    result.emplace_back(clusterToGeoJSON(c));
                };

                zoom.tree.range(lngX(minLng), latY(maxLat), lngX(maxLng), latY(minLat), visitor);

                return result;
            }


            GeoJSONFeatures getChildren(const std::uint32_t cluster_id) const
            {
                GeoJSONFeatures children;
                eachChild(cluster_id,
                          [&, this](const auto &c)
                          { children.push_back(this->clusterToGeoJSON(c)); });
                return children;
            }

            GeoJSONFeatures getLeaves(const std::uint32_t cluster_id,
                                      const std::uint32_t limit = 10,
                                      const std::uint32_t offset = 0) const
            {
                GeoJSONFeatures leaves;
                std::uint32_t skipped = 0;
                std::uint32_t limit_ = limit;
                eachLeaf(cluster_id, limit_, offset, skipped,
                         [&, this](const auto &c)
                         { leaves.push_back(this->clusterToGeoJSON(c)); });
                return leaves;
            }

            std::uint8_t getClusterExpansionZoom(std::uint32_t cluster_id) const
            {
                auto cluster_zoom = (cluster_id % 32) - 1;
                while (cluster_zoom <= options.maxZoom)
                {
                    std::uint32_t num_children = 0;

                    eachChild(cluster_id, [&](const auto &c)
                              {
                num_children++;
                cluster_id = c.id; });

                    cluster_zoom++;

                    if (num_children != 1)
                        break;
                }
                return cluster_zoom;
            }

        private:
            struct Zoom
            {
                kdbush::KDBush<Cluster, std::uint32_t> tree;
                std::vector<Cluster> clusters;

                Zoom() = default;

                Zoom(const GeoJSONFeatures &features_, const Options &options_)
                {
                    // generate a cluster object for each point
                    std::uint32_t i = 0;
                    clusters.reserve(features_.size());
                    for (const auto &f : features_)
                    {
                        if (options_.reduce)
                        {
                            const auto clusterProperties = options_.map(f.properties);
                            clusters.emplace_back(project(f.geometry.get<GeoJSONPoint>()), 1, i++,
                                                  clusterProperties);
                        }
                        else
                        {
                            clusters.emplace_back(project(f.geometry.get<GeoJSONPoint>()), 1, i++);
                        }
                    }
                    tree.fill(clusters);
                }

                Zoom(Zoom &previous, const double r, const std::uint8_t zoom, const Options &options_)
                {

                    // The zoom parameter is restricted to [minZoom, maxZoom] by caller
                    assert(((zoom + 1) & 0b11111) == (zoom + 1));

                    // Since point index is encoded in the upper 27 bits, clamp the count of clusters
                    const auto previous_clusters_size = std::min(
                        previous.clusters.size(), static_cast<std::vector<Cluster>::size_type>(0x7ffffff));

                    for (std::size_t i = 0; i < previous_clusters_size; i++)
                    {
                        auto &p = previous.clusters[i];

                        if (p.visited)
                        {
                            continue;
                        }

                        p.visited = true;

                        const auto num_points_origin = p.num_points;
                        auto num_points = num_points_origin;
                        auto cluster_size = previous.clusters.size();
                        // count the number of points in a potential cluster
                        previous.tree.within(p.pos.x, p.pos.y, r, [&](const auto &neighbor_id)
                                             {
                    assert(neighbor_id < cluster_size);
                    const auto &b = previous.clusters[neighbor_id];
                    // filter out neighbors that are already processed
                    if (!b.visited) {
                        num_points += b.num_points;
                    } });

                        auto clusterProperties = p.properties ? *p.properties : property_map{};
                        if (num_points >= options_.minPoints)
                        { // enough points to form a cluster
                            point<double> weight = p.pos * double(num_points_origin);
                            std::uint32_t id = static_cast<std::uint32_t>((i << 5) + (zoom + 1));

                            // find all nearby points
                            previous.tree.within(p.pos.x, p.pos.y, r, [&](const auto &neighbor_id)
                                                 {
                        assert(neighbor_id < cluster_size);
                        auto &b = previous.clusters[neighbor_id];

                        // filter out neighbors that are already processed
                        if (b.visited) {
                            return;
                        }

                        b.visited = true;
                        b.parent_id = id;

                        // accumulate coordinates for calculating weighted center
                        weight += b.pos * double(b.num_points);

                        if (options_.reduce && b.properties) {
                            // apply reduce function to update clusterProperites
                            options_.reduce(clusterProperties, *b.properties);
                        } });
                            p.parent_id = id;
                            clusters.emplace_back(weight / double(num_points), num_points, id,
                                                  clusterProperties);
                        }
                        else
                        {
                            clusters.emplace_back(p.pos, 1, p.id, clusterProperties);
                            if (num_points > 1)
                            {
                                previous.tree.within(p.pos.x, p.pos.y, r, [&](const auto &neighbor_id)
                                                     {
                            assert(neighbor_id < cluster_size);
                            auto &b = previous.clusters[neighbor_id];
                            // filter out neighbors that are already processed
                            if (b.visited) {
                                return;
                            }
                            b.visited = true;
                            clusters.emplace_back(b.pos, 1, b.id,
                                                  b.properties ? *b.properties : property_map{}); });
                            }
                        }
                    }

                    tree.fill(clusters);
                }
            };

            std::unordered_map<std::uint8_t, Zoom> zooms;

            std::uint8_t limitZoom(const std::uint8_t z) const
            {
                if (z < options.minZoom)
                    return options.minZoom;
                if (z > options.maxZoom + 1)
                    return options.maxZoom + 1;
                return z;
            }

            template <typename TVisitor>
            void eachChild(const std::uint32_t cluster_id, const TVisitor &visitor) const
            {
                const auto origin_id = cluster_id >> 5;
                const auto origin_zoom = cluster_id % 32;

                const auto zoom_iter = zooms.find(origin_zoom);
                if (zoom_iter == zooms.end())
                {
                    throw std::runtime_error("No cluster with the specified id.");
                }

                auto &zoom = zoom_iter->second;
                if (origin_id >= zoom.clusters.size())
                {
                    throw std::runtime_error("No cluster with the specified id.");
                }

                const double r = options.radius / (double(options.extent) * std::pow(2, origin_zoom - 1));
                const auto &origin = zoom.clusters[origin_id];

                bool hasChildren = false;

                zoom.tree.within(origin.pos.x, origin.pos.y, r, [&](const auto &id)
                                 {
            assert(id < zoom.clusters.size());
            const auto &cluster_child = zoom.clusters[id];
            if (cluster_child.parent_id == cluster_id) {
                visitor(cluster_child);
                hasChildren = true;
            } });

                if (!hasChildren)
                {
                    throw std::runtime_error("No cluster with the specified id.");
                }
            }

            template <typename TVisitor>
            void eachLeaf(const std::uint32_t cluster_id,
                          std::uint32_t &limit,
                          const std::uint32_t offset,
                          std::uint32_t &skipped,
                          const TVisitor &visitor) const
            {

                eachChild(cluster_id, [&, this](const auto &cluster_leaf)
                          {
            if (limit == 0)
                return;
            if (cluster_leaf.num_points > 1) {
                if (skipped + cluster_leaf.num_points <= offset) {
                    // skip the whole cluster
                    skipped += cluster_leaf.num_points;
                } else {
                    // enter the cluster
                    this->eachLeaf(cluster_leaf.id, limit, offset, skipped, visitor);
                    // exit the cluster
                }
            } else if (skipped < offset) {
                // skip a single point
                skipped++;
            } else {
                // visit a single point
                visitor(cluster_leaf);
                limit--;
            } });
            }

            GeoJSONFeature clusterToGeoJSON(const Cluster &c) const
            {
                return c.num_points == 1 ? features[c.id] : c.toGeoJSON();
            }

            static double lngX(double lng) {
                return lng / 360 + 0.5;
            }

            static double latY(double lat) {
                const double sine = std::sin(lat * M_PI / 180);
                const double y = 0.5 - 0.25 * std::log((1 + sine) / (1 - sine)) / M_PI;
                return std::min(std::max(y, 0.0), 1.0);
            }

            static point<double> project(const GeoJSONPoint &p) {
                const auto x = lngX(p.x);
                const auto y = latY(p.y);
                return { x, y };
            }
        };

    } // namespace supercluster
} // namespace mapbox
