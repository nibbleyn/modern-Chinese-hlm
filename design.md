# modern-Chinese-hlm: design

How the program and its web page work, and what a review of the whole code found.
[README.md](README.md) says how to build and run them.

## What it does

The translation is kept as HTML pages: the 80 main chapters, the original text, the
attachments (notes and essays linked from a chapter), 金瓶梅 (JPM) and two more
reference works (MDT, XXJ). The program keeps those pages consistent while they are
edited by hand:

- **numbering**: every line gets a visible paragraph.line number, and the text is cut
  into paragraphs ("pages") with a header between them
- **link fixing**: a link carries the key text it points to; the program finds that
  key in the target page and rewrites the link to the paragraph and line that hold it
- **reverse links**: each target paragraph gets a list of who links to it
- **tables**: content tables, image and poem indexes, attachment tables
- **search** and **stories**: a key searched over all chapters, or ranges of
  paragraphs gathered into one new page

## The run directory

Every path is relative to the working directory, and the program creates none of
these except the timestamped backups. In practice it is the repository root:

| Path | What | From |
| --- | --- | --- |
| `container/` | page skeletons and reference texts, see [The containers](#the-containers) | the repository |
| `utf8HTML/src/` (+ `original/`, `attachment/`, `JPM/`, `MDT/`, `XXJ/`) | the pages as edited | not in the repository |
| `utf8HTML/output/` (same subdirectories) | what a run writes, and the statistics files | not in the repository |
| `bodyTexts/output/`, `bodyTexts/afterFix/` | scratch body texts of the run | to be created |
| `test-results/` | output of the tests, choice 18 | to be created |
| `gb2312HTML/` | GB2312 pages to convert (tools 6, choice 22) | only for conversion |
| `utf8HTML/src<milliseconds>/` | backup of src made by each run | made by the run |

## Files and names

| Kind | Page | Body text prefix | Chapters |
| --- | --- | --- | --- |
| main | `a0NN.htm` | `Main` | 1–80 |
| attachment | `b0NN_k.htm` (k-th attachment of chapter NN) | `Attach` | 1–80, k 1–99 |
| original | `c0NN.htm` | `Org` | 1–80 |
| JPM | `dNNN.htm` | `Jpm` | 1–100 |
| MDT | `e0NN.htm` | `Mdt` | 1–55 |
| XXJ | `f0NN.htm` | `Xxj` | 1–16 |

Chapter numbers have 2 digits, 3 for JPM (`getChapterNameByTargetKind`). The kind
strings `main`, `original`, `attachment`, `jpm`, `mdt`, `xxj` map to `FILE_TYPE`
(`fileUtil.hpp`).

## The page model

**Body text.** The part of a page from the line holding `id="top"` to the line holding
`id="bottom"`. `Container::dissembleFromHTM` copies it into a text file, one HTML line
per line; everything else works on that file; `assembleBackToHTM` puts it back between
the page's own header and footer.

**Lines.** A text line is a line-number anchor, two spaces, the text and `<br>`:

```html
<a unhidden id="P90L3">90.3</a>  text of the line<br>
```

`LineNumber` is (para, line), written `P<para>L<line>`. Paragraphs start at
`StartNumber` 90; a number with line 0 is a paragraph header; `bottom` reads as the
last paragraph. Before each text line comes a `<br>`-only line ("leading BR").

**Paragraph headers** (`CoupledParaHeader`) separate the paragraphs, built from
templates with placeholders:

```html
<hr color="#F0BEC0"><b unhidden> 第2段 </b><a unhidden id="P91" href="#P92">向下</a> … <a unhidden id="P89" href="#P90">向上</a><hr …>
```

First, middle and last headers differ (`id="top"`, middle ids, `id="bottom"`).
Hidden headers use `hidden` and `<br>` instead of `<hr>`. `GenericParaHeader` is the
same idea as table rows in the content tables.

**Objects in a line** (`Object` subclasses; `scanForTypes` finds them):

| Object | Markup | Type name |
| --- | --- | --- |
| line number | `<a unhidden id="P…L…">` | `LINENUMBER` |
| space | `&nbsp;` | `SPACE` |
| poem | `<strong>` | `POEM` |
| poem translation | `<samp>`, shown as 『白话文翻译：…』 | `POEMTRANSLATION` |
| comment | `（<cite>…</cite>）` | `COMMENT` |
| personal comment | `<u style="…wavy…">`, shown as 夹批：… | `PERSONALCOMMENT` |
| citation | `<sub>` | `CITATION` |
| link | `<a … href=…>` | `LINKFROMMAIN`, `LINKFROMATTACHMENT`, `LINKFROMJPM` |

Each has a display text; `hidden`/`unhidden` in the start tag decides whether it is
shown.

**Links** (`Link`, `CoupledLink`):

```html
<a unhidden title="KEY" href="original\c007.htm#P92L3">↑<sub hidden>第7回第3篇92.3节</sub>annotation</a>
```

- the **key** (`title`) is the text the link points at
- the **refer para** after `#` is where it points; link fixing finds the key there
- the **citation** in `<sub>` says it in words
- `readType` decides the target: `↓` reverse, `title="IMAGE"` image, `title="子段` sub
  paragraph, no `.htm` same page, then by name `a0`/`aindex` main, `b0` attachment,
  `c0` original, `d` JPM, `e0` MDT
- directories in links use `\` (`attachment\`, `original\`, `JPM\`, `pictures\`)
- a key not found becomes `KeyNotFound<key>` and is skipped until fixed by hand

## The classes

**Core** (`utf8StringUtil`, `fileUtil`, `libraryInterface`, `Object`, `lineNumber`,
`paraHeader`, `link`, `linkTemplate`):
- `utf8StringUtil` holds the shared constants, `using namespace std`, the log levels
  and macros (`FUNCTION_OUTPUT`, `METHOD_OUTPUT`, both to `cout`, gated by the global
  `debug`), UTF-8 substring and length, template filling (`replacePart`), file name
  helpers.
- `fileUtil` holds the paths and kind tables; `libraryInterface` the umbrella include
  and `buildFileSet`.

**Containers** (`container`, `linkSetContainer`, `coupledBodyTextContainer`,
`formatedBodyTextContainer`):
- `Container` cuts a page into its body text and puts it back.
- `CoupledBodyTextContainer` knows the kinds and their directories, backs up src
  (`backupAndOverwriteInputHtmlFiles`) and fixes the return link of attachments.
- `LinkSetContainer` → `ListContainer` / `TableContainer` build new pages (search
  results, stories, tables) from a set of links.
- `FormatedBodyTextContainer` numbers a page that is not a chapter (a story) through
  `container/4.txt`.

**Body text** (`coupledBodyText*`, `coupledBodyTextWithLink*`, `numberingStat`):
- `CoupledBodyText` reads `bodyTexts/output/<prefix><file>.txt` and writes
  `bodyTexts/afterFix/…`; it classifies lines, numbers them (guided), searches keys,
  fetches ranges.
- `CoupledBodyTextWithLink` adds auto numbering, link fixing, rendering, reverse
  links, removing personal comments, and the static statistics tables.

**Links** (`coupledLink*`): `CoupledLink` is a link inside a body text that knows how
to find its key; `LinkFromMain`, `LinkFromAttachment`, `LinkFromJPM` know their
directories and prefixes. `coupledLinkFixingStat` writes and reads the link
statistics.

**Operations** (`numberingAndLinkFixing`, `attachmentTable`, `contentTable`, `search`,
`story`, `tools`, `gb2312ToUtf8`): the functions `main` calls.

## The run of one operation: Commander

Every numbering and link fixing choice is a `Commander` (`numberingAndLinkFixing.cpp`)
run over a range of files (`NonAttachmentCommander` over chapters,
`AttachmentCommander` over chapter × attachment):

1. **back up and overwrite src**: the src pages move to `utf8HTML/src<ms>/`, and the
   output pages are copied over src. *Output is the working copy*: what the last run
   wrote is what this run reads.
2. set up the statistics, dissemble every page into `bodyTexts/output/`, clear
   `bodyTexts/afterFix/`
3. run the command on each body text, `output` → `afterFix`
4. copy `afterFix` back to `output`, assemble the pages into `utf8HTML/output/`
5. write the statistics and tables the command produces

## Numbering

**Validation** (`validateFormatForNumbering`): every text line after the first header
has a leading BR. A violation stops the program (`exit(1)`).

**Guided numbering** keeps the paragraph breaks already in the page (plus one after
each image group) and renumbers the lines within them.

**Auto numbering**, the default:
1. `getLinesofReferencePage` reads how many characters fit on a displayed line
   (`container/referLines.txt`) and how many lines on a page (`container/referPage.txt`)
2. `scanByRenderingLines` renders each line and estimates its displayed lines; an image
   group counts as a page less one
3. `calculateParaHeaderPositions` starts a new paragraph whenever the next line would
   overflow the page
4. `paraGeneratedNumbering` writes the lines, pads each paragraph with `<br>` to a full
   page, and writes the headers
5. `doStatisticsByScanningLines` records every line: displayed lines, image, object
   types

## Link fixing

`fixLinksFromFile` goes over the numbered lines; for each `<a>…</a>` it builds a
`LinkFrom{Main,JPM,Attachment}`:
- links to images and to the same page are refixed
- links to other pages are refixed when that page is in the reference set: the key is
  searched in the target's body text (`findKey`); the first line holding it becomes
  the refer para and the citation
- a link from main to main gets a link to the original next to it
- the line is rewritten when forced or when the link changed

`LinkFromMain::logLink` records every link while fixing:

| File in `utf8HTML/output/` | Holds |
| --- | --- |
| `LinksFromMain.txt` | per target paragraph, its key and every link to it |
| `KeysOfLinksFromMain.txt` | per target, its keys |
| `ImageLinksFromMain.txt` | the image links per chapter |
| `LinksFromAttachment.txt` and companions | the same for attachments |
| `RefAttachments.txt` | `from:<P.L> name:<b0NN_k> title:<t> type:<0 reference, 1 personal>` |

`RefAttachments.txt` is read from src and written to output; the type is set by hand
in output and the next run copies output over src.

**Reverse links** (choice 10) read `LinksFromMain.txt` back and add 被引用 links with ↓
at each target paragraph, so they need a choice 8 run first.

## Rendering, search, stories, tables

- **rendering** (`getDisplayString`) walks the objects of a line in order and joins
  their display texts, hidden ones left out. It is what auto numbering measures, what
  search compares and what choice 9 writes.
- **search** (`search.cpp`) runs `findKey` over the body texts of main, original or
  JPM, skipping keys found only in link titles and, in main, in personal comments.
  Each hit becomes a link with its citation in a `TableContainer`,
  `utf8HTML/output/<key or output file>.htm`.
- **stories** (`story.cpp`): `reConstructStory(title)` reads the ranges
  `utf8HTML/output/<title>.txt`, copies those lines from the body texts into a
  `ListContainer`, and auto numbers the result into `utf8HTML/output/<title>.htm`.
- **content tables** (`contentTable.cpp`) are `TableContainer`s in `utf8HTML/output/`:
  `aindex` (main, original and translated titles), `cindex`, `dindex`, `eindex`,
  `findex`, `picIndex`, `poemIndex`, `bindex1`/`bindex2` (reference and personal
  attachments).
- **tools** (`tools(n)`): header and footer fixes from template pages (1–4, 9, 10),
  repair of unpaired personal comments in JPM (5), GB2312 to UTF-8 conversion (6, by
  `iconv`), a tag pair fix (8).

## The containers

`container/` holds what the program builds pages from:
- `container1/2/3.htm`: page skeletons with `XXX` (title) and `YYY` (display title)
- `N.txt`, `3front.txt`, `3back.txt`: the body texts of those pages
- `referLines.txt`, `referPage.txt`: the reference line and page for auto numbering
- `toCheck.txt`: input of a debug helper

## main and its choices

`src/translated.cpp` holds the choices as a table: number, title, parameters with
defaults, and what to run. `modern-Chinese-hlm <choice> [parameter...]` runs one,
`--list` prints them. Given no parameters a choice runs what `main` used to have hard
coded; a parameter that is none of its values (`true`/`false`, `main`/`original`/`jpm`,
a number) is refused with exit 2. The table is in [README.md](README.md#run-the-backend).

The choices depend on one another:
- number (4–7) before fixing links (8, 16, 17): links point at line numbers
- fix links from main (8) before reverse links (10): 10 reads `LinksFromMain.txt`
- fix links (8) before content tables (15) and stories (11): they use the statistics
- every Commander choice backs up src and overwrites it with output

## The web page

`web/hlm_web.py` uses the Python standard library only:
- `GET /` runs `modern-Chinese-hlm --list` and builds one form per choice, a field per
  parameter with its default
- `POST /run` starts the program with the choice and the fields as separate arguments
  (no shell), in the run directory, and streams its output; fields left at their
  defaults are not passed, so the choice behaves as with no parameters
- one run at a time, behind a lock; a browser that goes away does not free the lock
  until the program has ended
- `GET /files/…` browses `utf8HTML` read only; paths that resolve outside it are refused
- it listens on 127.0.0.1 unless told otherwise, and names the run directories that
  are missing

## Tests

`testSpecificFunction(n)` writes test n's output to `test-results/<n>.txt`;
`regressionTest()` runs tests 1–9. Nothing compares the results: they are checked by
hand against `expected-test-results/1.txt`…`9.txt` (`diff -r test-results
expected-test-results`). Test 10 (numbering statistics of chapter 72) has no expected
result.

| Test | Checks |
| --- | --- |
| 1 | keys found only in comments or link titles |
| 2 | line number links and paragraph headers, built and read back |
| 3 | links from main and attachments: parse, fix, render |
| 4 | the attachment list of a chapter |
| 5, 6 | list and table containers assembled into pages |
| 7, 8 | poem and space objects |
| 9 | display strings of real lines against expected strings |

`testCode.cpp` (choice 0, `tryCode`) is scratch code. Nearly all of its 3162 lines are
experiments unrelated to the translation (hex helpers, smart pointer and thread demos,
SNMP and IP parsing, RSU camera and radio channel code, a bus state machine); only a
few lines run, and most of `tryCode` is commented out. It also pulls Poco Net and
NetSSL headers into the build.

## Review: known defects

From a review of the whole code. **Confirmed** ones were checked in the code; the
others were found by reading it and are worth confirming when they are fixed. Paths
are under `src/`.

### Fixed with this review

| Where | What was wrong |
| --- | --- |
| `numberingAndLinkFixing.cpp` `fixLinksFrom{Main,JPM,Attachment}` | the `forceUpdateLink` argument was ignored, the commander always forced; it is passed on now, and main's default is `true`, as the runs always behaved |
| `translated.cpp` | a wrong search target, true/false value or number fell back to a default silently; it is refused now |
| `translated.cpp` choice 11 | a title given alone still wrote `story2`; the output now defaults to the title |
| `coupledBodyTextWithLink_autoNumbering.cpp` `paraGeneratedNumbering` | a line taller than a page left `totalLinesAbove` above the page size; the `size_t` difference wrapped and the padding wrote `<br>` without end, filling the disk. A paragraph is now padded only up to the page |
| `coupledBodyTextWithLink_autoNumbering.cpp` `getLinesofReferencePage` | a missing `container/referPage.txt` made the page 0 lines, so an image group wrapped to `SIZE_MAX` lines; an empty `referLines.txt` divided by zero, and an average line of 1 character never ended splitting. Both files are checked now, and numbering stops with a message, as it does on a malformed page |
| `numberingStat.cpp` `doStatisticsByScanningLines` | the input was not checked to have opened, and `while (!infile.eof())` never ended on a missing file; such a file is skipped now |
| `numberingAndLinkFixing.cpp` `refreshBodyTexts`, `refreshAttachmentBodyTexts` | the backup was made before the kind was set, so choices 13 and 14 backed up and overwrote the main pages whatever the kind, and choice 13 threw at its JPM step; the kind is set first now |
| `linkSetContainer.cpp` `TableContainer` | `*(start - 1)` read before the start of the header list at the first header, undefined behaviour in every content table (choice 15); the first header counts the entries above it |
| `test.cpp` tests 5 and 6 | the page they echo was read with `while (!eof())`, which never ended when it did not open, so choice 18 hung and kept the web page locked; they report the file and return now |

### High

None open.

### Medium

| Where | What happens |
| --- | --- |
| `coupledBodyTextContainer.cpp:94-99` | the backup moves a src page away before copying output over it; when the output page does not exist `copyTo` throws and that src page is only in the backup |
| `container.cpp:54-131` | the output page is truncated before the marks are checked; a failed assembly leaves a partial page that the next run copies over src |
| `coupledBodyTextWithLink_rendering.cpp:392-423` | removing personal comments (choice 21) leaves `<a</a>` fragments, and an unclosed `<u` throws `out_of_range` |
| `coupledBodyTextContainer.cpp:106-158` | the return link fix can rewrite an unrelated link, or insert the para at the start of a link |
| `numberingAndLinkFixing.cpp:9-11, 352-357` | choice 19 validates nothing: auto numbering stays disabled, and JPM 81–100 would be skipped |
| `coupledLinkFromJpm.cpp:20, 53` | JPM links to attachments get the JPM prefix (`d0NN_k.htm`) and the wrong body text prefix; MDT targets are not handled |
| `coupledLinkFixingStat.cpp:110` | reading `ImageLinksFromMain.txt` finds the count's digits in the chapter field and shifts every image link of that chapter |
| `fileUtil.cpp:70-97` | MDT and XXJ map to the main statistics file and main page names: numbering them (choice 4) overwrites `LinesOfMain.txt` |
| `link.cpp:11-27` | there is no XXJ link type, so links to `XXJ\f0NN.htm` read as same page; any name containing `d` not caught before reads as JPM |
| `utf8StringUtil.cpp:44-54` | `replacePart` rescans from the start; a value containing its own placeholder (e.g. a comment containing `XX`) never ends |
| `utf8StringUtil.cpp:156-163`, `paraHeader.cpp:23-40`, `lineNumber.cpp:25-41` | `stoi` on text that is not a number (a stray file in an attachment directory, a header without an id) throws and ends the run |
| `search.cpp:96-98` | the search key becomes the output file name: a key with `/` or `..` writes outside `utf8HTML/output` |
| everywhere `exit(1)` is called on a format error | a batch stops in the middle, after src was backed up and overwritten, with statistics half written |

### Low

- `gb2312ToUtf8.cpp:21-26`: the `iconv` processes are never waited for, leaving one zombie per file; failures go unnoticed.
- `attachmentTable.cpp:81`: reading `RefAttachments.txt` stops at the first empty line; the next save drops everything after it, hand set types included.
- `coupledLink.cpp:25-40`, `coupledBodyTextWithLink_linkFixing.cpp:32, 137`, `coupledBodyText_keySearching.cpp:19-46`: unclosed `<a`, `<u` or `title="` make `npos` arithmetic wrap: wrong replacements, endless loops or `out_of_range`.
- `tools.cpp:99, 132`: unchecked `find` results (`aindex`, `unsigned int` positions) throw on a page already fixed.
- `coupledLinkFixingStat.cpp:301`: `LinksFromAttachment.txt` names main targets `b…htm`.
- `lineNumber.cpp:96-103`, `link.cpp:156-158`: with several anchors in a line, the first form found, not the first anchor, is taken.
- `coupledBodyTextContainer.cpp:80-85`: the backup's `info.txt` is opened before its directory exists and is never written.
- `test.cpp:126-141, 673-705`: `markDifference` with `begin > 0` marks the wrong place; test 10 leaves an empty result file; a missing `test-results/` loses all output silently.
- `testCode.cpp:2581-2585`: `uint8_t bytes[] = {0x04, array[0], …}` narrows in braces, an error or warning depending on the compiler.
- headers rely on transitive includes (`<map>`, `<vector>`, `<memory>`, `<typeinfo>`).

### Clean up

- `testCode.cpp` could shrink to an empty `tryCode()`: nothing else uses it, and its
  includes are what pull Poco NetSSL into the build.
- `while (!stream.eof()) getline(...)` appears in many readers; `while (getline(...))`
  ends on a failed or missing file and does not add a blank last line.
