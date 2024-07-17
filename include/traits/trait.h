#pragma once

#include <type_traits>

namespace Amazing
{
	namespace Reflection
	{
		template<typename T>
		concept is_function = std::is_function_v<std::remove_pointer_t<T>> || std::is_member_function_pointer_v<T>;

		template<typename T>
		concept not_function = !is_function<T>;


		template<typename... Args>
		struct type_list;

		template<typename From, typename To>
		struct is_convertible
		{
			static constexpr bool value = std::is_convertible_v<From, To>;
		};

		template<typename... From, typename... To >
		struct is_convertible<type_list<From...>, type_list<To...>>
		{
			static constexpr bool value = (std::is_convertible_v<From, To> && ...);
		};

		template<typename From, typename To>
		static constexpr bool is_convertible_v = is_convertible<From, To>::value;


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

		template<typename T>
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
			static constexpr bool is_left_reference_variable = true;
		};

		template<typename T>
		struct variable_traits<T&> : variable_traits<T>
		{
			using parameter_type = T&;
			static constexpr bool is_right_reference_variable = true;
		};

		template<is_function T, typename Class>
		struct variable_traits<T Class::*>
		{
			using class_type = Class;
			using type = T Class::*;
			using parameter_type = T Class::*;

			static constexpr bool is_const_variable = false;
			static constexpr bool is_volatile_variable = false;
			static constexpr bool is_pointer_variable = false;
			static constexpr bool is_left_reference_variable = false;
			static constexpr bool is_right_reference_variable = false;
			static constexpr bool is_member_variable = false;
		};

		template<typename T, typename Class>
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
			requires is_function<T>
		struct remove_qualifier<T>
		{
			using type = T;
		};

		template<typename T>
		using remove_qualifier_t = remove_qualifier<T>::type;

	}
}