//
// JsonHelper.h
// JSON 解析辅助类（基于 rapidjson）
//
// 说明：
// 封装 rapidjson 的常用操作，提供简单的接口用于解析协议消息。
// 如果项目中没有 rapidjson，可以使用 git submodule 添加：
//   git submodule add https://github.com/Tencent/rapidjson.git third_party/rapidjson
//

#ifndef JSON_HELPER_H
#define JSON_HELPER_H

#include <string>
#include <vector>
#include <map>

// 如果使用 rapidjson，取消注释以下行并包含头文件
// #include "rapidjson/document.h"
// #include "rapidjson/writer.h"
// #include "rapidjson/stringbuffer.h"

// 临时方案：如果还没有 rapidjson，使用简单解析
// 后续替换为 rapidjson 时，只需要修改这个文件即可

namespace JsonHelper {
    // 解析 JSON 字符串为 Document 对象（rapidjson）
    // 如果使用 rapidjson：
    //   rapidjson::Document parse(const std::string& json);
    
    // 从 JSON 字符串中提取字段值（当前使用简单解析，后续替换为 rapidjson）
    std::string getString(const std::string& json, const std::string& key);
    int getInt(const std::string& json, const std::string& key);
    bool hasKey(const std::string& json, const std::string& key);
    
    // 解析数组（rapidjson 支持）
    std::vector<int> getIntArray(const std::string& json, const std::string& key);
    std::vector<std::string> getStringArray(const std::string& json, const std::string& key);
    
    // 构建 JSON 对象（rapidjson 支持）
    std::string buildJson(const std::map<std::string, std::string>& stringFields,
                         const std::map<std::string, int>& intFields);
}

#endif // JSON_HELPER_H
