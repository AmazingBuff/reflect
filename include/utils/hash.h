#pragma once
#include <string>
#include <vector>


// for hash function
namespace std
{
	template <>
	struct hash<std::pair<std::string, std::vector<std::string>>>
	{
		std::size_t operator()(const std::pair<std::string, std::vector<std::string>>& key) const
		{
			std::size_t sum = 0;
			for (auto& i : key.second)
				sum += hash<std::string>()(i);

			return hash<std::string>()(key.first) + sum;
		}
	};
}