//
// Created by AmazingBuff on 25-6-15.
//

#include "refl_action.h"
#include <fstream>
#include <filesystem>
#include <unordered_set>

struct DeclSignature
{
    std::string file_name;
    uint32_t line;
    uint32_t column;

    bool operator==(const DeclSignature& other) const
    {
        return file_name == other.file_name && line == other.line && column == other.column;
    }
};

template <>
struct std::hash<DeclSignature>
{
    size_t operator()(const DeclSignature& s) const noexcept
    {
        return llvm::hash_combine(std::hash<std::string>()(s.file_name), std::hash<uint32_t>()(s.line), std::hash<uint32_t>()(s.column));
    }
};

namespace Amazing::Reflect
{
    static std::string RecordMetaInfo(clang::CXXRecordDecl* RecordDecl)
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

        field_type += R"(default: return {Internal::Null_Type_Name, Internal::Null_Type_Name}; } })";
        field += R"(default: return obj; } })";

        std::string field_method = std::format(R"(
            [[nodiscard]] static constexpr uint32_t field_count() {{ return {}; }}
            {}
            {})",
            field_count, field_type, field);

        std::string meta_info = std::format(R"(
            template <>
            class MetaInfo<{}> {{
            public:
            [[nodiscard]] static const char* type_name() {{ return "{}"; }}
            {} }};)",
            qualified_class_name, qualified_class_name, field_method);
        return meta_info;
    }

    static std::string EnumMetaInfo(clang::EnumDecl* EnumDecl)
    {
        clang::PrintingPolicy policy(EnumDecl->getASTContext().getPrintingPolicy());
        policy.Bool = true;

        std::string qualified_class_name = EnumDecl->getQualifiedNameAsString();

        std::string underlying_type = EnumDecl->getIntegerType().getAsString(policy);
        std::string enum_name(R"([[nodiscard]] static const char* enum_name(uint32_t index) { switch (index) {)");
        std::string enum_value = std::format(R"([[nodiscard]] static constexpr {} enum_value(uint32_t index) {{ switch (index) {{)", underlying_type);
        std::string enum_field = std::format(R"([[nodiscard]] static constexpr {} enum_field(uint32_t index) {{ switch (index) {{)", qualified_class_name);

        uint32_t enum_index = 0;
        for (auto enum_iter = EnumDecl->enumerator_begin(); enum_iter != EnumDecl->enumerator_end(); ++enum_iter)
        {
            if (llvm::APInt value = enum_iter->getValue(); value.isNegative())
                enum_value += std::format(R"(case {}: return {};)", enum_index, value.getSExtValue());
            else
                enum_value += std::format(R"(case {}: return {};)", enum_index, value.getLimitedValue());

            enum_name += std::format(R"(case {}: return "{}";)", enum_index, enum_iter->getNameAsString());
            enum_field += std::format(R"(case {}: return {};)", enum_index, enum_iter->getQualifiedNameAsString());
            enum_index++;
        }

        enum_name += R"(default: return Internal::Null_Type_Name; } })";
        enum_value += R"(default: return 0; } })";
        enum_field += std::format(R"(default: return static_cast<{}>(0); }} }})", qualified_class_name);

        std::string enum_method = std::format(R"(
            [[nodiscard]] static constexpr uint32_t enum_count() {{ return {}; }}
            {}
            {}
            {})",
            enum_index, enum_name, enum_value, enum_field);

        std::string meta_info = std::format(R"(
            template <>
            class MetaInfo<{}> {{
            public:
            [[nodiscard]] static const char* type_name() {{ return "{}"; }}
            {} }};)",
            qualified_class_name, qualified_class_name, enum_method);
        return meta_info;
    }

    ReflAttributeAction::ReflAttributeAction(const std::vector<std::string>& input_directories, const std::string& output_directory)
        : m_input_directories(input_directories), m_output_directory(output_directory) {}

    std::unique_ptr<clang::ASTConsumer> ReflAttributeAction::CreateASTConsumer(clang::CompilerInstance& Compiler, llvm::StringRef InFile)
    {
        return std::make_unique<ReflAttributeConsumer>(Compiler.getASTContext(), InFile, m_input_directories, m_output_directory);
    }

    ReflAttributeConsumer::ReflAttributeConsumer(clang::ASTContext& ctx, const llvm::StringRef in_file, const std::vector<std::string>& input_directories, const std::string& output_directory)
        : m_visitor(ctx), m_source_file(in_file), m_output_directory(output_directory)
    {
        m_input_directories.reserve(input_directories.size());
        for (uint32_t i = 0; i < input_directories.size(); i++)
            m_input_directories[i] = input_directories[i];
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

        for (const std::string& input_directory : m_input_directories)
        {
            if (m_source_file.find(input_directory) != std::string::npos)
            {
                std::string relative_path = m_source_file.substr(input_directory.length());
                source = relative_path;
                break;
            }
        }

        std::filesystem::path output_directory(m_output_directory);
        source.replace_extension(".meta");

        if (!std::filesystem::exists(output_directory))
            std::filesystem::create_directories(output_directory);

        std::ofstream out(output_directory / source);
        if (out)
            out << m_visitor.GetMetaInfo();
        out.close();
    }

    ReflAttributeFactory::ReflAttributeFactory(const std::vector<std::string>& input_directories, const std::string& output_directory)
        : m_input_directories(input_directories), m_output_directory(output_directory) {}

    std::unique_ptr<clang::FrontendAction> ReflAttributeFactory::create()
    {
        return std::make_unique<ReflAttributeAction>(m_input_directories, m_output_directory);
    }

    ReflAttributeVisitor::ReflAttributeVisitor(clang::ASTContext& ctx) : m_context(ctx) {}

    static std::mutex s_mutex;
    static std::unordered_set<DeclSignature> s_decl_signatures;
    static bool skip_same_decl(const clang::Decl* decl, clang::ASTContext& ctx)
    {
        clang::SourceManager& source_manager = ctx.getSourceManager();
        clang::SourceLocation loc = decl->getLocation();

        if (loc.isInvalid() || !source_manager.isInMainFile(loc))
            return true;

        const clang::PresumedLoc ploc = source_manager.getPresumedLoc(loc);
        const DeclSignature signature{ploc.getFilename(), ploc.getLine(), ploc.getColumn(),};

        if (s_decl_signatures.contains(signature))
            return true;

        {
            std::lock_guard<std::mutex> lock(s_mutex);
            s_decl_signatures.insert(signature);
        }
        return false;
    }

    bool ReflAttributeVisitor::VisitCXXRecordDecl(clang::CXXRecordDecl* RecordDecl)
    {
        if (!skip_same_decl(RecordDecl, m_context))
        {
            for (auto iter = RecordDecl->attr_begin(); iter != RecordDecl->attr_end(); ++iter)
            {
                if (clang::isa<clang::AnnotateAttr>(*iter))
                {
                    clang::AnnotateAttr* attr = clang::dyn_cast<clang::AnnotateAttr>(*iter);
                    if (attr->getAnnotation() == "refl")
                        m_meta_infos += RecordMetaInfo(RecordDecl);
                }
            }
        }
        return true;
    }

    bool ReflAttributeVisitor::VisitEnumDecl(clang::EnumDecl* EnumDecl)
    {
        if (!skip_same_decl(EnumDecl, m_context))
        {
            for (auto iter = EnumDecl->attr_begin(); iter != EnumDecl->attr_end(); ++iter)
            {
                if (clang::isa<clang::AnnotateAttr>(*iter))
                {
                    clang::AnnotateAttr* attr = clang::dyn_cast<clang::AnnotateAttr>(*iter);
                    if (attr->getAnnotation() == "refl")
                        m_meta_infos += EnumMetaInfo(EnumDecl);
                }
            }
        }
        return true;
    }

    std::string ReflAttributeVisitor::GetMetaInfo() const
    {
        return m_meta_infos;
    }
}