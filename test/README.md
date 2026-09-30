# 公开测试

在实验包根目录完成配置和编译后运行：

- `make part1_test`（或 `make test`）：运行 `cnf/` 中的正确性测试，检查赋值与证明。
- `make part2_test`：运行 `performance/` 中的公开性能测试。
- `make public_test`：依次运行两部分；正确性测试失败时停止。

正确性测试日志位于 `build/`。初始版本包含教学用的正确性缺陷，出现测试失败需要定位源码中的问题。网站另外运行 private tests，本地通过不代表全部通过。
