#
# Copyright (c) 2025 HARDWARIO a.s.
#
# SPDX-License-Identifier: LicenseRef-HARDWARIO-5-Clause
#

'''hio-vex.py

West extension for HARDWARIO CHESTER platform.

Transforms an OpenVEX document: statements already in it are kept, and for
every grype match that is not covered by one, a statement is added if the
match can be disproved from the build:

 - the upstream fix is in the local checkout (fixed),
 - the vulnerable source files are not compiled (not_affected),
 - nothing of the matched component is compiled (not_affected).

Everything else is left to grype and to manual statements. Usage:

    grype sbom:<build>/<domain>/spdx/sbom.spdx.json -o json > grype.json
    west hio-vex --grype grype.json [--vex manual.openvex.json]
    grype sbom:<...>/sbom.spdx.json --vex <build>/<domain>/spdx/vex.openvex.json
'''

import json
import os
import re
import subprocess
import urllib.request
from datetime import datetime, timezone

import yaml
from west import log
from west.commands import WestCommand

CVE_API = 'https://cveawg.mitre.org/api/cve/'
AUTO = 'hio-vex:'  # status_notes prefix of generated statements


def git(repo, *args):
    r = subprocess.run(['git', '-C', repo, *args], capture_output=True, text=True)
    return r.stdout.strip() if r.returncode == 0 else None


def domains(build_dir):
    '''Return the build directories of all sysbuild domains, or of a plain build.'''
    path = os.path.join(build_dir, 'domains.yaml')
    if not os.path.exists(path):
        return [build_dir]
    with open(path) as f:
        return [d['build_dir'] for d in yaml.safe_load(f)['domains']]


def built_sources(build_dirs):
    '''Return the real paths of all files compiled in any of the build directories.'''
    built = set()
    for build_dir in build_dirs:
        path = os.path.join(build_dir, 'compile_commands.json')
        if not os.path.exists(path):
            raise SystemExit(f'{path} not found, build with CMAKE_EXPORT_COMPILE_COMMANDS=ON')
        with open(path) as f:
            built.update(os.path.realpath(os.path.join(c['directory'], c['file']))
                         for c in json.load(f))
    return built


def module_roots(build_dir, zephyr_dir):
    '''Return module name -> path from zephyr_modules.txt, plus zephyr itself.'''
    with open(os.path.join(build_dir, 'zephyr_modules.txt')) as f:
        roots = {n: p for n, p, _ in re.findall(r'"(.*?)":"(.*?)":"(.*?)"', f.read())}
    roots['zephyr'] = zephyr_dir
    return roots


def patch_id(repo, commit):
    '''Return the stable patch-id of a commit, or None if it is unknown.'''
    patch = git(repo, 'show', commit)
    if patch is None:
        return None
    r = subprocess.run(['git', 'patch-id', '--stable'], input=patch,
                       capture_output=True, text=True)
    return r.stdout.split()[0] if r.stdout else None


def fix_present(repo, sha):
    '''Return the local commit carrying the fix `sha`, a cherry-pick counts, or None.'''
    if git(repo, 'merge-base', '--is-ancestor', sha, 'HEAD') is not None:
        return sha
    subject, wanted = git(repo, 'log', '-1', '--format=%s', sha), patch_id(repo, sha)
    if not subject or not wanted:
        return None
    candidates = git(repo, 'log', '--format=%H', '-F', f'--grep={subject}', 'HEAD')
    return next((c for c in (candidates or '').split() if patch_id(repo, c) == wanted), None)


def decide(record, repo, built):
    '''Return (status, justification, note) disproving the CVE, or None.'''
    cna = record['containers']['cna']
    # only commits tagged as patch: other referenced commits may be the introducing one
    shas = sorted({m.group(1) for r in cna.get('references', []) if 'patch' in r.get('tags', [])
                   for m in [re.search(r'/commit/([0-9a-f]{40})$', r['url'])] if m})
    local = [fix_present(repo, sha) for sha in shas]
    if shas and all(local):
        return 'fixed', None, f'fix present as {", ".join(c[:12] for c in local)}'

    files = [f for a in cna.get('affected', []) for f in a.get('programFiles', [])]
    # a file that does not exist under repo means the path is not relative to it, so no verdict
    if files and all(f.endswith(('.c', '.S')) and os.path.exists(os.path.join(repo, f))
                     for f in files):
        if not any(os.path.realpath(os.path.join(repo, f)) in built for f in files):
            return ('not_affected', 'vulnerable_code_not_present',
                    f'not compiled: {", ".join(files)}')
    elif not files and not any(p.startswith(os.path.realpath(repo) + os.sep) for p in built):
        return 'not_affected', 'component_not_present', 'no source of the component is compiled'
    return None


def covered(doc, cve, artifact):
    '''Return True if a statement in doc already answers cve for the artifact.'''
    ids = {artifact['purl'], *artifact.get('cpes', [])}
    purl_base = artifact['purl'].split('@')[0]
    for st in doc['statements']:
        vuln = st['vulnerability']
        if cve not in [vuln.get('name'), *vuln.get('aliases', [])]:
            continue
        for p in st['products']:
            pid = [p.get('@id'), *p.get('identifiers', {}).values()]
            if ids & set(pid) or purl_base in pid:
                return True
    return False


def purl_name(purl):
    m = re.match(r'pkg:\w+/(?:[^/@?#]+/)?([^@?#]+)', purl)
    return m.group(1) if m else None


class HioVex(WestCommand):

    def __init__(self):
        super().__init__(
            'hio-vex',
            'add build-aware statements to an OpenVEX document',
            'Add OpenVEX statements for grype matches that are fixed in the '
            'local checkout or not compiled into the build.')

    def do_add_parser(self, parser_adder):
        parser = parser_adder.add_parser(self.name, help=self.help,
                                         description=self.description)
        parser.add_argument('-d', '--build-dir', default='build',
                            help='build directory (default: build)')
        parser.add_argument('--grype', required=True, help='grype JSON output (-o json)')
        parser.add_argument('--vex', help='OpenVEX document with manual statements')
        parser.add_argument('-o', '--output',
                            help='output file (default: <domain>/spdx/vex.openvex.json)')
        return parser

    def do_run(self, args, unknown_args):
        build_dirs = domains(args.build_dir)
        output = args.output or os.path.join(build_dirs[0], 'spdx', 'vex.openvex.json')
        now = datetime.now(timezone.utc).strftime('%Y-%m-%dT%H:%M:%SZ')

        if args.vex:
            with open(args.vex) as f:
                doc = json.load(f)
        else:
            doc = {'@context': 'https://openvex.dev/ns/v0.2.0',
                   '@id': f'https://hardwario.com/vex/{os.path.basename(build_dirs[0])}-{now}',
                   'author': 'HARDWARIO a.s.', 'version': 1, 'statements': []}
        # drop our earlier output so that feeding it back in does not duplicate it
        doc['statements'] = [s for s in doc['statements']
                             if not s.get('status_notes', '').startswith(AUTO)]
        doc['timestamp'] = now

        with open(args.grype) as f:
            matches = json.load(f)['matches']
        built = built_sources(build_dirs)
        roots = module_roots(build_dirs[0], next(
            p.abspath for p in self.manifest.get_projects(['zephyr'])))

        added = 0
        for m in matches:
            artifact, cve = m['artifact'], m['vulnerability']['id']
            root = roots.get(purl_name(artifact.get('purl', '')))
            if root is None or not cve.startswith('CVE-') or covered(doc, cve, artifact):
                continue
            try:
                with urllib.request.urlopen(CVE_API + cve, timeout=30) as r:
                    record = json.load(r)
                verdict = decide(record, root, built)
            except (OSError, KeyError, ValueError) as e:
                log.wrn(f'{cve} {artifact["name"]}: cannot evaluate ({e})')
                continue
            if verdict is None:
                log.inf(f'{cve} {artifact["name"]}: not disproved')
                continue
            status, justification, note = verdict
            statement = {
                'vulnerability': {'name': cve},
                'products': [{'@id': artifact['purl'],
                              'identifiers': {'purl': artifact['purl'],
                                              **({'cpe23': artifact['cpes'][0]}
                                                 if artifact.get('cpes') else {})}}],
                'status': status,
                'status_notes': f'{AUTO} {note}',
                'timestamp': now}
            if justification:
                statement['justification'] = justification
            doc['statements'].append(statement)
            log.inf(f'{cve} {artifact["name"]}: {status}, {note}')
            added += 1

        os.makedirs(os.path.dirname(output), exist_ok=True)
        with open(output, 'w') as f:
            json.dump(doc, f, indent=2)
        log.inf(f'{output}: {added} statements added, {len(doc["statements"])} total')
