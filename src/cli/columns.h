// How many terminal columns text takes: Hangul, CJK and full-width forms
// take two. Pure, so it is tested on its own. Only cli/ and the tests
// include this.
#pragma once

int text_columns(const char *utf8);
