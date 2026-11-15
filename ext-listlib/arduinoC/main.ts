/*
 * main.ts - 高级数据类型（列表 / 字典）用户库
 * 适用: Mind+ V1.8 上传模式 - Arduino C - 行空板 K10
 *
 * 说明:
 *   Python 的 list / dict 是运行时对象，本项目用「名字 -> 容器」的注册表
 *   在 C 里还原同样的语义，因此列表名可以用中文、随时新建、随时清空。
 */

enum STATTYPE {
    //% block="总和"
    sum,
    //% block="最大值"
    max,
    //% block="最小值"
    min,
    //% block="平均值"
    avg
}

// 取出参数里的原始文本（去掉 string 输入框自带的首尾引号）
function rawText(code: any): string {
    let s = String(code === undefined || code === null ? "" : code).trim();
    if (s.length >= 2) {
        let a = s.charAt(0);
        let b = s.charAt(s.length - 1);
        if ((a === '"' && b === '"') || (a === "'" && b === "'")) {
            s = s.substring(1, s.length - 1);
        }
    }
    return s;
}

// 安全取参数 code，防止参数为空时报错
function arg(parameter: any, key: string): string {
    if (!parameter) return "";
    let p = parameter[key];
    if (!p || p.code === undefined) return "";
    return String(p.code);
}

// 所有积木都带上这个头文件，重复调用按 id 去重
function useLib() {
    Generator.addInclude('listlib', '#include <listlib.h>');
}

//% color="#2962FF" iconWidth=50 iconHeight=40
namespace listlib {

    // ==================== 数字列表 ====================

    //% block="创建数字列表 [NAME]" blockType="command"
    //% NAME.shadow="string" NAME.defl="lst"
    export function createNumList(parameter: any, block: any) {
        useLib();
        Generator.addCode(`ListLib::createNum("${rawText(arg(parameter, "NAME"))}");`);
    }

    //% block="把数字列表 [NAME] 初始化为 [DATA]，分隔符 [SEP]" blockType="command"
    //% NAME.shadow="string" NAME.defl="lst"
    //% DATA.shadow="string" DATA.defl="1,2,3,4"
    //% SEP.shadow="string" SEP.defl=","
    export function initNumList(parameter: any, block: any) {
        useLib();
        Generator.addCode(`ListLib::initNum("${rawText(arg(parameter, "NAME"))}", ${arg(parameter, "DATA")}, ${arg(parameter, "SEP")});`);
    }

    //% block="向数字列表 [NAME] 追加 [VAL]" blockType="command"
    //% NAME.shadow="string" NAME.defl="lst"
    //% VAL.shadow="number" VAL.defl=0
    export function addNum(parameter: any, block: any) {
        useLib();
        Generator.addCode(`ListLib::addNum("${rawText(arg(parameter, "NAME"))}", ${arg(parameter, "VAL")});`);
    }

    //% block="在数字列表 [NAME] 第 [IDX] 项前插入 [VAL]" blockType="command"
    //% NAME.shadow="string" NAME.defl="lst"
    //% IDX.shadow="number" IDX.defl=0
    //% VAL.shadow="number" VAL.defl=0
    export function insertNum(parameter: any, block: any) {
        useLib();
        Generator.addCode(`ListLib::insertNum("${rawText(arg(parameter, "NAME"))}", ${arg(parameter, "IDX")}, ${arg(parameter, "VAL")});`);
    }

    //% block="把数字列表 [NAME] 第 [IDX] 项设为 [VAL]" blockType="command"
    //% NAME.shadow="string" NAME.defl="lst"
    //% IDX.shadow="number" IDX.defl=0
    //% VAL.shadow="number" VAL.defl=0
    export function setNum(parameter: any, block: any) {
        useLib();
        Generator.addCode(`ListLib::setNum("${rawText(arg(parameter, "NAME"))}", ${arg(parameter, "IDX")}, ${arg(parameter, "VAL")});`);
    }

    //% block="数字列表 [NAME] 第 [IDX] 项" blockType="reporter"
    //% NAME.shadow="string" NAME.defl="lst"
    //% IDX.shadow="number" IDX.defl=0
    export function getNum(parameter: any, block: any) {
        useLib();
        return `ListLib::getNum("${rawText(arg(parameter, "NAME"))}", ${arg(parameter, "IDX")})`;
    }

    //% block="删除数字列表 [NAME] 中第一个 [VAL]" blockType="command"
    //% NAME.shadow="string" NAME.defl="lst"
    //% VAL.shadow="number" VAL.defl=0
    export function removeValueNum(parameter: any, block: any) {
        useLib();
        Generator.addCode(`ListLib::removeValueNum("${rawText(arg(parameter, "NAME"))}", ${arg(parameter, "VAL")});`);
    }

    //% block="数字列表 [NAME] 包含 [VAL]？" blockType="boolean"
    //% NAME.shadow="string" NAME.defl="lst"
    //% VAL.shadow="number" VAL.defl=0
    export function containsNum(parameter: any, block: any) {
        useLib();
        return `ListLib::containsNum("${rawText(arg(parameter, "NAME"))}", ${arg(parameter, "VAL")})`;
    }

    //% block="数字列表 [NAME] 中 [VAL] 的位置（没有则 -1）" blockType="reporter"
    //% NAME.shadow="string" NAME.defl="lst"
    //% VAL.shadow="number" VAL.defl=0
    export function indexOfNum(parameter: any, block: any) {
        useLib();
        return `ListLib::indexOfNum("${rawText(arg(parameter, "NAME"))}", ${arg(parameter, "VAL")})`;
    }

    //% block="数字列表 [NAME] 的 [ST]" blockType="reporter"
    //% NAME.shadow="string" NAME.defl="lst"
    //% ST.shadow="dropdown" ST.options="STATTYPE"
    export function statNum(parameter: any, block: any) {
        useLib();
        let st = arg(parameter, "ST");
        let name = rawText(arg(parameter, "NAME"));
        if (st.indexOf("max") >= 0) return `ListLib::maxNum("${name}")`;
        if (st.indexOf("min") >= 0) return `ListLib::minNum("${name}")`;
        if (st.indexOf("avg") >= 0) return `ListLib::avgNum("${name}")`;
        return `ListLib::sumNum("${name}")`;
    }

    //% block="对数字列表 [NAME] 升序排序" blockType="command"
    //% NAME.shadow="string" NAME.defl="lst"
    export function sortNum(parameter: any, block: any) {
        useLib();
        Generator.addCode(`ListLib::sortNum("${rawText(arg(parameter, "NAME"))}");`);
    }

    //% block="用分隔符 [SEP] 拼接数字列表 [NAME]" blockType="reporter"
    //% NAME.shadow="string" NAME.defl="lst"
    //% SEP.shadow="string" SEP.defl=","
    export function dumpNum(parameter: any, block: any) {
        useLib();
        return `ListLib::dumpNum("${rawText(arg(parameter, "NAME"))}", ${arg(parameter, "SEP")})`;
    }

    //% block="把数字列表 [SRC] 的第 [A] 到 [B] 项复制到 [DST]" blockType="command"
    //% SRC.shadow="string" SRC.defl="lst"
    //% A.shadow="number" A.defl=0
    //% B.shadow="number" B.defl=-1
    //% DST.shadow="string" DST.defl="lst2"
    export function sliceNum(parameter: any, block: any) {
        useLib();
        Generator.addCode(`ListLib::sliceNum("${rawText(arg(parameter, "SRC"))}", ${arg(parameter, "A")}, ${arg(parameter, "B")}, "${rawText(arg(parameter, "DST"))}");`);
    }

    // ==================== 文本列表 ====================

    //% block="创建文本列表 [NAME]" blockType="command"
    //% NAME.shadow="string" NAME.defl="words"
    export function createStrList(parameter: any, block: any) {
        useLib();
        Generator.addCode(`ListLib::createStr("${rawText(arg(parameter, "NAME"))}");`);
    }

    //% block="把文本列表 [NAME] 初始化为 [DATA]，分隔符 [SEP]" blockType="command"
    //% NAME.shadow="string" NAME.defl="words"
    //% DATA.shadow="string" DATA.defl="红,绿,蓝"
    //% SEP.shadow="string" SEP.defl=","
    export function initStrList(parameter: any, block: any) {
        useLib();
        Generator.addCode(`ListLib::initStr("${rawText(arg(parameter, "NAME"))}", ${arg(parameter, "DATA")}, ${arg(parameter, "SEP")});`);
    }

    //% block="向文本列表 [NAME] 追加 [VAL]" blockType="command"
    //% NAME.shadow="string" NAME.defl="words"
    //% VAL.shadow="string" VAL.defl="hello"
    export function addStr(parameter: any, block: any) {
        useLib();
        Generator.addCode(`ListLib::addStr("${rawText(arg(parameter, "NAME"))}", ${arg(parameter, "VAL")});`);
    }

    //% block="在文本列表 [NAME] 第 [IDX] 项前插入 [VAL]" blockType="command"
    //% NAME.shadow="string" NAME.defl="words"
    //% IDX.shadow="number" IDX.defl=0
    //% VAL.shadow="string" VAL.defl="hello"
    export function insertStr(parameter: any, block: any) {
        useLib();
        Generator.addCode(`ListLib::insertStr("${rawText(arg(parameter, "NAME"))}", ${arg(parameter, "IDX")}, ${arg(parameter, "VAL")});`);
    }

    //% block="把文本列表 [NAME] 第 [IDX] 项设为 [VAL]" blockType="command"
    //% NAME.shadow="string" NAME.defl="words"
    //% IDX.shadow="number" IDX.defl=0
    //% VAL.shadow="string" VAL.defl="hello"
    export function setStr(parameter: any, block: any) {
        useLib();
        Generator.addCode(`ListLib::setStr("${rawText(arg(parameter, "NAME"))}", ${arg(parameter, "IDX")}, ${arg(parameter, "VAL")});`);
    }

    //% block="文本列表 [NAME] 第 [IDX] 项" blockType="reporter"
    //% NAME.shadow="string" NAME.defl="words"
    //% IDX.shadow="number" IDX.defl=0
    export function getStr(parameter: any, block: any) {
        useLib();
        return `ListLib::getStr("${rawText(arg(parameter, "NAME"))}", ${arg(parameter, "IDX")})`;
    }

    //% block="文本列表 [NAME] 包含 [VAL]？" blockType="boolean"
    //% NAME.shadow="string" NAME.defl="words"
    //% VAL.shadow="string" VAL.defl="hello"
    export function containsStr(parameter: any, block: any) {
        useLib();
        return `ListLib::containsStr("${rawText(arg(parameter, "NAME"))}", ${arg(parameter, "VAL")})`;
    }

    //% block="文本列表 [NAME] 中 [VAL] 的位置（没有则 -1）" blockType="reporter"
    //% NAME.shadow="string" NAME.defl="words"
    //% VAL.shadow="string" VAL.defl="hello"
    export function indexOfStr(parameter: any, block: any) {
        useLib();
        return `ListLib::indexOfStr("${rawText(arg(parameter, "NAME"))}", ${arg(parameter, "VAL")})`;
    }

    //% block="用分隔符 [SEP] 拼接文本列表 [NAME]" blockType="reporter"
    //% NAME.shadow="string" NAME.defl="words"
    //% SEP.shadow="string" SEP.defl=","
    export function joinStr(parameter: any, block: any) {
        useLib();
        return `ListLib::joinStr("${rawText(arg(parameter, "NAME"))}", ${arg(parameter, "SEP")})`;
    }

    //% block="把文本列表 [SRC] 的第 [A] 到 [B] 项复制到 [DST]" blockType="command"
    //% SRC.shadow="string" SRC.defl="words"
    //% A.shadow="number" A.defl=0
    //% B.shadow="number" B.defl=-1
    //% DST.shadow="string" DST.defl="words2"
    export function sliceStr(parameter: any, block: any) {
        useLib();
        Generator.addCode(`ListLib::sliceStr("${rawText(arg(parameter, "SRC"))}", ${arg(parameter, "A")}, ${arg(parameter, "B")}, "${rawText(arg(parameter, "DST"))}");`);
    }

    // ==================== 列表通用 ====================

    //% block="列表 [NAME] 的长度" blockType="reporter"
    //% NAME.shadow="string" NAME.defl="lst"
    export function listSize(parameter: any, block: any) {
        useLib();
        return `ListLib::size("${rawText(arg(parameter, "NAME"))}")`;
    }

    //% block="删除列表 [NAME] 第 [IDX] 项" blockType="command"
    //% NAME.shadow="string" NAME.defl="lst"
    //% IDX.shadow="number" IDX.defl=0
    export function removeAt(parameter: any, block: any) {
        useLib();
        Generator.addCode(`ListLib::removeAt("${rawText(arg(parameter, "NAME"))}", ${arg(parameter, "IDX")});`);
    }

    //% block="删除列表 [NAME] 的最后一项" blockType="command"
    //% NAME.shadow="string" NAME.defl="lst"
    export function popBack(parameter: any, block: any) {
        useLib();
        Generator.addCode(`ListLib::popBack("${rawText(arg(parameter, "NAME"))}");`);
    }

    //% block="清空列表 [NAME]" blockType="command"
    //% NAME.shadow="string" NAME.defl="lst"
    export function listClear(parameter: any, block: any) {
        useLib();
        Generator.addCode(`ListLib::clear("${rawText(arg(parameter, "NAME"))}");`);
    }

    //% block="把列表 [SRC] 完整复制到 [DST]" blockType="command"
    //% SRC.shadow="string" SRC.defl="lst"
    //% DST.shadow="string" DST.defl="lst2"
    export function copyList(parameter: any, block: any) {
        useLib();
        Generator.addCode(`ListLib::copy("${rawText(arg(parameter, "SRC"))}", "${rawText(arg(parameter, "DST"))}");`);
    }

    // ==================== 字典 dict ====================

    //% block="创建字典 [NAME]" blockType="command"
    //% NAME.shadow="string" NAME.defl="d"
    export function dictCreate(parameter: any, block: any) {
        useLib();
        Generator.addCode(`ListLib::dictCreate("${rawText(arg(parameter, "NAME"))}");`);
    }

    //% block="把字典 [NAME] 初始化为 [DATA]（例 temp:25,hum:60）" blockType="command"
    //% NAME.shadow="string" NAME.defl="d"
    //% DATA.shadow="string" DATA.defl="temp:25,hum:60"
    export function initDict(parameter: any, block: any) {
        useLib();
        Generator.addCode(`ListLib::initDict("${rawText(arg(parameter, "NAME"))}", ${arg(parameter, "DATA")}, ",", ":");`);
    }

    //% block="字典 [NAME] 设置键 [KEY] 为 [VAL]" blockType="command"
    //% NAME.shadow="string" NAME.defl="d"
    //% KEY.shadow="string" KEY.defl="temp"
    //% VAL.shadow="number" VAL.defl=0
    export function dictSet(parameter: any, block: any) {
        useLib();
        Generator.addCode(`ListLib::dictSet("${rawText(arg(parameter, "NAME"))}", ${arg(parameter, "KEY")}, ${arg(parameter, "VAL")});`);
    }

    //% block="字典 [NAME] 键 [KEY] 的值" blockType="reporter"
    //% NAME.shadow="string" NAME.defl="d"
    //% KEY.shadow="string" KEY.defl="temp"
    export function dictGet(parameter: any, block: any) {
        useLib();
        return `ListLib::dictGet("${rawText(arg(parameter, "NAME"))}", ${arg(parameter, "KEY")})`;
    }

    //% block="字典 [NAME] 包含键 [KEY]？" blockType="boolean"
    //% NAME.shadow="string" NAME.defl="d"
    //% KEY.shadow="string" KEY.defl="temp"
    export function dictHas(parameter: any, block: any) {
        useLib();
        return `ListLib::dictHas("${rawText(arg(parameter, "NAME"))}", ${arg(parameter, "KEY")})`;
    }

    //% block="删除字典 [NAME] 的键 [KEY]" blockType="command"
    //% NAME.shadow="string" NAME.defl="d"
    //% KEY.shadow="string" KEY.defl="temp"
    export function dictRemove(parameter: any, block: any) {
        useLib();
        Generator.addCode(`ListLib::dictRemove("${rawText(arg(parameter, "NAME"))}", ${arg(parameter, "KEY")});`);
    }

    //% block="字典 [NAME] 的键数量" blockType="reporter"
    //% NAME.shadow="string" NAME.defl="d"
    export function dictSize(parameter: any, block: any) {
        useLib();
        return `ListLib::dictSize("${rawText(arg(parameter, "NAME"))}")`;
    }

    //% block="字典 [NAME] 的所有键，用 [SEP] 连接" blockType="reporter"
    //% NAME.shadow="string" NAME.defl="d"
    //% SEP.shadow="string" SEP.defl=","
    export function dictKeys(parameter: any, block: any) {
        useLib();
        return `ListLib::dictKeys("${rawText(arg(parameter, "NAME"))}", ${arg(parameter, "SEP")})`;
    }

    //% block="清空字典 [NAME]" blockType="command"
    //% NAME.shadow="string" NAME.defl="d"
    export function dictClear(parameter: any, block: any) {
        useLib();
        Generator.addCode(`ListLib::dictClear("${rawText(arg(parameter, "NAME"))}");`);
    }
}
