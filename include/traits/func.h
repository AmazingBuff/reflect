#pragma once

#include "trait.h"
#include <array>

namespace Amazing
{
	namespace Reflection
	{
		namespace Internal
		{
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


			template<typename T, typename U>
			struct concat_types
			{
				using type = type_list<T, U>;
			};

			template<typename... T, typename... U>
			struct concat_types<type_list<T...>, type_list<U...>>
			{
				using type = type_list<T..., U...>;
			};

			template<typename... T, typename... U>
			struct concat_types<type_list<T...>, U...>
			{
				using type = type_list<T..., U...>;
			};

			template<typename... Args>
			struct head_type;

			template<typename Head, typename... Args>
			struct head_type<Head, Args...>
			{
				using type = Head;
				using remain = type_list<Args...>;
			};

			template<typename Head, typename... Args>
			struct head_type<type_list<Head, Args...>>
			{
				using type = Head;
				using remain = type_list<Args...>;
			};

			template<>
			struct head_type<> 
			{
				using type = type_list<>;
				using remain = type_list<>;
			};

			template<>
			struct head_type<type_list<>>
			{
				using type = type_list<>;
				using remain = type_list<>;
			};

			template<size_t N, typename... Args>
			struct remove_head;

			template<size_t N, typename Head, typename... Args>
			struct remove_head<N, Head, Args...>
			{
				using type = typename remove_head<N - 1, type_list<Args...>>::type;
			};

			template<size_t N, typename Head, typename... Args>
			struct remove_head<N, type_list<Head, Args...>>
			{
				using type = typename remove_head<N - 1, type_list<Args...>>::type;
			};

			template<typename Head, typename... Args>
			struct remove_head<0, Head, Args...>
			{
				using type = type_list<Head, Args...>;
			};

			template<typename Head, typename... Args>
			struct remove_head<0, type_list<Head, Args...>>
			{
				using type = type_list<Head, Args...>;
			};

			template<size_t N>
			struct remove_head<N>
			{
				using type = type_list<>;
			};

			template<size_t N>
			struct remove_head<N, type_list<>>
			{
				using type = type_list<>;
			};

			template<size_t N, typename... Args>
			struct reverse_type
			{
				using type = reverse_type<N - 1, typename concat_types<typename head_type<Args...>::remain, typename head_type<Args...>::type>::type>::type;
			};

			template<typename... Args>
			struct reverse_type<0, type_list<Args...>>
			{
				using type = type_list<Args...>;
			};

			template<size_t N, typename... Args>
			struct reverse_type<N, type_list<Args...>>
			{
				using type = reverse_type<N - 1, typename concat_types<typename head_type<Args...>::remain, typename head_type<Args...>::type>::type>::type;
			};

#ifdef STANDARD_LIBRARY
			template<typename... Args>
			struct count_types
			{
				static constexpr size_t value = sizeof...(Args);
			};
#else
			template<typename... Args>
			struct count_types
			{
				static constexpr size_t value = count_types<typename head_type<Args...>::remain>::value + 1;
			};

			template<typename... Args>
			struct count_types<type_list<Args...>>
			{
				static constexpr size_t value = count_types<typename head_type<Args...>::remain>::value + 1;
			};

			template<>
			struct count_types<type_list<>>
			{
				static constexpr size_t value = 0;
			};

			template<>
			struct count_types<>
			{
				static constexpr size_t value = 0;
			};
#endif
			
			template<typename... Args>
			struct overturn_types
			{
				using type = typename concat_types<typename overturn_types<typename head_type<Args...>::remain>::type, typename head_type<Args...>::type>::type;
			};

			template<typename... Args>
			struct overturn_types<type_list<Args...>>
			{
				using type = typename concat_types<typename overturn_types<typename head_type<Args...>::remain>::type, typename head_type<Args...>::type>::type;
			};

			template<>
			struct overturn_types<> 
			{
				using type = type_list<>;
			};

			template<>
			struct overturn_types<type_list<>> 
			{
				using type = type_list<>;
			};

			template<typename... Args>
			struct tail_type
			{
				using type = typename head_type<typename overturn_types<Args...>::type>::type;
				using remain = typename overturn_types<typename head_type<typename overturn_types<Args...>::type>::remain>::type;
			};

			template<typename... Args>
			struct tail_type<type_list<Args...>>
			{
				using type = typename head_type<typename overturn_types<Args...>::type>::type;
				using remain = typename overturn_types<typename head_type<typename overturn_types<Args...>::type>::remain>::type;
			};

			template<>
			struct tail_type<> 
			{
				using type = type_list<>;
				using remain = type_list<>;
			};

			template<>
			struct tail_type<type_list<>>
			{
				using type = type_list<>;
				using remain = type_list<>;
			};

			template<size_t Idx, typename F, typename List>
			constexpr typename function_traits<F>::return_type single_of(F&& f, List&& list)
			{
				return invoke(std::forward<F>(f), get_value<Idx>(list));
			}

			template<typename F, typename List, size_t... Idx>
				requires std::is_same_v<typename function_traits<F>::return_type, void>
			constexpr void all_of(F&& f, List&& list, std::index_sequence<Idx...>)
			{
				(single_of<Idx>(std::forward<F>(f), std::forward<List>(list)), ...);
			}

			template<typename F, typename List, size_t... Idx>
				requires(!std::is_same_v<typename function_traits<F>::return_type, void>)
			constexpr std::array<typename function_traits<F>::return_type, count_types<List>::value> 
				all_of(F&& f, List&& list, std::index_sequence<Idx...>)
			{
				return std::array<typename function_traits<F>::return_type, count_types<List>::value>{ single_of<Idx>(std::forward<F>(f), std::forward<List>(list))...};
			}

		}

		template<typename From, typename To>
		static constexpr bool is_convertible_v = Internal::is_convertible<From, To>::value;

		template<typename... Args>
		using head_type_t = typename Internal::head_type<Args...>::type;

		template<size_t N, typename... Args>
		using remove_head_t = typename Internal::remove_head<N, Args...>::type;

		template<size_t N, typename... Args>
		using reverse_type_t = typename Internal::reverse_type<N, Args...>::type;

		template<typename... Args>
		using overturn_types_t = typename Internal::overturn_types<Args...>::type;

		template<typename... Args>
		using tail_type_t = typename Internal::tail_type<Args...>::type;

		template<typename... Args>
		static constexpr size_t count_types_v = Internal::count_types<Args...>::value;


		template<is_function_pointer F, typename Arg, typename... Args>
		typename function_traits<F>::return_type invoke(F&& f, Arg&& arg, Args&&... args)
		{
			if constexpr (function_traits<F>::is_member_function)
				return (static_cast<Arg&&>(arg).*static_cast<F&&>(f))(std::forward<Args>(args)...);
			else
				return static_cast<F&&>(f)(std::forward<Arg>(arg), std::forward<Args>(args)...);
		}

		template<size_t Idx, typename F, typename... Args>
			requires is_function<std::remove_pointer_t<F>>
		constexpr typename function_traits<F>::return_type single_of(F&& f, Args&&... args)
		{
			return Internal::single_of<Idx, F, type_list<Args...>>(std::forward<F>(f), type_list<Args...>(args...));
		}

		template<typename F, typename... Args>
			requires (is_function<std::remove_pointer_t<F>> && std::is_same_v<typename function_traits<F>::return_type, void>)
		constexpr void all_of(F&& f, Args&&... args)
		{
			Internal::all_of(std::forward<F>(f), type_list<Args...>(args...), std::make_index_sequence<count_types_v<Args...>>{});
		}

		template<typename F, typename... Args>
			requires (is_function<std::remove_pointer_t<F>> && !std::is_same_v<typename function_traits<F>::return_type, void>)
		constexpr std::array<typename function_traits<F>::return_type, count_types_v<Args...>> all_of(F&& f, Args&&... args)
		{
			return std::move(Internal::all_of(std::forward<F>(f), type_list<Args...>(args...), std::make_index_sequence<count_types_v<Args...>>{}));
		}


	}
}