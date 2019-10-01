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
  int num = 18;
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
    reConstructStory(R"(尤二姐)", "story2");
    //    reConstructStory(R"(小红遗帕)", "story", false);
    break;
  case 10:
    autoNumberingResultStory("story2", R"(尤二姐)");
    break;
  case 11:
    refreshBodyTexts(MAIN);
    refreshBodyTexts(ORIGINAL);
    refreshBodyTexts(JPM, JPM_MIN_CHAPTER_NUMBER, JPM_MAX_CHAPTER_NUMBER);
    break;
  case 12:
    refreshAttachmentBodyTexts();
    break;
  case 13:
    generateContentTableForMainHtmls();
    break;
  case 14:
    generateContentTableForOriginalHtmls();
    break;
  case 15:
    generateContentTableForJPMHtmls();
    break;
  case 16:
    generateContentTableForReferenceAttachments();
    break;
  case 17:
    generateContentTableForPersonalAttachments();
    break;
  case 18:
    generateContentTableForImages();
    break;
  case 19:
    generateContentTableForPoems();
    break;
  case 20:
    fixLinksFromAttachment(false);
    break;
  case 21:
    validateParaSizeForAutoNumberingJPMHtmls();
    break;
  case 22:
    fixLinksFromJPM(false);
    break;
  case 23:
    appendReverseLinksForMain();
    break;
  case 24:
    testFunctions(3);
    break;
  case 25:
    tools(15);
    break;
  case 26:
    renderingBodyText();
    break;
  case 27:
    removePersonalViewpoints();
    break;
  default:
    FUNCTION_OUTPUT << "nothing gets executed." << endl;
  }
  return 0;
}
