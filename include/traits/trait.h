#pragma once

#include <type_traits>

namespace Amazing
{
	namespace Reflection
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
		static constexpr bool is_function_pointer_v = is_function_pointer<T>;
		
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

		template<typename... Args>
		struct type_list;

		template<typename First, typename... Args>
		struct type_list<First, Args...> : private type_list<Args...>
		{
			using type = First;
			using remain = type_list<Args...>;

			type_list() = default;
			type_list(First&& first, Args&&... args) : m_data(first), type_list<Args...>(std::forward<Args>(args)...) {}

			template<size_t Idx, typename... Args>
			friend constexpr const type_element_t<Idx, type_list, Args...>& get_value(const type_list<Args...>& list);

			template<size_t Idx, typename... Args>
			friend constexpr const type_element_t<Idx, type_list, Args...>&& get_value(const type_list<Args...>&& list);

			type m_data;
		};

		template<>
		struct type_list<> {};



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

	}
}