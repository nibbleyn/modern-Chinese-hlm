#include "test.hpp"

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
  case 7:
    testImage();
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
