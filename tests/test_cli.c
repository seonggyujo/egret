// Command line arguments, column widths and service program paths.
#include "check.h"
#include "cli/args.h"
#include "cli/columns.h"
#include "probe/service_program.h"

static bool parse(CliArgs *a, int argc, const char *const *argv) { return cli_parse(argc, (char **)argv, a); }

void test_cli_args(void) {
  CliArgs a;
  const char *list[] = {"egret", "--list", "-a"};
  CHECK(parse(&a, 3, list) && a.list && a.all && a.port_count == 0);
  const char *ports[] = {"egret", "3000", "5173", "3000", "-y"};
  CHECK(parse(&a, 5, ports) && a.yes && a.port_count == 2 && a.ports[0] == 3000 && a.ports[1] == 5173);
  const char *zero[] = {"egret", "0"};
  CHECK(!parse(&a, 2, zero) && a.bad && a.bad[0] == '0');
  const char *big[] = {"egret", "65536"};
  CHECK(!parse(&a, 2, big));
  const char *sign[] = {"egret", "+80"};
  CHECK(!parse(&a, 2, sign));
  const char *flag[] = {"egret", "--kill"};
  CHECK(!parse(&a, 2, flag));
  CHECK_STR(a.bad, "--kill");
  const char *mixed[] = {"egret", "--list", "3000"};
  CHECK(!parse(&a, 3, mixed) && a.bad == NULL);
  const char *help[] = {"egret", "-h"};
  CHECK(parse(&a, 2, help) && a.help);
  const char *max[] = {"egret", "65535", "1"};
  CHECK(parse(&a, 3, max) && a.ports[0] == 65535 && a.ports[1] == 1);
}

void test_text_columns(void) {
  CHECK(text_columns("PORT") == 4);
  CHECK(text_columns("") == 0);
  CHECK(text_columns("C:\\사용자\\코드") == 3 + 6 + 1 + 4);
  CHECK(text_columns("caf\xc3\xa9") == 4);
}

void test_service_program(void) {
  char out[300];
  const char *win = "C:\\WINDOWS";
  service_program("\"C:\\Program Files\\PostgreSQL\\18\\bin\\pg_ctl.exe\" runservice -N \"pg\"", win, out, sizeof out);
  CHECK_STR(out, "C:\\Program Files\\PostgreSQL\\18\\bin\\pg_ctl.exe");
  service_program("C:\\Program Files\\VMware\\vmware-authd.exe", win, out, sizeof out);
  CHECK_STR(out, "C:\\Program Files\\VMware\\vmware-authd.exe");
  service_program("C:\\Program Files\\Foo Bar\\foo.EXE -k run", win, out, sizeof out);
  CHECK_STR(out, "C:\\Program Files\\Foo Bar\\foo.EXE");
  service_program("C:\\WINDOWS\\system32\\svchost.exe -k netsvcs -p", win, out, sizeof out);
  CHECK_STR(out, "C:\\WINDOWS\\system32\\svchost.exe");
  service_program("\\SystemRoot\\System32\\x.exe", win, out, sizeof out);
  CHECK_STR(out, "C:\\WINDOWS\\System32\\x.exe");
  service_program("system32\\y.exe -s", win, out, sizeof out);
  CHECK_STR(out, "C:\\WINDOWS\\system32\\y.exe");
  service_program("\\??\\C:\\drv\\z.exe", win, out, sizeof out);
  CHECK_STR(out, "C:\\drv\\z.exe");
  service_program("   ", win, out, sizeof out);
  CHECK_STR(out, "");
}
