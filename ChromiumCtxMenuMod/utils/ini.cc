#include "ini.hh"

#include <cctype>
#include <fstream>
#include <sstream>

namespace ini {

    namespace {

        std::string Trim(const std::string& s) {
            const auto start = s.find_first_not_of(" \t\r\n");
            if (start == std::string::npos)
                return "";
            const auto end = s.find_last_not_of(" \t\r\n");
            return s.substr(start, end - start + 1);
        }

    } // namespace

    // ---------------- 构造 / 加载 ----------------

    IniFile::IniFile(const std::string& path) {
        LoadFromFile(path);
    }

    IniFile::IniFile(std::istream& in) {
        LoadFromStream(in);
    }

    bool IniFile::LoadFromFile(const std::string& path) {
        std::ifstream fin(path);
        if (!fin) {
            data_.clear();
            loaded_ = false;
            return false;
        }
        return LoadFromStream(fin);
    }

    bool IniFile::LoadFromStream(std::istream& in) {
        data_.clear();

        std::string line;
        std::string current_section;

        while (std::getline(in, line)) {
            line = Trim(line);
            if (line.empty() || line[0] == ';' || line[0] == '#')
                continue;

            // [section]
            if (line.front() == '[' && line.back() == ']') {
                current_section = Trim(line.substr(1, line.size() - 2));
                continue;
            }

            const auto pos = line.find('=');
            if (pos == std::string::npos)
                continue;

            const std::string k = Trim(line.substr(0, pos));
            const std::string v = Trim(line.substr(pos + 1));
            if (k.empty())
                continue;

            // 重复 key 以“最后一次为准”（如需“第一次为准”改这里即可）
            data_[current_section][k] = v;
        }

        loaded_ = true;
        return true;
    }

    // ---------------- 查询 ----------------

    namespace {

        // 在 data_ 里找 (section, key)，找不到返回 nullptr
        const std::string* Lookup(
            const std::unordered_map<std::string,
                                     std::unordered_map<std::string, std::string>>& data,
            const std::string& section,
            const std::string& key) {
            auto sit = data.find(section);
            if (sit == data.end())
                return nullptr;
            auto kit = sit->second.find(key);
            if (kit == sit->second.end())
                return nullptr;
            return &kit->second;
        }

    } // namespace

    std::optional<int> IniFile::ReadInt(const std::string& section,
                                        const std::string& key) const {
        const std::string* v = Lookup(data_, section, key);
        if (!v)
            return std::nullopt;

        std::istringstream ss(*v);
        int val;
        if (ss >> val)
            return val;
        return std::nullopt;
    }

    std::optional<std::string> IniFile::ReadString(const std::string& section,
                                                   const std::string& key) const {
        const std::string* v = Lookup(data_, section, key);
        if (!v)
            return std::nullopt;
        return *v;
    }

    std::unordered_set<int> IniFile::ReadIntSet(const std::string& section,
                                                const std::string& key) const {
        std::unordered_set<int> result;
        const std::string* v = Lookup(data_, section, key);
        if (!v)
            return result;

        std::string normalized = *v;
        for (char& c : normalized) {
            if (c == ',' || c == ';' || c == '\t')
                c = ' ';
        }

        std::istringstream ss(normalized);
        int val;
        while (ss >> val) {
            result.insert(val);
        }
        return result;
    }

    bool IniFile::Contains(const std::string& section,
                           const std::string& key) const {
        return Lookup(data_, section, key) != nullptr;
    }

    std::unordered_set<std::string> IniFile::Sections() const {
        std::unordered_set<std::string> out;
        for (const auto& kv : data_)
            out.insert(kv.first);
        return out;
    }

} // namespace ini