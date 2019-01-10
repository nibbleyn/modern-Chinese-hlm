#include "bodyText.hpp"

bool KeyStartAndCommentStartNotFound(const string &testStr, const string &key) {
  if (debug >= LOG_INFO)
    cout << testStr << " and key: " << key << endl;
  auto keyStartBegin = testStr.find(keyStartChars);
  auto commentStartBegin = testStr.find(commentBeginChars);
  if (keyStartBegin == string::npos and commentStartBegin == string::npos)
    return true;
  return false;
}

bool isOnlyPartOfOtherKeys(const string &orgLine, const string &key) {
  string line = orgLine;
  while (true) {
    auto keyBegin = line.find(key);
    if (keyBegin == string::npos) // no key any more
      break;
    auto keyEnd = line.find(keyEndChars, keyBegin);
    if (keyEnd == string::npos) // no keyEnd any more
      return false;
    string testStr = line.substr(keyBegin, keyEnd - keyBegin);
    if (not KeyStartAndCommentStartNotFound(testStr, key))
      return false;
    else {
      string beforeKey = line.substr(0, keyBegin);
      if (debug >= LOG_INFO)
        cout << beforeKey << endl;
      // if only part of comment
      if (beforeKey.find(keyStartChars) == string::npos)
        return false;
    }
    line = line.substr(keyEnd + keyEndChars.length());
    if (debug >= LOG_INFO)
      cout << line << endl;
  }
  return true;
}

/**
 * find a keyword in a numbered bodytext file
 * whose name is specified as fullPath
 * @param key the keyword used to search
 * @param fullPath the fullpath of the bodytext file
 * @param lineNumber the return value of the line's number like P8L2
 * @param needChange return value indicating a valid line is found
 * and thus need to change original link
 * @return changed key (to KeyNotFound + key + reason) if it is not found,
 * otherwise the original key
 */
bool BodyText::findKey(const string &key, const string &file, int attachNo) {
  string attachmentPart{""};
  if (attachNo != 0)
    attachmentPart = attachmentFileMiddleChar + TurnToString(attachNo);
  string fullPath =
      BODY_TEXT_OUTPUT + filePrefix + file + attachmentPart + BODY_TEXT_SUFFIX;

  ifstream infile(fullPath);
  if (!infile) {
    searchError = "file doesn't exist:" + fullPath;
    cout << searchError << endl;
    return false;
  }
  bool found = false;
  // To search in all the lines in referred file
  while (!infile.eof()) {
    string line{""};
    getline(infile, line); // Saves the line

    if (line.find(key) == string::npos) // not appear in this line
    {
      continue;
    }
    // if "key" is only part of the key of another link, skip this line
    if (isOnlyPartOfOtherKeys(line, key)) {
      if (debug >= LOG_INFO)
        cout << "found as key only: " << line << endl;
      continue;
    }

    LineNumber ln;
    ln.loadFromContainedLine(line);
    if (not ln.valid()) {
      searchError =
          "file doesn't get numbered:" + fullPath + " at line:" + line;
      cout << searchError << endl;
      return false;
    }

    // special hack for ignoring one lineNumber
    if (not ignoreSet.empty() and
        ignoreSet.find(ln.asString()) != ignoreSet.end()) {
      continue;
    }

    found = true;
    result.insert(ln.asString());
    if (onlyFirst) {
      break;
    }
  }
  return found;
}

// reformat to smaller paragraphs
void BodyText::reformatParagraphToSmallerSize(const string &sampleBlock)
{}

// regrouping to make total size smaller
void BodyText::regroupingParagraphs(const string &sampleBlock, const string &sampleFirstLine,
        const string &sampleWholeLine)
{}

// add line number before each paragraph
void BodyText::addLineNumber(const string &separatorColor, bool hidden)
{}
