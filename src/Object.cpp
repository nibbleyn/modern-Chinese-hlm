#include "Object.hpp"

extern int debug;

string getNameOfObjectType(OBJECT_TYPE type) {
  if (type == OBJECT_TYPE::LINENUMBER)
    return "LINENUMBER";
  else if (type == OBJECT_TYPE::SPACE)
    return "SPACE";
  else if (type == OBJECT_TYPE::POEM)
    return "POEM";
  else if (type == OBJECT_TYPE::LINKFROMMAIN)
    return "LINKFROMMAIN";
  else if (type == OBJECT_TYPE::PERSONALCOMMENT)
    return "PERSONALCOMMENT";
  else if (type == OBJECT_TYPE::POEMTRANSLATION)
    return "POEMTRANSLATION";
  else if (type == OBJECT_TYPE::COMMENT)
    return "COMMENT";
  return "";
}

size_t Space::loadFirstFromContainedLine(const string &containedLine,
                                         size_t after) {
  m_bodyText = displaySpace;
  m_fullString = space;
  if (debug >= LOG_INFO) {
    METHOD_OUTPUT << "m_fullString: " << endl;
    METHOD_OUTPUT << m_fullString << endl;
  }
  return containedLine.find(space, after);
}

size_t Poem::loadFirstFromContainedLine(const string &containedLine,
                                        size_t after) {
  m_fullString = getWholeStringBetweenTags(containedLine, poemBeginChars,
                                           poemEndChars, after);
  if (debug >= LOG_INFO) {
    METHOD_OUTPUT << "m_fullString: " << endl;
    METHOD_OUTPUT << m_fullString << endl;
  }
  m_bodyText =
      getIncludedStringBetweenTags(m_fullString, endOfBeginTag, poemEndChars);
  return containedLine.find(poemBeginChars, after);
}

string Poem::getWholeString() {
  return poemBeginChars + unhiddenDisplayProperty + endOfBeginTag + m_bodyText +
         poemEndChars;
}

string Poem::getDisplayString() { return m_bodyText; }
size_t Poem::displaySize() { return getDisplayString().length(); }

void testSpace() {
  string line =
      R"(<a unhidden id="P11L1">11.1</a>&nbsp;&nbsp; <strong unhidden>杜鹃无语正黄昏，荷锄归去掩重门。青灯照壁人初睡，冷雨敲窗被未温。</strong>&nbsp;&nbsp;&nbsp;&nbsp;<samp unhidden font style="font-size: 13.5pt; font-family:楷体; color:#ff00ff">（像杜鹃啼血一样）我泣尽了血泪默默无语，只发现愁惨的黄昏正在降临，只好扛着花锄忍痛归去，一层层带上身后的门。闺中点起青冷的灯光，摇摇曳曳照射着四壁，我才要躺下，拉上尚是冰凉的被裘，却又听见轻寒的春雨敲打着窗棂，更增加了一层寒意。</samp><br>)";
  std::unique_ptr<Space> sp = std::make_unique<Space>();
  FUNCTION_OUTPUT << "first appearance offset: " << endl;
  FUNCTION_OUTPUT << sp->loadFirstFromContainedLine(line) << endl;
  FUNCTION_OUTPUT << "length: " << sp->length()
                  << " display size: " << sp->displaySize() << endl;
  FUNCTION_OUTPUT << "whole string: " << endl;
  FUNCTION_OUTPUT << sp->getWholeString() << endl;
  FUNCTION_OUTPUT << "display as:" << sp->getDisplayString() << "||" << endl;
}

void testPoem() {
  string poemStr =
      R"(<strong unhidden>杜鹃无语正黄昏，荷锄归去掩重门。青灯照壁人初睡，冷雨敲窗被未温。</strong>)";
  string line =
      R"(<a unhidden id="P11L1">11.1</a>&nbsp;&nbsp; <strong unhidden>杜鹃无语正黄昏，荷锄归去掩重门。青灯照壁人初睡，冷雨敲窗被未温。</strong>&nbsp;&nbsp;&nbsp;&nbsp;<samp unhidden font style="font-size: 13.5pt; font-family:楷体; color:#ff00ff">（像杜鹃啼血一样）我泣尽了血泪默默无语，只发现愁惨的黄昏正在降临，只好扛着花锄忍痛归去，一层层带上身后的门。闺中点起青冷的灯光，摇摇曳曳照射着四壁，我才要躺下，拉上尚是冰凉的被裘，却又听见轻寒的春雨敲打着窗棂，更增加了一层寒意。</samp><br>)";
  std::unique_ptr<Poem> poem1 = std::make_unique<Poem>();
  poem1->loadFirstFromContainedLine(poemStr);
  FUNCTION_OUTPUT << "length: " << poem1->length()
                  << " display size: " << poem1->displaySize() << endl;
  FUNCTION_OUTPUT << "whole string: " << endl;
  FUNCTION_OUTPUT << poem1->getWholeString() << endl;
  FUNCTION_OUTPUT << "display as:" << endl;
  FUNCTION_OUTPUT << poem1->getDisplayString() << "||" << endl;
  std::unique_ptr<Poem> poem2 = std::make_unique<Poem>();
  FUNCTION_OUTPUT << "first appearance offset: " << endl;
  FUNCTION_OUTPUT << poem2->loadFirstFromContainedLine(line) << endl;
  FUNCTION_OUTPUT << "length: " << poem2->length()
                  << " display size: " << poem2->displaySize() << endl;
  FUNCTION_OUTPUT << "whole string: " << endl;
  FUNCTION_OUTPUT << poem2->getWholeString() << endl;
  FUNCTION_OUTPUT << "display as:" << endl;
  FUNCTION_OUTPUT << poem2->getDisplayString() << "||" << endl;
}
