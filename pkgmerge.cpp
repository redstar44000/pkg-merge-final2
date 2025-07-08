#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <algorithm>

namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: drag a folder onto the exe.\n";
        return 1;
    }

    fs::path folder = argv[1];
    if (!fs::exists(folder) || !fs::is_directory(folder)) {
        std::cerr << "Invalid folder.\n";
        return 1;
    }

    std::vector<fs::path> files;
    for (auto& entry : fs::directory_iterator(folder)) {
        if (entry.is_regular_file() && entry.path().extension() == ".pkg")
            files.push_back(entry.path());
    }

    std::sort(files.begin(), files.end(), [](const fs::path& a, const fs::path& b) {
        return a.filename().string() < b.filename().string();
    });

    if (files.empty()) {
        std::cerr << "No .pkg files found.\n";
        return 1;
    }

    fs::path out_path = folder / "merged.pkg";
    std::ofstream out(out_path, std::ios::binary);

    for (auto& file : files) {
        std::ifstream in(file, std::ios::binary);
        if (!in) {
            std::cerr << "Could not open file: " << file << "\n";
            continue;
        }

        out << in.rdbuf();
        std::cout << "Added: " << file.filename() << "\n";
    }

    std::cout << "\nFinal file created: " << out_path << "\n";
    return 0;
}
