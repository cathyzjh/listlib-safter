#!/usr/bin/env python3
"""Prepare a JS test from this extension's restricted TypeScript syntax.
This is a generator regression test, not a substitute for a Mind+ compiler.
Run the resulting .mjs with Node.js (no npm dependencies).
"""
from pathlib import Path
import re
import sys
root = Path(__file__).resolve().parents[1]
out = Path(sys.argv[1]).resolve()
s = (root / 'arduinoC/main.ts').read_text()
s = s[s.index('// Mind+ 已生成'):]
s = re.sub(r'namespace listlib_safter\s*\{', '', s, count=1)
s = s[:s.rfind('}')]
s = re.sub(r':\s*(any|string)\b', '', s)
s = s.replace('export function', 'function')
functions = re.findall(r'function (\w+)\(parameter, block\)', s)
prelude = '''import assert from 'node:assert/strict';
import fs from 'node:fs';
let commands = [];
const Generator = {addInclude(id, code) {assert.equal(code, '#include <listlib.h>');}, addCode(code) {commands.push(code);}};
'''
tests = r'''
const parameter = {};
for (const key of ['NAME','SRC','DST']) parameter[key] = {code:'nameVariable'};
for (const key of ['DATA','SEP','KEY','VAL']) parameter[key] = {code:'"1"'};
for (const key of ['IDX','A','B']) parameter[key] = {code:'0'};
parameter.ST = {code:'LISTLIB_SAFER_STATTYPE.max'};
// VAL must be numeric for numeric/dictionary setters.
const cpp = ['#include "listlib.h"', 'void generatorCoverage() {', 'String nameVariable("n");'];
for (const fn of FUNCTIONS) {
  commands = [];
  parameter.VAL = {code: /Str/.test(fn.name) ? '"text"' : '1'};
  const result = fn(parameter, null);
  const code = result === undefined ? commands.join('\n') : `(void)(${result});`;
  assert.ok(code.includes('ListLib::'), fn.name);
  assert.ok(!code.includes('"nameVariable"'), fn.name + ' quoted a variable');
  cpp.push(code);
}
for (const value of ['"中文"', '"a\\\"b"', '"a\\\\b"', '"a\\nb"', '"x\\\"); arbitrary(); //"', 'String("dynamic")']) {
  commands = [];
  createNumList({NAME:{code:value}}, null);
  assert.equal(commands[0], `ListLib::createNum(${value});`);
  cpp.push(commands[0]);
}
commands=[]; createNumList({}, null); assert.equal(commands[0], 'ListLib::createNum("");');
assert.equal(statNum({NAME:{code:'nameVariable'},ST:{code:'LISTLIB_SAFER_STATTYPE.max'}},null),'ListLib::maxNum(nameVariable)');
assert.equal(statNum({NAME:{code:'nameVariable'},ST:{code:'1'}},null),'ListLib::maxNum(nameVariable)');
cpp.push('}');
fs.writeFileSync(CPP_PATH, cpp.join('\n'));
console.log(`PASS: ${FUNCTIONS.length} block generators; expression names, escaped literals, empty names, statistics`);
'''
import json
tests = tests.replace('FUNCTIONS', '[' + ','.join(functions) + ']')
tests = tests.replace('CPP_PATH', json.dumps(str(out.with_suffix('.cpp'))))
out.parent.mkdir(parents=True, exist_ok=True)
out.write_text(prelude+s+tests)
print(out)
