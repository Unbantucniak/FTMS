# 扶摇航空票务系统 (FTMS)

一个包含 **Qt 桌面前端** 与 **Qt/C++ 后端服务** 的演示级航班票务系统，示范航班查询、预订、改签及订单管理流程。本仓库已整理为便于在 GitHub 上分享与协作的结构，新的贡献者可以按本文档快速完成本地构建与部署。

## 目录结构
```
.
├─ backend/           # QtBackendServer：负责 TCP 服务与 MySQL 数据访问
├─ frontend/          # QtDesktopClient：桌面 UI 与业务交互
├─ common/include/    # data_model.h 等共享结构体
├─ init_flights.sql   # 样例数据（航班、用户、订票记录）
├─ CMakeLists.txt     # 根构建脚本，串联前后端
└─ build/             # 推荐的本地构建输出目录（已加入 .gitignore）
```
> 如果你有额外的脚本或服务，请在 README 中补充新的目录说明，保持贡献者对整体结构的认知。

## 依赖环境
| 组件 | 说明 |
| ---- | ---- |
| Qt 6.9.3 (MinGW 64-bit) | 提供 Core/Gui/Widgets/Network/Sql 模块，前后端均需 |
| CMake ≥ 3.26 & Ninja(可选) | 正式构建脚本使用 CMake；使用 Ninja 可缩短构建时间 |
| MySQL / MariaDB 10.x+ | 后端示例默认连接本地 `FTMSDB` 数据库 |
| 编译器 | Windows: MinGW 13 (Qt 6.9.3 附带)；Linux/macOS 使用 clang/gcc 11+ |

> 本仓库示例在 Windows 11 + Qt 6.9.3 MinGW 环境开发。若你使用 MSVC/Clang，请确保已安装对应的 Qt 工具链。

## 快速开始
### 1. 克隆仓库
```powershell
git clone https://github.com/<your-account>/FTMS.git
cd FTMS
```

### 2. 准备数据库
1. 新建数据库：`CREATE DATABASE FTMSDB CHARACTER SET utf8mb4;`
2. 导入示例数据：
   ```powershell
   mysql -u root -p FTMSDB < init_flights.sql
   ```
3. 根据需要修改 `backend/main.cpp` 中的数据库主机、端口、账号及密码；也可改成读取环境变量（参考 `.env.example`）。

### 3. 一次性构建前后端
```powershell
cmake -S . -B build -G "Ninja" -DQT_HOST_PATH="%Qt6_DIR%"
cmake --build build
```

- 构建成功后，在 `build/backend/QtBackendServer.exe` 和 `build/frontend/QtDesktopClient.exe` 可找到可执行程序。
- 若你只想构建其中一端，可在 `cmake --build build --target QtBackendServer`（或 `QtDesktopClient`）。

### 4. 运行
1. 先启动后端（需提前保证 MySQL 服务就绪）：
   ```powershell
   .\build\backend\QtBackendServer.exe
   ```
2. 再启动前端：
   ```powershell
   .\build\frontend\QtDesktopClient.exe
   ```
3. 默认 TCP 端口为 `12345`。若需修改，请同步更新前端 `network/tcp_client.cpp` 中的连接参数。

## 配置与环境变量
- `.env.example` 提供了数据库主机、账号、端口、后端监听端口等字段，维护者可复制为 `.env` 并在自定义加载逻辑时复用。
- 当前版本仍在源码中直接设置数据库/端口，适合演示或本地开发。若你要部署到生产环境，建议：
  1. 将 `backend/main.cpp` 读取 `.env` 或 `QSettings`；
  2. 将 `frontend/network/tcp_client.cpp` 中的服务器地址改为配置项；
  3. 避免将真实账号密码提交到仓库。

## 常见工作流
| 任务 | 命令 |
| ---- | ---- |
| 清理并重新配置 | `cmake -S . -B build -G "Ninja" -DCMAKE_BUILD_TYPE=Release -U *` |
| 前端调试构建 | `cmake --build build --target QtDesktopClient` |
| 后端调试构建 | `cmake --build build --target QtBackendServer` |
| 运行单元/组件测试 | `ctest --test-dir build --output-on-failure` *(如后续添加测试)* |

## 贡献指南
- 参考 `CONTRIBUTING.md`，其中包含分支命名、编码规范、提交流程与必跑检查。
- 建议先开 Issue 说明需求或缺陷，再提交 PR，便于讨论实现方案。

## CI / GitHub Actions
- `.github/workflows/build.yml` 会在 push/PR 时自动安装 Qt 6.9.3、构建前后端并运行可能存在的测试。
- 在开启仓库前请确认 GitHub Actions 已启用，并根据需要调节 Qt 版本或构建矩阵。

## 许可证
- 请根据你的开源策略在此处注明（如 MIT、Apache-2.0 等）。若尚未决定，可临时写 “TBD”。

---
若你在部署或二次开发过程中遇到问题，欢迎通过 Issue 反馈，或补充到 README 以帮助更多贡献者。祝使用顺利！
