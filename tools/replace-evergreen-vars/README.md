`replace-evergreen-vars.py` is a utility to replace Variables on an Evergreen project.

Usage: `replace-evergreen-vars.py [-h] [--dry-run] replacements_file evergreen_project`

The `replacements_file` is a YAML file specifying old and new values:
```
replacements:
    - old: foo1
      new: bar1
    - old: foo2
      new: bar2
```

To run, install `requests` and `PyYAML` packages. Consider using a Python virtual environment to install dependencies:

```bash
python3 -m venv .venv
. ./.venv/bin/activate
pip install requests
pip install PyYAML
python3 replace-evergreen-vars.py --help
```
