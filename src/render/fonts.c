// The DirectWrite factory, made on first use.
#include <initguid.h>

#include "render/fonts.h"

#include "base/log.h"

static IDWriteFactory *g_factory;

IDWriteFactory *fonts_factory(void) {
  if (!g_factory) {
    HRESULT hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, &IID_IDWriteFactory,
                                     (IUnknown **)&g_factory);
    if (FAILED(hr)) log_error("fonts", "DWriteCreateFactory failed: 0x%08lx", (unsigned long)hr);
  }
  return g_factory;
}
