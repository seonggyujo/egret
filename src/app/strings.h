// Every piece of text egret shows, in Korean or English. Without a saved
// choice the language follows the Windows display language. Port numbers,
// program names and the --list column titles stay as they are.
#pragma once

#include <stdbool.h>
#include <stddef.h>

typedef enum { LANG_EN, LANG_KO } Lang;

typedef struct {
  // The popup.
  const char *search_hint, *col_port, *col_process, *col_folder, *col_uptime;
  const char *kind_service, *kind_system, *kind_wsl, *no_access, *kernel;
  const char *tip_browser, *tip_folder, *tip_stop, *tip_stop_admin, *tip_lan, *tip_wsl, *tip_heron,
      *tip_settings;
  const char *cancel, *stop, *show, *hide, *showing_all, *empty_title, *empty_hint, *stopping;
  // The row's right-click menu and the tray menu.
  const char *menu_browser, *menu_folder, *menu_stop, *menu_copy_port;
  const char *tray_open, *tray_show_all, *tray_settings, *tray_quit;
  // Settings.
  const char *settings, *back, *theme, *theme_system, *theme_light, *theme_dark, *language, *hotkey,
      *hotkey_press, *autostart, *cli_path, *cli_path_hint, *hidden_programs, *hidden_hint, *hidden_none,
      *open_settings_folder;
  // What stopping came to.
  const char *stopped, *already_gone, *needs_admin, *denied_cli, *blocked_system, *blocked_wsl,
      *no_engine, *stop_failed;
  // The command line.
  const char *cli_usage, *cli_confirm, *cli_cancelled;
} Strings;

const Strings *strings(void);
Lang strings_lang(void);
// Shows lang everywhere and remembers it.
void strings_set_lang(Lang lang);

// The texts with numbers or names in them. Each writes into out.
// Status bar: "6개 리슨 중, 14개 숨김".
void format_status(char *out, size_t cap, int shown, int tucked);
// "6 listening" when nothing is tucked away; "20개 모두 보는 중" with show-all.
void format_status_all(char *out, size_t cap, int total);
// The confirm row: "java (PID 30112) 끌까요?".
void format_confirm(char *out, size_t cap, const char *name, unsigned pid);
// The confirm row for a service: "postgres: 관리자 권한으로 서비스 postgresql-x64-18을 멈출까요?".
void format_confirm_admin(char *out, size_t cap, const char *name, const char *service);
// After stopping, in the status bar: "5173 node: 껐습니다".
void format_stop_outcome(char *out, size_t cap, unsigned port, const char *name, const char *outcome);
// Search with no hits: "'51'에 맞는 포트가 없습니다".
void format_no_match(char *out, size_t cap, const char *query);
// Row menu: "목록에서 빼기 (Discord.exe)".
void format_hide_program(char *out, size_t cap, const char *exe);
// Tray tooltip: "egret: 서버 3개", "egret: 서버 없음".
void format_tray_tip(char *out, size_t cap, int servers);
// When RegisterHotKey refused: "Ctrl+Alt+P는 다른 프로그램이 쓰고 있습니다".
void format_hotkey_taken(char *out, size_t cap, const char *hotkey);
// The command line.
void format_cli_no_listener(char *out, size_t cap, unsigned port);
void format_cli_not_port(char *out, size_t cap, const char *arg);
