#include "paraHeader.hpp"

const string ParaHeader::firstParaHeader =
    R"(<b unhidden> 第1段 </b><a unhidden id="PXX" href="#PYY">v向下</a>&nbsp;&nbsp;&nbsp;&nbsp;<a unhidden id="top" href="#bottom">页面底部->||</a><hr color="#COLOR">)";
const string ParaHeader::MiddleParaHeader =
    R"(<hr color="#COLOR"><b unhidden> 第ZZ段 </b><a unhidden id="PXX" href="#PYY">向下</a>&nbsp;&nbsp;&nbsp;&nbsp;<a unhidden id="PWW" href="#PUU">向上</a><hr color="#COLOR">)";
const string ParaHeader::lastParaHeader =
    R"(<hr color="#COLOR"><a unhidden id="bottom" href="#top">||<-页面顶部</a>&nbsp;&nbsp;&nbsp;&nbsp;<a unhidden id="PXX" href="#PYY">^向上</a><hr color="#COLOR">)";

/**
 * generate real first Paragragh header
 * by filling right name and href
 * and right color
 * @param startNumber the start number as the name of first paragraph
 * @return first Paragragh header after fixed
 */
void ParaHeader::fixFirstParaHeaderFromTemplate() {
  m_result = firstParaHeader;
  if (m_hidden) {
    replacePart(m_result, unhiddenDisplayProperty, hiddenDisplayProperty);
    replacePart(m_result, R"(<hr color="#COLOR">)", brTab);
  } else
    replacePart(m_result, "COLOR", m_color);
  replacePart(m_result, "XX", TurnToString(m_startNumber));
  replacePart(m_result, "YY", TurnToString(m_startNumber + 1));
}

void ParaHeader::loadFromFirstParaHeader(const string &header) {
  if (header.find(unhiddenDisplayProperty) == string::npos)
    m_hidden = true;
  cout << getIncludedStringBetweenTags(header, R"(id="P)", titleEndChars)
       << endl;
  m_startNumber = TurnToInt(
      getIncludedStringBetweenTags(header, R"(id="P)", titleEndChars));
  cout << getIncludedStringBetweenTags(header, R"(color="#)", referParaEndChar)
       << endl;
  if (not m_hidden)
    m_color =
        getIncludedStringBetweenTags(header, R"(color="#)", referParaEndChar);
}

/**
 * generate real middle Paragragh header
 * by filling right number of this paragraph
 * right name for uplink and downlink
 * and right href to uplink and downlink
 * and right color for different files
 * and if this is last middle paragraph
 * make downlink pointing to bottom
 * @param startNumber the number of first paragraph header
 * @param currentParaNo number of paragraphs before this header
 * @param color the separator line color
 * @param lastPara whether this is the last middle paragraph
 * @return Paragragh header after fixed
 */
void ParaHeader::fixMiddleParaHeaderFromTemplate() {
  m_result = MiddleParaHeader;
  if (m_hidden) {
    replacePart(m_result, unhiddenDisplayProperty, hiddenDisplayProperty);
    replacePart(m_result, R"(<hr color="#COLOR">)", brTab);
  } else
    replacePart(m_result, "COLOR", m_color);
  replacePart(m_result, "XX", TurnToString(m_startNumber + m_currentParaNo));
  if (m_lastPara == true) {
    replacePart(m_result, "PYY", R"(bottom)");
  } else
    replacePart(m_result, "YY",
                TurnToString(m_startNumber + m_currentParaNo + 1));
  replacePart(m_result, "ZZ", TurnToString(m_currentParaNo + 1));
  replacePart(m_result, "WW", TurnToString(m_startNumber - m_currentParaNo));
  replacePart(m_result, "UU",
              TurnToString(m_startNumber - m_currentParaNo + 1));
}

void ParaHeader::loadFromMiddleParaHeader(const string &header) {
  cout << getIncludedStringBetweenTags(header, R"(id="P)", titleEndChars)
       << endl;
  m_currentParaNo = TurnToInt(getIncludedStringBetweenTags(header, R"(id="P)",
                                                           titleEndChars)) -
                    m_startNumber;
}

/**
 * generate real last Paragragh header
 * by filling right name and href
 * and right color
 * @param startNumber the start number as the name of first paragraph
 * @param lastParaNo number of paragraphs before this header
 * @param color the separator line color
 * @return last Paragragh header after fixed
 */
void ParaHeader::fixLastParaHeaderFromTemplate() {
  m_result = lastParaHeader;
  if (m_hidden) {
    replacePart(m_result, unhiddenDisplayProperty, hiddenDisplayProperty);
    replacePart(m_result, R"(<hr color="#COLOR">)", brTab);
  } else
    replacePart(m_result, "COLOR", m_color);
  replacePart(m_result, "XX", TurnToString(m_startNumber - m_currentParaNo));
  replacePart(m_result, "YY",
              TurnToString(m_startNumber - m_currentParaNo + 1));
}

void ParaHeader::loadFromLastParaHeader(const string &header) {
  cout << getIncludedStringBetweenTags(header, R"(id="P)", titleEndChars)
       << endl;
  m_currentParaNo = m_startNumber - TurnToInt(getIncludedStringBetweenTags(
                                        header, R"(id="P)", titleEndChars));
}

static const string frontParaHeader =
    R"(<tr><td width="50%"><b unhidden> 第1回 - 第QQ回 </b><a unhidden id="PXX" href="#PYY">v向下QQ回</a></td><td width="50%"><a unhidden id="top" href="#bottom">页面底部->||</a></td></tr>)";
string fixFrontParaHeaderFromTemplate(int startNumber, const string &color,
                                      int totalPara, const string &units,
                                      bool hidden) {
  string link = frontParaHeader;
  if (hidden) {
    replacePart(link, unhiddenDisplayProperty, hiddenDisplayProperty);
    replacePart(link, R"(<hr color="#COLOR">)", brTab);
  } else if (not color.empty())
    replacePart(link, "COLOR", color);
  else
    replacePart(link, "COLOR", MAIN_SEPERATOR_COLOR);
  replacePart(link, "XX", TurnToString(startNumber));
  replacePart(link, "YY", TurnToString(startNumber + 1));
  replacePart(link, "QQ", TurnToString(totalPara));
  if (units != defaultUnit)
    replacePart(link, defaultUnit, units);
  return link;
}

static const string insertParaHeader =
    R"(<tr><td width="50%"><b unhidden> 第PP回 - 第QQ回 </b><a unhidden id="PXX" href="#PYY">向下ZD回</a></td><td width="50%"><a unhidden id="PWW" href="#PUU">向上ZU回</a></td></tr>)";

string insertParaHeaderFromTemplate(int startNumber, int seqOfPara,
                                    int startParaNo, int endParaNo,
                                    int totalPara, int preTotalPara,
                                    const string &color, const string &units,
                                    bool hidden, bool lastPara) {
  string link = insertParaHeader;
  if (hidden) {
    replacePart(link, unhiddenDisplayProperty, hiddenDisplayProperty);
    replacePart(link, R"(<hr color="#COLOR">)", brTab);
  } else if (not color.empty())
    replacePart(link, "COLOR", color);
  else
    replacePart(link, "COLOR", MAIN_SEPERATOR_COLOR);
  replacePart(link, "XX", TurnToString(startNumber + seqOfPara));
  if (lastPara == true) {
    replacePart(link, "PYY", R"(bottom)");
  } else
    replacePart(link, "YY", TurnToString(startNumber + seqOfPara + 1));
  replacePart(link, "ZD", TurnToString(totalPara));
  replacePart(link, "ZU", TurnToString(preTotalPara));
  replacePart(link, "WW", TurnToString(startNumber - seqOfPara));
  replacePart(link, "UU", TurnToString(startNumber - seqOfPara + 1));
  replacePart(link, "PP", TurnToString(startParaNo));
  replacePart(link, "QQ", TurnToString(endParaNo));
  if (units != defaultUnit)
    replacePart(link, defaultUnit, units);
  return link;
}

static const string backParaHeader =
    R"(<tr><td width="50%"><a unhidden id="bottom" href="#top">||<-页面顶部</a></td><td width="50%"><a unhidden id="PXX" href="#PYY">^向上ZZ回</a></td></tr>)";

string fixBackParaHeaderFromTemplate(int startNumber, int seqOfPara,
                                     int totalPara, const string &color,
                                     const string &units, bool hidden) {
  string link = backParaHeader;
  if (hidden) {
    replacePart(link, unhiddenDisplayProperty, hiddenDisplayProperty);
    replacePart(link, R"(<hr color="#COLOR">)", brTab);
  } else if (not color.empty())
    replacePart(link, "COLOR", color);
  else
    replacePart(link, "COLOR", MAIN_SEPERATOR_COLOR);
  replacePart(link, "ZZ", TurnToString(totalPara));
  replacePart(link, "XX", TurnToString(startNumber - seqOfPara));
  replacePart(link, "YY", TurnToString(startNumber - seqOfPara + 1));
  if (units != defaultUnit)
    replacePart(link, defaultUnit, units);
  return link;
}
