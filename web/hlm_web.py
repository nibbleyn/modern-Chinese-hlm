#!/usr/bin/env python3
"""A web page to run the choices of modern-Chinese-hlm's main.

The choices and their parameters come from the program itself (`--list`), so this
page never falls out of step with main. A run is the program started with the
choice and the parameters as separate arguments, never through a shell, and its
output is streamed into the page as it comes. One run at a time: the choices
rewrite the htmls. utf8HTML can be browsed read only, to look at the results.

  python3 web/hlm_web.py                 # http://127.0.0.1:8080/
  python3 web/hlm_web.py --binary debug/modern-Chinese-hlm --workdir . --port 8080

Only the standard library is used. It listens on 127.0.0.1 unless --host says
otherwise: anyone who can reach the page can rewrite the htmls.
"""

import argparse
import html
import mimetypes
import os
import subprocess
import sys
import threading
import time
import urllib.parse
from http import HTTPStatus
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer

REPO = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DATA_DIR = "utf8HTML"

STYLE = """
body { font-family: sans-serif; margin: 1.5em; background: #f6f6f6; }
h1 { font-size: 1.4em; }
.choice { background: #fff; border: 1px solid #ccc; border-radius: 4px; margin: .4em 0; }
.choice summary { padding: .5em .8em; cursor: pointer; }
.choice form { padding: 0 .8em .8em; }
.choice label { display: block; margin: .3em 0; }
.choice input[type=text] { width: 24em; }
.num { display: inline-block; width: 2.2em; color: #666; }
pre { background: #111; color: #ddd; padding: 1em; white-space: pre-wrap; }
.busy { color: #a00; }
nav a { margin-right: 1em; }
"""


def page(title, body):
    return ("<!DOCTYPE html><html><head><meta charset='utf-8'>"
            f"<title>{html.escape(title)}</title><style>{STYLE}</style></head><body>"
            "<nav><a href='/'>choices</a>"
            f"<a href='/files/'>{DATA_DIR}</a></nav>{body}</body></html>")


class Front:
    def __init__(self, binary, workdir):
        self.binary = binary
        self.workdir = workdir
        self.lock = threading.Lock()
        self.running = None  # a description of the run in progress

    def choices(self):
        """[(num, title, [(name, default)])] as main lists them."""
        out = subprocess.run([self.binary, "--list"], cwd=self.workdir,
                             stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                             timeout=30, check=True).stdout.decode("utf-8", "replace")
        listed = []
        for line in out.splitlines():
            fields = line.split("\t")
            if len(fields) < 2 or not fields[0].isdigit():
                continue
            params = [tuple(f.split("=", 1)) if "=" in f else (f, "") for f in fields[2:]]
            listed.append((int(fields[0]), fields[1], params))
        return listed


class Handler(BaseHTTPRequestHandler):
    front = None  # set in main

    # ---- pages
    def do_GET(self):
        path = urllib.parse.urlparse(self.path).path
        if path == "/":
            return self.index()
        if path == "/files" or path.startswith("/files/"):
            return self.files(urllib.parse.unquote(path[len("/files"):]))
        self.send_error(HTTPStatus.NOT_FOUND)

    def do_POST(self):
        if urllib.parse.urlparse(self.path).path != "/run":
            return self.send_error(HTTPStatus.NOT_FOUND)
        length = int(self.headers.get("Content-Length", 0))
        form = urllib.parse.parse_qs(self.rfile.read(length).decode("utf-8"),
                                     keep_blank_values=True)
        self.run(form)

    def index(self):
        try:
            listed = self.front.choices()
        except (OSError, subprocess.SubprocessError) as e:
            return self.reply(page("modern-Chinese-hlm", "<h1>modern-Chinese-hlm</h1>"
                                   f"<p class='busy'>cannot run {html.escape(self.front.binary)} "
                                   f"--list: {html.escape(str(e))}</p>"))
        busy = self.front.running
        parts = ["<h1>modern-Chinese-hlm</h1>",
                 f"<p>program <code>{html.escape(self.front.binary)}</code>, "
                 f"run in <code>{html.escape(self.front.workdir)}</code></p>"]
        if busy:
            parts.append(f"<p class='busy'>running now: {html.escape(busy)}</p>")
        for num, title, params in listed:
            fields = "".join(
                f"<label>{html.escape(name)} "
                f"<input type='text' name='p{i}' value='{html.escape(default, quote=True)}'>"
                f"<input type='hidden' name='d{i}' value='{html.escape(default, quote=True)}'>"
                f"</label>"
                for i, (name, default) in enumerate(params))
            parts.append(
                f"<details class='choice'><summary><span class='num'>{num}</span>"
                f"{html.escape(title)}</summary>"
                f"<form method='post' action='/run'>"
                f"<input type='hidden' name='num' value='{num}'>{fields}"
                f"<button type='submit'>run</button></form></details>")
        self.reply(page("modern-Chinese-hlm", "".join(parts)))

    def run(self, form):
        num = form.get("num", [""])[0]
        if not num.isdigit():
            return self.send_error(HTTPStatus.BAD_REQUEST, "no choice")
        params, defaults = [], []
        i = 0
        while f"p{i}" in form:
            params.append(form[f"p{i}"][0])
            defaults.append(form.get(f"d{i}", [""])[0])
            i += 1
        # Left as they were, the parameters are not passed at all, so that a choice
        # does what it does without any: choice 11 then makes both of its stories.
        if params == defaults:
            params = []
        argv = [self.front.binary, num] + params

        if not self.front.lock.acquire(blocking=False):
            return self.reply(page("busy", f"<p class='busy'>another run is in progress: "
                                   f"{html.escape(self.front.running or '')}</p>"),
                              HTTPStatus.CONFLICT)
        try:
            shown = " ".join(argv[1:])
            self.front.running = shown
            self.send_response(HTTPStatus.OK)
            self.send_header("Content-Type", "text/html; charset=utf-8")
            self.end_headers()
            self.write(page("run " + shown, "")[:-len("</body></html>")])
            self.write(f"<h1>choice {html.escape(num)}</h1><p><code>"
                       f"{html.escape(' '.join(argv))}</code></p><pre>")
            start = time.monotonic()
            try:
                proc = subprocess.Popen(argv, cwd=self.front.workdir, stdout=subprocess.PIPE,
                                        stderr=subprocess.STDOUT)
                for line in proc.stdout:
                    self.write(html.escape(line.decode("utf-8", "replace")))
                code = proc.wait()
                self.write(f"</pre><p>exit {code} after {time.monotonic() - start:.1f} s</p>")
            except OSError as e:
                self.write(f"</pre><p class='busy'>cannot run: {html.escape(str(e))}</p>")
            self.write("<p><a href='/'>back to the choices</a></p></body></html>")
        finally:
            self.front.running = None
            self.front.lock.release()

    def files(self, rel):
        root = os.path.realpath(os.path.join(self.front.workdir, DATA_DIR))
        target = os.path.realpath(os.path.join(root, rel.lstrip("/")))
        # nothing outside utf8HTML, however the path is spelt
        if os.path.commonpath([root, target]) != root or not os.path.exists(target):
            return self.send_error(HTTPStatus.NOT_FOUND)
        if os.path.isdir(target):
            if not self.path.endswith("/"):
                self.send_response(HTTPStatus.MOVED_PERMANENTLY)
                self.send_header("Location", self.path + "/")
                return self.end_headers()
            entries = sorted(os.listdir(target))
            items = "".join(
                f"<li><a href='{urllib.parse.quote(e)}{'/' if os.path.isdir(os.path.join(target, e)) else ''}'>"
                f"{html.escape(e)}</a></li>" for e in entries)
            up = "" if target == root else "<li><a href='../'>..</a></li>"
            shown = os.path.relpath(target, os.path.dirname(root))
            return self.reply(page(shown, f"<h1>{html.escape(shown)}</h1><ul>{up}{items}</ul>"))
        kind = mimetypes.guess_type(target)[0] or "application/octet-stream"
        if kind.startswith("text/"):
            kind += "; charset=utf-8"
        with open(target, "rb") as f:
            data = f.read()
        self.send_response(HTTPStatus.OK)
        self.send_header("Content-Type", kind)
        self.send_header("Content-Length", str(len(data)))
        self.end_headers()
        self.wfile.write(data)

    # ---- helpers
    def reply(self, text, status=HTTPStatus.OK):
        data = text.encode("utf-8")
        self.send_response(status)
        self.send_header("Content-Type", "text/html; charset=utf-8")
        self.send_header("Content-Length", str(len(data)))
        self.end_headers()
        self.wfile.write(data)

    def write(self, text):
        # A browser that goes away stops the page, not the run: the run is still read
        # to its end, so the lock is not released while the program writes the htmls.
        if getattr(self, "gone", False):
            return
        try:
            self.wfile.write(text.encode("utf-8"))
            self.wfile.flush()
        except (BrokenPipeError, ConnectionResetError):
            self.gone = True


def main():
    parser = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    parser.add_argument("--binary", default=os.path.join(REPO, "debug", "modern-Chinese-hlm"),
                        help="the built program (default: debug/modern-Chinese-hlm)")
    parser.add_argument("--workdir", default=REPO,
                        help=f"where the program runs, the directory holding {DATA_DIR} "
                             "(default: the repository)")
    parser.add_argument("--host", default="127.0.0.1")
    parser.add_argument("--port", type=int, default=8080)
    args = parser.parse_args()

    binary = os.path.abspath(args.binary)
    if not os.access(binary, os.X_OK):
        sys.exit(f"{binary} is not an executable; build the project first, or pass --binary")
    workdir = os.path.abspath(args.workdir)
    if not os.path.isdir(os.path.join(workdir, DATA_DIR)):
        print(f"note: {workdir} has no {DATA_DIR}; the choices will not find their htmls",
              file=sys.stderr)

    Handler.front = Front(binary, workdir)
    server = ThreadingHTTPServer((args.host, args.port), Handler)
    print(f"serving http://{args.host}:{args.port}/  ({binary}, in {workdir})")
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        pass


if __name__ == "__main__":
    main()
