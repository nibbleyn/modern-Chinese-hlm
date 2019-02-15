#include "Object.hpp"

size_t Space::loadFirstFromContainedLine(const string &containedLine) {
  m_bodyText = displaySpace;
  return containedLine.find(space);
}

Poem::Poem(const string &poemString) {
  loadFirstFromContainedLine(poemString);
  cout << m_bodyText << endl;
}

size_t Poem::loadFirstFromContainedLine(const string &containedLine) {
  string begin = poemBeginChars;
  string end = poemEndChars;
  auto endPos = containedLine.find(end);
  if (containedLine.find(begin) == string::npos or endPos == string::npos)
    return string::npos;
  auto beginPos = containedLine.find_last_of(endOfBeginTag, endPos);
  m_bodyText = containedLine.substr(beginPos + 1, endPos - beginPos - 1);
  return beginPos + 1;
}
string Poem::getWholeString() {
  return poemBeginChars + unhiddenDisplayPropterty + endOfBeginTag +
         m_bodyText + poemEndChars;
}

string Poem::getDisplayString() { return m_bodyText; }
size_t Poem::length() { return getWholeString().length(); }
size_t Poem::displaySize() { return getDisplayString().length(); }

void testSpace() {
  string line =
      R"(<a unhidden name="P11L1">11.1</a>&nbsp;&nbsp;&nbsp;&nbsp;<strong unhidden>杜鹃无语正黄昏，荷锄归去掩重门。青灯照壁人初睡，冷雨敲窗被未温。</strong>&nbsp;&nbsp;&nbsp;&nbsp;<samp unhidden font style="font-size: 13.5pt; font-family:楷体; color:#ff00ff">（像杜鹃啼血一样）我泣尽了血泪默默无语，只发现愁惨的黄昏正在降临，只好扛着花锄忍痛归去，一层层带上身后的门。闺中点起青冷的灯光，摇摇曳曳照射着四壁，我才要躺下，拉上尚是冰凉的被裘，却又听见轻寒的春雨敲打着窗棂，更增加了一层寒意。</samp><br>)";
  Space sp;
  cout << "first appearance offset: " << sp.loadFirstFromContainedLine(line)
       << endl
       << "length: " << sp.length() << " display size: " << sp.displaySize()
       << endl;
  cout << "whole string: " << sp.getWholeString() << endl;
  cout << "display as:" << sp.getDisplayString() << "||" << endl;
}

void testPoem() {
  string poemStr =
      R"(<strong unhidden>杜鹃无语正黄昏，荷锄归去掩重门。青灯照壁人初睡，冷雨敲窗被未温。</strong>)";
  string line =
      R"(<a unhidden name="P11L1">11.1</a>&nbsp;&nbsp;&nbsp;&nbsp;<strong unhidden>杜鹃无语正黄昏，荷锄归去掩重门。青灯照壁人初睡，冷雨敲窗被未温。</strong>&nbsp;&nbsp;&nbsp;&nbsp;<samp unhidden font style="font-size: 13.5pt; font-family:楷体; color:#ff00ff">（像杜鹃啼血一样）我泣尽了血泪默默无语，只发现愁惨的黄昏正在降临，只好扛着花锄忍痛归去，一层层带上身后的门。闺中点起青冷的灯光，摇摇曳曳照射着四壁，我才要躺下，拉上尚是冰凉的被裘，却又听见轻寒的春雨敲打着窗棂，更增加了一层寒意。</samp><br>)";
  Poem poem1(poemStr);
  cout << "length: " << poem1.length()
       << " display size: " << poem1.displaySize() << endl;
  cout << "whole string: " << poem1.getWholeString() << endl;
  cout << "display as:" << poem1.getDisplayString() << "||" << endl;
  Poem poem2;
  cout << "first appearance offset: " << poem2.loadFirstFromContainedLine(line)
       << endl
       << "length: " << poem2.length()
       << " display size: " << poem2.displaySize() << endl;
  cout << "whole string: " << poem2.getWholeString() << endl;
  cout << "display as:" << poem2.getDisplayString() << "||" << endl;
}
