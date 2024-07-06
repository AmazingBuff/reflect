#pragma once
#include <unordered_map>
#include "utils/macro.h"
#include "utils/hash.h"

namespace Amazing
{	
	class ObjectFactory
	{
	public:
		template<typename T, typename... Args>
		static int generate_type_info(std::vector<std::string>& infos)
		{
			infos.emplace_back(typeid(T).name());
			if constexpr (sizeof...(Args) > 0)
				return generate_type_info<Args...>(infos);
			else
				return 0;
		}

		static int register_object(const std::string& name, const std::vector<std::string>& infos, void* func)
		{
			get_func_map()[std::make_pair(name, infos)] = func;
			return 0;
		}

		template<typename Object, typename... Args>
		static Object* create_object_func(Args&&... args)
		{
			return new Object(std::forward<Args>(args)...);
		}

		template<typename Object, typename... Args>
		static Object* create_object(const std::string& name, Args&&... args)
		{
			std::vector<std::string> infos;
			generate_type_info<Args...>(infos);

			auto& s_reflect_map = get_func_map();
			auto it = s_reflect_map.find(std::make_pair(name, infos));
			if (it != s_reflect_map.end())
				return reinterpret_cast<decltype(&create_object_func<Object, Args...>)>(it->second)(std::forward<Args>(args)...);
			else
			{
				std::cerr << "object not found" << std::endl;
				return nullptr;
			}
		}
	
	private:
		static std::unordered_map<std::pair<std::string, std::vector<std::string>>, void*>& get_func_map()
		{
			static std::unordered_map<std::pair<std::string, std::vector<std::string>>, void*> s_reflect_map;
			return s_reflect_map;
		}
	};
}