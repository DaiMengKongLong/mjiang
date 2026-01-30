# RapidJSON 集成指南

> 本文档说明如何将 rapidjson 集成到项目中，替换简单的字符串解析。

---

## 为什么需要 RapidJSON？

当前项目使用简单的字符串解析来提取 JSON 字段，这种方式有以下限制：

- ❌ 不支持嵌套对象和数组
- ❌ 解析复杂 JSON 容易出错
- ❌ 无法验证 JSON 格式是否正确
- ❌ 构建 JSON 对象需要手动拼接字符串

使用 rapidjson 可以：
- ✅ 完整支持所有 JSON 特性（嵌套对象、数组等）
- ✅ 自动验证 JSON 格式
- ✅ 提供类型安全的 API
- ✅ 支持构建复杂的 JSON 对象

---

## 集成步骤

### 方法 1：使用 Git Submodule（推荐）

```bash
cd server
git submodule add https://github.com/Tencent/rapidjson.git third_party/rapidjson
git submodule update --init --recursive
```

### 方法 2：直接下载

```bash
cd server
mkdir -p third_party
cd third_party
git clone https://github.com/Tencent/rapidjson.git
```

### 方法 3：使用系统安装的 rapidjson

如果系统已安装 rapidjson（如通过包管理器），可以直接使用：

```bash
# macOS
brew install rapidjson

# Linux (Ubuntu/Debian)
apt-get install rapidjson-dev
```

---

## 更新 CMakeLists.txt

在 `server/CMakeLists.txt` 中添加 rapidjson 头文件路径：

```cmake
# 如果使用 git submodule
target_include_directories(mahjong_server_ws PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/rapidjson/include
)

# 或如果使用系统安装的 rapidjson
# find_package(RapidJSON REQUIRED)
# target_link_libraries(mahjong_server_ws PRIVATE RapidJSON::rapidjson)
```

---

## 更新 JsonHelper.cpp

修改 `server/src/JsonHelper.cpp`，使用 rapidjson API：

```cpp
#include "JsonHelper.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <sstream>

namespace JsonHelper {
    
    std::string getString(const std::string& json, const std::string& key) {
        rapidjson::Document doc;
        doc.Parse(json.c_str());
        
        if (doc.HasParseError() || !doc.IsObject()) {
            return "";
        }
        
        if (!doc.HasMember(key.c_str()) || !doc[key.c_str()].IsString()) {
            return "";
        }
        
        return doc[key.c_str()].GetString();
    }
    
    int getInt(const std::string& json, const std::string& key) {
        rapidjson::Document doc;
        doc.Parse(json.c_str());
        
        if (doc.HasParseError() || !doc.IsObject()) {
            return 0;
        }
        
        if (!doc.HasMember(key.c_str()) || !doc[key.c_str()].IsInt()) {
            return 0;
        }
        
        return doc[key.c_str()].GetInt();
    }
    
    bool hasKey(const std::string& json, const std::string& key) {
        rapidjson::Document doc;
        doc.Parse(json.c_str());
        
        if (doc.HasParseError() || !doc.IsObject()) {
            return false;
        }
        
        return doc.HasMember(key.c_str());
    }
    
    std::vector<int> getIntArray(const std::string& json, const std::string& key) {
        std::vector<int> result;
        rapidjson::Document doc;
        doc.Parse(json.c_str());
        
        if (doc.HasParseError() || !doc.IsObject()) {
            return result;
        }
        
        if (!doc.HasMember(key.c_str()) || !doc[key.c_str()].IsArray()) {
            return result;
        }
        
        const rapidjson::Value& arr = doc[key.c_str()];
        for (rapidjson::SizeType i = 0; i < arr.Size(); i++) {
            if (arr[i].IsInt()) {
                result.push_back(arr[i].GetInt());
            }
        }
        
        return result;
    }
    
    std::vector<std::string> getStringArray(const std::string& json, const std::string& key) {
        std::vector<std::string> result;
        rapidjson::Document doc;
        doc.Parse(json.c_str());
        
        if (doc.HasParseError() || !doc.IsObject()) {
            return result;
        }
        
        if (!doc.HasMember(key.c_str()) || !doc[key.c_str()].IsArray()) {
            return result;
        }
        
        const rapidjson::Value& arr = doc[key.c_str()];
        for (rapidjson::SizeType i = 0; i < arr.Size(); i++) {
            if (arr[i].IsString()) {
                result.push_back(arr[i].GetString());
            }
        }
        
        return result;
    }
    
    std::string buildJson(const std::map<std::string, std::string>& stringFields,
                         const std::map<std::string, int>& intFields) {
        rapidjson::Document doc;
        doc.SetObject();
        rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
        
        // 添加字符串字段
        for (const auto& pair : stringFields) {
            rapidjson::Value key(pair.first.c_str(), allocator);
            rapidjson::Value value(pair.second.c_str(), allocator);
            doc.AddMember(key, value, allocator);
        }
        
        // 添加整数字段
        for (const auto& pair : intFields) {
            rapidjson::Value key(pair.first.c_str(), allocator);
            doc.AddMember(key, pair.second, allocator);
        }
        
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        doc.Accept(writer);
        
        return buffer.GetString();
    }
    
} // namespace JsonHelper
```

---

## 更新 JsonHelper.h

修改 `server/src/JsonHelper.h`，取消注释 rapidjson 相关代码：

```cpp
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
```

---

## 测试

编译并运行服务器，测试 JSON 解析是否正常工作：

```bash
cd server/build
cmake ..
make
./mahjong_server_ws
```

使用浏览器控制台测试（参考 `TESTING_GUIDE.md`），确认能正确解析嵌套对象和数组。

---

## 优势

使用 rapidjson 后，可以：

1. **解析复杂的 JSON**：
   ```json
   {
     "type": "room_info",
     "players": [
       {"seat": 0, "playerId": "user_001", "nickname": "玩家1"},
       {"seat": 1, "playerId": "user_002", "nickname": "玩家2"}
     ]
   }
   ```

2. **验证 JSON 格式**：自动检测格式错误

3. **类型安全**：编译时检查类型，减少运行时错误

4. **性能更好**：rapidjson 是高性能的 JSON 库

---

## 注意事项

- rapidjson 是 header-only 库，不需要链接额外的库文件
- 确保 rapidjson 版本兼容（建议使用最新稳定版）
- 如果遇到编译错误，检查头文件路径是否正确
