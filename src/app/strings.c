// The language choice and the texts with numbers or names in them. Source
// is UTF-8 (see strings_ko.c).
#include "app/strings.h"

#include <stdio.h>
#include <string.h>
#include <windows.h>

#include "app/prefs.h"
#include "app/strings_tables.h"

static int g_lang = -1;

static Lang system_lang(void) {
  return PRIMARYLANGID(GetUserDefaultUILanguage()) == LANG_KOREAN ? LANG_KO : LANG_EN;
}

Lang strings_lang(void) {
  if (g_lang < 0) {
    char saved[8];
    g_lang = prefs_get(PREF_LANG, saved, sizeof saved) ? (strcmp(saved, "ko") == 0 ? LANG_KO : LANG_EN)
                                                       : system_lang();
  }
  return (Lang)g_lang;
}

const Strings *strings(void) { return strings_lang() == LANG_KO ? &STRINGS_KO : &STRINGS_EN; }

void strings_set_lang(Lang lang) {
  g_lang = lang;
  prefs_set(PREF_LANG, lang == LANG_KO ? "ko" : "en");
}

static bool ko(void) { return strings_lang() == LANG_KO; }

void format_status(char *out, size_t cap, int shown, int tucked) {
  if (tucked == 0) snprintf(out, cap, ko() ? "%d개 리슨 중" : "%d listening", shown);
  else snprintf(out, cap, ko() ? "%d개 리슨 중, %d개 숨김" : "%d listening, %d hidden", shown, tucked);
}

void format_status_all(char *out, size_t cap, int total) {
  snprintf(out, cap, ko() ? "%d개 모두 보는 중" : "Showing all %d", total);
}

void format_confirm(char *out, size_t cap, const char *name, unsigned pid) {
  snprintf(out, cap, ko() ? "%s (PID %u) 끌까요?" : "Stop %s (PID %u)?", name, pid);
}

void format_confirm_admin(char *out, size_t cap, const char *name, const char *service) {
  if (service[0]) {
    snprintf(out, cap, ko() ? "관리자 권한으로 서비스 %s 멈출까요?" : "Stop the service %s as administrator?",
             service);
  } else {
    snprintf(out, cap, ko() ? "관리자 권한으로 %s 끌까요?" : "Stop %s as administrator?", name);
  }
}

void format_stop_outcome(char *out, size_t cap, unsigned port, const char *name, const char *outcome) {
  snprintf(out, cap, "%u %s: %s", port, name, outcome);
}

void format_no_match(char *out, size_t cap, const char *query) {
  snprintf(out, cap, ko() ? "'%s'에 맞는 포트가 없습니다" : "No port matches '%s'", query);
}

void format_hide_program(char *out, size_t cap, const char *exe) {
  snprintf(out, cap, ko() ? "목록에서 빼기 (%s)" : "Take off the list (%s)", exe);
}

void format_tray_tip(char *out, size_t cap, int servers) {
  if (servers == 0) snprintf(out, cap, ko() ? "egret: 서버 없음" : "egret: nothing listening");
  else if (ko()) snprintf(out, cap, "egret: 서버 %d개", servers);
  else snprintf(out, cap, servers == 1 ? "egret: 1 server" : "egret: %d servers", servers);
}

void format_hotkey_taken(char *out, size_t cap, const char *hotkey) {
  snprintf(out, cap, ko() ? "%s 단축키를 다른 프로그램이 쓰고 있습니다" : "Another program already uses %s",
           hotkey);
}

void format_cli_no_listener(char *out, size_t cap, unsigned port) {
  snprintf(out, cap, ko() ? "%u: 리슨 중인 프로그램이 없습니다." : "%u: nothing listens on it.", port);
}

void format_cli_not_port(char *out, size_t cap, const char *arg) {
  snprintf(out, cap, ko() ? "포트 번호가 아닙니다: %s" : "Not a port number: %s", arg);
}
