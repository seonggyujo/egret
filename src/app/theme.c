// The two palettes and which one is on.
#include "app/theme.h"

#include <string.h>
#include <windows.h>

#include "app/prefs.h"

static const ThemeColors LIGHT = {
    .bg = 0xFFFBFBFB, .header = 0xFFFFFFFF, .header_line = 0xFFE5E5E5, .line = 0xFFF0F0F0,
    .hover = 0xFFF2F2F2, .selected = 0xFFEAEAEA, .status_bg = 0xFFF3F3F3, .window_border = 0xFFDCDCDC,
    .text = 0xFF1B1B1B, .text2 = 0xFF5F5F5F, .dim = 0xFF767676,
    .button = 0xFFFFFFFF, .button_line = 0xFFD6D6D6, .button_hover = 0xFFF5F5F5, .scroll = 0xFFB0B0B0,
    .strike = 0xFFFFE27A, .strike_line = 0xFFF0D468, .strike_text2 = 0xFF4A3F00,
    .solid = 0xFF1B1B1B, .solid_text = 0xFFFFFFFF,
    .mark = 0xFF1B1B1B, .bill = 0xFFF2C230,
    .heron = 0xFF2E7D4F, .docker = 0xFF1F5FA8, .lan = 0xFF8A5A00, .error = 0xFFB3261E,
    .switch_on = 0xFF1B1B1B, .switch_knob = 0xFFFFFFFF, .segment_bg = 0xFFEDEDED, .segment_on = 0xFFFFFFFF,
};

static const ThemeColors DARK = {
    .bg = 0xFF1F1F1F, .header = 0xFF262626, .header_line = 0xFF333333, .line = 0xFF292929,
    .hover = 0xFF2A2A2A, .selected = 0xFF333333, .status_bg = 0xFF1A1A1A, .window_border = 0xFF3A3A3A,
    .text = 0xFFF2F2F2, .text2 = 0xFFABABAB, .dim = 0xFF8C8C8C,
    .button = 0xFF2B2B2B, .button_line = 0xFF454545, .button_hover = 0xFF363636, .scroll = 0xFF6A6A6A,
    .strike = 0xFFF2C230, .strike_line = 0xFFD9AD27, .strike_text2 = 0xFF3D3200,
    .solid = 0xFF1B1B1B, .solid_text = 0xFFFFFFFF,
    .mark = 0xFFF2F2F2, .bill = 0xFFF2C230,
    .heron = 0xFF7FCB9B, .docker = 0xFF8DBCF0, .lan = 0xFFF2C230, .error = 0xFFF48771,
    .switch_on = 0xFFF2F2F2, .switch_knob = 0xFF1F1F1F, .segment_bg = 0xFF2B2B2B, .segment_on = 0xFF454545,
};

static int g_mode = -1;
static bool g_system_dark;

// A DWORD under HKCU\...\Themes\Personalize; 1 (light) when missing.
static DWORD personalize(const wchar_t *name) {
  DWORD value = 1, size = sizeof value;
  RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", name,
               RRF_RT_REG_DWORD, NULL, &value, &size);
  return value;
}

ThemeMode theme_mode(void) {
  if (g_mode < 0) {
    char saved[16] = "";
    prefs_get(PREF_THEME, saved, sizeof saved);
    g_mode = strcmp(saved, "light") == 0 ? THEME_LIGHT : strcmp(saved, "dark") == 0 ? THEME_DARK : THEME_SYSTEM;
    g_system_dark = personalize(L"AppsUseLightTheme") == 0;
  }
  return (ThemeMode)g_mode;
}

bool theme_dark(void) {
  ThemeMode m = theme_mode();
  return m == THEME_DARK || (m == THEME_SYSTEM && g_system_dark);
}

const ThemeColors *theme(void) { return theme_dark() ? &DARK : &LIGHT; }

void theme_set_mode(ThemeMode mode) {
  g_mode = mode;
  prefs_set(PREF_THEME, mode == THEME_LIGHT ? "light" : mode == THEME_DARK ? "dark" : "system");
}

void theme_system_changed(void) { g_system_dark = personalize(L"AppsUseLightTheme") == 0; }

bool theme_taskbar_light(void) { return personalize(L"SystemUsesLightTheme") != 0; }
