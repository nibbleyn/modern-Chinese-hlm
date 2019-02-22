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
