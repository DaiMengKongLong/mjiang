## 联机麻将协议说明（初版）

> 本文档用于约定 C++ 服务器与 Cocos2d-x 客户端之间的消息格式，便于客户端与服务器并行开发。  
> 目前采用 **WebSocket/TCP + JSON 文本协议** 的形式，后续如需改为二进制/Protobuf，只需保持字段语义一致。

---

### 1. 基本约定

- **字段命名**：全部使用 `camelCase`。
- **编码**：UTF-8。
- **消息顶层字段**：
  - `type`：字符串，消息类型标识。
  - 其余字段根据不同 `type` 决定。

示例：

```json
{
  "type": "join_room",
  "roomId": "12345",
  "playerId": "user_001",
  "nickname": "玩家1"
}
```

---

### 2. 客户端 -> 服务器（C2S）消息

#### 2.1 加入房间 `join_room`

- **说明**：客户端请求加入某个房间（或创建并加入）。
- **方向**：C2S

```json
{
  "type": "join_room",
  "roomId": "12345",
  "playerId": "user_001",
  "nickname": "玩家1"
}
```

- **字段说明**：
  - `roomId`：房间号，字符串。为空时可由服务器分配。
  - `playerId`：玩家唯一 ID。
  - `nickname`：玩家昵称，用于显示。

#### 2.2 出牌 `play_card`

- **说明**：轮到自己出牌时，从手牌中打出一张牌。
- **方向**：C2S

```json
{
  "type": "play_card",
  "card": 23
}
```

- `card`：整型牌编号，编码规则与服务器 `GameLogic` 一致（例如 0~33）。

#### 2.3 选择动作 `choose_action`

- **说明**：当服务器提示可碰/杠/胡/过时，客户端返回选择的动作。
- **方向**：C2S

```json
{
  "type": "choose_action",
  "action": "PENG",
  "card": 23
}
```

- `action`：字符串，取值范围：
  - `"PENG"` / `"GANG"` / `"HU"` / `"GUO"`（过）
- `card`：相关牌的编号。

---

### 3. 服务器 -> 客户端（S2C）消息

#### 3.1 房间信息 `room_info`

- **说明**：加入房间成功后，服务器返回当前房间和玩家座位信息。
- **方向**：S2C

```json
{
  "type": "room_info",
  "roomId": "12345",
  "players": [
    { "seat": 0, "playerId": "user_001", "nickname": "玩家1" },
    { "seat": 1, "playerId": "user_002", "nickname": "玩家2" }
  ]
}
```

- `players`：
  - `seat`：座位号，从 0 开始。
  - `playerId`：玩家 ID。
  - `nickname`：玩家昵称。

#### 3.2 发牌 `deal_cards`

- **说明**：一局开始时，向每个玩家发送自己的起始手牌。
- **方向**：S2C

```json
{
  "type": "deal_cards",
  "seat": 0,
  "handCards": [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13]
}
```

- `seat`：该手牌所属玩家的座位号。
- `handCards`：整型数组，表示该玩家的起始手牌。

#### 3.3 轮到你出牌 `your_turn`

- **说明**：服务器通知本客户端轮到自己出牌。
- **方向**：S2C

```json
{
  "type": "your_turn",
  "allowedActions": ["PLAY_CARD"]
}
```

- `allowedActions`：当前允许的操作列表，初版可以固定为 `["PLAY_CARD"]`。

#### 3.4 某玩家打出一张牌 `player_play_card`

- **说明**：某个座位玩家打出了一张牌，需在 UI 上更新牌河。
- **方向**：S2C

```json
{
  "type": "player_play_card",
  "seat": 1,
  "card": 23
}
```

- `seat`：出牌玩家座位号。
- `card`：打出的牌编号。

#### 3.5 询问动作 `ask_action`

- **说明**：服务器检查规则后，发现本玩家可以对刚打出的牌进行碰/杠/胡/过，询问玩家选择。
- **方向**：S2C

```json
{
  "type": "ask_action",
  "card": 23,
  "actions": ["PENG", "GANG", "GUO"]
}
```

- `card`：触发动作的牌。
- `actions`：可选动作列表，客户端据此显示对应按钮。

#### 3.6 一局结算 `round_result`

- **说明**：一局结束后的结算信息。
- **方向**：S2C

```json
{
  "type": "round_result",
  "winnerSeat": 2,
  "scores": [10, -5, -5, 0],
  "detail": {
    "winnerFan": 3,
    "fanTypes": ["PING_HU", "MEN_QING"]
  }
}
```

- `winnerSeat`：赢家座位号，流局时可为 -1。
- `scores`：每个座位本局输赢分。
- `detail`：可选的扩展信息（番型等），客户端可选择展示。

---

### 4. 错误与状态消息（预留）

#### 4.1 通用错误 `error`

```json
{
  "type": "error",
  "code": "INVALID_ACTION",
  "message": "当前状态无法执行该操作"
}
```

- `code`：错误码字符串。
- `message`：人类可读描述，便于调试。

> 后续在服务器和客户端中遇到新的错误场景时，请同步更新此文档，并在 `dev_log.md` 里记录触发条件与修复方案。

