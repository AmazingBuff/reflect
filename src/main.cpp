#include <fstream>
#include <refl/relf.h>

std::string read_file(const std::string& filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open())
        throw std::runtime_error("failed to open file!");

    size_t file_size = file.tellg();
    std::string buffer(file_size, '\0');
    file.seekg(0);
    file.read(buffer.data(), static_cast<int64_t>(file_size));
    file.close();
    return buffer;
}

class Vt
{
public:
    Vt() = default;
    Vt(const Vt&) = default;
    Vt(Vt&&) = default;
    Vt& operator=(const Vt&) = default;
    Vt& operator=(Vt&&) = default;

    explicit operator bool() const
    {
        return v;
    }

private:
    bool v = false;
};


int main()
{
    Amazing::MetaInfo<Vt>::name();

    Vt vt;
    //bool sty = vt;

    int c = 6;
    Amazing::Ref ref(c);
    Amazing::Ref ref2 = ref;

    auto f = ref2.get<int>();

    std::string file = read_file(RES_DIR"test.cpp");
}