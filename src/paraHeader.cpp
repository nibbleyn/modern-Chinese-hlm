#include "paraHeader.hpp"

static const string firstParaHeader =
    R"(<b unhidden> 第1段 </b><a unhidden id="PXX" href="#PYY">v向下</a>&nbsp;&nbsp;&nbsp;&nbsp;<a unhidden id="top" href="#bottom">页面底部->||</a><hr color="#COLOR">)";
static const string MiddleParaHeader =
    R"(<hr color="#COLOR"><b unhidden> 第ZZ段 </b><a unhidden id="PXX" href="#PYY">向下</a>&nbsp;&nbsp;&nbsp;&nbsp;<a unhidden id="PWW" href="#PUU">向上</a><hr color="#COLOR">)";
static const string lastParaHeader =
    R"(<hr color="#COLOR"><a unhidden id="bottom" href="#top">||<-页面顶部</a>&nbsp;&nbsp;&nbsp;&nbsp;<a unhidden id="PXX" href="#PYY">^向上</a><hr color="#COLOR">)";

/**
 * generate real first Paragragh header
 * by filling right name and href
 * and right color
 * @param startNumber the start number as the name of first paragraph
 * @return first Paragragh header after fixed
 */
string fixFirstParaHeaderFromTemplate(int startNumber, const string &color,
                                      bool hidden) {
  string link = firstParaHeader;
  if (hidden) {
    link = replacePart(link, "unhidden", "hidden");
    link = replacePart(link, R"(<hr color="#COLOR">)", "<br>");
  } else
    link = replacePart(link, "COLOR", color);
  link = replacePart(link, "XX", TurnToString(startNumber));
  link = replacePart(link, "YY", TurnToString(startNumber + 1));

  return link;
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
string fixMiddleParaHeaderFromTemplate(int startNumber, int currentParaNo,
                                       const string &color, bool hidden,
                                       bool lastPara) {
  string link = MiddleParaHeader;
  if (hidden) {
    link = replacePart(link, "unhidden", "hidden");
    link = replacePart(link, R"(<hr color="#COLOR">)", "<br>");
  } else
    link = replacePart(link, "COLOR", color);
  link = replacePart(link, "XX", TurnToString(startNumber + currentParaNo));
  if (lastPara == true) {
    link = replacePart(link, "PYY", R"(bottom)");
  } else
    link =
        replacePart(link, "YY", TurnToString(startNumber + currentParaNo + 1));
  link = replacePart(link, "ZZ", TurnToString(currentParaNo + 1));
  link = replacePart(link, "WW", TurnToString(startNumber - currentParaNo));
  link = replacePart(link, "UU", TurnToString(startNumber - currentParaNo + 1));
  return link;
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
string fixLastParaHeaderFromTemplate(int startNumber, int lastParaNo,
                                     const string &color, bool hidden) {
  string link = lastParaHeader;
  if (hidden) {
    link = replacePart(link, "unhidden", "hidden");
    link = replacePart(link, R"(<hr color="#COLOR">)", "<br>");
  } else
    link = replacePart(link, "COLOR", color);
  link = replacePart(link, "XX", TurnToString(startNumber - lastParaNo));
  link = replacePart(link, "YY", TurnToString(startNumber - lastParaNo + 1));
  return link;
}

static const string frontParaHeader =
    R"(<tr><td width="50%"><b unhidden> 第1回 - 第QQ回 </b><a unhidden id="PXX" href="#PYY">v向下QQ回</a></td><td width="50%"><a unhidden id="top" href="#bottom">页面底部->||</a></td></tr>)";
string fixFrontParaHeaderFromTemplate(int startNumber, const string &color,
                                      int totalPara, bool hidden) {
  string link = frontParaHeader;
  if (hidden) {
    link = replacePart(link, "unhidden", "hidden");
    link = replacePart(link, R"(<hr color="#COLOR">)", "<br>");
  } else if (not color.empty())
    link = replacePart(link, "COLOR", color);
  else
    link = replacePart(link, "COLOR", MAIN_SEPERATOR_COLOR);
  link = replacePart(link, "XX", TurnToString(startNumber));
  link = replacePart(link, "YY", TurnToString(startNumber + 1));
  link = replacePart(link, "QQ", TurnToString(totalPara));

  return link;
}

static const string insertParaHeader =
    R"(<tr><td width="50%"><b unhidden> 第PP回 - 第QQ回 </b><a unhidden id="PXX" href="#PYY">向下ZD回</a></td><td width="50%"><a unhidden id="PWW" href="#PUU">向上ZU回</a></td></tr>)";

string insertParaHeaderFromTemplate(int startNumber, int seqOfPara,
                                    int startParaNo, int endParaNo,
                                    int totalPara, int preTotalPara,
                                    const string &color, bool hidden,
                                    bool lastPara) {
  string link = insertParaHeader;
  if (hidden) {
    link = replacePart(link, "unhidden", "hidden");
    link = replacePart(link, R"(<hr color="#COLOR">)", "<br>");
  } else if (not color.empty())
    link = replacePart(link, "COLOR", color);
  else
    link = replacePart(link, "COLOR", MAIN_SEPERATOR_COLOR);
  link = replacePart(link, "XX", TurnToString(startNumber + seqOfPara));
  if (lastPara == true) {
    link = replacePart(link, "PYY", R"(bottom)");
  } else
    link = replacePart(link, "YY", TurnToString(startNumber + seqOfPara + 1));
  link = replacePart(link, "ZD", TurnToString(totalPara));
  link = replacePart(link, "ZU", TurnToString(preTotalPara));
  link = replacePart(link, "WW", TurnToString(startNumber - seqOfPara));
  link = replacePart(link, "UU", TurnToString(startNumber - seqOfPara + 1));
  link = replacePart(link, "PP", TurnToString(startParaNo));
  link = replacePart(link, "QQ", TurnToString(endParaNo));
  return link;
}

static const string backParaHeader =
    R"(<tr><td width="50%"><a unhidden id="bottom" href="#top">||<-页面顶部</a></td><td width="50%"><a unhidden id="PXX" href="#PYY">^向上ZZ回</a></td></tr>)";

string fixBackParaHeaderFromTemplate(int startNumber, int seqOfPara,
                                     int totalPara, const string &color,
                                     bool hidden) {
  string link = backParaHeader;
  if (hidden) {
    link = replacePart(link, "unhidden", "hidden");
    link = replacePart(link, R"(<hr color="#COLOR">)", "<br>");
  } else if (not color.empty())
    link = replacePart(link, "COLOR", color);
  else
    link = replacePart(link, "COLOR", MAIN_SEPERATOR_COLOR);
  link = replacePart(link, "ZZ", TurnToString(totalPara));
  link = replacePart(link, "XX", TurnToString(startNumber - seqOfPara));
  link = replacePart(link, "YY", TurnToString(startNumber - seqOfPara + 1));
  return link;
}
