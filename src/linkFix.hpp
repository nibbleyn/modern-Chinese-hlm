#pragma once
#include "container.hpp"
#include "coupledBodyTextWithLink.hpp"

// link fixing for main files
void fixLinksFromMain(bool forceUpdate = true);

// link fixing for attachment files
void fixLinksFromAttachment(bool forceUpdate = true);
