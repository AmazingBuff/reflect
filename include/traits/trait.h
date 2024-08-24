#pragma once

#include <type_traits>

namespace Amazing
{
	namespace Trait
	{
		template<typename T>
		concept is_function = std::is_function_v<T>;

		template<typename T>
		concept not_function = !is_function<T>;

		template<typename T>
		concept is_function_pointer = not_function<T> && (is_function<std::remove_pointer_t<T>> || std::is_member_function_pointer_v<T>);

		template<typename T>
		concept not_function_pointer = !is_function_pointer<T>;


		template<typename T>
		struct is_template : std::false_type {};

		template<template <typename...> typename Tp, typename... Args>
		struct is_template<Tp<Args...>> : std::true_type {};

		template<typename T>
		static constexpr bool is_template_v = is_template<T>::value;

		template<typename T, typename U>
		struct is_same_template : std::false_type {};

		template<template <typename...> typename Tp, typename... ArgsT, typename... ArgsU>
		struct is_same_template<Tp<ArgsT...>, Tp<ArgsU...>> : std::true_type {};

		template<typename T, typename U>
		static constexpr bool is_same_template_v = is_same_template<T, U>::value;


		template<size_t Idx, template <typename...> typename List, typename... Args>
		struct type_element;

		template<size_t Idx, template <typename...> typename List, typename First, typename... Args>
		struct type_element<Idx, List, First, Args...>
		{
			using type = typename type_element<Idx - 1, List, Args...>::type;
			using list = typename type_element<Idx - 1, List, Args...>::list;
		};

		template<template <typename...> typename List, typename First, typename... Args>
		struct type_element<0, List, First, Args...>
		{
			using type = First;
			using list = List<First, Args...>;
		};

		template<size_t Idx, template <typename...> typename List, typename... Args>
		using type_element_t = typename type_element<Idx, List, Args...>::type;


		struct null_type {};

		template<typename... Args>
		struct type_list;

		template<typename First, typename... Args>
		struct type_list<First, Args...> : private type_list<Args...>
		{
			using type = First;
			using remain = type_list<Args...>;

			type_list() = default;

			type_list(First&& first, Args&&... args)
				requires(!std::is_class_v<First>)
				: m_data(std::forward<First>(first)), type_list<Args...>(std::forward<Args>(args)...) {}

			type_list(const First& first, Args&&... args) 
				requires(std::is_class_v<First>)
				: m_data(first), type_list<Args...>(std::forward<Args>(args)...) {}

			type_list(const type_list& other)
				: m_data(other.m_data), type_list<Args...>(other.get_rest()) {}

			type_list(type_list&& other) noexcept
				: m_data(std::move(other.m_data)), type_list<Args...>(std::move(other.get_rest())) {}

			constexpr type_list& operator=(const type_list& other)
			{
				m_data = other.m_data;
				get_rest() = other.get_rest();
				return *this;
			}

			constexpr const type_list& operator=(const type_list& other) const
			{
				m_data = other.m_data;
				get_rest() = other.get_rest();
				return *this;
			}

			template<size_t Idx, typename... Args>
			friend constexpr const type_element_t<Idx, type_list, Args...>& get_value(const type_list<Args...>& list);

			template<size_t Idx, typename... Args>
			friend constexpr const type_element_t<Idx, type_list, Args...>&& get_value(const type_list<Args...>&& list);

			template<size_t Idx, typename... Args>
			friend constexpr type_element_t<Idx, type_list, Args...>& get_value(type_list<Args...>& list);

			template<size_t Idx, typename... Args>
			friend constexpr type_element_t<Idx, type_list, Args...>&& get_value(type_list<Args...>&& list);


			inline constexpr type_list<Args...>& get_rest()
			{
				return *this;
			}

			inline constexpr const type_list<Args...>& get_rest() const
			{
				return *this;
			}

			type m_data = type();
		};

		template<>
		struct type_list<> 
		{
			null_type m_data;

			constexpr const type_list& operator=(const type_list& other)
			{
				return *this;
			}
		};

		template<size_t Idx, typename... Args>
		constexpr const type_element_t<Idx, type_list, Args...>& get_value(const type_list<Args...>& list)
		{
			using type = typename type_element<Idx, type_list, Args...>::list;
			return static_cast<const type&>(list).m_data;
		}

		template<size_t Idx, typename... Args>
		constexpr const type_element_t<Idx, type_list, Args...>&& get_value(const type_list<Args...>&& list)
		{
			using type = typename type_element<Idx, type_list, Args...>::list;
			return static_cast<const type&&>(list).m_data;
		}

		template<size_t Idx, typename... Args>
		constexpr type_element_t<Idx, type_list, Args...>& get_value(type_list<Args...>& list)
		{
			using type = typename type_element<Idx, type_list, Args...>::list;
			return static_cast<type&>(list).m_data;
		}

		template<size_t Idx, typename... Args>
		constexpr type_element_t<Idx, type_list, Args...>&& get_value(type_list<Args...>&& list)
		{
			using type = typename type_element<Idx, type_list, Args...>::list;
			return static_cast<type&&>(list).m_data;
		}


		template<typename>
		struct function_traits;

		template<typename Ret, typename... Args>
		struct function_traits<Ret(Args...)>
		{
			using return_type = Ret;
			using argument_type = type_list<Args...>;

			static constexpr bool is_member_function = false;
		};

		template<typename Ret, typename... Args>
		struct function_traits<Ret(*)(Args...)> : function_traits<Ret(Args...)> {};

		template<typename Class, typename Ret, typename... Args>
		struct function_traits<Ret(Class::*)(Args...)>
		{
			using class_type = Class;
			using return_type = Ret;
			using argument_type = type_list<Args...>;

			static constexpr bool is_member_function = true;
			static constexpr bool is_const_member_function = false;
		};

		template<typename Class, typename Ret, typename... Args>
		struct function_traits<Ret(Class::*)(Args...) const> : function_traits<Ret(Class::*)(Args...)>
		{
			static constexpr bool is_const_member_function = true;
		};


		template<typename T>
		struct variable_traits
		{
			using type = T;
			using parameter_type = T;

			static constexpr bool is_const_variable = false;
			static constexpr bool is_volatile_variable = false;
			static constexpr bool is_pointer_variable = false;
			static constexpr bool is_left_reference_variable = false;
			static constexpr bool is_right_reference_variable = false;
			static constexpr bool is_member_variable = false;
		};

		template<not_function T>
		struct variable_traits<const T> : variable_traits<T>
		{
			using parameter_type = const T;
			static constexpr bool is_const_variable = true;
		};

		template<typename T>
		struct variable_traits<volatile T> : variable_traits<T>
		{
			using parameter_type = volatile T;
			static constexpr bool is_volatile_variable = true;
		};

		template<not_function T>
		struct variable_traits<T*> : variable_traits<T>
		{
			using parameter_type = T*;
			static constexpr bool is_pointer_variable = true;
		};

		template<typename T>
		struct variable_traits<T&&> : variable_traits<T>
		{
			using parameter_type = T&&;
			static constexpr bool is_right_reference_variable = true;
		};

		template<typename T>
		struct variable_traits<T&> : variable_traits<T>
		{
			using parameter_type = T&;
			static constexpr bool is_left_reference_variable = true;
		};

		template<typename Ret, typename Class, typename... Args>
		struct variable_traits<Ret(Class::*)(Args...)> : variable_traits<Ret(*)(Args...)>
		{
			using class_type = Class;
			using parameter_type = Ret(Class::*)(Args...);
			using return_type = Ret;
			using argument_type = type_list<Args...>;
			static constexpr bool is_member_variable = true;
		};

		template<typename Ret, typename Class, typename... Args>
		struct variable_traits<Ret(Class::*)(Args...) const> : variable_traits<Ret(*)(Args...)>
		{
			using class_type = Class;
			using parameter_type = Ret(Class::*)(Args...);
			using return_type = Ret;
			using argument_type = type_list<Args...>;
			static constexpr bool is_member_variable = true;
		};

		template<not_function T, typename Class>
		struct variable_traits<T Class::*> : variable_traits<T>
		{
			using class_type = Class;
			using parameter_type = T Class::*;

			static constexpr bool is_member_variable = true;
		};

		template<typename T>
		struct remove_qualifier
		{
			using type = variable_traits<T>::type;
		};

		template<typename T>
		using remove_qualifier_t = remove_qualifier<T>::type;


		template<typename T>
		struct template_traits;

		template<template <typename...> typename Tp, typename... Args>
		struct template_traits<Tp<Args...>>
		{
			using type = type_list<Args...>;
			
			template<typename... T>
			using apply = Tp<T...>;
		};

		template<typename T, typename... Args>
		using apply_template = typename template_traits<T>::template apply<Args...>;
	}
}