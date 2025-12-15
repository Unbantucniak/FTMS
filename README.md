# 扶摇航空票务系统 (FTMS)

一个包含 **Qt 桌面前端** 与 **Qt/C++ 后端服务** 的演示级航班票务系统，示范航班查询、预订、改签及订单管理流程。本仓库已整理为便于在 GitHub 上分享与协作的结构，新的贡献者可以按本文档快速完成本地构建与部署。

## 目录结构
```
.
├─ backend/           # QtBackendServer：负责 TCP 服务与 SQLite 数据访问
│  ├─ db/             # 数据库管理模块（SQLite）
│  ├─ network/        # TCP 服务器与客户端连接处理
│  └─ ai/             # 出行助手（智能客服）
├─ frontend/          # QtDesktopClient：桌面 UI 与业务交互
│  ├─ ui/             # 主窗口、登录注册、航班搜索、订单管理等界面
│  └─ network/        # TCP 客户端通信模块
├─ common/include/    # data_model.h 等共享结构体
├─ tools/             # 辅助工具
│  └─ generate_flights.py  # 航班数据生成器（10000+ 条航班）
├─ CMakeLists.txt     # 根构建脚本，串联前后端
└─ build/             # 推荐的本地构建输出目录（已加入 .gitignore）
```
> SQLite 数据库文件 `ftms.db` 会在首次运行后端时自动创建在 build/backend 目录下。

## 依赖环境
| 组件 | 说明 |
| ---- | ---- |
| Qt 6.9.3 (MinGW 64-bit) | 提供 Core/Gui/Widgets/Network/Sql 模块，前后端均需 |
| CMake ≥ 3.26 & Ninja(可选) | 正式构建脚本使用 CMake；使用 Ninja 可缩短构建时间 |
| SQLite (Qt 内置) | 零配置数据库，Qt 自带 QSQLITE 驱动，无需额外安装 |
| Python 3.7+ (可选) | 用于运行 `tools/generate_flights.py` 生成航班数据 |
| 编译器 | Windows: MinGW 13 (Qt 6.9.3 附带)；Linux/macOS 使用 clang/gcc 11+ |

> 本仓库在 Windows 11 + Qt 6.9.3 MinGW 环境开发。使用 SQLite 单文件数据库，开箱即用。

## 快速开始
### 1. 克隆仓库
```powershell
git clone https://github.com/<your-account>/FTMS.git
cd FTMS
```

### 2. 构建前后端
```powershell
cmake -S . -B build -G "Ninja"
cmake --build build
```

- 构建成功后，在 `build/Desktop_Qt_6_9_3_MinGW_64_bit-Debug/backend/QtBackendServer.exe` 和 `frontend/QtDesktopClient.exe` 可找到可执行程序。
- 若只想构建某一端：`cmake --build build --target QtBackendServer`（或 `QtDesktopClient`）。

### 3. 初次运行（创建数据库）
**首次启动后端会自动创建空数据库表结构：**
```powershell
cd build\Desktop_Qt_6_9_3_MinGW_64_bit-Debug
.\backend\QtBackendServer.exe
```
数据库文件 `ftms.db` 将自动创建在当前目录。

### 4. 导入航班数据（可选）
使用 Python 工具生成 10000+ 条中国主要机场的航班数据：
```powershell
cd tools
python generate_flights.py
```
按提示输入数据库路径（或直接回车使用默认路径），数据将自动导入。

### 5. 启动系统
1. **启动后端服务器**：
   ```powershell
   cd build\Desktop_Qt_6_9_3_MinGW_64_bit-Debug
   .\backend\QtBackendServer.exe
   ```
   看到 "服务器正在监听端口 12345..." 表示成功。

2. **启动客户端**：
   ```powershell
   .\frontend\QtDesktopClient.exe
   ```

3. **默认连接配置**：
   - 服务器地址：`127.0.0.1`
   - TCP 端口：`12345`
   - 若需修改，请同步更新前端 `network/tcp_client.cpp` 和后端 `main.cpp`

## 核心功能
- ✈️ **航班查询**：按出发地、目的地、日期搜索航班，支持快速日期选择
- 🎫 **在线订票**：可视化座位选择（每排 6 座 A-F 布局），支持靠窗/过道偏好
- 📋 **订单管理**：查看历史订单、退票、改签
- 👤 **用户系统**：注册、登录、个人信息管理、密码修改
- 🤖 **出行助手**：ChatGPT 风格的智能客服界面，支持航班咨询
- 🎨 **现代 UI**：深色主题、圆角卡片设计、流畅动画效果

## 配置与部署
- **数据库位置**：默认在程序运行目录生成 `ftms.db`，可通过修改 `backend/main.cpp` 中的路径参数自定义
- **TCP 端口**：默认 `12345`，可在 `backend/main.cpp` 和 `frontend/network/tcp_client.cpp` 中修改
- **部署优势**：
  - ✅ 无需安装 MySQL/MariaDB
  - ✅ 数据库驱动 Qt 内置，无需额外配置
  - ✅ 单个 `.db` 文件，易于备份与迁移
  - ✅ 编译后的 exe 文件可直接复制到其他 Windows 电脑运行

## 数据生成工具
`tools/generate_flights.py` 提供了强大的航班数据生成能力：

| 特性 | 说明 |
| ---- | ---- |
| 机场数量 | 50+ 中国主要机场（北上广深成杭等一二线城市 + 拉萨、三亚等旅游城市） |
| 航空公司 | 15 家（国航、东航、南航、海航、春秋、吉祥等） |
| 航班数量 | 10000 条（可自定义） |
| 时间跨度 | 未来 60 天 |
| 价格策略 | 根据航线距离 + 热门程度智能定价 |
| 座位配置 | 120/132/144/156/168/180 座（6 的倍数，适配前端座位选择） |

**使用方法**：
```powershell
cd tools
python generate_flights.py
# 按提示输入数据库路径，或直接回车使用默认路径
```

## 常见工作流
| 任务 | 命令 |
| ---- | ---- |
| 清理并重新配置 | `cmake -S . -B build -G "Ninja" -DCMAKE_BUILD_TYPE=Release` |
| 前端调试构建 | `cmake --build build --target QtDesktopClient` |
| 后端调试构建 | `cmake --build build --target QtBackendServer` |
| 重置数据库 | 删除 `ftms.db` 后重新启动后端 |
| 生成新航班数据 | `python tools/generate_flights.py` |

## 贡献指南
- 参考 `CONTRIBUTING.md`，其中包含分支命名、编码规范、提交流程与必跑检查。
- 建议先开 Issue 说明需求或缺陷，再提交 PR，便于讨论实现方案。

---
若你在部署或二次开发过程中遇到问题，欢迎通过 Issue 反馈，或补充到 README 以帮助更多贡献者。祝使用顺利！
