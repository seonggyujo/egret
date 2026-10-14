// The HTTP response parser, the container list and the engine pipe names.
#include <stdlib.h>
#include <string.h>

#include "check.h"
#include "docker/containers_json.h"
#include "docker/engine_pipes.h"
#include "docker/http.h"

static HttpState parse(const char *text, bool eof, HttpResponse *r) { return http_parse(text, strlen(text), eof, r); }

void test_http_parse(void) {
  HttpResponse r = {0};
  CHECK(parse("HTTP/1.1 200 OK\r\nContent-Length: 5\r\n\r\nhello", false, &r) == HTTP_DONE);
  CHECK(r.status == 200 && r.body.len == 5);
  CHECK_STR(r.body.data, "hello");
  CHECK(parse("HTTP/1.1 200 OK\r\ncontent-length: 5\r\n\r\nhel", false, &r) == HTTP_INCOMPLETE);
  CHECK(parse("HTTP/1.1 200 OK\r\ncontent-length: 5\r\n\r\nhel", true, &r) == HTTP_BAD);
  CHECK(parse("HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n4;ext=1\r\nWiki\r\n5\r\npedia\r\n0\r\n"
              "X-Trailer: 1\r\n\r\n",
              false, &r) == HTTP_DONE);
  CHECK_STR(r.body.data, "Wikipedia");
  CHECK(parse("HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n4\r\nWi", false, &r) == HTTP_INCOMPLETE);
  CHECK(parse("HTTP/1.1 204 No Content\r\nServer: x\r\n\r\n", false, &r) == HTTP_DONE);
  CHECK(r.status == 204 && r.body.len == 0);
  CHECK(parse("HTTP/1.0 200 OK\r\n\r\nto the end", false, &r) == HTTP_INCOMPLETE);
  CHECK(parse("HTTP/1.0 200 OK\r\n\r\nto the end", true, &r) == HTTP_DONE);
  CHECK_STR(r.body.data, "to the end");
  CHECK(parse("garbage\r\n\r\n", false, &r) == HTTP_BAD);
  CHECK(parse("HTTP/1.1 404 Not Found\r\nContent-Length: 2\r\n\r\n{}", false, &r) == HTTP_DONE && r.status == 404);
  http_response_free(&r);
}

void test_containers_json(void) {
  const char *json =
      "[{\"Id\":\"a3f9c1e\",\"Names\":[\"/shop-redis\"],\"Image\":\"redis:7-alpine\",\"Created\":1700000000,"
      "\"Ports\":[{\"IP\":\"0.0.0.0\",\"PrivatePort\":6379,\"PublicPort\":6379,\"Type\":\"tcp\"},"
      "{\"IP\":\"::\",\"PrivatePort\":6379,\"PublicPort\":6379,\"Type\":\"tcp\"},"
      "{\"PrivatePort\":7000,\"Type\":\"tcp\"},"
      "{\"IP\":\"0.0.0.0\",\"PrivatePort\":53,\"PublicPort\":5353,\"Type\":\"udp\"}]},"
      "{\"Id\":\"b\",\"Names\":[],\"Image\":\"x\",\"Ports\":[]}]";
  RawContainer *list = NULL;
  int n = docker_parse_containers(json, strlen(json), &list);
  CHECK(n == 2);
  if (n == 2) {
    CHECK_STR(list[0].name, "shop-redis");
    CHECK_STR(list[0].image, "redis:7-alpine");
    CHECK(list[0].port_count == 1 && list[0].ports[0].public_port == 6379 && list[0].ports[0].private_port == 6379);
    CHECK_STR(list[0].ports[0].ip, "0.0.0.0");
    CHECK(list[0].created == 116444736000000000ULL + 1700000000ULL * 10000000ULL);
    CHECK_STR(list[1].name, "");
    CHECK(list[1].port_count == 0);
  }
  free(list);
  CHECK(docker_parse_containers("{}", 2, &list) == -1 && list == NULL);
  CHECK(docker_parse_containers("[]", 2, &list) == 0 && list == NULL);
}

void test_engine_pipes(void) {
  wchar_t pipe[260];
  CHECK(engine_pipe_from_host("npipe:////./pipe/docker_engine", pipe, 260) && wcscmp(pipe, L"\\\\.\\pipe\\docker_engine") == 0);
  CHECK(engine_pipe_from_host("npipe://./pipe/podman-machine-default", pipe, 260) &&
        wcscmp(pipe, L"\\\\.\\pipe\\podman-machine-default") == 0);
  CHECK(!engine_pipe_from_host("tcp://127.0.0.1:2375", pipe, 260));
  CHECK(!engine_pipe_from_host("npipe:////./pipe/", pipe, 260));
  CHECK(!engine_pipe_from_host(NULL, pipe, 260));
}
