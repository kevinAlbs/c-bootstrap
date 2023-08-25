"""
Replaces Variables on an Evergreen project.

replacements-file is a YAML file specifying Variable replacements of the form:

replacements:
    - old: foo1
      new: bar1
    - old: foo2
      new: bar2

Use '--help' for more information.
"""

import argparse
from dataclasses import dataclass
import json
import requests
import yaml
import pathlib
import unittest
import os


class EvgApi:
    def __init__(self):
        path: pathlib.Path = pathlib.Path().home() / ".evergreen.yml"
        with path.open() as file:
            evg_settings = yaml.load(file, Loader=yaml.FullLoader)

        self._api_server_host = "https://evergreen.mongodb.com/rest/v2"
        self._api_key = evg_settings["api_key"]
        self._api_user = evg_settings["user"]

    def project_get(self, project):
        resp: requests.Response = requests.get(
            f"{self._api_server_host}/projects/{project}", params={}, headers={'Api-User': self._api_user, 'Api-Key': self._api_key})
        if resp.status_code != 200:
            raise RuntimeError(
                "Failed to GET. Got status: {}. Got response: {}".format(resp.status_code, resp.text))
        return resp.json()

    def project_patch(self, project, payload):
        resp: requests.Response = requests.patch(
            f"{self._api_server_host}/projects/{project}",
            json=payload, params={}, headers={'Api-User': self._api_user, 'Api-Key': self._api_key})
        if resp.status_code != 200:
            raise RuntimeError(
                "Failed to PATCH. Got status: {}. Got response: {}".format(resp.status_code, resp.text))


@dataclass
class Replacement:
    old: str
    new: str


def do_replacements(project: str, repls: list[Replacement], dry_run=False):
    evg = EvgApi()
    vars_old: dict[str, str] = evg.project_get(project)["variables"]["vars"]
    vars_new: dict[str, str] = {}
    for repl in repls:
        # Find a match for the old value.
        matches = [kv_old for kv_old in vars_old.items() if kv_old[1]
                   == repl.old]
        if len(matches) != 1:
            raise RuntimeError(
                "Found {} matches for old value: {}. Expected 1. Not proceeding.".format(len(matches), repl.old))
        key = matches[0][0]
        vars_new[key] = repl.new

    payload = {
        "variables": {
            "vars": vars_new
        }
    }

    if dry_run:
        print("Going to apply the following variables:\n{}".format(
            json.dumps(payload, indent=4)))
        print("Doing dry run. Not proceeding")
        return

    evg.project_patch(project, payload)
    print("Project {} has been updated".format(project))


def main():
    parser = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('replacements_file', type=pathlib.Path,
                        help='File specifying variable replacements')
    parser.add_argument('evergreen_project', type=str,
                        help='Evergreen project to update. Example: `mongo-c-driver`')
    parser.add_argument('--dry-run', '-n', action="store_true",
                        help="Do not alter variables")
    args = parser.parse_args()

    replacements_yaml = yaml.load(
        args.replacements_file.open("r"), Loader=yaml.FullLoader)
    if "replacements" not in replacements_yaml:
        raise RuntimeError(
            "{} does not contain key 'replacements'. Run --help to see expected format".format(args.replacements_file))
    replacements: list[Replacement] = []
    for repl in replacements_yaml["replacements"]:
        replacements.append(Replacement(repl["old"], repl["new"]))

    do_replacements(args.evergreen_project, replacements, dry_run=args.dry_run)


class SelfTest(unittest.TestCase):

    def test_replace_ok(self):
        evg = EvgApi()
        project = "mongo-c-driver"

        # Add a test variable.
        payload = {
            "variables": {
                "vars": {
                    "SELFTEST_VAR": "unique-value-foo1",
                }
            }
        }
        evg.project_patch(project, payload)

        # Check that replacement succeeds.
        do_replacements(
            project, [Replacement("unique-value-foo1", "unique-value-foo2")])
        got = evg.project_get(project)
        self.assertEqual(got["variables"]["vars"]
                         ["SELFTEST_VAR"], "unique-value-foo2")

        # Remove test variable.
        payload = {
            "variables": {
                "vars_to_delete": ["SELFTEST_VAR"]
            }
        }
        evg.project_patch(project, payload)

    def test_replace_no_match(self):
        project = "mongo-c-driver"

        with self.assertRaises(RuntimeError) as ctx:
            do_replacements(project, [Replacement(
                "unique-value-does-not-match", "unique-value-foo2")])
        self.assertIn("Found 0 matches", str(ctx.exception))


if __name__ == "__main__":
    if "SELFTEST" in os.environ:
        unittest.main()
    else:
        main()
