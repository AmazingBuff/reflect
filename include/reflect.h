#pragma once

#include "object.h"

#define REGISTER_CLASS(class_name, ...)																																														\
			static std::vector<std::string> APPLY(CONCATENATE_2, class_name##_##infos, CONCATENATE(__VA_ARGS__));																												\
			static int APPLY(CONCATENATE_2, class_name##_##info, CONCATENATE(__VA_ARGS__)) = Amazing::ObjectFactory::generate_type_info<##__VA_ARGS__>(APPLY(CONCATENATE_2, class_name##_##infos, CONCATENATE(__VA_ARGS__)));	\
			static int APPLY(CONCATENATE_2, class_name##_##temp, CONCATENATE(__VA_ARGS__)) = Amazing::ObjectFactory::register_object(#class_name, APPLY(CONCATENATE_2, class_name##_##infos, CONCATENATE(__VA_ARGS__)), reinterpret_cast<void*>(&Amazing::ObjectFactory::create_object_func<class_name, ##__VA_ARGS__>))

#define CREATE_CLASS(class_name, ...) Amazing::ObjectFactory::create_object<class_name>(#class_name, ##__VA_ARGS__)