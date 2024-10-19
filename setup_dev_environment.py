#!/usr/bin/python

import subprocess

def setup():
    create_venv = ["python3 -m venv venv"]
    install_requirements = ["venv/bin/python -m pip --require-virtualenv install -r requirements.txt"]
    install_node_js = ["npm ci"]
    setup_precommit = ["venv/bin/pre-commit install"]
    def run_command(command):
        result = subprocess.run(command, shell=True, check=True)
        result.check_returncode()

    for command in (create_venv,
                    install_requirements,
                    install_node_js,
                    setup_precommit
                    ):
        run_command(command)


if __name__ == "__main__":
    setup()
