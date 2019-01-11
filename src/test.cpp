#include "gb2312ToUtf8.hpp"
#include "linkFix.hpp"
#include "numbering.hpp"
#include "search.hpp"
#include "story.hpp"

int debug = LOG_INFO;
// int debug = LOG_EXCEPTION;

void testFunctions() {
  SEPERATE("HLM test", " started ");
  testSearchTextIsOnlyPartOfOtherKeys();
  testLineNumber();
  testLinkOperation();
  testAttachmentOperations();
  testContainer();
  testConstructSubStory();
  testFindFirstInFiles();
  testRemovePersonalViewpoints();
  testConvertToUtf8();
}
