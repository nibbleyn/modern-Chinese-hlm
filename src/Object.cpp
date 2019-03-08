#include "Object.hpp"

string getNameOfObjectType(OBJECT_TYPE type) {
  if (type == OBJECT_TYPE::LINENUMBER)
    return "LINENUMBER";
  else if (type == OBJECT_TYPE::IMAGEREF)
    return "IMAGEREF";
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

static const string leftImgReferenceTemplate =
    R"(<var unhidden font style="font-size: 13.5pt; font-family:楷体; color:#ff00ff">（见左图）<-----XX</var>)";
static const string rightImgReferenceTemplate =
    R"(<var unhidden font style="font-size: 13.5pt; font-family:楷体; color:#ff00ff">XX----->（见右图）</var>)";

string fixImgReferenceFromTemplate(bool toLeft, const string &caption) {
  string link =
      (toLeft == true) ? leftImgReferenceTemplate : rightImgReferenceTemplate;
  replacePart(link, "XX", caption);
  return link;
}

size_t ImageReferText::loadFirstFromContainedLine(const string &containedLine,
                                                  size_t after) {
  string begin = ImgRefBeginChars;
  string end = ImgRefEndChars;
  //  cout << containedLine << endl;
  auto beginPos = containedLine.find(begin, after);
  auto endPos = containedLine.find(end, after);
  if (beginPos == string::npos or endPos == string::npos)
    return string::npos;

  string part = containedLine.substr(beginPos, endPos - beginPos);
  //  cout << part << endl;
  beginPos = part.find(endOfImgRefBeginTag);
  m_bodyText = part.substr(beginPos + endOfImgRefBeginTag.length());
  if (m_bodyText.find(leftImgRefChars) != string::npos) {
    m_toLeft = true;
    m_bodyText = m_bodyText.substr(leftImgRefChars.length());
  } else {
    m_toLeft = false;
    m_bodyText =
        m_bodyText.substr(0, m_bodyText.length() - rightImgRefChars.length());
  }
  return containedLine.find(begin, after);
}

string ImageReferText::getWholeString() {
  return fixImgReferenceFromTemplate(m_toLeft, m_bodyText);
}
string ImageReferText::getDisplayString() {
  return (m_toLeft == true) ? leftImgRefChars + m_bodyText
                            : m_bodyText + rightImgRefChars;
}
size_t ImageReferText::length() { return getWholeString().length(); }
size_t ImageReferText::displaySize() { return getDisplayString().length(); }

size_t Space::loadFirstFromContainedLine(const string &containedLine,
                                         size_t after) {
  m_bodyText = displaySpace;
  return containedLine.find(space, after);
}

size_t Poem::loadFirstFromContainedLine(const string &containedLine,
                                        size_t after) {
  string begin = poemBeginChars;
  string end = poemEndChars;
  auto endPos = containedLine.find(end, after);
  if (containedLine.find(begin, after) == string::npos or
      endPos == string::npos)
    return string::npos;
  auto beginPos = containedLine.find_last_of(endOfBeginTag, endPos);
  m_bodyText = containedLine.substr(beginPos + 1, endPos - beginPos - 1);
  return containedLine.find(begin, after);
}

string Poem::getWholeString() {
  return poemBeginChars + unhiddenDisplayPropterty + endOfBeginTag +
         m_bodyText + poemEndChars;
}

string Poem::getDisplayString() { return m_bodyText; }
size_t Poem::length() { return getWholeString().length(); }
size_t Poem::displaySize() { return getDisplayString().length(); }

void testImageReference() {
  string line =
      R"(<a unhidden id="P3L2">3.2</a>&nbsp;&nbsp; 贾母又说：“请姑娘们来。今日有远客来，就不必上学去了。”众人答应了一声，便派了两个人去请。没多久，只见三个奶妈和五六个丫鬟，簇拥着三个姊妹来了。第一个乃二姐<var unhidden font style="font-size: 13.5pt; font-family:楷体; color:#ff00ff">迎春----->（见右图）</var>，生的肌肤微丰，合中身材，腮凝新荔，鼻腻鹅脂，温柔沉默，观之可亲。第二个是三妹<var unhidden font style="font-size: 13.5pt; font-family:楷体; color:#ff00ff">（见左图）<-----探春</var>，削肩细腰，长挑身材，鸭蛋脸面，俊眼修眉，顾盼神飞，文彩精华，见之忘俗。（<cite unhidden>迎春外表就有懦小姐的感觉，探春外表就文采甚好，可起诗社。</cite>）<br>)";
  ImageReferText imgRef;
  auto start = imgRef.loadFirstFromContainedLine(line);
  cout << "first appearance offset: " << start << endl
       << "length: " << imgRef.length()
       << " display size: " << imgRef.displaySize() << endl;
  cout << "whole string: " << imgRef.getWholeString() << endl;
  cout << "display as:" << imgRef.getDisplayString() << "||" << endl;
  line = line.substr(start + imgRef.length());
  cout << "first appearance offset: " << imgRef.loadFirstFromContainedLine(line)
       << endl
       << "length: " << imgRef.length()
       << " display size: " << imgRef.displaySize() << endl;
  cout << "whole string: " << imgRef.getWholeString() << endl;
  cout << "display as:" << imgRef.getDisplayString() << "||" << endl;
}

void testSpace() {
  string line =
      R"(<a unhidden id="P11L1">11.1</a>&nbsp;&nbsp; <strong unhidden>杜鹃无语正黄昏，荷锄归去掩重门。青灯照壁人初睡，冷雨敲窗被未温。</strong>&nbsp;&nbsp;&nbsp;&nbsp;<samp unhidden font style="font-size: 13.5pt; font-family:楷体; color:#ff00ff">（像杜鹃啼血一样）我泣尽了血泪默默无语，只发现愁惨的黄昏正在降临，只好扛着花锄忍痛归去，一层层带上身后的门。闺中点起青冷的灯光，摇摇曳曳照射着四壁，我才要躺下，拉上尚是冰凉的被裘，却又听见轻寒的春雨敲打着窗棂，更增加了一层寒意。</samp><br>)";
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
      R"(<a unhidden id="P11L1">11.1</a>&nbsp;&nbsp; <strong unhidden>杜鹃无语正黄昏，荷锄归去掩重门。青灯照壁人初睡，冷雨敲窗被未温。</strong>&nbsp;&nbsp;&nbsp;&nbsp;<samp unhidden font style="font-size: 13.5pt; font-family:楷体; color:#ff00ff">（像杜鹃啼血一样）我泣尽了血泪默默无语，只发现愁惨的黄昏正在降临，只好扛着花锄忍痛归去，一层层带上身后的门。闺中点起青冷的灯光，摇摇曳曳照射着四壁，我才要躺下，拉上尚是冰凉的被裘，却又听见轻寒的春雨敲打着窗棂，更增加了一层寒意。</samp><br>)";
  Poem poem1;
  poem1.loadFirstFromContainedLine(poemStr);
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
