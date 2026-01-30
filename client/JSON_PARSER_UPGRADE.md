# 客户端 JSON 解析升级指南

> 本文档说明如何将客户端的简单 JSON 解析替换为 rapidjson。

---

## 当前状态

客户端 `NetGameController.cpp` 中的 `JsonHelper` 使用简单的字符串解析，有以下限制：

- ❌ 不支持嵌套对象和数组
- ❌ 解析 `room_info` 中的 `players` 数组可能失败
- ❌ 无法验证 JSON 格式

---

## Cocos2d-x 中的 JSON 库

Cocos2d-x 通常自带 rapidjson，可以直接使用。检查方法：

1. 查看 Cocos2d-x 源码目录：`cocos2d/external/json/rapidjson/`
2. 或查看项目配置，确认是否已包含 rapidjson

---

## 升级步骤

### 1. 确认 rapidjson 可用

在 `NetGameController.cpp` 中尝试包含：

```cpp
#include "json/rapidjson/document.h"
#include "json/rapidjson/writer.h"
#include "json/rapidjson/stringbuffer.h"
```

如果编译通过，说明 rapidjson 可用。

### 2. 更新 NetGameController.cpp

替换 `JsonHelper` 命名空间的实现：

```cpp
#include "json/rapidjson/document.h"
#include "json/rapidjson/writer.h"
#include "json/rapidjson/stringbuffer.h"

namespace JsonHelper {
    
    std::string getString(const std::string& json, const std::string& key) {
        rapidjson::Document doc;
        doc.Parse(json.c_str());
        
        if (doc.HasParseError() || !doc.IsObject()) {
            CCLOGWARN("[JsonHelper] JSON 解析失败或不是对象");
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
    
} // namespace JsonHelper
```

### 3. 完善 handleRoomInfo

使用 rapidjson 后，可以正确解析 `players` 数组：

```cpp
void NetGameController::handleRoomInfo(const std::string& jsonText) {
    rapidjson::Document doc;
    doc.Parse(jsonText.c_str());
    
    if (doc.HasParseError() || !doc.IsObject()) {
        CCLOGERROR("[NetGameController] 解析 room_info 失败");
        return;
    }
    
    RoomInfo info;
    if (doc.HasMember("roomId") && doc["roomId"].IsString()) {
        info.roomId = doc["roomId"].GetString();
    }
    
    if (doc.HasMember("players") && doc["players"].IsArray()) {
        const rapidjson::Value& players = doc["players"];
        for (rapidjson::SizeType i = 0; i < players.Size(); i++) {
            const rapidjson::Value& player = players[i];
            if (player.IsObject()) {
                RoomInfo::PlayerInfo pInfo;
                if (player.HasMember("seat") && player["seat"].IsInt()) {
                    pInfo.seat = player["seat"].GetInt();
                }
                if (player.HasMember("playerId") && player["playerId"].IsString()) {
                    pInfo.playerId = player["playerId"].GetString();
                }
                if (player.HasMember("nickname") && player["nickname"].IsString()) {
                    pInfo.nickname = player["nickname"].GetString();
                }
                info.players.push_back(pInfo);
            }
        }
    }
    
    if (gameLayer_) {
        gameLayer_->onRoomInfo(info);
    }
}
```

---

## 测试

升级后，测试以下场景：

1. **解析 room_info**：确认能正确解析 `players` 数组
2. **解析 deal_cards**：确认能正确解析 `handCards` 数组
3. **解析 ask_action**：确认能正确解析 `actions` 数组
4. **错误处理**：发送格式错误的 JSON，确认不会崩溃

---

## 注意事项

- Cocos2d-x 的 rapidjson 路径可能不同，根据实际项目调整 include 路径
- 如果项目中没有 rapidjson，可以参考服务器端的 `RAPIDJSON_SETUP.md` 添加
- 确保所有 JSON 解析都通过 `JsonHelper`，不要直接使用 rapidjson API

---

## 优势

使用 rapidjson 后：

- ✅ 完整支持嵌套对象和数组
- ✅ 自动验证 JSON 格式
- ✅ 类型安全，减少运行时错误
- ✅ 性能更好
