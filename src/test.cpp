#include "Object.hpp"
#include "gb2312ToUtf8.hpp"
#include "linkFix.hpp"
#include "numbering.hpp"
#include "search.hpp"
#include "story.hpp"

int debug = LOG_INFO;
// int debug = LOG_EXCEPTION;

void testFunctions(int num) {
  SEPERATE("HLM test", " started ");
  switch (num) {
  case 1:
    testSearchTextIsOnlyPartOfOtherKeys();
    break;
  case 2:
    testLineNumber();
    break;
  case 3:
    testLinkOperation();
    break;
  case 4:
    testAttachmentOperations();
    break;
  case 5:
    testContainer(2);
    break;
  case 6:
    testConstructSubStory();
    break;
  case 7:
    testFindFirstInFiles();
    break;
  case 8:
    testPoem();
    break;
  case 9:
    testSpace();
    break;
  case 10:
    testImageReference();
    break;
  case 11:
    testMixedObjects();
    break;
  default:
    cout << "no test executed." << endl;
  }
}
