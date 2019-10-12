#include "contentTable.hpp"
#include "numberingAndLinkFixing.hpp"
#include "search.hpp"
#include "story.hpp"
#include "test.hpp"
#include "tools.hpp"

int debug = LOG_INFO;
// int debug = LOG_EXCEPTION;

int main(int argc, char **argv) {
  // change this number to run different functions
  int num = 24;
  switch (num) {
  case 1:
    //    search(R"(枕)", SEARCH_IN_MAIN, true);
    //    search(R"(邢夫人)", SEARCH_IN_ORIGINAL);
    //    search(R"(闲)", SEARCH_IN_JPM);
    search(R"(风露)");
    break;
  case 2:
    validateMainHtmls();
    break;
  case 3:
    validateAttachmentHtmls();
    break;
  case 4:
    numberJPMHtmls(false);
    break;
  case 5:
    numberOriginalHtmls(false);
    break;
  case 6:
    numberMainHtmls(false);
    break;
  case 7:
    numberAttachmentHtmls(false);
    break;
  case 8:
    fixLinksFromMain(false);
    break;
  case 9:
    renderingBodyText();
    break;
  case 10:
    appendReverseLinksForMain();
    break;
  case 11:
    reConstructStory(R"(尤二姐)", "story2");
    //    reConstructStory(R"(小红遗帕)", "story", false);
    break;
  case 12:
    autoNumberingResultStory("story2", R"(尤二姐)");
    break;
  case 13:
    refreshBodyTexts(MAIN);
    refreshBodyTexts(ORIGINAL);
    refreshBodyTexts(JPM, JPM_MIN_CHAPTER_NUMBER, JPM_MAX_CHAPTER_NUMBER);
    break;
  case 14:
    refreshAttachmentBodyTexts();
    break;
  case 15:
    generateContentTable(TABLE_TYPE::MAIN);
    generateContentTable(TABLE_TYPE::ATTACHMENT, ATTACHMENT_TYPE::REFERENCE);
    generateContentTable(TABLE_TYPE::ATTACHMENT, ATTACHMENT_TYPE::PERSONAL);
    break;
  case 16:
    fixLinksFromAttachment(false);
    break;
  case 17:
    fixLinksFromJPM(false);
    break;
  case 18:
    testFunctions(2);
    break;
  case 19:
    validateParaSizeForAutoNumberingJPMHtmls();
    break;
  case 20:
    tools(15);
    break;
  case 21:
    removePersonalViewpoints();
    break;
  default:
    FUNCTION_OUTPUT << "nothing gets executed." << endl;
  }
  return 0;
}
