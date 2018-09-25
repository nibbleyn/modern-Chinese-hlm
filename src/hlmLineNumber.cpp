#include "hlmLineNumber.hpp"
#include <regex>

extern bool debug;

const string LineNumber::LineNumberStart = R"(<a unhidden name=")";
int LineNumber::StartNumber = START_PARA_NUMBER;
int LineNumber::Limit = START_PARA_NUMBER * 2;

/**
 * retrieve lineNumber from the link at the beginning of containedLine
 * and read from it the paraNumber and lineNumber
 * @param containedLine the numbered line of one body text file
 */
void LineNumber::loadFromContainedLine(const string &containedLine) {
  string start = LineNumberStart;
  auto linkBegin = containedLine.find(start);
  if (linkBegin != string::npos) // found name in lineName
  {
    string end = R"(")";
    string lineName = containedLine.substr(linkBegin + start.length());
    if (debug)
      cout << lineName << endl;
    auto linkEnd = lineName.find(end);
    if (debug)
      cout << lineName.substr(0, linkEnd) << endl;
    if (lineName.substr(0, linkEnd) == "bottom")
      readFromString(R"(P)" + TurnToString(Limit - 1));
    else
      readFromString(lineName.substr(0, linkEnd));
  }
}
/**
 * generate the line prefix for numbering a line
 * @return the prefix to add
 */
string LineNumber::generateLinePrefix() {
  string result{""};
  if (lineNumber != 0)
    result = LineNumberStart + asString() +
             R"(">)";
  return result;
}

/**
 * read from a string of format "PxxLyy"
 * and assign xx to paraNumber
 * and assign yy to lineNumber
 * @param name a string of format "PxxLyy"
 */
void LineNumber::readFromString(const string &name) {
  auto lineNumberStart = name.find(R"(L)");
  if (lineNumberStart != string::npos) {
    // sign of already numbered
    lineNumber = TurnToInt(name.substr(lineNumberStart + 1));
    if (name.substr(0, 1) == R"(P)")
      paraNumber = TurnToInt(name.substr(1, lineNumberStart - 1));
  } else if (name.substr(0, 1) == R"(P)") {
    paraNumber = TurnToInt(name.substr(1));
    if (paraNumber >= Limit) {
      cout << "too limit to hold such paragraph: " << paraNumber << endl;
    }
  } else
    paraNumber = TurnToInt(name.substr(0)); // temporarily accept non-P number
}

/**
 * replace specific parts of a template link to actual value
 * @param linkString the link has key to replace
 * @param key the identified part to replace in the link
 * @param toReplace the actual value to replace in the link
 * @return the link after replacing all parts
 */
string replacePart(string &linkString, const string &key,
                   const string &toReplace) {
  while (true) {
    auto partBegin = linkString.find(key);
    if (partBegin == string::npos)
      break;
    linkString.replace(partBegin, key.length(), toReplace);
  }
  return linkString;
}