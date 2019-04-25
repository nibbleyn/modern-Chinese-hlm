#include "contentTable.hpp"
#include "tools.hpp"

int main(int argc, char **argv) {
  // change this number to run different functions
  int num = 16;
  switch (num) {
  case 1:
    refreshBodyTexts(MAIN);
    refreshBodyTexts(ORIGINAL);
    refreshBodyTexts(JPM);
    break;
  case 2:
    refreshAttachmentBodyTexts();
    break;
  case 3:
    generateContentTableForMainHtmls();
    break;
  case 4:
    generateContentTableForOriginalHtmls();
    break;
  case 5:
    generateContentTableForJPMHtmls();
    break;
  case 6:
    generateContentTableForReferenceAttachments();
    break;
  case 7:
    generateContentTableForPersonalAttachments();
    break;
  case 8:
    findFirstInFiles();
    break;
  case 9:
    numberJPMHtmls(3, false);
    break;
  case 10:
    numberOriginalHtmls(false);
    break;
  case 11:
    numberMainHtmls(false);
    break;
  case 12:
    numberAttachmentHtmls(false);
    break;
  case 13:
    fixLinksFromMain(false);
    break;
  case 14:
    fixLinksFromAttachment(false);
    break;
  case 15:
    reConstructStory("mapping.txt", "story");
    break;
  case 16:
    testFunctions(11);
    break;
  case 17:
    tools(15);
    break;
    //   hidden paragraph headers, no force update
  case 18:
    numberOriginalHtmls(false, true);
    break;
  case 19:
    numberMainHtmls(false, true);
    break;
  case 20:
    numberAttachmentHtmls(false, true);
    break;
  default:
    FUNCTION_OUTPUT << "nothing gets executed." << endl;
  }
  return 0;
}
