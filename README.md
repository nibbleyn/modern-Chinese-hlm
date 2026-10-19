# modern-Chinese-hlm

Tools for translating 《脂砚斋重评石头记》 (the first 80 chapters of 红楼梦) into modern
Chinese: a C++ program that numbers the paragraphs of the chapter HTMLs, fixes and
renders the links between them, searches them and rebuilds stories from them, and a
web page to run it from a browser.

- **backend**: `src/`, the C++ program; `main()` in `src/translated.cpp` picks one of
  its choices from the command line
- **frontend**: `web/hlm_web.py`, a small web server that lists those choices, runs
  them and shows their output

Both are built and run on Ubuntu. [design.md](design.md) describes how they work, and
lists the known defects.

## Prerequisites

```bash
sudo apt install g++ make libpoco-dev python3
```

The program links PocoFoundation, PocoJSON and PocoNet. The web page needs only the
Python 3 standard library (3.7 or newer).

## The run directory

Every path in the program is relative to the directory it runs in, and the program
creates none of them. Run it from the repository root, which has `container/`, with
the HTMLs placed in it as `utf8HTML/`:

| Directory | Holds |
| --- | --- |
| `container/` | page skeletons and reference texts, in the repository |
| `utf8HTML/src/` and its `original/`, `attachment/`, `JPM/`, `MDT/`, `XXJ/` | the pages as edited, the input |
| `utf8HTML/output/` with the same subdirectories | what the program writes |
| `bodyTexts/output/`, `bodyTexts/afterFix/` | scratch files of a run |
| `test-results/` | the output of the tests, choice 18 |

`utf8HTML` is not in this repository. Put it, or a link to it, in the repository root,
and make the scratch directories once:

```bash
cd ~/git/modern-Chinese-hlm
ln -s <where the HTMLs are>/utf8HTML utf8HTML     # or copy it here
mkdir -p bodyTexts/output bodyTexts/afterFix test-results
```

Each numbering or link fixing run first moves `utf8HTML/src` into a timestamped
`utf8HTML/src<milliseconds>/` and copies `utf8HTML/output` over it: output is the
working copy. Those backups pile up and can be removed once a run is checked.

## Build the backend

**With Eclipse CDT**, as the project is set up (`.project`, `.cproject`): import the
directory as an existing project and build the Debug configuration. The program is
`debug/modern-Chinese-hlm`.

**Without Eclipse**, the same thing from a shell, from the repository root:

```bash
mkdir -p debug
g++ -std=c++14 -g -O0 -o debug/modern-Chinese-hlm src/*.cpp \
    -lPocoFoundation -lPocoJSON -lPocoNet -pthread
```

`debug/` is ignored by git.

## Run the backend

```bash
cd ~/git/modern-Chinese-hlm                      # the run directory, see above
./debug/modern-Chinese-hlm --list                # every choice and its parameters
./debug/modern-Chinese-hlm <choice> [parameter...]
./debug/modern-Chinese-hlm                       # no choice: choice 0
```

Parameters are given in order. A parameter left out takes its default, and a choice
given no parameters at all does what it always did, so `11` alone still makes both of
its stories. An unknown choice, more parameters than it takes, or a parameter that is none of its values (`true`/`false`, `main`/`original`/`jpm`, a number) exits with 2.

| Choice | Does | Parameters (default) |
| --- | --- | --- |
| 0 | try code under development | |
| 1 | search a key | key (学名), target `main`/`original`/`jpm` (main), over specific object (false), output file |
| 2 | validate main htmls | |
| 3 | validate attachment htmls | |
| 4 | number XXJ, MDT and JPM htmls | |
| 5 | number original htmls | |
| 6 | number main htmls | |
| 7 | number attachment htmls | |
| 8 | fix links from main | force update link (true) |
| 9 | render body text | |
| 10 | append reverse links for main | |
| 11 | reconstruct a story | title (尤二姐), output file (the title plus GENERATED), kind `main`/`original`/`attachment`/`jpm` (main), auto numbering (true); none: 尤二姐 and 小红遗帕 |
| 12 | auto number a result story | story html file (story2), output html file (尤二姐) |
| 13 | refresh body texts of main, original and JPM | |
| 14 | refresh attachment body texts | |
| 15 | generate content tables | |
| 16 | fix links from attachments | force update link (true) |
| 17 | fix links from JPM | force update link (true) |
| 18 | regression test, then one specific test | specific test number (10) |
| 19 | validate paragraph sizes for auto numbering JPM htmls | |
| 20 | run a tool | tool number (10) |
| 21 | remove personal viewpoints | |
| 22 | fix GB2312 characters of XXJ | |

`--list` prints the same, one choice per line, tab separated: the number, the title,
then `name=default` for each parameter.

```bash
modern-Chinese-hlm 1 枕 main true          # search 枕 over the main text, per object
modern-Chinese-hlm 1 邢夫人 original        # search the original text
modern-Chinese-hlm 11 小红遗帕 story main false
modern-Chinese-hlm 8 false                 # fix the links from main, rewriting only those that changed
```

## Run the frontend

Build the backend first, then from the repository root:

```bash
python3 web/hlm_web.py                           # the run directory is the repository root
```

and open <http://127.0.0.1:8080/>.

| Option | Default |
| --- | --- |
| `--binary` | `debug/modern-Chinese-hlm` in this repository |
| `--workdir` | the repository root; the program runs there, so it has to hold what [the run directory](#the-run-directory) lists |
| `--host` | `127.0.0.1` |
| `--port` | `8080` |

The page asks the program for its choices (`--list`), so it always shows what
`main()` has:

- **Choices**: each is a box to open, with a field per parameter filled with its
  default. **run** starts the program with the choice and the fields as separate
  arguments, never through a shell. Left as they are, the fields are not passed at
  all, so the choice does what it does without parameters.
- **Output**: printed into the page as the program writes it, then its exit code and
  how long it took.
- **One run at a time**: the choices rewrite the HTMLs, so a second run is refused
  while one is going. Closing the page does not stop a run.
- **utf8HTML**: the link at the top browses the HTMLs read only, to look at what a
  run wrote. Nothing outside `utf8HTML` is served.

It listens on 127.0.0.1 only. Anyone who can reach the page can rewrite the HTMLs, so
give `--host 0.0.0.0` only on a network you trust.

## Troubleshooting

| Seen | Because |
| --- | --- |
| `... is not an executable; build the project first` | the backend is not built, or `--binary` points elsewhere |
| `note: ... has no ...`, or a red line on the page naming directories | the run directory lacks them, see [the run directory](#the-run-directory) |
| the page shows no choices | a program built before `--list` existed, which runs choice 0 instead: rebuild it |
| `cannot run ... --list` on the page | the program could not be started, or failed; the message says why |
| `another run is in progress` | wait for the run shown at the top of the page to finish |
