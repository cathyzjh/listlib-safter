#!/usr/bin/env python3
"""Create a deterministic Mind+ archive from the versioned source, then verify it."""
import argparse
import hashlib
import json
from pathlib import Path
import re
import zipfile

ROOT = Path(__file__).resolve().parents[1]

def package(output_dir):
    config = json.loads((ROOT/'config.json').read_text())
    version = config['version']
    if not re.fullmatch(r'\d+\.\d+\.\d+', version):
        raise ValueError('invalid semantic version')
    if config['asset']['arduinoC']['version'] != version:
        raise ValueError('extension and Arduino versions do not match')
    files = [ROOT/'LICENSE.TXT', ROOT/'README.md', ROOT/'config.json']
    files += sorted(p for p in (ROOT/'arduinoC').rglob('*') if p.is_file() and not p.name.startswith('.'))
    required = {'config.json','arduinoC/main.ts','arduinoC/libraries/listlib/listlib.h','LICENSE.TXT','README.md'}
    payload = {str(p.relative_to(ROOT)):p.read_bytes() for p in files}
    if not required <= payload.keys():
        raise ValueError('missing required extension files')
    if 'listlib-safter-V'+version+'.mpext' not in payload['README.md'].decode():
        raise ValueError('README installation filename does not match the release version')
    output_dir.mkdir(parents=True, exist_ok=True)
    archive = output_dir/f'listlib-safter-V{version}.mpext'
    with zipfile.ZipFile(archive, 'w', compression=zipfile.ZIP_DEFLATED) as z:
        for name,data in sorted(payload.items()):
            info=zipfile.ZipInfo(name, date_time=(2026,1,1,0,0,0))
            info.compress_type=zipfile.ZIP_DEFLATED
            info.external_attr=0o100644 << 16
            z.writestr(info,data)
    with zipfile.ZipFile(archive) as z:
        if z.testzip() is not None:
            raise ValueError('archive CRC verification failed')
        if set(z.namelist())!=set(payload):
            raise ValueError('archive inventory mismatch')
        for name,data in payload.items():
            if z.read(name)!=data:
                raise ValueError('archive content mismatch: '+name)
        packaged=json.loads(z.read('config.json'))
        if packaged['version']!=version or packaged['asset']['arduinoC']['version']!=version:
            raise ValueError('packaged version mismatch')
    checksum=hashlib.sha256(archive.read_bytes()).hexdigest()
    archive.with_suffix(archive.suffix+'.sha256').write_text(f'{checksum}  {archive.name}\n')
    print(f'PASS: verified {len(payload)} packaged files, version {version}\n{archive}\nSHA-256: {checksum}')
    return archive

if __name__=='__main__':
    parser=argparse.ArgumentParser()
    parser.add_argument('--output-dir', type=Path, default=ROOT/'dist')
    package(parser.parse_args().output_dir.resolve())
