/*
 * main.ts - 高级数据类型（列表 / 字典）用户库
 * 适用: Mind+ V1.8 上传模式 - Arduino C - 行空板 K10
 *
 * 说明:
 *   Python 的 list / dict 是运行时对象，本项目用「名字 -> 容器」的注册表
 *   在 C 里还原同样的语义，因此列表名可以用中文、随时新建、随时清空。
 */

enum LISTLIB_SAFER_STATTYPE {
    //% block="总和"
    sum,
    //% block="最大值"
    max,
    //% block="最小值"
    min,
    //% block="平均值"
    avg
}

// Mind+ 已生成合法的 C++ 字符串/表达式，直接传递，不拆引号或重新拼接字面量。
function nameCode(parameter: any, key: string): string {
    return arg(parameter, key).trim() || '""';
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
namespace listlib_safter {

    // ==================== 数字列表 ====================

    //% block="创建数字列表 [NAME]" blockType="command"
    //% NAME.shadow="string" NAME.defl="lst"
    export function createNumList(parameter: any, block: any) {
        useLib();
        Generator.addCode(`ListLib::createNum(${nameCode(parameter, "NAME")});`);
    }

    //% block="把数字列表 [NAME] 初始化为 [DATA]，分隔符 [SEP]" blockType="command"
    //% NAME.shadow="string" NAME.defl="lst"
    //% DATA.shadow="string" DATA.defl="1,2,3,4"
    //% SEP.shadow="string" SEP.defl=","
    export function initNumList(parameter: any, block: any) {
        useLib();
        Generator.addCode(`ListLib::initNum(${nameCode(parameter, "NAME")}, ${arg(parameter, "DATA")}, ${arg(parameter, "SEP")});`);
    }

    //% block="向数字列表 [NAME] 追加 [VAL]" blockType="command"
    //% NAME.shadow="string" NAME.defl="lst"
    //% VAL.shadow="number" VAL.defl=0
    export function addNum(parameter: any, block: any) {
        useLib();
        Generator.addCode(`ListLib::addNum(${nameCode(parameter, "NAME")}, ${arg(parameter, "VAL")});`);
    }

    //% block="在数字列表 [NAME] 第 [IDX] 项前插入 [VAL]" blockType="command"
    //% NAME.shadow="string" NAME.defl="lst"
    //% IDX.shadow="number" IDX.defl=0
    //% VAL.shadow="number" VAL.defl=0
    export function insertNum(parameter: any, block: any) {
        useLib();
        Generator.addCode(`ListLib::insertNum(${nameCode(parameter, "NAME")}, ${arg(parameter, "IDX")}, ${arg(parameter, "VAL")});`);
    }

    //% block="把数字列表 [NAME] 第 [IDX] 项设为 [VAL]" blockType="command"
    //% NAME.shadow="string" NAME.defl="lst"
    //% IDX.shadow="number" IDX.defl=0
    //% VAL.shadow="number" VAL.defl=0
    export function setNum(parameter: any, block: any) {
        useLib();
        Generator.addCode(`ListLib::setNum(${nameCode(parameter, "NAME")}, ${arg(parameter, "IDX")}, ${arg(parameter, "VAL")});`);
    }

    //% block="数字列表 [NAME] 第 [IDX] 项" blockType="reporter"
    //% NAME.shadow="string" NAME.defl="lst"
    //% IDX.shadow="number" IDX.defl=0
    export function getNum(parameter: any, block: any) {
        useLib();
        return `ListLib::getNum(${nameCode(parameter, "NAME")}, ${arg(parameter, "IDX")})`;
    }

    //% block="删除数字列表 [NAME] 中第一个 [VAL]" blockType="command"
    //% NAME.shadow="string" NAME.defl="lst"
    //% VAL.shadow="number" VAL.defl=0
    export function removeValueNum(parameter: any, block: any) {
        useLib();
        Generator.addCode(`ListLib::removeValueNum(${nameCode(parameter, "NAME")}, ${arg(parameter, "VAL")});`);
    }

    //% block="数字列表 [NAME] 包含 [VAL]？" blockType="boolean"
    //% NAME.shadow="string" NAME.defl="lst"
    //% VAL.shadow="number" VAL.defl=0
    export function containsNum(parameter: any, block: any) {
        useLib();
        return `ListLib::containsNum(${nameCode(parameter, "NAME")}, ${arg(parameter, "VAL")})`;
    }

    //% block="数字列表 [NAME] 中 [VAL] 的位置（没有则 -1）" blockType="reporter"
    //% NAME.shadow="string" NAME.defl="lst"
    //% VAL.shadow="number" VAL.defl=0
    export function indexOfNum(parameter: any, block: any) {
        useLib();
        return `ListLib::indexOfNum(${nameCode(parameter, "NAME")}, ${arg(parameter, "VAL")})`;
    }

    //% block="数字列表 [NAME] 的 [ST]" blockType="reporter"
    //% NAME.shadow="string" NAME.defl="lst"
    //% ST.shadow="dropdown" ST.options="LISTLIB_SAFER_STATTYPE"
    export function statNum(parameter: any, block: any) {
        useLib();
        let st = arg(parameter, "ST").replace(/['"]/g, "").split(".").pop();
        let name = nameCode(parameter, "NAME");
        if (st === "max" || st === "1") return `ListLib::maxNum(${name})`;
        if (st === "min" || st === "2") return `ListLib::minNum(${name})`;
        if (st === "avg" || st === "3") return `ListLib::avgNum(${name})`;
        return `ListLib::sumNum(${name})`;
    }

    //% block="对数字列表 [NAME] 升序排序" blockType="command"
    //% NAME.shadow="string" NAME.defl="lst"
    export function sortNum(parameter: any, block: any) {
        useLib();
        Generator.addCode(`ListLib::sortNum(${nameCode(parameter, "NAME")});`);
    }

    //% block="用分隔符 [SEP] 拼接数字列表 [NAME]" blockType="reporter"
    //% NAME.shadow="string" NAME.defl="lst"
    //% SEP.shadow="string" SEP.defl=","
    export function dumpNum(parameter: any, block: any) {
        useLib();
        return `ListLib::dumpNum(${nameCode(parameter, "NAME")}, ${arg(parameter, "SEP")})`;
    }

    //% block="把数字列表 [SRC] 的第 [A] 到 [B] 项复制到 [DST]" blockType="command"
    //% SRC.shadow="string" SRC.defl="lst"
    //% A.shadow="number" A.defl=0
    //% B.shadow="number" B.defl=-1
    //% DST.shadow="string" DST.defl="lst2"
    export function sliceNum(parameter: any, block: any) {
        useLib();
        Generator.addCode(`ListLib::sliceNum(${nameCode(parameter, "SRC")}, ${arg(parameter, "A")}, ${arg(parameter, "B")}, ${nameCode(parameter, "DST")});`);
    }

    // ==================== 文本列表 ====================

    //% block="创建文本列表 [NAME]" blockType="command"
    //% NAME.shadow="string" NAME.defl="words"
    export function createStrList(parameter: any, block: any) {
        useLib();
        Generator.addCode(`ListLib::createStr(${nameCode(parameter, "NAME")});`);
    }

    //% block="把文本列表 [NAME] 初始化为 [DATA]，分隔符 [SEP]" blockType="command"
    //% NAME.shadow="string" NAME.defl="words"
    //% DATA.shadow="string" DATA.defl="红,绿,蓝"
    //% SEP.shadow="string" SEP.defl=","
    export function initStrList(parameter: any, block: any) {
        useLib();
        Generator.addCode(`ListLib::initStr(${nameCode(parameter, "NAME")}, ${arg(parameter, "DATA")}, ${arg(parameter, "SEP")});`);
    }

    //% block="向文本列表 [NAME] 追加 [VAL]" blockType="command"
    //% NAME.shadow="string" NAME.defl="words"
    //% VAL.shadow="string" VAL.defl="hello"
    export function addStr(parameter: any, block: any) {
        useLib();
        Generator.addCode(`ListLib::addStr(${nameCode(parameter, "NAME")}, ${arg(parameter, "VAL")});`);
    }

    //% block="在文本列表 [NAME] 第 [IDX] 项前插入 [VAL]" blockType="command"
    //% NAME.shadow="string" NAME.defl="words"
    //% IDX.shadow="number" IDX.defl=0
    //% VAL.shadow="string" VAL.defl="hello"
    export function insertStr(parameter: any, block: any) {
        useLib();
        Generator.addCode(`ListLib::insertStr(${nameCode(parameter, "NAME")}, ${arg(parameter, "IDX")}, ${arg(parameter, "VAL")});`);
    }

    //% block="把文本列表 [NAME] 第 [IDX] 项设为 [VAL]" blockType="command"
    //% NAME.shadow="string" NAME.defl="words"
    //% IDX.shadow="number" IDX.defl=0
    //% VAL.shadow="string" VAL.defl="hello"
    export function setStr(parameter: any, block: any) {
        useLib();
        Generator.addCode(`ListLib::setStr(${nameCode(parameter, "NAME")}, ${arg(parameter, "IDX")}, ${arg(parameter, "VAL")});`);
    }

    //% block="文本列表 [NAME] 第 [IDX] 项" blockType="reporter"
    //% NAME.shadow="string" NAME.defl="words"
    //% IDX.shadow="number" IDX.defl=0
    export function getStr(parameter: any, block: any) {
        useLib();
        return `ListLib::getStr(${nameCode(parameter, "NAME")}, ${arg(parameter, "IDX")})`;
    }

    //% block="文本列表 [NAME] 包含 [VAL]？" blockType="boolean"
    //% NAME.shadow="string" NAME.defl="words"
    //% VAL.shadow="string" VAL.defl="hello"
    export function containsStr(parameter: any, block: any) {
        useLib();
        return `ListLib::containsStr(${nameCode(parameter, "NAME")}, ${arg(parameter, "VAL")})`;
    }

    //% block="文本列表 [NAME] 中 [VAL] 的位置（没有则 -1）" blockType="reporter"
    //% NAME.shadow="string" NAME.defl="words"
    //% VAL.shadow="string" VAL.defl="hello"
    export function indexOfStr(parameter: any, block: any) {
        useLib();
        return `ListLib::indexOfStr(${nameCode(parameter, "NAME")}, ${arg(parameter, "VAL")})`;
    }

    //% block="用分隔符 [SEP] 拼接文本列表 [NAME]" blockType="reporter"
    //% NAME.shadow="string" NAME.defl="words"
    //% SEP.shadow="string" SEP.defl=","
    export function joinStr(parameter: any, block: any) {
        useLib();
        return `ListLib::joinStr(${nameCode(parameter, "NAME")}, ${arg(parameter, "SEP")})`;
    }

    //% block="把文本列表 [SRC] 的第 [A] 到 [B] 项复制到 [DST]" blockType="command"
    //% SRC.shadow="string" SRC.defl="words"
    //% A.shadow="number" A.defl=0
    //% B.shadow="number" B.defl=-1
    //% DST.shadow="string" DST.defl="words2"
    export function sliceStr(parameter: any, block: any) {
        useLib();
        Generator.addCode(`ListLib::sliceStr(${nameCode(parameter, "SRC")}, ${arg(parameter, "A")}, ${arg(parameter, "B")}, ${nameCode(parameter, "DST")});`);
    }

    // ==================== 列表通用 ====================

    //% block="列表 [NAME] 的长度" blockType="reporter"
    //% NAME.shadow="string" NAME.defl="lst"
    export function listSize(parameter: any, block: any) {
        useLib();
        return `ListLib::size(${nameCode(parameter, "NAME")})`;
    }

    //% block="删除列表 [NAME] 第 [IDX] 项" blockType="command"
    //% NAME.shadow="string" NAME.defl="lst"
    //% IDX.shadow="number" IDX.defl=0
    export function removeAt(parameter: any, block: any) {
        useLib();
        Generator.addCode(`ListLib::removeAt(${nameCode(parameter, "NAME")}, ${arg(parameter, "IDX")});`);
    }

    //% block="删除列表 [NAME] 的最后一项" blockType="command"
    //% NAME.shadow="string" NAME.defl="lst"
    export function popBack(parameter: any, block: any) {
        useLib();
        Generator.addCode(`ListLib::popBack(${nameCode(parameter, "NAME")});`);
    }

    //% block="清空列表 [NAME]" blockType="command"
    //% NAME.shadow="string" NAME.defl="lst"
    export function listClear(parameter: any, block: any) {
        useLib();
        Generator.addCode(`ListLib::clear(${nameCode(parameter, "NAME")});`);
    }

    //% block="把列表 [SRC] 完整复制到 [DST]" blockType="command"
    //% SRC.shadow="string" SRC.defl="lst"
    //% DST.shadow="string" DST.defl="lst2"
    export function copyList(parameter: any, block: any) {
        useLib();
        Generator.addCode(`ListLib::copy(${nameCode(parameter, "SRC")}, ${nameCode(parameter, "DST")});`);
    }

    // ==================== 字典 dict ====================

    //% block="创建字典 [NAME]" blockType="command"
    //% NAME.shadow="string" NAME.defl="d"
    export function dictCreate(parameter: any, block: any) {
        useLib();
        Generator.addCode(`ListLib::dictCreate(${nameCode(parameter, "NAME")});`);
    }

    //% block="把字典 [NAME] 初始化为 [DATA]（例 temp:25,hum:60）" blockType="command"
    //% NAME.shadow="string" NAME.defl="d"
    //% DATA.shadow="string" DATA.defl="temp:25,hum:60"
    export function initDict(parameter: any, block: any) {
        useLib();
        Generator.addCode(`ListLib::initDict(${nameCode(parameter, "NAME")}, ${arg(parameter, "DATA")}, ",", ":");`);
    }

    //% block="字典 [NAME] 设置键 [KEY] 为 [VAL]" blockType="command"
    //% NAME.shadow="string" NAME.defl="d"
    //% KEY.shadow="string" KEY.defl="temp"
    //% VAL.shadow="number" VAL.defl=0
    export function dictSet(parameter: any, block: any) {
        useLib();
        Generator.addCode(`ListLib::dictSet(${nameCode(parameter, "NAME")}, ${arg(parameter, "KEY")}, ${arg(parameter, "VAL")});`);
    }

    //% block="字典 [NAME] 键 [KEY] 的值" blockType="reporter"
    //% NAME.shadow="string" NAME.defl="d"
    //% KEY.shadow="string" KEY.defl="temp"
    export function dictGet(parameter: any, block: any) {
        useLib();
        return `ListLib::dictGet(${nameCode(parameter, "NAME")}, ${arg(parameter, "KEY")})`;
    }

    //% block="字典 [NAME] 包含键 [KEY]？" blockType="boolean"
    //% NAME.shadow="string" NAME.defl="d"
    //% KEY.shadow="string" KEY.defl="temp"
    export function dictHas(parameter: any, block: any) {
        useLib();
        return `ListLib::dictHas(${nameCode(parameter, "NAME")}, ${arg(parameter, "KEY")})`;
    }

    //% block="删除字典 [NAME] 的键 [KEY]" blockType="command"
    //% NAME.shadow="string" NAME.defl="d"
    //% KEY.shadow="string" KEY.defl="temp"
    export function dictRemove(parameter: any, block: any) {
        useLib();
        Generator.addCode(`ListLib::dictRemove(${nameCode(parameter, "NAME")}, ${arg(parameter, "KEY")});`);
    }

    //% block="字典 [NAME] 的键数量" blockType="reporter"
    //% NAME.shadow="string" NAME.defl="d"
    export function dictSize(parameter: any, block: any) {
        useLib();
        return `ListLib::dictSize(${nameCode(parameter, "NAME")})`;
    }

    //% block="字典 [NAME] 的所有键，用 [SEP] 连接" blockType="reporter"
    //% NAME.shadow="string" NAME.defl="d"
    //% SEP.shadow="string" SEP.defl=","
    export function dictKeys(parameter: any, block: any) {
        useLib();
        return `ListLib::dictKeys(${nameCode(parameter, "NAME")}, ${arg(parameter, "SEP")})`;
    }

    //% block="清空字典 [NAME]" blockType="command"
    //% NAME.shadow="string" NAME.defl="d"
    export function dictClear(parameter: any, block: any) {
        useLib();
        Generator.addCode(`ListLib::dictClear(${nameCode(parameter, "NAME")});`);
    }

    //% block="删除整个列表 [NAME] 并释放内存" blockType="command"
    //% NAME.shadow="string" NAME.defl="lst"
    export function destroyList(parameter: any, block: any) {
        useLib();
        Generator.addCode(`ListLib::destroy(${nameCode(parameter, "NAME")});`);
    }

    //% block="删除整个字典 [NAME] 并释放内存" blockType="command"
    //% NAME.shadow="string" NAME.defl="d"
    export function destroyDict(parameter: any, block: any) {
        useLib();
        Generator.addCode(`ListLib::dictDestroy(${nameCode(parameter, "NAME")});`);
    }

    //% block="上次容器操作的错误码（0为成功）" blockType="reporter"
    export function lastError(parameter: any, block: any) {
        useLib();
        return `ListLib::lastError()`;
    }

    //% block="上次容器操作的错误说明" blockType="reporter"
    export function lastErrorText(parameter: any, block: any) {
        useLib();
        return `ListLib::lastErrorText()`;
    }

    //% block="清除容器错误状态" blockType="command"
    export function resetError(parameter: any, block: any) {
        useLib();
        Generator.addCode(`ListLib::resetError();`);
    }
}
