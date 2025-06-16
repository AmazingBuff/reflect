#pragma once

#include "ref.h"

namespace Amazing::Reflect
{
    struct Parameter
    {
        const char* name;
        const char* type;
    };

    namespace Internal
    {
        constexpr static const char* Null_Type_Name = "NullType";

        class NullType
        {
        public:
            [[nodiscard]] static const char* type_name() {return Null_Type_Name;}
        };

        class NullFunction : public NullType
        {
        public:
            [[nodiscard]] static uint32_t parameter_count() {return 0;}
            [[nodiscard]] static Parameter return_type() {return {Null_Type_Name, Null_Type_Name};}
            [[nodiscard]] static Parameter parameter_type(uint32_t index) {return {Null_Type_Name, Null_Type_Name};}
        };

        class NullMethod : public NullType
        {
        public:
            [[nodiscard]] static uint32_t parameter_count() {return 0;}
            [[nodiscard]] static Parameter return_type() {return {Null_Type_Name, Null_Type_Name};}
            [[nodiscard]] static Parameter parameter_type(uint32_t index) {return {Null_Type_Name, Null_Type_Name};}
        };

        class NullClass : public NullType
        {
        public:
            [[nodiscard]] static uint32_t field_count() {return 0;}
            [[nodiscard]] static Parameter field_type(uint32_t index) {return {Null_Type_Name, Null_Type_Name};}
            [[nodiscard]] static Ref field(const Ref& obj, uint32_t index) { return obj; }

            [[nodiscard]] static uint32_t static_field_count() {return 0;}
            [[nodiscard]] static Parameter static_field_type(uint32_t index) {return {Null_Type_Name, Null_Type_Name};}
            [[nodiscard]] static Ref static_field(const Ref& obj, uint32_t index) { return obj; }

            [[nodiscard]] static uint32_t method_count() {return 0;}
            [[nodiscard]] static NullMethod* method(uint32_t index) {return nullptr;}

            [[nodiscard]] static uint32_t static_method_count() {return 0;}
            [[nodiscard]] static NullMethod* static_method(uint32_t index) {return nullptr;}
        };

        class NullEnum : public NullType
        {
        public:
            [[nodiscard]] static uint32_t enum_count() {return 0;}
            [[nodiscard]] static Any enum_value(uint32_t index) {return {};}
            [[nodiscard]] static const char* enum_name(uint32_t index) {return Null_Type_Name;}
        };
    }
}