#include "linkFix.hpp"
#include "numbering.hpp"
#include "test.hpp"

int main(int argc, char **argv) {
  // change this number to run different functions
  int num = 10;
  switch (num) {
  case 1:
    testFunctions();
    break;
  case 2:
    numberOriginalHtmls();
    break;
  case 3:
    numberMainHtmls();
    break;
  case 4:
    numberAttachmentHtmls();
    break;
  case 5:
    fixLinksFromMain();
    break;
  case 6:
    fixLinksFromAttachment();
    break;
    //   hidden paragraph headers
  case 7:
    numberOriginalHtmls(true);
    break;
  case 8:
    numberMainHtmls(true);
    break;
  case 9:
    numberAttachmentHtmls(true);
    break;
  case 10:
    generateContentIndexTableForAttachments();
    break;
  default:
    cout << "nothing gets executed." << endl;
  }
  return 0;
}
