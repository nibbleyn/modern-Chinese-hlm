#pragma once

#include <fstream>
#include <iomanip>
#include <sstream>

#include <iostream>
#include <set>
#include <string>
using namespace std;

#include "utf8StringUtil.hpp"

enum class OBJECT_TYPE {
  IMAGE,
  IMAGEREF,
  POEM,
  LINENUMBER,
  LINK,
  POEMTRANSLATION,
  COMMENT,
  PERSONALCOMMENT
};

class Object {
public:
  Object() = default;
  virtual ~Object(){};
  virtual size_t length() = 0;
  virtual size_t displaySize() = 0;
  virtual size_t loadFirstFromContainedLine(const string &containedLine) = 0;

protected:
  string m_bodyText{""};
};

static const string imgBeginChars = R"(<img)";
class Image : public Object {
public:
  Image(const string &imageString);
  size_t displaySize();

private:
  size_t height{0};
  size_t width{0};
};

static const string leftImgBeginChars = R"(<var)";
static const string leftImgEndChars = R"(</var>)";
static const string rightImgBeginChars = R"(<var)";
static const string rightImgEndChars = R"(</var>)";
class ImageReferText : public Object {
public:
  ImageReferText(const string &referTextString);
  size_t displaySize();

private:
  bool toLeft{true};
};

static const string poemBeginChars = R"(<strong)";
static const string poemEndChars = R"(</strong>)";
class Poem : public Object {
public:
  Poem() = default;
  Poem(const string &poemString);
  size_t loadFirstFromContainedLine(const string &containedLine);
  size_t length();
  size_t displaySize();

private:
};

void testPoem();
