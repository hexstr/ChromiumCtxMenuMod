#pragma once

#include <istream>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace ini {

    class IniFile {
    public:
        // RAII：默认构造为空；带参构造会立刻尝试加载
        IniFile() = default;
        explicit IniFile(const std::string& path); // 从文件加载
        explicit IniFile(std::istream& in);        // 从流加载

        // 手动加载（会清空已有内容）
        bool LoadFromFile(const std::string& path);
        bool LoadFromStream(std::istream& in);

        // 是否已成功加载（文件路径版本失败时为 false）
        bool IsValid() const { return loaded_; }

        // 查询接口（不修改内部状态，const）
        std::optional<int> ReadInt(const std::string& section,
                                   const std::string& key) const;
        std::optional<std::string> ReadString(const std::string& section,
                                              const std::string& key) const;
        std::unordered_set<int> ReadIntSet(const std::string& section,
                                           const std::string& key) const;

        // 辅助接口（可选）
        bool Contains(const std::string& section, const std::string& key) const;
        std::unordered_set<std::string> Sections() const;

    private:
        using KeyMap = std::unordered_map<std::string, std::string>;

        std::unordered_map<std::string, KeyMap> data_;
        bool loaded_ = false;
    };

} // namespace ini