#include "contentTable.hpp"
#include "numberingAndLinkFixing.hpp"
#include "search.hpp"
#include "story.hpp"
#include "test.hpp"
#include "tools.hpp"
#include "testCode.hpp"

#include <cstdlib>
#include <functional>
#include <iostream>

int debug = LOG_INFO;
// int debug = LOG_EXCEPTION;

namespace {

using Args = std::vector<std::string>;

struct Param {
  const char *name;
  const char *defaultValue;
};

struct Choice;
using Run = std::function<void(const Choice &, const Args &)>;

struct Choice {
  int num;
  const char *title;
  std::vector<Param> params;
  Run run;

  // the i-th parameter given on the command line, else its default
  std::string param(const Args &args, size_t i) const {
    return i < args.size() ? args[i] : params.at(i).defaultValue;
  }
};

bool isTrue(const std::string &value) {
  return value == "1" || value == "true" || value == "yes";
}

int searchTarget(const std::string &target) {
  if (target == "original")
    return SEARCH_IN_ORIGINAL;
  if (target == "jpm")
    return SEARCH_IN_JPM;
  return SEARCH_IN_MAIN;
}

int number(const std::string &value) { return std::atoi(value.c_str()); }

// a choice without parameters
Choice plain(int num, const char *title, std::function<void()> f) {
  return {num, title, {}, [f](const Choice &, const Args &) { f(); }};
}

// What main can do. Given no parameters, each choice runs what it always ran.
const std::vector<Choice> &choices() {
  static const std::vector<Choice> list{
      plain(0, "try code under development", [] { tryCode(); }),
      {1,
       "search a key",
       {{"key", R"(学名)"},
        {"target (main, original, jpm)", "main"},
        {"over specific object (true/false)", "false"},
        {"output file", ""}},
       [](const Choice &c, const Args &a) {
         search(c.param(a, 0), searchTarget(c.param(a, 1)),
                isTrue(c.param(a, 2)), c.param(a, 3));
       }},
      plain(2, "validate main htmls", [] { validateMainHtmls(); }),
      plain(3, "validate attachment htmls", [] { validateAttachmentHtmls(); }),
      plain(4, "number XXJ, MDT and JPM htmls",
            [] {
              numberXXJHtmls(false);
              numberMDTHtmls(false);
              numberJPMHtmls(false);
            }),
      plain(5, "number original htmls", [] { numberOriginalHtmls(false); }),
      plain(6, "number main htmls", [] { numberMainHtmls(false); }),
      plain(7, "number attachment htmls", [] { numberAttachmentHtmls(false); }),
      {8,
       "fix links from main",
       {{"force update link (true/false)", "false"}},
       [](const Choice &c, const Args &a) {
         fixLinksFromMain(isTrue(c.param(a, 0)));
       }},
      plain(9, "render body text", [] { renderingBodyText(); }),
      plain(10, "append reverse links for main",
            [] { appendReverseLinksForMain(); }),
      {11,
       "reconstruct a story",
       {{"title", R"(尤二姐)"},
        {"output file", "story2"},
        {"kind (main, original, attachment, jpm)", "main"},
        {"auto numbering (true/false)", "true"}},
       [](const Choice &c, const Args &a) {
         if (a.empty()) {
           // the two stories this choice has always made
           reConstructStory(R"(尤二姐)", "story2");
           reConstructStory(R"(小红遗帕)", "story", MAIN, false);
           return;
         }
         reConstructStory(c.param(a, 0), c.param(a, 1), c.param(a, 2),
                          isTrue(c.param(a, 3)));
       }},
      {12,
       "auto number a result story",
       {{"story html file", "story2"}, {"output html file", R"(尤二姐)"}},
       [](const Choice &c, const Args &a) {
         autoNumberingResultStory(c.param(a, 0), c.param(a, 1));
       }},
      plain(13, "refresh body texts of main, original and JPM",
            [] {
              refreshBodyTexts(MAIN);
              refreshBodyTexts(ORIGINAL);
              refreshBodyTexts(JPM, JPM_MIN_CHAPTER_NUMBER,
                               JPM_MAX_CHAPTER_NUMBER);
            }),
      plain(14, "refresh attachment body texts",
            [] { refreshAttachmentBodyTexts(); }),
      plain(15, "generate content tables",
            [] {
              generateContentTable(TABLE_TYPE::XXJ);
              generateContentTable(TABLE_TYPE::MDT);
              generateContentTable(TABLE_TYPE::MAIN);
              generateContentTable(TABLE_TYPE::ATTACHMENT,
                                   ATTACHMENT_TYPE::REFERENCE);
              generateContentTable(TABLE_TYPE::ATTACHMENT,
                                   ATTACHMENT_TYPE::PERSONAL);
            }),
      {16,
       "fix links from attachments",
       {{"force update link (true/false)", "false"}},
       [](const Choice &c, const Args &a) {
         fixLinksFromAttachment(isTrue(c.param(a, 0)));
       }},
      {17,
       "fix links from JPM",
       {{"force update link (true/false)", "false"}},
       [](const Choice &c, const Args &a) {
         fixLinksFromJPM(isTrue(c.param(a, 0)));
       }},
      {18,
       "regression test, then one specific test",
       {{"specific test number", "10"}},
       [](const Choice &c, const Args &a) {
         regressionTest();
         testSpecificFunction(number(c.param(a, 0)));
       }},
      plain(19, "validate paragraph sizes for auto numbering JPM htmls",
            [] { validateParaSizeForAutoNumberingJPMHtmls(); }),
      {20,
       "run a tool",
       {{"tool number", "10"}},
       [](const Choice &c, const Args &a) { tools(number(c.param(a, 0))); }},
      plain(21, "remove personal viewpoints", [] { removePersonalViewpoints(); }),
      plain(22, "fix GB2312 characters of XXJ", [] { gb2312FixXXJ(); }),
  };
  return list;
}

// one line per choice, tab separated: number, title, then name=default for each
// parameter; what the web front builds its forms from
void listChoices() {
  for (const auto &choice : choices()) {
    std::cout << choice.num << '\t' << choice.title;
    for (const auto &p : choice.params)
      std::cout << '\t' << p.name << '=' << p.defaultValue;
    std::cout << '\n';
  }
}

void usage(const char *program) {
  std::cout << "usage: " << program << " [choice [parameter...]]\n"
            << "       " << program << " --list\n"
            << "without a choice, choice 0 runs\n";
}

} // namespace

int main(int argc, char **argv) {
  Args args(argv + 1, argv + argc);
  if (!args.empty() && args[0] == "--list") {
    listChoices();
    return 0;
  }
  if (!args.empty() && (args[0] == "--help" || args[0] == "-h")) {
    usage(argv[0]);
    return 0;
  }

  int num = 0;
  if (!args.empty()) {
    char *end = nullptr;
    num = static_cast<int>(std::strtol(args[0].c_str(), &end, 10));
    if (end == args[0].c_str() || *end != '\0') {
      usage(argv[0]);
      return 2;
    }
    args.erase(args.begin());
  }

  for (const auto &choice : choices()) {
    if (choice.num != num)
      continue;
    if (args.size() > choice.params.size()) {
      std::cout << "choice " << num << " takes " << choice.params.size()
                << " parameter(s), got " << args.size() << std::endl;
      return 2;
    }
    choice.run(choice, args);
    return 0;
  }
  FUNCTION_OUTPUT << "nothing gets executed." << endl;
  return 2;
}
