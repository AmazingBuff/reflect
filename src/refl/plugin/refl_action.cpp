//
// Created by AmazingBuff on 25-6-15.
//

#include "refl_action.h"
#include <fstream>
#include <filesystem>

namespace Amazing::Reflect
{
    static std::string recordMetaInfo(clang::CXXRecordDecl* RecordDecl)
    {
        clang::PrintingPolicy policy(RecordDecl->getASTContext().getPrintingPolicy());
        policy.Bool = true;


        std::string qualified_class_name = RecordDecl->getQualifiedNameAsString();

        std::string field_type(R"([[nodiscard]] static Parameter field_type(uint32_t index) { switch (index) {)");
        std::string field = std::format(R"([[nodiscard]] static Ref field(const {}& obj, uint32_t index) {{)", qualified_class_name);

        uint32_t field_count = 0;
        for (auto field_iter = RecordDecl->field_begin(); field_iter != RecordDecl->field_end(); ++field_iter)
        {
            uint32_t index = field_iter->getFieldIndex();
            std::string name = field_iter->getNameAsString();
            std::string type = field_iter->getType().getAsString(policy);

            field_type += std::format(R"(case {}: return {{ "{}", "{}" }};)", index, name, type);
            field += std::format(R"(case {}: return obj.{};)", index, name);

            field_count++;
        }

        field_type += R"(default: return {Null_Type_Name, Null_Type_Name}; } })";
        field += R"(default: return obj; } })";

        std::string field_method = std::format(R"([[nodiscard]] static uint32_t field_count() {{ return {}; }}\n{}\n{})", field_count, field_type, field);


        std::string meta_info = std::format(R"(class MetaInfo<{}> {{public: {} }};)", qualified_class_name, field_method);
        return meta_info;
    }

    std::unique_ptr<clang::ASTConsumer> ReflAttributeAction::CreateASTConsumer(clang::CompilerInstance& Compiler, llvm::StringRef InFile)
    {
        return std::make_unique<ReflAttributeConsumer>(InFile);
    }

    void ReflAttributeConsumer::HandleTranslationUnit(clang::ASTContext& Context)
    {
        // Traversing the translation unit decl via a RecursiveASTVisitor
        // will visit all nodes in the AST.
        m_visitor.TraverseDecl(Context.getTranslationUnitDecl());
        WriteMetaInfoToFile();
    }

    void ReflAttributeConsumer::WriteMetaInfoToFile() const
    {
        if (m_visitor.GetMetaInfo().empty())
            return;

        std::filesystem::path source(m_source_file);
        source = source.filename();
        std::filesystem::path output_directory(m_output_directory);
        source.replace_extension(".meta");

        if (!std::filesystem::exists(output_directory))
            std::filesystem::create_directories(output_directory);

        // 写入元数据到文件
        std::ofstream out(output_directory / source);
        if (out)
            out << m_visitor.GetMetaInfo();
    }

    bool ReflAttributeVisitor::VisitCXXRecordDecl(clang::CXXRecordDecl* RecordDecl)
    {
        for (auto iter = RecordDecl->attr_begin(); iter != RecordDecl->attr_end(); ++iter)
        {
            if (clang::isa<clang::AnnotateAttr>(*iter))
            {
                clang::AnnotateAttr* attr = clang::dyn_cast<clang::AnnotateAttr>(*iter);
                if (attr->getAnnotation() == "refl")
                   m_meta_infos += recordMetaInfo(RecordDecl);
            }
        }
        return true;
    }
}