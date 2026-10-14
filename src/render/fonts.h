// The shared DirectWrite factory. Only render/ includes this.
#pragma once

#include <dwrite.h>

IDWriteFactory *fonts_factory(void);
