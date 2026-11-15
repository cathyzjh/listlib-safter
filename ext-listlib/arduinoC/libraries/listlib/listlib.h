/*
 * listlib.h - 对标 Python list / dict 的轻量容器库
 * 适用: 行空板 K10 (ESP32-S3) / Mind+ 上传模式 Arduino C
 *
 * 设计思路:
 *   Python 里的 lst / d 是"运行时对象"，名字不参与编译。
 *   这里同样用「名字字符串 -> 容器」的注册表实现，
 *   所以列表名可以是中文、可以随时新建，不需要在 C 里声明变量。
 *
 * 用法示例:
 *   ListLib::createNum("数据");
 *   ListLib::addNum("数据", 25.5);
 *   double t = ListLib::getNum("数据", 0);
 */

#ifndef __LISTLIB_H__
#define __LISTLIB_H__

#include <Arduino.h>
#include <vector>
#include <map>
#include <algorithm>

class ListLib {
public:
    // ---------- 内部结构 ----------
    struct Obj {
        int type;                  // 0 = 数字列表, 1 = 文本列表
        std::vector<double> n;     // 数字元素
        std::vector<String>  s;    // 文本元素
        Obj() : type(0) {}
    };

    static std::map<String, Obj> &store() {
        static std::map<String, Obj> m;
        return m;
    }

    static std::map<String, std::map<String, double> > &dictStore() {
        static std::map<String, std::map<String, double> > m;
        return m;
    }

    static Obj *find(const String &name) {
        std::map<String, Obj>::iterator it = store().find(name);
        if (it == store().end()) return NULL;
        return &(it->second);
    }

    // Python 风格负索引: -1 表示最后一个
    static int norm(int idx, int len) {
        if (idx < 0) idx = len + idx;
        return idx;
    }

    // ---------- 创建 / 清空 ----------
    static void createNum(const String &name) {
        Obj &o = store()[name];
        o.type = 0;
        o.n.clear();
        o.s.clear();
    }

    static void createStr(const String &name) {
        Obj &o = store()[name];
        o.type = 1;
        o.n.clear();
        o.s.clear();
    }

    static void clear(const String &name) {
        Obj *o = find(name);
        if (!o) return;
        o->n.clear();
        o->s.clear();
    }

    static int size(const String &name) {
        Obj *o = find(name);
        if (!o) return 0;
        return (o->type == 0) ? (int)o->n.size() : (int)o->s.size();
    }

    static bool exists(const String &name) {
        return find(name) != NULL;
    }

    // ---------- 数字列表: 增 ----------
    static void addNum(const String &name, double val) {
        Obj &o = store()[name];
        if (o.type != 0) return;
        o.n.push_back(val);
    }

    static void insertNum(const String &name, int idx, double val) {
        Obj &o = store()[name];
        if (o.type != 0) return;
        int len = (int)o.n.size();
        int i = norm(idx, len);
        if (i < 0) i = 0;
        if (i > len) i = len;
        o.n.insert(o.n.begin() + i, val);
    }

    // ---------- 文本列表: 增 ----------
    static void addStr(const String &name, const String &val) {
        Obj &o = store()[name];
        if (o.type != 1) return;
        o.s.push_back(val);
    }

    static void insertStr(const String &name, int idx, const String &val) {
        Obj &o = store()[name];
        if (o.type != 1) return;
        int len = (int)o.s.size();
        int i = norm(idx, len);
        if (i < 0) i = 0;
        if (i > len) i = len;
        o.s.insert(o.s.begin() + i, val);
    }

    // ---------- 读 / 改 ----------
    static double getNum(const String &name, int idx) {
        Obj *o = find(name);
        if (!o || o->type != 0 || o->n.empty()) return 0;
        int i = norm(idx, (int)o->n.size());
        if (i < 0 || i >= (int)o->n.size()) return 0;
        return o->n[i];
    }

    static String getStr(const String &name, int idx) {
        Obj *o = find(name);
        if (!o || o->type != 1 || o->s.empty()) return String("");
        int i = norm(idx, (int)o->s.size());
        if (i < 0 || i >= (int)o->s.size()) return String("");
        return o->s[i];
    }

    static void setNum(const String &name, int idx, double val) {
        Obj *o = find(name);
        if (!o || o->type != 0) return;
        int i = norm(idx, (int)o->n.size());
        if (i < 0 || i >= (int)o->n.size()) return;
        o->n[i] = val;
    }

    static void setStr(const String &name, int idx, const String &val) {
        Obj *o = find(name);
        if (!o || o->type != 1) return;
        int i = norm(idx, (int)o->s.size());
        if (i < 0 || i >= (int)o->s.size()) return;
        o->s[i] = val;
    }

    // ---------- 删 ----------
    static bool removeAt(const String &name, int idx) {
        Obj *o = find(name);
        if (!o) return false;
        int len = (o->type == 0) ? (int)o->n.size() : (int)o->s.size();
        int i = norm(idx, len);
        if (i < 0 || i >= len) return false;
        if (o->type == 0) o->n.erase(o->n.begin() + i);
        else              o->s.erase(o->s.begin() + i);
        return true;
    }

    static bool popBack(const String &name) {
        Obj *o = find(name);
        if (!o) return false;
        if (o->type == 0) {
            if (o->n.empty()) return false;
            o->n.pop_back();
        } else {
            if (o->s.empty()) return false;
            o->s.pop_back();
        }
        return true;
    }

    static bool removeValueNum(const String &name, double val) {
        Obj *o = find(name);
        if (!o || o->type != 0) return false;
        for (size_t i = 0; i < o->n.size(); i++) {
            if (o->n[i] == val) {
                o->n.erase(o->n.begin() + i);
                return true;
            }
        }
        return false;
    }

    static bool removeValueStr(const String &name, const String &val) {
        Obj *o = find(name);
        if (!o || o->type != 1) return false;
        for (size_t i = 0; i < o->s.size(); i++) {
            if (o->s[i] == val) {
                o->s.erase(o->s.begin() + i);
                return true;
            }
        }
        return false;
    }

    // ---------- 查找 ----------
    static bool containsNum(const String &name, double val) {
        Obj *o = find(name);
        if (!o || o->type != 0) return false;
        for (size_t i = 0; i < o->n.size(); i++) {
            if (o->n[i] == val) return true;
        }
        return false;
    }

    static bool containsStr(const String &name, const String &val) {
        Obj *o = find(name);
        if (!o || o->type != 1) return false;
        for (size_t i = 0; i < o->s.size(); i++) {
            if (o->s[i] == val) return true;
        }
        return false;
    }

    static int indexOfNum(const String &name, double val) {
        Obj *o = find(name);
        if (!o || o->type != 0) return -1;
        for (size_t i = 0; i < o->n.size(); i++) {
            if (o->n[i] == val) return (int)i;
        }
        return -1;
    }

    static int indexOfStr(const String &name, const String &val) {
        Obj *o = find(name);
        if (!o || o->type != 1) return -1;
        for (size_t i = 0; i < o->s.size(); i++) {
            if (o->s[i] == val) return (int)i;
        }
        return -1;
    }

    // ---------- 统计 / 排序 ----------
    static double sumNum(const String &name) {
        Obj *o = find(name);
        if (!o || o->type != 0) return 0;
        double s = 0;
        for (size_t i = 0; i < o->n.size(); i++) s += o->n[i];
        return s;
    }

    static double maxNum(const String &name) {
        Obj *o = find(name);
        if (!o || o->type != 0 || o->n.empty()) return 0;
        double m = o->n[0];
        for (size_t i = 1; i < o->n.size(); i++) if (o->n[i] > m) m = o->n[i];
        return m;
    }

    static double minNum(const String &name) {
        Obj *o = find(name);
        if (!o || o->type != 0 || o->n.empty()) return 0;
        double m = o->n[0];
        for (size_t i = 1; i < o->n.size(); i++) if (o->n[i] < m) m = o->n[i];
        return m;
    }

    static double avgNum(const String &name) {
        Obj *o = find(name);
        if (!o || o->type != 0 || o->n.empty()) return 0;
        return sumNum(name) / (double)o->n.size();
    }

    static void sortNum(const String &name) {
        Obj *o = find(name);
        if (!o || o->type != 0) return;
        std::sort(o->n.begin(), o->n.end());
    }

    static void sortStr(const String &name) {
        Obj *o = find(name);
        if (!o || o->type != 1) return;
        std::sort(o->s.begin(), o->s.end());
    }

    // ---------- 切片 / 拼接 / 复制 ----------
    static String joinStr(const String &name, const String &sep) {
        Obj *o = find(name);
        if (!o || o->type != 1) return String("");
        String out = "";
        for (size_t i = 0; i < o->s.size(); i++) {
            if (i) out += sep;
            out += o->s[i];
        }
        return out;
    }

    // 把 [start, end) 的片段复制成新列表（Python 的 lst[a:b]）
    static void sliceNum(const String &src, int start, int end, const String &dst) {
        Obj *a = find(src);
        Obj &b = store()[dst];
        b.type = 0;
        b.n.clear();
        b.s.clear();
        if (!a || a->type != 0) return;
        int len = (int)a->n.size();
        int s = norm(start, len);
        int e = norm(end, len);
        if (s < 0) s = 0;
        if (e > len) e = len;
        for (int i = s; i < e; i++) b.n.push_back(a->n[i]);
    }

    static void sliceStr(const String &src, int start, int end, const String &dst) {
        Obj *a = find(src);
        Obj &b = store()[dst];
        b.type = 1;
        b.n.clear();
        b.s.clear();
        if (!a || a->type != 1) return;
        int len = (int)a->s.size();
        int s = norm(start, len);
        int e = norm(end, len);
        if (s < 0) s = 0;
        if (e > len) e = len;
        for (int i = s; i < e; i++) b.s.push_back(a->s[i]);
    }

    static void copy(const String &src, const String &dst) {
        Obj *a = find(src);
        if (!a) return;
        Obj &b = store()[dst];
        b.type = a->type;
        b.n = a->n;
        b.s = a->s;
    }

    // ---------- 字典 dict ----------
    static void dictCreate(const String &name) {
        dictStore()[name];
    }

    static void dictClear(const String &name) {
        std::map<String, std::map<String, double> >::iterator it = dictStore().find(name);
        if (it != dictStore().end()) it->second.clear();
    }

    static void dictSet(const String &name, const String &key, double val) {
        dictStore()[name][key] = val;
    }

    static double dictGet(const String &name, const String &key) {
        std::map<String, std::map<String, double> >::iterator it = dictStore().find(name);
        if (it == dictStore().end()) return 0;
        std::map<String, double>::iterator k = it->second.find(key);
        if (k == it->second.end()) return 0;
        return k->second;
    }

    static bool dictHas(const String &name, const String &key) {
        std::map<String, std::map<String, double> >::iterator it = dictStore().find(name);
        if (it == dictStore().end()) return false;
        return it->second.find(key) != it->second.end();
    }

    static bool dictRemove(const String &name, const String &key) {
        std::map<String, std::map<String, double> >::iterator it = dictStore().find(name);
        if (it == dictStore().end()) return false;
        return it->second.erase(key) > 0;
    }

    static int dictSize(const String &name) {
        std::map<String, std::map<String, double> >::iterator it = dictStore().find(name);
        if (it == dictStore().end()) return 0;
        return (int)it->second.size();
    }

    static String dictKeys(const String &name, const String &sep) {
        std::map<String, std::map<String, double> >::iterator it = dictStore().find(name);
        if (it == dictStore().end()) return String("");
        String out = "";
        bool first = true;
        for (std::map<String, double>::iterator k = it->second.begin(); k != it->second.end(); ++k) {
            if (!first) out += sep;
            out += k->first;
            first = false;
        }
        return out;
    }

    // ---------- 批量初始化（对应 Python 的 lst = [1, 2, 3]） ----------
    // 分隔符为空时退回逗号，避免死循环
    static String sepOf(const String &sep) {
        return sep.length() ? sep : String(",");
    }

    // 例: initNum("lst", "1,2,3,4", ",")
    static void initNum(const String &name, const String &data, const String &sep) {
        createNum(name);
        String s = sepOf(sep);
        int len = (int)data.length();
        int start = 0;
        while (start <= len) {
            int pos = data.indexOf(s, start);
            String tok = (pos < 0) ? data.substring(start) : data.substring(start, pos);
            tok.trim();
            if (tok.length() > 0) addNum(name, tok.toDouble());
            if (pos < 0) break;
            start = pos + (int)s.length();
        }
    }

    // 例: initStr("words", "红,绿,蓝", ",")
    static void initStr(const String &name, const String &data, const String &sep) {
        createStr(name);
        String s = sepOf(sep);
        int len = (int)data.length();
        int start = 0;
        while (start <= len) {
            int pos = data.indexOf(s, start);
            String tok = (pos < 0) ? data.substring(start) : data.substring(start, pos);
            tok.trim();
            if (tok.length() > 0) addStr(name, tok);
            if (pos < 0) break;
            start = pos + (int)s.length();
        }
    }

    // 例: initDict("d", "temp:25,hum:60", ",", ":")
    static void initDict(const String &name, const String &data, const String &sep, const String &kvSep) {
        dictCreate(name);
        String s = sepOf(sep);
        String kv = kvSep.length() ? kvSep : String(":");
        int len = (int)data.length();
        int start = 0;
        while (start <= len) {
            int pos = data.indexOf(s, start);
            String tok = (pos < 0) ? data.substring(start) : data.substring(start, pos);
            tok.trim();
            if (tok.length() > 0) {
                int kp = tok.indexOf(kv);
                if (kp > 0) {
                    String k = tok.substring(0, kp);
                    String v = tok.substring(kp + (int)kv.length());
                    k.trim();
                    v.trim();
                    if (k.length() > 0) dictSet(name, k, v.toDouble());
                }
            }
            if (pos < 0) break;
            start = pos + (int)s.length();
        }
    }

    // ---------- 调试：把列表内容拼成文本 ----------
    static String dumpNum(const String &name, const String &sep) {
        Obj *o = find(name);
        if (!o || o->type != 0) return String("");
        String out = "";
        for (size_t i = 0; i < o->n.size(); i++) {
            if (i) out += sep;
            out += String(o->n[i], 2);
        }
        return out;
    }
};

#endif
