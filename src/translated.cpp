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
  int num = 6;
  switch (num) {
  case 1:
    numberJPMHtmls(false);
    break;
  case 2:
    numberOriginalHtmls(false);
    break;
  case 3:
    numberMainHtmls(false);
    break;
  case 4:
    numberAttachmentHtmls(false);
    break;
  case 5:
    fixLinksFromMain(false);
    break;
  case 6:
    //    reConstructStory(R"(小红遗帕)", "story");
    reConstructStory(R"(尤二姐)", "story2");
    break;
  case 7:
    refreshBodyTexts(MAIN);
    refreshBodyTexts(ORIGINAL);
    refreshBodyTexts(JPM, JPM_MIN_CHAPTER_NUMBER, JPM_MAX_CHAPTER_NUMBER);
    break;
  case 8:
    refreshAttachmentBodyTexts();
    break;
  case 9:
    generateContentTableForMainHtmls();
    break;
  case 10:
    generateContentTableForOriginalHtmls();
    break;
  case 11:
    generateContentTableForJPMHtmls();
    break;
  case 12:
    generateContentTableForReferenceAttachments();
    break;
  case 13:
    generateContentTableForPersonalAttachments();
    break;
  case 14:
    //    search(R"(邢夫人)");
    //    search(R"(邢夫人)", SEARCH_IN_ORIGINAL);
    search(R"(闲)", SEARCH_IN_JPM);
    break;
  case 15:
    fixLinksFromAttachment(false);
    break;
  case 16:
    testFunctions(4);
    break;
  case 17:
    tools(15);
    break;
  case 18:
    validateMainHtmls();
    break;
  case 19:
    validateAttachmentHtmls();
    break;
  case 20:
    validateParaSizeForAutoNumberingJPMHtmls();
    break;
  case 21:
    fixLinksFromJPM(false);
    break;
  default:
    FUNCTION_OUTPUT << "nothing gets executed." << endl;
  }
  return 0;
}
