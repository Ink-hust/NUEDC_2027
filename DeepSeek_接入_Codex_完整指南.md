# 将 DeepSeek 接入 Codex：完整配置、官方登录保留、模型切换与故障排查指南

> 适用于第一次使用 Codex / DeepSeek API 的用户，也适用于已经使用过其中任意一方、但尚未完成 DeepSeek 接入 Codex 的用户。

本文推荐使用：

- 最新版 Codex
- 最新版 CC Switch
- DeepSeek 官方原生 Responses API

本文**不推荐继续使用需要本地协议转换路由的旧版 DeepSeek 配置**。

如果你的 CC Switch 或 DeepSeek Provider 仍然提示 `Needs Routing`，请优先：

1. 升级 CC Switch；
2. 删除旧 DeepSeek Provider；
3. 使用最新版 DeepSeek 预设重新创建；
4. 使用原生 Responses 配置；

而不是继续沿用旧路由方案。

---

## 目录

1. 最终要实现什么
2. 需要准备什么
3. 核心概念：登录认证与模型 Provider 是两回事
4. 第一次配置前：先确保官方 Codex 正常
5. 检查 Codex 登录状态
6. Windows 找不到 `codex` 命令怎么办
7. 备份 Codex 配置
8. 开启 CC Switch 的官方登录保护
9. 添加 DeepSeek
10. 填写 DeepSeek API Key
11. 检查是否为原生 Responses 配置
12. 启用 DeepSeek
13. 为什么需要重启 Codex
14. 成功后的正常表现
15. 如何确认请求真的走 DeepSeek
16. 为什么 DeepSeek 模式下只有 DeepSeek 模型
17. 如何从 DeepSeek 切回 GPT
18. 如何重新切回 DeepSeek
19. 是否可以热切换
20. 旧配置需要路由时怎么办
21. 401 Unauthorized 的原因与修复
22. `config.toml` 排查指南
23. Windows 环境变量排查
24. 不同用户应该从哪里开始
25. 推荐的日常使用方式
26. 最终自检清单
27. 常见问题 FAQ
28. 官方下载与文档链接
29. 七条必须记住的原则

---

# 1. 最终要实现什么

完成配置后，推荐状态是：

```text
Codex
├─ 官方身份：ChatGPT / OpenAI OAuth
└─ 当前模型：DeepSeek

CC Switch
└─ 管理 DeepSeek Provider 与 API Key
```

简单来说：

```text
“你是谁”
由 ChatGPT / OpenAI 官方登录决定

“当前请求发给哪个模型”
由 CC Switch 当前 Provider 决定
```

因此完全可能出现下面这种状态：

```text
Codex 中显示：
已登录你的 ChatGPT 官方账号

实际使用模型：
DeepSeek
```

这是正常的。

---

# 2. 需要准备什么

## 2.1 Codex / ChatGPT 桌面客户端

OpenAI 官方下载页面：

- https://chatgpt.com/download/

OpenAI Codex 官方页面：

- https://openai.com/codex/

---

## 2.2 ChatGPT / OpenAI 官方账号

建议先使用官方账号正常登录一次 Codex。

本文推荐长期保留：

```text
ChatGPT OAuth 登录
```

而不是让 DeepSeek API Key 替代 Codex 的官方身份认证。

---

## 2.3 DeepSeek API Key

注意：

```text
DeepSeek 网页聊天账号
≠
DeepSeek API Key
```

即使你可以在 DeepSeek 网页或 App 中正常聊天，也不代表已经拥有可供 Codex 使用的 API Key。

DeepSeek API 平台：

- https://platform.deepseek.com/

DeepSeek API 文档：

- https://api-docs.deepseek.com/zh-cn/

创建出的 API Key 通常类似：

```text
sk-xxxxxxxxxxxxxxxxxxxx
```

### 安全提醒

不要公开完整 API Key。

排障时可以写成：

```text
sk-abcd***1234
```

不要：

- 上传到 GitHub
- 发到公开群聊
- 直接贴进论坛
- 完整截图
- 发给陌生人

---

## 2.4 CC Switch

CC Switch 官网：

- https://ccswitch.io/

CC Switch 官方 GitHub：

- https://github.com/farion1231/cc-switch

CC Switch 官方 Releases：

- https://github.com/farion1231/cc-switch/releases

建议使用最新版。

### Windows

优先下载 MSI 安装版。

也可以使用 Portable 便携版。

### macOS

可以使用 DMG，也可以通过 Homebrew：

```bash
brew install --cask cc-switch
```

### Linux

根据发行版选择：

- Debian / Ubuntu：`.deb`
- Fedora / RHEL：`.rpm`
- 其他发行版：AppImage

---

# 3. 核心概念：登录认证与模型 Provider 是两回事

整个配置中有两个不同的东西。

## 3.1 Codex 登录认证

通常保存在：

```text
~/.codex/auth.json
```

Windows 通常是：

```text
C:\Users\你的用户名\.codex\auth.json
```

它决定 Codex 当前使用：

```text
ChatGPT OAuth
```

还是：

```text
API Key
```

---

## 3.2 Codex 当前 Provider / 模型配置

主要与：

```text
~/.codex/config.toml
```

相关。

Windows：

```text
C:\Users\你的用户名\.codex\config.toml
```

它可能决定：

- 当前 Provider
- Base URL
- 模型目录
- 自定义模型
- 第三方配置

因此：

> 官方登录状态和当前模型 Provider 不是同一件事。

这是理解整个教程的关键。

---

# 4. 第一次配置前：先确保官方 Codex 正常

强烈建议先建立一个完全正常的官方状态，再接入 DeepSeek。

推荐顺序：

```text
先恢复官方 Codex
↓
确认 GPT 可用
↓
确认 ChatGPT 登录正常
↓
再配置 CC Switch
↓
最后添加 DeepSeek
```

不要第一次打开 Codex 就直接塞入第三方 API Key。

---

## 4.1 先切换为 OpenAI Official

打开 CC Switch：

```text
CC Switch
→ Codex
→ OpenAI Official
```

如果当前存在旧的第三方路由配置，建议先不要使用它。

---

## 4.2 启动 Codex

如果 Codex 提示登录：

选择：

```text
Sign in with ChatGPT
```

不要选择：

```text
API Key
```

推荐状态：

```text
ChatGPT OAuth
```

---

# 5. 检查 Codex 登录状态

如果系统能够直接识别 `codex` 命令，在 PowerShell 中运行：

```powershell
codex login status
```

理想结果：

```text
Logged in using ChatGPT
```

如果看到：

```text
Logged in using an API key - sk-xxxx
```

先不要继续添加 DeepSeek。

执行：

```powershell
codex logout
codex login
```

重新选择：

```text
Sign in with ChatGPT
```

然后再次运行：

```powershell
codex login status
```

直到看到：

```text
Logged in using ChatGPT
```

---

# 6. Windows 找不到 `codex` 命令怎么办

如果出现：

```text
codex : 无法将“codex”项识别为 cmdlet、函数、脚本文件或可运行程序
```

并不一定意味着 Codex 没安装。

Codex 桌面端可能自带 CLI，只是没有加入 PATH。

打开：

```text
C:\Users\你的用户名\.codex\config.toml
```

寻找类似：

```toml
CODEX_CLI_PATH = '...codex.exe'
```

例如：

```text
C:\Users\User\AppData\Local\OpenAI\Codex\bin\xxxxxxxx\codex.exe
```

然后运行：

```powershell
& '完整的codex.exe路径' login status
```

例如：

```powershell
& 'C:\Users\User\AppData\Local\OpenAI\Codex\bin\xxxxxxxx\codex.exe' login status
```

---

# 7. 备份 Codex 配置

在开始第三方配置前，建议备份：

Windows：

```text
C:\Users\你的用户名\.codex
```

macOS / Linux：

```text
~/.codex
```

至少备份：

```text
config.toml
```

例如复制为：

```text
config.toml.backup
```

不要公开分享：

- `auth.json`
- 完整 API Key
- OAuth Token

---

# 8. 开启 CC Switch 的官方登录保护

打开：

```text
CC Switch
→ 设置
→ 通用
→ Codex 应用增强
```

找到：

```text
切换第三方时保留官方登录
```

将其开启。

建议长期保持开启。

目标是：

```text
auth.json
→ 继续保存官方 ChatGPT 登录

config.toml
→ CC Switch 管理第三方 Provider
```

这样 DeepSeek API Key 就不会取代 Codex 官方 OAuth 身份。

---

# 9. 添加 DeepSeek

进入：

```text
CC Switch
→ Codex
→ 添加供应商
```

优先选择最新版内置：

```text
DeepSeek
```

预设。

不建议第一次配置时手工填写：

- `base_url`
- `wire_api`
- `model_provider`
- `model_catalog_json`
- 旧版 Chat Completions 参数
- 旧版本地路由参数

---

# 10. 填写 DeepSeek API Key

在 DeepSeek Provider 中填写：

```text
DeepSeek API Key
```

例如：

```text
sk-xxxxxxxxxxxxxxxx
```

然后保存。

注意：

```text
DeepSeek API Key
≠
OpenAI API Key

DeepSeek API Key
≠
ChatGPT 登录凭据
```

---

# 11. 检查是否为原生 Responses 配置

新版 DeepSeek 已经可以通过原生 Responses API 与 Codex 对接。

推荐目标：

```text
DeepSeek
+
Responses / Native Responses
+
https://api.deepseek.com
```

DeepSeek 官方 Codex 接入文档：

- https://api-docs.deepseek.com/quick_start/agent_integrations/codex/

DeepSeek Responses API 文档：

- https://api-docs.deepseek.com/zh-cn/guides/responses_api/

---

## 如果仍显示 `Needs Routing`

本文不推荐继续使用旧路由方案。

应该：

1. 更新 CC Switch 到最新版；
2. 完全退出 Codex；
3. 删除旧 DeepSeek Provider；
4. 使用最新版 DeepSeek 预设重新添加；
5. 重新填写 DeepSeek API Key；
6. 确认 Provider 使用 Responses / Native Responses；
7. 不使用旧版协议转换路由。

---

# 12. 启用 DeepSeek

在 CC Switch：

```text
Codex
→ DeepSeek
→ 启用 / 切换
```

确认当前 Provider 已经变成：

```text
DeepSeek
```

---

# 13. 为什么需要重启 Codex

切换 Provider 后，建议：

```text
完全退出 Codex
↓
重新打开
```

因为 Provider 切换可能改变：

- 模型目录
- `model_provider`
- Base URL
- Provider metadata

不要假定 Codex 一定能够完整热刷新这些内容。

推荐统一使用：

```text
切 Provider
→ 重启 Codex
```

---

# 14. 成功后的正常表现

成功后可能出现：

```text
Codex 账号：
仍然是你的 ChatGPT 官方账号

模型：
DeepSeek
```

这是正确状态。

再次运行：

```powershell
codex login status
```

仍然应该看到：

```text
Logged in using ChatGPT
```

而不是：

```text
Logged in using an API key
```

---

# 15. 如何确认请求真的走 DeepSeek

可以通过以下几种方法判断。

## 方法一：看 Codex 模型名称

当前模型显示为：

```text
DeepSeek ...
```

## 方法二：看 CC Switch

当前 Provider 显示：

```text
DeepSeek
```

## 方法三：看 CC Switch 请求记录

如果启用了请求统计，可以查看最近请求对应的 Provider。

## 方法四：看 DeepSeek API 后台

运行 Codex 一段时间后查看：

```text
API Usage
```

如果产生调用记录或余额变化，说明请求确实进入 DeepSeek。

---

# 16. 为什么 DeepSeek 模式下只有 DeepSeek 模型

这是正常现象。

当：

```text
Provider = DeepSeek
```

Codex 当前模型目录可能只显示 DeepSeek 模型。

这不代表：

```text
ChatGPT 官方登录消失
GPT 权限丢失
账号被注销
```

只是当前 Provider 已切换为 DeepSeek。

切回：

```text
OpenAI Official
```

并重启 Codex 后，官方 GPT 模型通常会重新出现。

---

# 17. 如何从 DeepSeek 切回 GPT

推荐流程：

```text
1. 完全退出 Codex

2. 打开 CC Switch

3. Codex
   → OpenAI Official

4. 确认不再使用旧版协议转换路由

5. 重新启动 Codex
```

一般不需要重新登录 ChatGPT。

---

# 18. 如何重新切回 DeepSeek

推荐流程：

```text
1. 完全退出 Codex

2. 打开 CC Switch

3. Codex
   → DeepSeek
   → 启用

4. 确认使用原生 Responses 配置

5. 重新启动 Codex
```

不需要重新输入 ChatGPT 账号。

---

# 19. 是否可以热切换

可以快速切换 CC Switch Provider。

但是不建议依赖 Codex 对以下内容进行完整热刷新：

- Provider
- Base URL
- 模型目录
- 模型 metadata

因此最稳妥的方法仍然是：

```text
切换 CC Switch Provider
+
重启 Codex
```

不需要重新登录账号。

---

# 20. 旧配置需要路由时怎么办

如果你的旧 DeepSeek Provider 显示：

```text
Needs Routing
```

或者依赖：

```text
127.0.0.1:15721
```

等本地协议转换地址，本文建议直接迁移，而不是继续维护旧路由。

迁移步骤：

```text
1. 完全退出 Codex

2. 更新 CC Switch 到最新版

3. 备份 ~/.codex/config.toml

4. 删除 CC Switch 中旧 DeepSeek Provider

5. 使用最新版 DeepSeek 预设重新创建

6. 重新填写 API Key

7. 确认 Base URL 为：
   https://api.deepseek.com

8. 确认使用：
   Responses / Native Responses

9. 不启用旧 Codex Local Routing

10. 启用 DeepSeek

11. 重启 Codex
```

---

# 21. 401 Unauthorized 的原因与修复

典型报错：

```text
401 Unauthorized

Incorrect API key provided: sk-xxxx

https://api.openai.com/v1/responses

invalid_api_key
```

如果你刚从 DeepSeek 切回 OpenAI 后遇到这种错误，通常意味着：

```text
请求地址：
OpenAI

认证 Key：
仍然是 DeepSeek API Key
```

实际过程：

```text
DeepSeek API Key
↓
被发送给 api.openai.com
↓
OpenAI 无法识别
↓
401 invalid_api_key
```

---

## 21.1 第一件事：检查登录状态

运行：

```powershell
codex login status
```

如果看到：

```text
Logged in using an API key - sk-xxxx
```

说明 Codex 当前认证已经被 API Key 登录取代。

---

## 21.2 修复方法

先在 CC Switch：

```text
Codex
→ OpenAI Official
```

然后运行：

```powershell
codex logout
codex login
```

选择：

```text
Sign in with ChatGPT
```

最后检查：

```powershell
codex login status
```

必须恢复为：

```text
Logged in using ChatGPT
```

然后重启 Codex。

---

# 22. `config.toml` 排查指南

Windows：

```text
C:\Users\你的用户名\.codex\config.toml
```

macOS / Linux：

```text
~/.codex/config.toml
```

恢复官方状态时，重点检查：

```toml
model_provider = "custom"
```

以及：

```toml
[model_providers.custom]
```

同时观察：

```toml
base_url = "..."
```

```toml
experimental_bearer_token = "sk-..."
```

```toml
model_catalog_json = "..."
```

不要看到：

```toml
[model_providers.custom]
```

就立刻删除。

真正应该判断的是：

```text
这个 Provider 是谁？
Base URL 指向哪里？
有没有第三方 API Key？
有没有第三方模型目录？
login status 当前是什么？
```

---

# 23. Windows 环境变量排查

如果已经恢复 ChatGPT 登录，但请求仍然拿旧 API Key，可以检查环境变量。

PowerShell：

```powershell
Get-ChildItem Env: | Where-Object {
    $_.Name -match 'OPENAI|CODEX|API_KEY|BASE_URL'
}
```

重点观察：

```text
OPENAI_API_KEY
CODEX_API_KEY
OPENAI_BASE_URL
```

如果其中仍然存在旧的第三方 Key，需要进一步清理。

---

## 23.1 检查 `.env`

可以检查：

```text
C:\Users\你的用户名\.codex\.env
```

是否存在。

PowerShell：

```powershell
Test-Path "$HOME\.codex\.env"
```

如果返回：

```text
True
```

可以查看变量名，但不要公开完整 API Key。

---

# 24. 不同用户应该从哪里开始

## A. Codex 和 DeepSeek 都没用过

```text
安装 Codex
↓
使用 ChatGPT 官方登录
↓
确认 GPT 正常
↓
安装 CC Switch
↓
开启“切换第三方时保留官方登录”
↓
创建 DeepSeek API Key
↓
添加 DeepSeek
↓
启用 DeepSeek
↓
重启 Codex
```

---

## B. 已经使用 Codex

先检查：

```powershell
codex login status
```

确认：

```text
Logged in using ChatGPT
```

然后：

```text
安装 CC Switch
↓
开启保留官方登录
↓
添加 DeepSeek
```

---

## C. 只使用过 DeepSeek 网页或 App

你还需要：

```text
DeepSeek API Key
```

然后：

```text
安装 Codex
↓
使用 ChatGPT 官方登录
↓
安装 CC Switch
↓
添加 DeepSeek API Key
```

---

## D. 已经有 DeepSeek API Key

直接：

```text
Codex 官方登录
↓
CC Switch
↓
开启保留官方登录
↓
添加 DeepSeek Provider
```

---

## E. Codex 和 DeepSeek 都使用过，但没有接起来

确认：

```text
Codex login status = Logged in using ChatGPT
```

然后直接从：

```text
CC Switch 添加 DeepSeek
```

开始。

---

## F. 已经配置坏了

如果遇到：

```text
GPT 消失
401 Unauthorized
invalid_api_key
```

先恢复官方状态：

```text
CC Switch
→ OpenAI Official
↓
codex logout
↓
codex login
↓
Sign in with ChatGPT
↓
确认 Logged in using ChatGPT
↓
确认 GPT 恢复
```

然后再重新按照本文流程添加 DeepSeek。

---

# 25. 推荐的日常使用方式

## 使用 GPT

```text
CC Switch Provider：
OpenAI Official

Codex 登录：
ChatGPT OAuth

然后：
重启 Codex
```

---

## 使用 DeepSeek

```text
CC Switch Provider：
DeepSeek

“切换第三方时保留官方登录”：
开启

DeepSeek：
原生 Responses 配置

然后：
重启 Codex
```

---

# 26. 最终自检清单

## 软件

- [ ] Codex 已更新至最新版
- [ ] CC Switch 已更新至最新版
- [ ] CC Switch 来自官方渠道

## 官方登录

- [ ] Codex 已通过 ChatGPT 登录
- [ ] `codex login status` 显示 `Logged in using ChatGPT`

## CC Switch

- [ ] 已开启“切换第三方时保留官方登录”

## DeepSeek

- [ ] 已创建 DeepSeek API Key
- [ ] API Key 有效
- [ ] API 账户有可用额度
- [ ] 使用最新版 DeepSeek 预设
- [ ] Base URL 使用官方地址
- [ ] 使用 Responses / Native Responses

## 旧路由

- [ ] 不再使用旧 `Needs Routing` 配置
- [ ] 不依赖旧版 Codex 本地协议转换路由

## 最终运行状态

- [ ] CC Switch 当前 Provider 为 DeepSeek
- [ ] 重启 Codex 后出现 DeepSeek 模型
- [ ] 请求可以正常执行
- [ ] DeepSeek API 后台产生调用
- [ ] `codex login status` 仍然显示 ChatGPT

---

# 27. 常见问题 FAQ

## Q1：切换到 DeepSeek 后，GPT 模型全没了，正常吗？

正常。

当前 Provider 是 DeepSeek 时，Codex 的模型目录可能只展示 DeepSeek。

切回 OpenAI Official 并重启 Codex 后，GPT 模型通常会重新出现。

---

## Q2：为什么 Codex 仍然显示我的 OpenAI / ChatGPT 账号？

因为登录认证和模型 Provider 是两套状态。

你可以：

```text
保持 ChatGPT 官方登录
+
使用 DeepSeek 模型
```

这是本文推荐的状态。

---

## Q3：我可以在同一个模型菜单里同时看到 GPT 和 DeepSeek 吗？

通常不应依赖这种方式。

更可靠的方法是：

```text
CC Switch 切换 Provider
↓
重启 Codex
```

---

## Q4：切换 DeepSeek 后，`codex login status` 应该显示什么？

理想状态仍然是：

```text
Logged in using ChatGPT
```

如果变成：

```text
Logged in using an API key
```

说明第三方 API Key 已经替代官方登录，需要修复。

---

## Q5：看到 `Needs Routing` 怎么办？

不要继续开启旧路由。

建议：

```text
升级 CC Switch
↓
删除旧 DeepSeek Provider
↓
使用最新版 DeepSeek 预设重新创建
↓
使用原生 Responses
```

---

## Q6：为什么以前的教程都要求开路由？

因为以前 DeepSeek 与 Codex 的接口格式存在差异，需要 CC Switch 进行协议转换。

现在 DeepSeek 已经提供原生 Responses API，因此新配置不应再依赖旧协议转换方案。

---

## Q7：出现 `401 invalid_api_key` 怎么办？

第一步：

```powershell
codex login status
```

如果显示 API Key 登录：

```powershell
codex logout
codex login
```

然后选择：

```text
Sign in with ChatGPT
```

---

## Q8：可以直接使用 DeepSeek 官方配置脚本吗？

可以。

如果你只打算长期使用 DeepSeek，DeepSeek 官方 Codex 接入方式也可以使用。

但如果你经常：

```text
DeepSeek
↕
GPT
```

并希望保留官方 ChatGPT 登录，使用 CC Switch 管理 Provider 会更方便。

不建议同时让 DeepSeek 官方脚本和 CC Switch 反复修改同一套 `~/.codex/config.toml`。

---

# 28. 官方下载与文档链接

## OpenAI / Codex

ChatGPT / Codex 官方下载：

https://chatgpt.com/download/

OpenAI Codex：

https://openai.com/codex/

---

## CC Switch

官网：

https://ccswitch.io/

GitHub：

https://github.com/farion1231/cc-switch

Releases：

https://github.com/farion1231/cc-switch/releases

---

## DeepSeek

DeepSeek API Platform：

https://platform.deepseek.com/

DeepSeek API 中文文档：

https://api-docs.deepseek.com/zh-cn/

DeepSeek 官方 Codex 接入指南：

https://api-docs.deepseek.com/quick_start/agent_integrations/codex/

DeepSeek Responses API：

https://api-docs.deepseek.com/zh-cn/guides/responses_api/

---

# 29. 七条必须记住的原则

```text
1. Codex 官方登录 ≠ 当前模型 Provider

2. DeepSeek API Key 不应该覆盖 ChatGPT OAuth

3. 使用最新版 CC Switch 和最新版 DeepSeek 预设

4. 旧配置显示 Needs Routing：
   升级并重建 Provider
   不要继续沿用旧路由

5. 新版 DeepSeek 使用原生 Responses API

6. 切换 Provider 后重启 Codex

7. 出现 401 invalid_api_key：
   第一件事检查 codex login status
```

只要遵守这七条，DeepSeek 接入 Codex 的配置会稳定很多，也能避免绝大多数因为旧教程、旧 Provider、认证覆盖或本地路由残留造成的问题。
