#include "linkFix.hpp"
#include "numbering.hpp"
#include "test.hpp"
#include "tools.hpp"

int main(int argc, char **argv) {
  // change this number to run different functions
  //  int num = 1;
  //	  int num = 2;
  //  int num = 3;
  //	  int num = 4;
  //  int num = 5;
  int num = 12;
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
    fixLinksFromMain();
    break;
  case 6:
    fixLinksFromAttachment();
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
  case 10:
    generateContentIndexTableForAttachments();
    break;
  case 11:
    testFunctions(14);
    break;
  case 12:
    tools(2);
    break;
  default:
    cout << "nothing gets executed." << endl;
  }
  return 0;
}
