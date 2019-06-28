#pragma once
#include "libraryInterface.hpp"

// make a sub story
void reConstructStory(const string &title,
                      const string &outputFilename = emptyString,
                      const string &kind = MAIN, bool autoNumbering = true);
void autoNumberingResultStory(const string &htmlFilename,
                              const string &outputHtmlFilename = emptyString);
