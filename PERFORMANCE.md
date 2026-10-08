# Part 2：性能优化记录

## 实现

- `src/coursesat.cpp`：命令行求解器默认开启 BVA（bounded variable addition），仅对二元子句进行因式分解（`factor=1, factorsize=2`）。重复的二元约束可以通过扩展变量压缩，减少后续传播和搜索。使用原有证明生成和模型恢复代码，不自行删除原因子句。
- 只修改命令行程序的初始化默认值，不改变库接口的默认配置：增量 API 客户端可能在 solve 之后添加未声明的变量，而 BVA 的扩展变量需要预先保留用户变量空间。DIMACS 输入先声明变量总数，适合开启该策略。命令行仍可用 `--factor=0` 覆盖。
- `src/heap.hpp`：上浮/下沉改为移动空位，减少每一层交换和位置表访问，保留原来的比较条件和相等时左子优先的行为。通用 update 仍支持分数增加或减少。
- 保留 Part 1 的回溯重传播修复。

## 实验

同一台机器、同样的 release 构建参数，命令均为 `solver input.cnf proof.drat`，每例 15 秒。SAT 逐子句验证模型，UNSAT 用 `build/drat-trim` 验证证明，证明验证耗时不计入求解时间。超时计 30 秒 PAR-2。

完整单轮配对结果（秒）：

| 实例 | 修改前 | 修改后 |
|---|---:|---:|
| public-01 | 0.2236 | 0.2309 |
| public-02 | 0.6464 | 0.6399 |
| public-03 | 0.8102 | 0.8024 |
| public-04 | 0.2662 | 0.0114 |
| public-05 | 2.7079 | 0.0191 |
| public-06 | 0.0023 | 0.0022 |
| public-07 | 5.1741 | 5.1506 |
| public-08 | 超时 | 超时 |
| PAR-2 | 39.8308 | 36.8565 |

PAR-2 降低约 7.5%；共同解出的七例总时间约从 9.83 秒降至 6.86 秒，降低约 30%。前七例另有三轮交替运行，收益主要仍集中于 public-04/05；三轮完整流程由于外层命令的 240 秒限制被中止，不能作为完整三轮成绩。堆改动未显示出可靠的独立端到端提速，不把计时噪声当作收益。

尝试并放弃：只用稳定模式、只用聚焦模式、禁用 chrono、延长重启间隔、调整评分衰减、加大学得子句清理间隔、对长子句也开启 BVA。多种配置使 public-07 从可解变成超时。最终采用较保守的二元 BVA。

公开集只有八例，结果不能保证 private 集提速；尤其 public-08 仍未解决。

## 正确性和复现

- `make part1_test`：282 ok、0 failed；额外 CDCL 回归 4/4。
- `python3 scripts/test-performance.py`：7/8，所有解出实例的模型/证明通过，最后一例超时。
- 堆随机回归：200000 次插入、分数增加/减少、取最大值、清空，包含相等分数，使用 ASan/UBSan 通过。

```sh
# 修改前先保存基线二进制（不要在修改后才保存）
cp build/coursesat /tmp/coursesat-baseline
# 修改后
make -j4
make part1_test
python3 scripts/compare-performance.py /tmp/coursesat-baseline build/coursesat --repeats 3
# 比较包含独立证明验证，整个流程可能需要数分钟。
g++ -std=c++11 -O1 -g -fsanitize=address,undefined -Isrc test/heap-regression.cpp -o /tmp/heap-test
/tmp/heap-test
```

最终策略在 `src/` 中，正常固定命令运行即生效，不依赖测试脚本或额外参数。
