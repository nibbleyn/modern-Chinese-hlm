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
    replacePart(link, "unhidden", "hidden");
    replacePart(link, R"(<hr color="#COLOR">)", "<br>");
  } else
    replacePart(link, "COLOR", color);
  replacePart(link, "XX", TurnToString(startNumber));
  replacePart(link, "YY", TurnToString(startNumber + 1));

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
    replacePart(link, "unhidden", "hidden");
    replacePart(link, R"(<hr color="#COLOR">)", "<br>");
  } else
    replacePart(link, "COLOR", color);
  replacePart(link, "XX", TurnToString(startNumber + currentParaNo));
  if (lastPara == true) {
    replacePart(link, "PYY", R"(bottom)");
  } else
    replacePart(link, "YY", TurnToString(startNumber + currentParaNo + 1));
  replacePart(link, "ZZ", TurnToString(currentParaNo + 1));
  replacePart(link, "WW", TurnToString(startNumber - currentParaNo));
  replacePart(link, "UU", TurnToString(startNumber - currentParaNo + 1));
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
    replacePart(link, "unhidden", "hidden");
    replacePart(link, R"(<hr color="#COLOR">)", "<br>");
  } else
    replacePart(link, "COLOR", color);
  replacePart(link, "XX", TurnToString(startNumber - lastParaNo));
  replacePart(link, "YY", TurnToString(startNumber - lastParaNo + 1));
  return link;
}

static const string frontParaHeader =
    R"(<tr><td width="50%"><b unhidden> 第1回 - 第QQ回 </b><a unhidden id="PXX" href="#PYY">v向下QQ回</a></td><td width="50%"><a unhidden id="top" href="#bottom">页面底部->||</a></td></tr>)";
string fixFrontParaHeaderFromTemplate(int startNumber, const string &color,
                                      int totalPara, const string &units,
                                      bool hidden) {
  string link = frontParaHeader;
  if (hidden) {
    replacePart(link, "unhidden", "hidden");
    replacePart(link, R"(<hr color="#COLOR">)", "<br>");
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
    replacePart(link, "unhidden", "hidden");
    replacePart(link, R"(<hr color="#COLOR">)", "<br>");
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
    replacePart(link, "unhidden", "hidden");
    replacePart(link, R"(<hr color="#COLOR">)", "<br>");
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
