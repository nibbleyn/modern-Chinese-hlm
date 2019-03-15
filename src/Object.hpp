#pragma once
#include "utf8StringUtil.hpp"

enum class OBJECT_TYPE {
  SPACE,
  LINENUMBER,
  IMAGE,
  IMAGEREF,
  POEM,
  POEMTRANSLATION,
  LINKFROMMAIN,
  LINKFROMATTACHMENT,
  COMMENT,
  PERSONALCOMMENT,
  BR
};

string getNameOfObjectType(OBJECT_TYPE type);

class Object {
public:
  Object() = default;
  virtual ~Object(){};
  virtual string getWholeString() = 0;
  virtual string getDisplayString() = 0;
  virtual size_t length() = 0;
  virtual size_t displaySize() = 0;
  virtual size_t loadFirstFromContainedLine(const string &containedLine,
                                            size_t after = 0) = 0;

protected:
  string m_bodyText{""};
};

static const string imgBeginChars = R"(<img)";
class Image : public Object {
public:
  Image() = default;
  string getWholeString();
  size_t loadFirstFromContainedLine(const string &containedLine,
                                    size_t after = 0);
  string getDisplayString() { return ""; };
  size_t length() { return getWholeString().length(); };
  size_t displaySize() { return getDisplayString().length(); };

private:
  string m_path{""};
  string m_filename{""};
  bool m_leftAlign{false};
  size_t m_width{0};
  string m_fullString{""};
};

static const string ImgRefBeginChars = R"(<var)";
static const string endOfImgRefBeginTag = R"(">)";
static const string ImgRefEndChars = R"(</var>)";
static const string leftImgRefChars = R"(（见左图）<-----)";
static const string rightImgRefChars = R"(----->（见右图）)";
class ImageReferText : public Object {
public:
  ImageReferText() = default;
  string getWholeString();
  string getDisplayString();
  size_t loadFirstFromContainedLine(const string &containedLine,
                                    size_t after = 0);
  size_t length();
  size_t displaySize();

private:
  bool m_toLeft{true};
};

static const string space = R"(&nbsp;)";
static const string doubleSpace = space + space;
static const string fourSpace = space + space + space + space;
class Space : public Object {
public:
  Space() = default;
  string getWholeString() { return space; };
  string getDisplayString() { return displaySpace; };
  size_t loadFirstFromContainedLine(const string &containedLine,
                                    size_t after = 0);
  size_t length() { return space.length(); };
  size_t displaySize() { return displaySpace.length(); };

private:
};

static const string poemBeginChars = R"(<strong )";
static const string poemEndChars = R"(</strong>)";
class Poem : public Object {
public:
  Poem() = default;
  string getWholeString();
  string getDisplayString();
  size_t loadFirstFromContainedLine(const string &containedLine,
                                    size_t after = 0);
  size_t length();
  size_t displaySize();

private:
};

void testImage();
void testSpace();
void testPoem();
void testImageReference();
