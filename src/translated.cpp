//#include "hlm.hpp"
#include "hlmLinkFixing.hpp"
#include "hlmNumbering.hpp"

int main(int argc, char **argv) {
//  testFunctions();

  //  cout << "refer to readme about how to use this tool." << endl;
  reformatTxtFilesForReader();
//  numberOriginalHtmls();
//  numberMainHtmls();
//  numberAttachmentHtmls();

  // hidden paragraph headers
//  numberOriginalHtmls(true);
//  numberMainHtmls(true);
//  numberAttachmentHtmls(true);

  fixLinksFromMain();
//  fixLinksFromAttachment();
  return 0;
}
