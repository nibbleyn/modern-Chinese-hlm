#include "tools.hpp"

int main(int argc, char **argv) {
  // change this number to run different functions
  int num = 11;
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
    fixLinksFromAttachment(false);
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
  case 11:
    testFunctions(1);
    break;
  case 12:
    tools(16);
    break;
  default:
    FUNCTION_OUTPUT << "nothing gets executed." << endl;
  }
  return 0;
}
