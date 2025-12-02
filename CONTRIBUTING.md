# 贡献指南

感谢你愿意为 **FTMS** 做出贡献！下面的约定可帮助新同学快速上手，也让 PR 审阅更顺畅。

## 开发环境
- Qt 6.9.3 (含 Core/Gui/Widgets/Network/Sql) + MinGW 64-bit 或等价工具链
- CMake ≥ 3.26，推荐 Ninja 作为生成器
- MySQL/MariaDB 10.x+（本地开发建议启用本机实例）
- 可选：clang-format 16（若后续引入格式化钩子）

## 分支 & 提交流程
1. Fork 本仓库并 `git clone`。
2. 按功能创建分支：`feat/<summary>`、`fix/<issue-id>` 等。
3. 修改前请先 pull 主仓库最新代码：
   ```powershell
   git remote add upstream https://github.com/<owner>/FTMS.git
   git fetch upstream
   git rebase upstream/main
   ```
4. 提交信息遵循 Conventional Commits：
   - `feat: add order timeline`
   - `fix: handle db reconnect`
   - `chore: update ci workflow`
5. 推送到远程并创建 PR，模板中简述：
   - 变更内容 & 动机
   - 影响范围（UI/后端/数据库）
   - 自检步骤（构建、测试、截图）

## 必跑检查
| 模块 | 命令 |
| ---- | ---- |
| 前端 Qt 客户端 | `cmake -S frontend -B build/frontend` & `cmake --build build/frontend` |
| 后端 Qt 服务 | `cmake -S backend -B build/backend` & `cmake --build build/backend` |
| 集成构建 | `cmake -S . -B build` & `cmake --build build` |
| 数据库脚本 | 确保 `init_flights.sql` 可在空库执行通过 |
| 自动化测试 | 若添加测试，请补充 `ctest --output-on-failure` 结果 |

## 代码风格
- C++ 17，保持头/源拆分，公共模型放在 `common/include`。
- Qt 信号槽优先使用新语法：`connect(sender, &Sender::signal, this, &Receiver::slot);`
- 网络/数据库相关日志尽量使用 `qDebug()/qWarning()` 并包含上下文。
- UI 代码需加简短注释说明布局意图；CSS/QSS 建议集中在 `main_window.cpp`。

## 数据与配置
- 不要提交真实数据库密码、API Key，可放入 `.env` 或 `*.template.json`。
- 若迁移脚本或配置有更新，请同步 README 及示例文件。

## Issue & PR
- Issue 模板建议包含：环境、复现步骤、期望/实际结果、日志截图。
- PR 通过后记得同步最新 `main`，避免遗留分支。

欢迎你在文档、UI、稳定性、CI 等任意方面贡献力量，也可以提出建议或 TODO。若不确定实现方式，直接开 Issue 讨论即可。感谢支持！
