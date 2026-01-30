//
// JsonHelper.cpp
// JSON 解析辅助类实现
//
// 说明：
// 当前使用简单字符串解析作为临时方案。
// 要使用 rapidjson，需要：
// 1. 添加 rapidjson 到项目（git submodule 或直接下载）
// 2. 修改此文件，使用 rapidjson 的 API
// 3. 更新 CMakeLists.txt 添加 rapidjson 头文件路径
//

#include "JsonHelper.h"
#include <sstream>
#include <algorithm>
#include <map>

namespace JsonHelper {
    
    // ========== 简单字符串解析（临时方案）==========
    // 注意：这些函数对嵌套对象和数组支持不完整
    // 实际项目中应使用 rapidjson
    
    std::string getString(const std::string& json, const std::string& key) {
        std::string searchKey = "\"" + key + "\"";
        size_t pos = json.find(searchKey);
        if (pos == std::string::npos) return "";
        
        pos = json.find(":", pos);
        if (pos == std::string::npos) return "";
        pos++;
        
        while (pos < json.length() && (json[pos] == ' ' || json[pos] == '\t')) pos++;
        
        if (pos >= json.length()) return "";
        
        if (json[pos] == '"') {
            pos++;
            size_t end = json.find('"', pos);
            if (end == std::string::npos) return "";
            return json.substr(pos, end - pos);
        }
        
        return "";
    }
    
    int getInt(const std::string& json, const std::string& key) {
        std::string searchKey = "\"" + key + "\"";
        size_t pos = json.find(searchKey);
        if (pos == std::string::npos) return 0;
        
        pos = json.find(":", pos);
        if (pos == std::string::npos) return 0;
        pos++;
        
        while (pos < json.length() && (json[pos] == ' ' || json[pos] == '\t')) pos++;
        
        int value = 0;
        bool negative = false;
        if (pos < json.length() && json[pos] == '-') {
            negative = true;
            pos++;
        }
        
        while (pos < json.length() && json[pos] >= '0' && json[pos] <= '9') {
            value = value * 10 + (json[pos] - '0');
            pos++;
        }
        
        return negative ? -value : value;
    }
    
    bool hasKey(const std::string& json, const std::string& key) {
        std::string searchKey = "\"" + key + "\"";
        return json.find(searchKey) != std::string::npos;
    }
    
    std::vector<int> getIntArray(const std::string& json, const std::string& key) {
        std::vector<int> result;
        std::string searchKey = "\"" + key + "\"";
        size_t pos = json.find(searchKey);
        if (pos == std::string::npos) return result;
        
        pos = json.find("[", pos);
        if (pos == std::string::npos) return result;
        pos++;
        
        size_t end = json.find("]", pos);
        if (end == std::string::npos) return result;
        
        std::string arrayStr = json.substr(pos, end - pos);
        std::istringstream iss(arrayStr);
        std::string token;
        while (std::getline(iss, token, ',')) {
            token.erase(0, token.find_first_not_of(" \t"));
            token.erase(token.find_last_not_of(" \t") + 1);
            if (!token.empty()) {
                try {
                    result.push_back(std::stoi(token));
                } catch (...) {
                    // 忽略解析错误
                }
            }
        }
        
        return result;
    }
    
    std::vector<std::string> getStringArray(const std::string& json, const std::string& key) {
        std::vector<std::string> result;
        std::string searchKey = "\"" + key + "\"";
        size_t pos = json.find(searchKey);
        if (pos == std::string::npos) return result;
        
        pos = json.find("[", pos);
        if (pos == std::string::npos) return result;
        pos++;
        
        size_t end = json.find("]", pos);
        if (end == std::string::npos) return result;
        
        std::string arrayStr = json.substr(pos, end - pos);
        std::istringstream iss(arrayStr);
        std::string token;
        while (std::getline(iss, token, ',')) {
            token.erase(0, token.find_first_not_of(" \t\""));
            token.erase(token.find_last_not_of(" \t\"") + 1);
            if (!token.empty()) {
                result.push_back(token);
            }
        }
        
        return result;
    }
    
    std::string buildJson(const std::map<std::string, std::string>& stringFields,
                         const std::map<std::string, int>& intFields) {
        std::ostringstream oss;
        oss << "{";
        
        bool first = true;
        
        // 添加字符串字段
        for (const auto& pair : stringFields) {
            if (!first) oss << ",";
            oss << "\"" << pair.first << "\":\"" << pair.second << "\"";
            first = false;
        }
        
        // 添加整数字段
        for (const auto& pair : intFields) {
            if (!first) oss << ",";
            oss << "\"" << pair.first << "\":" << pair.second;
            first = false;
        }
        
        oss << "}";
        return oss.str();
    }
    
} // namespace JsonHelper
