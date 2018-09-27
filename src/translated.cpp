#include "hlm.hpp"

int main(int argc, char **argv) {
  testFunctions();

  cout << "refer to readme about how to use this tool." << endl;

//  numberOriginalHtmls();
//  numberMainHtmls();
//  numberAttachmentHtmls();

  fixLinksFromMain();
//  fixLinksFromAttachment();
  return 0;
}
