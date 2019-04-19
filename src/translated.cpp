#include "contentTable.hpp"
#include "tools.hpp"

int main(int argc, char **argv) {
  // change this number to run different functions
  int num = 12;
  switch (num) {
  case 13:
    refreshBodyTexts(MAIN);
    refreshBodyTexts(ORIGINAL);
    refreshBodyTexts(JPM);
    break;
  case 14:
    refreshAttachmentBodyTexts();
    break;
  case 15:
    generateContentTableForMainHtmls();
    break;
  case 16:
    generateContentTableForOriginalHtmls();
    break;
  case 17:
    generateContentTableForJPMHtmls();
    break;
  case 18:
    generateContentTableForReferenceAttachments();
    break;
  case 19:
    generateContentTableForPersonalAttachments();
    break;
  case 20:
    findFirstInFiles();
    break;
  case 1:
    numberJPMHtmls(3, false);
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
    fixLinksFromAttachment(false);
    break;
  case 10:
    reConstructStory("mapping.txt", "story");
    break;
  case 11:
    testFunctions(5);
    break;
  case 12:
    tools(15);
    break;
    //   hidden paragraph headers, no force update
  case 7:
    numberOriginalHtmls(false, true);
    break;
  case 8:
    numberMainHtmls(false, true);
    break;
  case 9:
    numberAttachmentHtmls(false, true);
    break;
  default:
    FUNCTION_OUTPUT << "nothing gets executed." << endl;
  }
  return 0;
}
