#include "paraHeader.hpp"

const string colorInTemplate = R"(<hr color="#COLOR">)";
const string colorValueInTemplate = R"(COLOR)";
const string groupIdBeginChars = R"(id="P)";
const string lineColorBeginChars = R"(color="#)";

void ParaHeader::readType(const string &header) {
  if (header.find(topIdBeginChars) != string::npos)
    m_type = PARA_TYPE::FIRST;
  else if (header.find(bottomIdBeginChars) != string::npos)
    m_type = PARA_TYPE::LAST;
  else if (header.find(citationChapterNo) != string::npos)
    m_type = PARA_TYPE::MIDDLE;
}

void ParaHeader::loadFromFirstParaHeader(const string &header) {
  if (header.find(unhiddenDisplayProperty) == string::npos)
    m_hidden = true;
  m_startNumber = TurnToInt(
      getIncludedStringBetweenTags(header, groupIdBeginChars, titleEndChars));
  if (not m_hidden)
    m_color = getIncludedStringBetweenTags(header, lineColorBeginChars,
                                           referParaEndChar);
}

void ParaHeader::loadFromMiddleParaHeader(const string &header) {
  m_currentParaNo = TurnToInt(getIncludedStringBetweenTags(
                        header, groupIdBeginChars, titleEndChars)) -
                    m_startNumber;
}

void ParaHeader::loadFromLastParaHeader(const string &header) {
  m_currentParaNo =
      m_startNumber - TurnToInt(getIncludedStringBetweenTags(
                          header, groupIdBeginChars, titleEndChars));
}

void ParaHeader::loadFrom(const string &header) {
  readType(header);
  if (isFirstParaHeader())
    loadFromFirstParaHeader(header);
  else if (isLastParaHeader())
    loadFromLastParaHeader(header);
  else
    loadFromMiddleParaHeader(header);
}

void ParaHeader::fixFromTemplate() {
  if (isFirstParaHeader())
    fixFirstParaHeaderFromTemplate();
  else if (isLastParaHeader())
    fixLastParaHeaderFromTemplate();
  else
    fixMiddleParaHeaderFromTemplate();
}

const string CoupledParaHeader::firstParaHeader =
    R"(<b unhidden> 第1段 </b><a unhidden id="PXX" href="#PYY">v向下</a>&nbsp;&nbsp;&nbsp;&nbsp;<a unhidden id="top" href="#bottom">页面底部->||</a><hr color="#COLOR">)";
const string CoupledParaHeader::MiddleParaHeader =
    R"(<hr color="#COLOR"><b unhidden> 第ZZ段 </b><a unhidden id="PXX" href="#PYY">向下</a>&nbsp;&nbsp;&nbsp;&nbsp;<a unhidden id="PWW" href="#PUU">向上</a><hr color="#COLOR">)";
const string CoupledParaHeader::lastParaHeader =
    R"(<hr color="#COLOR"><a unhidden id="bottom" href="#top">||<-页面顶部</a>&nbsp;&nbsp;&nbsp;&nbsp;<a unhidden id="PXX" href="#PYY">^向上</a><hr color="#COLOR">)";
const string CoupledParaHeader::firstParaHeaderDispText =
    R"(第1段 v向下    页面底部->||)";
const string CoupledParaHeader::MiddleParaHeaderDispText =
    R"(第ZZ段 向下    向上)";
const string CoupledParaHeader::lastParaHeaderDispText =
    R"(||<-页面顶部    ^向上)";

/**
 * generate real first Paragragh header
 * by filling right name and href
 * and right color
 * @param startNumber the start number as the name of first paragraph
 * @return first Paragragh header after fixed
 */
void CoupledParaHeader::fixFirstParaHeaderFromTemplate() {
  m_result = firstParaHeader;
  if (m_hidden) {
    replacePart(m_result, unhiddenDisplayProperty, hiddenDisplayProperty);
    replacePart(m_result, colorInTemplate, brTab);
  } else
    replacePart(m_result, colorValueInTemplate, m_color);
  replacePart(m_result, "XX", TurnToString(m_startNumber));
  replacePart(m_result, "YY", TurnToString(m_startNumber + 1));
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
void CoupledParaHeader::fixMiddleParaHeaderFromTemplate() {
  m_result = MiddleParaHeader;
  if (m_hidden) {
    replacePart(m_result, unhiddenDisplayProperty, hiddenDisplayProperty);
    replacePart(m_result, colorInTemplate, brTab);
  } else
    replacePart(m_result, colorValueInTemplate, m_color);
  replacePart(m_result, "XX", TurnToString(m_startNumber + m_currentParaNo));
  if (m_lastPara == true) {
    replacePart(m_result, "PYY", bottomParagraphIndicator);
  } else
    replacePart(m_result, "YY",
                TurnToString(m_startNumber + m_currentParaNo + 1));
  replacePart(m_result, "ZZ", TurnToString(m_currentParaNo + 1));
  replacePart(m_result, "WW", TurnToString(m_startNumber - m_currentParaNo));
  replacePart(m_result, "UU",
              TurnToString(m_startNumber - m_currentParaNo + 1));
}

void CoupledParaHeader::fixUnhiddenMiddleParaHeaderDispTextFromTemplate() {
  m_displayText = MiddleParaHeaderDispText;
  replacePart(m_displayText, "ZZ", TurnToString(m_currentParaNo + 1));
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
void CoupledParaHeader::fixLastParaHeaderFromTemplate() {
  m_result = lastParaHeader;
  if (m_hidden) {
    replacePart(m_result, unhiddenDisplayProperty, hiddenDisplayProperty);
    replacePart(m_result, colorInTemplate, brTab);
  } else
    replacePart(m_result, colorValueInTemplate, m_color);
  replacePart(m_result, "XX", TurnToString(m_startNumber - m_currentParaNo));
  replacePart(m_result, "YY",
              TurnToString(m_startNumber - m_currentParaNo + 1));
}

string CoupledParaHeader::getDisplayString() {

  if (m_hidden)
    m_displayText = "";
  else {
    if (isFirstParaHeader())
      m_displayText = firstParaHeaderDispText;
    else if (isLastParaHeader())
      m_displayText = lastParaHeaderDispText;
    else
      fixUnhiddenMiddleParaHeaderDispTextFromTemplate();
  }
  return m_displayText;
}

const string GenericParaHeader::firstParaHeader =
    R"(<tr><td width="50%"><b unhidden> 第1回 - 第QQ回 </b><a unhidden id="PXX" href="#PYY">v向下QQ回</a></td><td width="50%"><a unhidden id="top" href="#bottom">页面底部->||</a></td></tr>)";
const string GenericParaHeader::MiddleParaHeader =
    R"(<tr><td width="50%"><b unhidden> 第PP回 - 第QQ回 </b><a unhidden id="PXX" href="#PYY">向下ZD回</a></td><td width="50%"><a unhidden id="PWW" href="#PUU">向上ZU回</a></td></tr>)";
const string GenericParaHeader::lastParaHeader =
    R"(<tr><td width="50%"><a unhidden id="bottom" href="#top">||<-页面顶部</a></td><td width="50%"><a unhidden id="PXX" href="#PYY">^向上ZZ回</a></td></tr>)";

void GenericParaHeader::fixFirstParaHeaderFromTemplate() {
  m_result = firstParaHeader;
  if (m_hidden) {
    replacePart(m_result, unhiddenDisplayProperty, hiddenDisplayProperty);
    replacePart(m_result, colorInTemplate, brTab);
  } else if (not m_color.empty())
    replacePart(m_result, colorValueInTemplate, m_color);
  else
    replacePart(m_result, colorValueInTemplate, MAIN_SEPERATOR_COLOR);
  replacePart(m_result, "XX", TurnToString(m_startNumber));
  replacePart(m_result, "YY", TurnToString(m_startNumber + 1));
  replacePart(m_result, "QQ", TurnToString(m_totalPara));
  if (m_units != defaultUnit)
    replacePart(m_result, defaultUnit, m_units);
}

void GenericParaHeader::fixMiddleParaHeaderFromTemplate() {
  m_result = MiddleParaHeader;
  if (m_hidden) {
    replacePart(m_result, unhiddenDisplayProperty, hiddenDisplayProperty);
    replacePart(m_result, colorInTemplate, brTab);
  } else if (not m_color.empty())
    replacePart(m_result, colorValueInTemplate, m_color);
  else
    replacePart(m_result, colorValueInTemplate, MAIN_SEPERATOR_COLOR);
  replacePart(m_result, "XX", TurnToString(m_startNumber + m_seqOfPara));
  if (m_lastPara == true) {
    replacePart(m_result, "PYY", bottomParagraphIndicator);
  } else
    replacePart(m_result, "YY", TurnToString(m_startNumber + m_seqOfPara + 1));
  replacePart(m_result, "ZD", TurnToString(m_totalPara));
  replacePart(m_result, "ZU", TurnToString(m_preTotalPara));
  replacePart(m_result, "WW", TurnToString(m_startNumber - m_seqOfPara));
  replacePart(m_result, "UU", TurnToString(m_startNumber - m_seqOfPara + 1));
  replacePart(m_result, "PP", TurnToString(m_startParaNo));
  replacePart(m_result, "QQ", TurnToString(m_endParaNo));
  if (m_units != defaultUnit)
    replacePart(m_result, defaultUnit, m_units);
}

void GenericParaHeader::fixLastParaHeaderFromTemplate() {
  m_result = lastParaHeader;
  if (m_hidden) {
    replacePart(m_result, unhiddenDisplayProperty, hiddenDisplayProperty);
    replacePart(m_result, colorInTemplate, brTab);
  } else if (not m_color.empty())
    replacePart(m_result, colorValueInTemplate, m_color);
  else
    replacePart(m_result, colorValueInTemplate, MAIN_SEPERATOR_COLOR);
  replacePart(m_result, "ZZ", TurnToString(m_totalPara));
  replacePart(m_result, "XX", TurnToString(m_startNumber - m_seqOfPara));
  replacePart(m_result, "YY", TurnToString(m_startNumber - m_seqOfPara + 1));
  if (m_units != defaultUnit)
    replacePart(m_result, defaultUnit, m_units);
}
