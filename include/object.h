#pragma once
#include <map>
#include "utils/macro.h"
#include "utils/hash.h"

namespace Amazing
{	
	// todo:
	// support convertible type to create object

	class ObjectFactory
	{
	public:
		using type_names = std::vector<std::string>;

		template<typename T, typename... Args>
		static int generate_type_info(type_names& infos)
		{
			infos.emplace_back(typeid(T).name());
			if constexpr (sizeof...(Args) > 0)
				return generate_type_info<Args...>(infos);
			else
				return 0;
		}

		static int register_object(const std::string& name, const type_names& infos, void* func)
		{
			get_func_map().emplace(name, std::make_pair(infos, func));
			return 0;
		}

		template<typename Object, typename... Args>
		static Object* create_object_func(Args&&... args)
			requires std::is_constructible_v<Object, Args...>
		{
			return new Object(std::forward<Args>(args)...);
		}

		template<typename Object, typename... Args>
		static Object* create_object(const std::string& name, Args&&... args)
			requires std::is_constructible_v<Object, Args...>
		{

			type_names infos;
			generate_type_info<Args...>(infos);

			auto& s_reflect_map = get_func_map();
			auto begin = s_reflect_map.lower_bound(name);
			auto end = s_reflect_map.upper_bound(name);
			auto& it = begin;

			for (; it != end; it++)
			{
				
			}



			if (it != s_reflect_map.end())
				return reinterpret_cast<decltype(&create_object_func<Object, Args...>)>(it->second)(std::forward<Args>(args)...);
			else
			{
				std::cerr << "object not found" << std::endl;
				return nullptr;
			}
		}
	
	private:
		static std::multimap<std::string, std::pair<type_names, void*>>& get_func_map()
		{
			static std::multimap<std::string, std::pair<type_names, void*>> s_reflect_map;
			return s_reflect_map;
		}
	};
}