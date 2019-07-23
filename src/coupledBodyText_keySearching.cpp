#include "coupledBodyText.hpp"

bool isFoundAsNonKeys(const string &line, const string &key) {
  unsigned int searchStart = 0;
  while (true) {
    auto keyBegin = line.find(key, searchStart);
    // no key found any more
    if (keyBegin == string::npos)
      break;
    auto testBeginPos = line.rfind(titleStartChars, keyBegin);
    auto endOfTag = testBeginPos;
    // if titleStartChars exists
    if (testBeginPos != string::npos and testBeginPos > searchStart) {
      string testStr =
          line.substr(testBeginPos + titleStartChars.length(),
                      keyBegin - testBeginPos - titleStartChars.length());
      if (testStr.find(titleEndChars) != string::npos)
        return true;
      endOfTag = line.find(titleEndChars, keyBegin);
    } else
      return true;
    searchStart = endOfTag + 1;
  }
  return false;
}

bool isFoundOutsidePersonalComments(const string &line, const string &key) {
  unsigned int searchStart = 0;
  while (true) {
    auto keyBegin = line.find(key, searchStart);
    // no key found any more
    if (keyBegin == string::npos)
      break;
    auto testBeginPos = line.rfind(personalCommentStartChars, keyBegin);
    // if personalCommentStartChars exists
    if (testBeginPos != string::npos and testBeginPos > searchStart) {
      string testStr = line.substr(
          testBeginPos + personalCommentStartChars.length(),
          keyBegin - testBeginPos - personalCommentStartChars.length());
      if (debug >= LOG_INFO)
        FUNCTION_OUTPUT << testStr << endl;
      if (testStr.find(personalCommentEndChars) != string::npos)
        return true;
    } else
      return true;
    searchStart = line.find(personalCommentEndChars, keyBegin) + 1;
  }
  return false;
}

/**
 * find a keyword in a numbered bodytext file
 */
bool CoupledBodyText::findKey(const string &key) {
  ifstream infile(m_inputFile);
  if (!infile) {
    m_searchError = ERROR_FILE_NOT_EXIST + m_inputFile;
    METHOD_OUTPUT << m_searchError << endl;
    return false;
  }
  bool found = false;
  while (!infile.eof()) {
    string line{emptyString};
    getline(infile, line);

    // if not appear in this line
    if (line.find(key) == string::npos) {
      continue;
    }

    // skip text in image group
    if (isImageGroupLine(line)) {
      continue;
    }

    // if "keys" found are only part of "key" of another link, skip this line
    if (not isFoundAsNonKeys(line, key)) {
      continue;
    }

    // if "keys" found are only part of personal comments, skip them
    if (m_ignorePersonalComments and
        not isFoundOutsidePersonalComments(line, key)) {
      continue;
    }

    LineNumberPlaceholderLink ln;
    ln.loadFirstFromContainedLine(line);
    if (not ln.get().valid()) {
      m_searchError =
          "file doesn't get numbered:" + m_inputFile + " at line:" + line;
      METHOD_OUTPUT << m_searchError << endl;
      return false;
    }

    // special hack for ignoring one lineNumber
    if (not m_ignoreSet.empty() and
        m_ignoreSet.find(ln.getParaLineString()) != m_ignoreSet.end()) {
      continue;
    }

    found = true;
    m_result.insert(ln.getParaLineString());
    if (m_onlyFirst) {
      break;
    }
  }
  return found;
}
