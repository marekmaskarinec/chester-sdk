#!/usr/bin/env python3
import yaml
import sys
import json
import os

PREFIX = 'com.hardwario.chester.app.'


def panic(msg):
    print(msg, file=sys.stderr)
    sys.exit(1)


if len(sys.argv) != 3:
    panic("Usage: gen-applications.py <app> <out>")

app = sys.argv[1]
out = sys.argv[2]

# "<variant>-sb": same firmware, but MCUboot is signed with HARDWARIO's own key
# (written to $RUNNER_TEMP by the workflow from a CI secret).
SB_SUFFIX = '-sb'
SB_KEY = '$RUNNER_TEMP/chester-sb-key.pem'
sb = app.endswith(SB_SUFFIX)
if sb:
    app = app[:-len(SB_SUFFIX)]


def build_cmd(variant=None):
    update = f'west chester-update --variant "{variant}"' if variant else 'west chester-update'
    build = 'west build'
    if sb:
        # Kconfig string values need their own literal quotes (no spaces in path)
        build += f' -- -DSB_CONFIG_BOOT_SIGNATURE_KEY_FILE=\\"{SB_KEY}\\"'
    return f'rm -rf build && {update} && {build}'


def fw_name(name):
    return name + ' SB' if sb else name


if out not in ['path', 'cmd', 'name']:
    panic("Unknown out type supported: path, cmd, name")

fw_bundle = f'{PREFIX}{app}'
path = os.path.dirname(os.path.realpath(__file__))

name = app.split('.')[0]

if out == 'path':
    print(f'chester-sdk/chester/applications/{name}')
    sys.exit(0)

project_yml_path = os.path.join(path, '../applications', name , 'project.yaml')
if os.path.exists(project_yml_path):
    project = yaml.safe_load(open(project_yml_path, 'r'))
    if not project.get('variants', None):
        if out == 'cmd':
            print(build_cmd())
            sys.exit(0)
        if out == 'name':
            print(fw_name(project['project']['fw_name']))
            sys.exit(0)

    for variant in project['variants']:
        if variant['fw_bundle'] == fw_bundle:
            # print(json.dumps(variant, sort_keys=True))
            # sys.exit(0)
            if out == 'cmd':
                print(build_cmd(variant['name']))
                sys.exit(0)
            if out == 'name':
                print(fw_name(variant['fw_name']))
                sys.exit(0)

    panic(f'Variant not found: {fw_bundle}')
else:
    panic(f'Project not found: {name}')
