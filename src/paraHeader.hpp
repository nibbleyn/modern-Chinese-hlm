#include "fileUtil.hpp"
#include "utf8StringUtil.hpp"

string fixFirstParaHeaderFromTemplate(int startNumber, const string &color,
                                      bool hidden = false);
string fixMiddleParaHeaderFromTemplate(int startNumber, int currentParaNo,
                                       const string &color, bool hidden = false,
                                       bool lastPara = false);
string fixLastParaHeaderFromTemplate(int startNumber, int lastParaNo,
                                     const string &color, bool hidden = false);

string fixFrontParaHeaderFromTemplate(int startNumber, const string &color,
                                      int totalPara, bool hidden = false);

string insertParaHeaderFromTemplate(int startNumber, int seqOfPara,
                                    int startParaNo, int endParaNo,
                                    int totalPara, int preTotalPara,
                                    const string &color, bool hidden = false,
                                    bool lastPara = false);
string fixBackParaHeaderFromTemplate(int startNumber, int seqOfPara,
                                     int totalPara, const string &color,
                                     bool hidden = false);
