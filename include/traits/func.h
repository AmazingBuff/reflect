#pragma once

#include "trait.h"
#include <array>

namespace Amazing
{
	namespace Trait
	{
		template<typename F, typename Arg, typename... Args>
		typename function_traits<std::decay_t<F>>::return_type invoke(F&& f, Arg&& arg, Args&&... args)
		{
			if constexpr (function_traits<std::decay_t<F>>::is_member_function)
			{
				if constexpr (std::is_pointer_v<Arg>)
					return ((*static_cast<Arg&&>(arg)).*f)(std::forward<Args>(args)...);
				else
					return (static_cast<Arg&&>(arg).*f)(std::forward<Args>(args)...);
			}
			else
				return static_cast<F&&>(f)(std::forward<Arg>(arg), std::forward<Args>(args)...);
		}

		namespace Internal
		{
			template<typename F, typename... Args>
			struct is_invocable
			{
				static constexpr bool value = std::is_invocable_v<F, Args...> || std::is_nothrow_invocable_v<F, Args...>;
			};

			template<typename F, typename... Args>
			struct is_invocable<F, type_list<Args...>>
			{
				static constexpr bool value = std::is_invocable_v<F, Args...> || std::is_nothrow_invocable_v<F, Args...>;
			};

			template<typename Ret, typename F, typename... Args>
			struct is_invocable_r
			{
				static constexpr bool value = std::is_invocable_r_v<Ret, F, Args...> || std::is_nothrow_invocable_r_v<Ret, F, Args...>;
			};

			template<typename Ret, typename F, typename... Args>
			struct is_invocable_r<Ret, F, type_list<Args...>>
			{
				static constexpr bool value = std::is_invocable_r_v<Ret, F, Args...> || std::is_nothrow_invocable_r_v<Ret, F, Args...>;
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
			struct get_type
			{
				using type = typename head_type<typename remove_head<N, Args...>::type>::type;
			};

			template<size_t N, typename... Args>
			struct get_type<N, type_list<Args...>>
			{
				using type = typename head_type<typename remove_head<N, Args...>::type>::type;
			};

			template<size_t N, typename... Args>
			struct reverse_type
			{
				using type = typename reverse_type<N - 1, typename concat_types<typename head_type<Args...>::remain, typename head_type<Args...>::type>::type>::type;
			};

			template<typename... Args>
			struct reverse_type<0, type_list<Args...>>
			{
				using type = type_list<Args...>;
			};

			template<size_t N, typename... Args>
			struct reverse_type<N, type_list<Args...>>
			{
				using type = typename reverse_type<N - 1, typename concat_types<typename head_type<Args...>::remain, typename head_type<Args...>::type>::type>::type;
			};

#ifdef _HAS_CXX11
			template<typename... Args>
			struct count_types
			{
				static constexpr size_t value = sizeof...(Args);
			};

			template<typename... Args>
			struct count_types<type_list<Args...>>
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

			template<typename F, typename List, size_t... Idx>
				requires(!function_traits<std::decay_t<F>>::is_member_function)
			constexpr typename function_traits<std::decay_t<F>>::return_type apply(F&& f, List&& list, std::index_sequence<Idx...>)
			{
				return Trait::invoke(std::forward<F>(f), get_value<Idx>(std::forward<List>(list))...);
			}

			template<typename F, typename Class, typename List, size_t... Idx>
				requires(function_traits<std::decay_t<F>>::is_member_function)
			constexpr typename function_traits<std::decay_t<F>>::return_type apply(F&& f, Class&& c, List&& list, std::index_sequence<Idx...>)
			{
				return Trait::invoke(std::forward<F>(f), std::forward<Class>(c), get_value<Idx>(std::forward<List>(list))...);
			}

			template<size_t Idx, typename F, typename List>
			constexpr typename function_traits<std::decay_t<F>>::return_type single_of(F&& f, List&& list)
			{
				if constexpr (function_traits<std::decay_t<F>>::is_member_function)
				{
					if constexpr (is_same_template_v<typename function_traits<std::decay_t<F>>::argument_type,
									std::decay_t<decltype(get_value<Idx + 1>(std::forward<List>(list)))>>)
						return apply(std::forward<F>(f), get_value<0>(std::forward<List>(list)), get_value<Idx + 1>(std::forward<List>(list)));
					else
						return Trait::invoke(std::forward<F>(f), get_value<0>(std::forward<List>(list)), get_value<Idx + 1>(std::forward<List>(list)));
				}
				else
				{
					if constexpr (is_same_template_v<typename function_traits<std::decay_t<F>>::argument_type,
						std::decay_t<decltype(get_value<Idx>(std::forward<List>(list)))>>)
						return apply(std::forward<F>(f), get_value<Idx>(std::forward<List>(list)));
					else
						return Trait::invoke(std::forward<F>(f), get_value<Idx>(std::forward<List>(list)));
				}
			}

			template<typename F, typename List, size_t... Idx>
				requires std::is_same_v<typename function_traits<std::decay_t<F>>::return_type, void>
			constexpr void all_of(F&& f, List&& list, std::index_sequence<Idx...>)
			{
				(single_of<Idx>(std::forward<F>(f), std::forward<List>(list)), ...);
			}

			template<typename F, typename List, size_t... Idx>
				requires(!std::is_same_v<typename function_traits<std::decay_t<F>>::return_type, void>)
			constexpr std::array<typename function_traits<std::decay_t<F>>::return_type, count_types<List>::value>
				all_of(F&& f, List&& list, std::index_sequence<Idx...>)
			{
				return std::array<typename function_traits<std::decay_t<F>>::return_type, count_types<List>::value>{ single_of<Idx>(std::forward<F>(f), std::forward<List>(list))...};
			}

		}

		template<typename F, typename... Args>
		static constexpr bool is_invocable_v = Internal::is_invocable<F, Args...>::value;

		template<typename Ret, typename F, typename... Args>
		static constexpr bool is_invocable_r_v = Internal::is_invocable_r<Ret, F, Args...>::value;

		template<typename... Args>
		using head_type_t = typename Internal::head_type<Args...>::type;

		template<size_t N, typename... Args>
		using remove_head_t = typename Internal::remove_head<N, Args...>::type;

		template<size_t N, typename... Args>
		using get_type_t = typename Internal::get_type<N, Args...>::type;

		template<size_t N, typename... Args>
		using reverse_type_t = typename Internal::reverse_type<N, Args...>::type;

		template<typename... Args>
		using overturn_types_t = typename Internal::overturn_types<Args...>::type;

		template<typename... Args>
		using tail_type_t = typename Internal::tail_type<Args...>::type;

		template<typename... Args>
		static constexpr size_t count_types_v = Internal::count_types<Args...>::value;

		template<is_function_pointer F, typename... Args>
			requires(!function_traits<std::decay_t<F>>::is_member_function)
		typename function_traits<std::decay_t<F>>::return_type apply(F&& f, type_list<Args...>&& args)
		{
			return Internal::apply(std::forward<F>(f), std::forward<type_list<Args...>>(args), std::make_index_sequence<count_types_v<Args...>>{});
		}

		template<typename F, typename Arg, typename... Args>
			requires function_traits<std::decay_t<F>>::is_member_function
		typename function_traits<std::decay_t<F>>::return_type apply(F&& f, Arg&& arg, type_list<Args...>&& args)
		{
			return Internal::apply(std::forward<F>(f), std::forward<Arg>(arg), std::forward<type_list<Args...>>(args), std::make_index_sequence<count_types_v<Args...>>{});
		}

		template<size_t Idx, typename F, typename Arg, typename... Args>
		constexpr typename function_traits<std::decay_t<F>>::return_type single_of(F&& f, Arg&& arg, Args&&... args)
		{
			return Internal::single_of<Idx, F, type_list<Arg, Args...>>(std::forward<F>(f), type_list<Arg, Args...>(std::forward<Arg>(arg), std::forward<Args>(args)...));
		}

		template<typename F, typename Arg, typename... Args>
			requires (std::is_same_v<typename function_traits<std::decay_t<F>>::return_type, void>)
		constexpr void all_of(F&& f, Arg&& arg, Args&&... args)
		{
			if constexpr (function_traits<std::decay_t<F>>::is_member_function)
				Internal::all_of(std::forward<F>(f), type_list<Arg, Args...>(std::forward<Arg>(arg), std::forward<Args>(args)...), std::make_index_sequence<count_types_v<Args...>>{});
			else
				Internal::all_of(std::forward<F>(f), type_list<Arg, Args...>(std::forward<Arg>(arg), std::forward<Args>(args)...), std::make_index_sequence<count_types_v<Args...> + 1>{});
		}

		template<typename F, typename Arg, typename... Args>
			requires (!std::is_same_v<typename function_traits<F>::return_type, void>)
		constexpr std::array<typename function_traits<F>::return_type, count_types_v<Args...>> all_of(F&& f, Arg&& arg, Args&&... args)
		{
			if constexpr (function_traits<F>::is_member_function)
				return std::move(Internal::all_of(std::forward<F>(f), type_list<Arg, Args...>(std::forward<Arg>(arg), std::forward<Args>(args)...), std::make_index_sequence<count_types_v<Args...>>{}));
			else
				return std::move(Internal::all_of(std::forward<F>(f), type_list<Arg, Args...>(std::forward<Arg>(arg), std::forward<Args>(args)...), std::make_index_sequence<count_types_v<Args...> + 1>{}));
		}

	}
}