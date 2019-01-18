#pragma once
#include "container.hpp"
#include "coupledBodyTextWithLink.hpp"

// link fixing for main files
void fixLinksFromMain();

void generateContentIndexTableForAttachments();

// link fixing for attachment files
void fixLinksFromAttachment();

void testContainer();
void testRemovePersonalViewpoints();
void testfixTagPairEnd();
