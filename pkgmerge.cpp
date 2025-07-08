
#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include <regex>

namespace fs = std::filesystem;

// مرتب‌سازی عددی طبیعی
bool natural_compare(const fs::path& a, const fs::path& b) {
    static const std::regex re(R"(\d+)");
    std::sregex_iterator it_a(a.filename().string().begin(), a.filename().string().end(), re);
    std::sregex_iterator it_b(b.filename().string().begin(), b.filename().string().end(), re);
    std::sregex_iterator end;

    while (it_a != end && it_b != end) {
        int na = std::stoi((*it_a).str());
        int nb = std::stoi((*it_b).str());
        if (na != nb)
            return na < nb;
        ++it_a;
        ++it_b;
    }

    return a.filename().string() < b.filename().string();
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << " لطفاً پوشه حاوی فایل‌های .pkg را روی این فایل exe بیندازید.\n";
        return 1;
    }

    fs::path input_dir(argv[1]);
    if (!fs::exists(input_dir) || !fs::is_directory(input_dir)) {
        std::cerr << " مسیر نامعتبر است یا پوشه وجود ندارد.\n";
        return 1;
    }

    std::vector<fs::path> pkg_files;
    for (const auto& entry : fs::directory_iterator(input_dir)) {
        if (entry.is_regular_file() && entry.path().extension() == ".pkg") {
            pkg_files.push_back(entry.path());
        }
    }

    if (pkg_files.empty()) {
        std::cerr << " هیچ فایل .pkg داخل پوشه یافت نشد.\n";
        return 1;
    }

    std::sort(pkg_files.begin(), pkg_files.end(), natural_compare);

    std::string out_name = input_dir.filename().string() + "-merged.pkg";
    fs::path out_path = input_dir / out_name;

    std::ofstream out(out_path, std::ios::binary);
    if (!out) {
        std::cerr << " خطا در ساخت فایل خروجی: " << out_path << '\n';
        return 1;
    }

    for (const auto& file : pkg_files) {
        std::ifstream in(file, std::ios::binary);
        if (!in) {
            std::cerr << " خطا در باز کردن فایل: " << file << '\n';
            continue;
        }

        out << in.rdbuf();
        std::cout << " اضافه شد: " << file.filename() << '\n';
    }

    std::cout << "\n<< فایل نهایی ساخته شد: " << out_path << '\n";
    return 0;
}
