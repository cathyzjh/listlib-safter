# 高级数据类型（列表 / 字典）· Mind+ 用户库

给 **Mind+ V1.8 上传模式 + Arduino C + 行空板 K10** 补上一组「对标 Python 高级数据类型」的积木。

> v1.1.0：新增 3 个**批量初始化**积木，一次填一串值，不用逐条追加。

官方 K10 上传扩展只有固定长度数组，没有 `append / pop / 排序 / 切片 / 字典`。这个库用「名字 → 容器」的运行时注册表，把 Python 的 `list`、`dict` 语义在 C 里还原：

- 列表名可以随时新建，**支持中文名**（`数据`、`温度历史` 都行）
- 列表长度自动增长，不用提前写死
- 索引从 0 开始，**支持负索引**（`-1` 是最后一项），和 Python 一致
- 越界读取返回 0 / 空文本，不会让程序跑飞

## 一、安装

### 方式 A：直接导入成品库（推荐）

1. 打开 Mind+，进入**上传模式**，选择主控 **行空板 K10**
2. 点左下角「扩展」→ 顶部「用户库」→「导入用户库」
3. 选择 `doubao-listlib-thirdex-V1.0.0.mpext`
4. 回到扩展页，能看到「高级数据类型（列表/字典）」卡片，积木区多出 `listlib` 分类

### 方式 B：导入源码目录调试

把 `config.json` 直接拖入「导入用户库」对话框（选择 config.json 文件），会进入**调试模式**，改代码后点积木区右上角的刷新按钮即可生效。

## 二、积木清单

### 数字列表（对标 `list` 存数字）

| 积木 | 生成代码 | 等价 Python |
| --- | --- | --- |
| 创建数字列表 `lst` | `ListLib::createNum("lst");` | `lst = []` |
| 把数字列表 `lst` 初始化为 `1,2,3,4`，分隔符 `,` | `ListLib::initNum("lst", "1,2,3,4", ",");` | `lst = [1,2,3,4]`（一次赋值，不用逐条 append） |
| 向数字列表 `lst` 追加 `25.5` | `ListLib::addNum("lst", 25.5);` | `lst.append(25.5)` |
| 在数字列表 `lst` 第 `0` 项前插入 `9` | `ListLib::insertNum("lst", 0, 9);` | `lst.insert(0, 9)` |
| 把数字列表 `lst` 第 `1` 项设为 `7` | `ListLib::setNum("lst", 1, 7);` | `lst[1] = 7` |
| 数字列表 `lst` 第 `0` 项 | `ListLib::getNum("lst", 0)` | `lst[0]` |
| 删除数字列表 `lst` 中第一个 `7` | `ListLib::removeValueNum("lst", 7);` | `lst.remove(7)` |
| 数字列表 `lst` 包含 `7`？ | `ListLib::containsNum("lst", 7)` | `7 in lst` |
| 数字列表 `lst` 中 `7` 的位置 | `ListLib::indexOfNum("lst", 7)` | `lst.index(7)` |
| 数字列表 `lst` 的 总和/最大值/最小值/平均值 | `ListLib::sumNum("lst")` 等 | `sum(lst)` / `max(lst)` / `min(lst)` |
| 对数字列表 `lst` 升序排序 | `ListLib::sortNum("lst");` | `lst.sort()` |
| 用分隔符 `,` 拼接数字列表 `lst` | `ListLib::dumpNum("lst", ",")` | `",".join(map(str,lst))` |
| 把数字列表 `lst` 的第 `0` 到 `3` 项复制到 `lst2` | `ListLib::sliceNum(...)` | `lst2 = lst[0:3]` |

### 文本列表（对标 `list` 存字符串）

| 积木 | 等价 Python |
| --- | --- |
| 创建文本列表 `words` | `words = []` |
| 把文本列表 `words` 初始化为 `红,绿,蓝`，分隔符 `,` | `words = ["红","绿","蓝"]` |
| 向文本列表 `words` 追加 `"hello"` | `words.append("hello")` |
| 在文本列表 `words` 第 `0` 项前插入 | `words.insert(0, x)` |
| 把文本列表 `words` 第 `1` 项设为 | `words[1] = x` |
| 文本列表 `words` 第 `0` 项 | `words[0]` |
| 文本列表 `words` 包含 `"hello"`？ | `"hello" in words` |
| 文本列表 `words` 中 `"hello"` 的位置 | `words.index("hello")` |
| 用分隔符 `,` 拼接文本列表 `words` | `",".join(words)` |
| 把文本列表 `words` 的第 `0` 到 `3` 项复制到 `words2` | `words2 = words[0:3]` |

### 列表通用

| 积木 | 等价 Python |
| --- | --- |
| 列表 `lst` 的长度 | `len(lst)` |
| 删除列表 `lst` 第 `0` 项 | `del lst[0]` / `lst.pop(0)` |
| 删除列表 `lst` 的最后一项 | `lst.pop()` |
| 清空列表 `lst` | `lst.clear()` |
| 把列表 `lst` 完整复制到 `lst2` | `lst2 = lst.copy()` |

### 字典（对标 `dict`）

值统一按数字存储，键是文本。

| 积木 | 等价 Python |
| --- | --- |
| 创建字典 `d` | `d = {}` |
| 把字典 `d` 初始化为 `temp:25,hum:60` | `d = {"temp":25, "hum":60}` |
| 字典 `d` 设置键 `"temp"` 为 `25` | `d["temp"] = 25` |
| 字典 `d` 键 `"temp"` 的值 | `d["temp"]` |
| 字典 `d` 包含键 `"temp"`？ | `"temp" in d` |
| 删除字典 `d` 的键 `"temp"` | `del d["temp"]` |
| 字典 `d` 的键数量 | `len(d)` |
| 字典 `d` 的所有键，用 `,` 连接 | `",".join(d.keys())` |
| 清空字典 `d` | `d.clear()` |

## 三、批量初始化怎么用

三个积木都支持「一次填一串」，创建和赋值一步完成（等价 `lst = [1,2,3]`）：

| 积木 | 输入示例 | 结果 |
| --- | --- | --- |
| 把数字列表 `lst` 初始化为 `...` | `1,2,3,4` 或 `25.5, 24.1 ,23.8` | 4 个元素，空格和多余的逗号会自动忽略 |
| 把数字列表 `lst` 初始化为 `...`，分隔符 ` ` | `10 20 30` | 空格分隔也能拆，适合粘贴传感器数据 |
| 把文本列表 `words` 初始化为 `...` | `红,绿,蓝` | 3 个文本元素 |
| 把文本列表 `words` 初始化为 `...`，分隔符 `|` | `a|b|c` | 文本本身带逗号时换个分隔符 |
| 把字典 `d` 初始化为 `...` | `temp:25,hum:60` | 2 个键，值分别是 25 和 60 |

规则：
- 每个元素前后的空格会被自动去掉
- 结尾多打的逗号不会生成空元素，空项会被跳过
- 字典里格式不对的项（没有冒号）会被跳过；`noVal:` 这种会存成 0
- 初始化是**覆盖**：原来列表里的内容会被清空

## 四、K10 示例：温湿度历史（自动淘汰旧数据）

效果：每次读温湿度存进列表，列表最多保留 10 条，超了就删最早的一条 —— 这就是 Python 里
`history.append(t); if len(history) > 10: history.pop(0)` 的等价写法。

积木顺序：

1. `创建数字列表` 名字填 `温度历史`
2. `创建数字列表` 名字填 `湿度历史`
3. 放进「循环执行」：
   - `向数字列表 [温度历史] 追加 [读取 温度（摄氏度℃）]`
   - `向数字列表 [湿度历史] 追加 [读取 湿度（%RH）]`
   - `如果 [列表 温度历史 的长度] > 10 那么` → `删除列表 [温度历史] 第 [0] 项`
   - 同样的判断给 `湿度历史`
   - `用分隔符 [,] 拼接数字列表 [温度历史]` → 用「缓存显示文字」显示到屏幕第 1 行
   - `等待 1 秒`

生成的 C 代码大致是：

```cpp
void loop() {
    ListLib::addNum("温度历史", temp_humi.read_temp());
    ListLib::addNum("湿度历史", temp_humi.read_hum());
    if (ListLib::size("温度历史") > 10) { ListLib::removeAt("温度历史", 0); }
    if (ListLib::size("湿度历史") > 10) { ListLib::removeAt("湿度历史", 0); }
    display.drawText(ListLib::dumpNum("温度历史", ","), 0, 1, ...);
    delay(1000);
}
```

## 五、字典示例：把传感器数据打包

```
创建字典 d
字典 d 设置键 "temp" 为 [读取 温度（摄氏度℃）]
字典 d 设置键 "hum"  为 [读取 湿度（%RH）]
如果 [字典 d 包含键 "temp"？] 那么
    缓存显示文字（用分隔符 " | " 拼接 ...）→ 显示
```

## 六、常见问题

**1. 导入后库显示「不可用」**
`config.json` 里的 `board` 写的是 `esp32s3bit`（K10 在 V1.x 用户库体系里的主控标识）。
若显示不可用，用记事本打开 `config.json`，把 `"board": ["esp32s3bit"]` 临时改成
`"board": ["esp32s3bit", "esp32", "mpython"]` 再重新导入；仍不行就把该行换成你在
Mind+ 安装目录下 K10 主控库 `config.json` 里看到的 `board` 值。

**2. 报 `'ListLib' has not been declared`**
说明 `arduinoC/libraries/listlib/listlib.h` 没被打包进去。确认该文件存在，且目录层级是
`arduinoC/libraries/listlib/listlib.h`（必须多一层子目录，不能直接放 libraries 下）。

**3. 取到 0 或空文本**
索引越界，或列表名拼错（名字必须和「创建」积木里写的完全一致，中文也算）。
`-1` 表示最后一项，`-2` 倒数第二项，和 Python 一样。

**4. 能不能存文本进数字列表？**
不能混用。数字列表只收数字，文本列表只收文本，这是 C 和 Python 最大的区别 ——
Python 的 `list` 可以混装，C 必须定类型。要混装就拆成两个列表。

**5. 内存占用**
K10 是 ESP32-S3（512KB SRAM + 8MB PSRAM），几百个元素的列表毫无压力；
但别在循环里无限 `追加` 不清理，长期跑项目建议像示例那样固定上限。

## 七、许可

MIT，随便改、随便发给同学。改完记得把 `config.json` 里的 `author`、`id` 换成你自己的，
否则会和其他人的库冲突。

