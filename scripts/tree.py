#!/usr/bin/env python3
"""Generate an HTML tree of all files
"""

import os
import sys
import warnings
from subprocess import check_output as check_cmd_output
from typing import Tuple

warnings.filterwarnings("error", category=Warning)

IGNORE_FILES: Tuple[str] = ("netlify.toml",)

INDEX_TEMPLATE: str = f"""
<!DOCTYPE html>
<html lang="en">
    <head>
        <meta charset="UTF-8">
        <meta http-equiv="X-UA-Compatible" content="IE=edge">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">

        <meta name="description" content="Ari-web file hosting index">
        <meta name="keywords" content="website webdev linux programming ari opensource free cdn file files file-hosting file_hosting git github">
        <meta name="robots" content="follow"/>

        <title>Ari::ari -> Files</title>

        <link rel="stylesheet" href="//cdn.jsdelivr.net/npm/hack-font@3/build/web/hack.min.css">

        <style>
            * {{ background-color: #262220; color: #f9f6e8; font-family: Hack, hack, monospace, sans, opensans, sans-serif; }}
            body {{ padding: 2rem; }}
            h1 {{ text-align: center; margin: 1em; }}
            li {{ margin: 0.5em; }}
            a {{ text-decoration: none; text-shadow: 0px 0px 6px white; }}
        </style>
    </head>

    <body>
        <h1><a href='https://ari-web.xyz/'>Ari-web</a> file hosting index</h1>

        <p align='center'>
            <i>Ignored files: {', '.join(IGNORE_FILES)} | Last built at {check_cmd_output('date').decode()} | <a href='/git'>source code</a></i>
        </p>
        <hr/>

        <div>
            %s
        </div>
    </body>
</html>
"""


def generate_tree(path: str, html: str = "") -> str:
    """Generate a tree in HTML of files in specified path

    Parameters
    ----------
    path (str): The path
    html (str): Starting HTML

    Returns
    -------
    str:The HTML tree
    """

    for file in os.listdir(path):
        if file.startswith(".") or file in IGNORE_FILES:
            continue

        rel = path + "/" + file

        if os.path.isdir(rel):
            html += f"<li>{file}/</li><ul>"
            html += generate_tree(rel)
            html += "</ul>"
        else:
            html += f"<li><a href='{rel}'>{file}</a></li>"

    return html


def main() -> int:
    """Entry/main function"""

    with open("index.html", "w", encoding="utf-8") as index:
        index.write(INDEX_TEMPLATE % (generate_tree(".")))

    return 0


if __name__ == "__main__":
    assert main.__annotations__.get("return") is int, "main() should return an integer"
    sys.exit(main())
