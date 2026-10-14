// rows_build: merging IPv4 and IPv6, LAN, sorting, heron-c ancestry and
// the kind of each row.
#include "check.h"
#include "model/rows.h"
#include "raw_fixture.h"

void test_rows_merge_and_sort(void) {
  RawSocket socks[] = {fx_socket(5173, 100, false, true, false), fx_socket(5173, 100, true, true, false),
                       fx_socket(3000, 200, false, false, true), fx_socket(3000, 201, true, false, false)};
  RawProcess procs[] = {fx_proc(100, 50, "node.exe"), fx_proc(200, 60, "node.exe"), fx_proc(201, 60, "java.exe"),
                        fx_proc(50, 40, "pwsh.exe"), fx_proc(40, 30, "heron-c.exe"), fx_proc(60, 1, "cmd.exe")};
  RawProcInfo infos[] = {fx_info(100, "C:\\nodejs\\node.exe", "C:\\code\\site"), fx_info(200, "C:\\nodejs\\node.exe", ""),
                         fx_info(201, "C:\\jdk\\bin\\java.exe", "C:\\code\\api")};
  FX_SCAN(raw, socks, procs, infos);
  ListenerList rows = {0};
  rows_build(&raw, &rows);
  CHECK(rows.count == 3);
  CHECK(rows.items[0].port == 3000 && rows.items[0].pid == 200 && rows.items[0].lan);
  CHECK(rows.items[1].port == 3000 && rows.items[1].pid == 201 && rows.items[1].lan && rows.items[1].ipv6);
  const Listener *site = &rows.items[2];
  CHECK(site->port == 5173 && site->ipv4 && site->ipv6 && !site->lan);
  CHECK(site->kind == KIND_PROCESS && site->opened && site->heron);
  CHECK_STR(site->name, "node");
  CHECK_STR(site->exe, "node.exe");
  CHECK_STR(site->folder, "C:\\code\\site");
  CHECK_STR(rows.items[0].folder, "C:\\nodejs");  // no cwd: the program's folder
  CHECK(!rows.items[0].heron);
  listener_list_free(&rows);
}

// Seen on a real PC: an app started at sign-in runs in System32, which says
// nothing about it, so its own folder shows.
void test_rows_folder_in_windows(void) {
  RawSocket socks[] = {fx_socket(26737, 300, false, true, false)};
  RawProcess procs[] = {fx_proc(300, 1, "SOOPLiveLauncher.exe")};
  RawProcInfo infos[] = {fx_info(300, "C:\\Users\\me\\AppData\\Local\\SOOP\\SOOPLiveLauncher.exe",
                                 "C:\\Windows\\system32")};
  FX_SCAN(raw, socks, procs, infos);
  ListenerList rows = {0};
  rows_build(&raw, &rows);
  CHECK(rows.count == 1 && rows.items[0].kind == KIND_PROCESS);
  CHECK_STR(rows.items[0].folder, "C:\\Users\\me\\AppData\\Local\\SOOP");
  listener_list_free(&rows);
}

void test_rows_system(void) {
  RawSocket socks[] = {fx_socket(445, 4, true, false, true), fx_socket(135, 1556, false, false, true),
                       fx_socket(5040, 900, false, false, true), fx_socket(7000, 0, false, true, false)};
  RawProcess procs[] = {fx_proc(4, 0, "System"), fx_proc(1556, 1188, "svchost.exe"), fx_proc(900, 1188, "svc.exe")};
  RawProcInfo infos[] = {fx_info(900, "c:\\windows\\System32\\svc.exe", "")};
  FX_SCAN(raw, socks, procs, infos);
  RawService services[] = {fx_service(1556, "RpcSs", "C:\\WINDOWS\\system32\\svchost.exe")};
  raw.services = services;
  raw.service_count = 1;
  ListenerList rows = {0};
  rows_build(&raw, &rows);
  CHECK(rows.count == 4);
  for (int i = 0; i < rows.count; i++) CHECK(rows.items[i].kind == KIND_SYSTEM);
  CHECK_STR(rows.items[1].name, "System");
  listener_list_free(&rows);
}

// Seen on a real PC: postgres.exe cannot be opened, its parent pg_ctl.exe
// runs the service postgresql-x64-18.
void test_rows_service_postgres(void) {
  RawSocket socks[] = {fx_socket(5432, 6948, false, false, true), fx_socket(5432, 6948, true, false, true),
                       fx_socket(902, 5048, false, true, false)};
  RawProcess procs[] = {fx_proc(6948, 5072, "postgres.exe"), fx_proc(5072, 1188, "pg_ctl.exe"),
                        fx_proc(5048, 1188, "vmware-authd.exe")};
  RawProcInfo infos[] = {fx_info(5048, "C:\\Program Files\\VMware\\vmware-authd.exe", "C:\\WINDOWS\\system32")};
  FX_SCAN(raw, socks, procs, infos);
  RawService services[] = {
      fx_service(5072, "postgresql-x64-18", "C:\\Program Files\\PostgreSQL\\18\\bin\\pg_ctl.exe"),
      fx_service(5048, "VMAuthdService", "C:\\Program Files\\VMware\\vmware-authd.exe")};
  raw.services = services;
  raw.service_count = 2;
  ListenerList rows = {0};
  rows_build(&raw, &rows);
  CHECK(rows.count == 2);
  const Listener *vm = &rows.items[0], *pg = &rows.items[1];
  CHECK(pg->kind == KIND_SERVICE && !pg->opened && pg->lan);
  CHECK_STR(pg->name, "postgres");
  CHECK_STR(pg->service, "postgresql-x64-18");
  CHECK_STR(pg->folder, "C:\\Program Files\\PostgreSQL\\18\\bin");
  CHECK(vm->kind == KIND_SERVICE && vm->opened);
  CHECK_STR(vm->folder, "C:\\Program Files\\VMware");  // not its System32 working folder
  listener_list_free(&rows);
}

void test_rows_container_and_wsl(void) {
  RawSocket socks[] = {fx_socket(6379, 300, false, false, true), fx_socket(6379, 300, true, false, true),
                       fx_socket(8000, 400, false, true, false), fx_socket(9000, 300, false, false, true)};
  RawProcess procs[] = {fx_proc(300, 1, "com.docker.backend.exe"), fx_proc(400, 1, "wslrelay.exe")};
  RawProcInfo infos[] = {fx_info(300, "C:\\Docker\\com.docker.backend.exe", ""),
                         fx_info(400, "C:\\Program Files\\WSL\\wslrelay.exe", "")};
  FX_SCAN(raw, socks, procs, infos);
  RawContainer c;
  memset(&c, 0, sizeof c);
  strcpy(c.id, "a3f9c1e00000");
  strcpy(c.name, "shop-redis");
  strcpy(c.image, "redis:7-alpine");
  c.created = 777;
  c.ports[0].public_port = 6379;
  c.port_count = 1;
  raw.containers = &c;
  raw.container_count = 1;
  ListenerList rows = {0};
  rows_build(&raw, &rows);
  CHECK(rows.count == 3);
  const Listener *redis = &rows.items[0];
  CHECK(redis->kind == KIND_CONTAINER && redis->started == 777);
  CHECK_STR(redis->name, "shop-redis");
  CHECK_STR(redis->image, "redis:7-alpine");
  CHECK_STR(redis->container_id, "a3f9c1e00000");
  CHECK_STR(redis->exe, "");
  CHECK(rows.items[1].kind == KIND_WSL);
  CHECK(rows.items[2].kind == KIND_PROCESS);  // a backend port no container publishes
  listener_list_free(&rows);
}
