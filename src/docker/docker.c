// Listing and stopping containers through the first engine pipe that
// answers.
#include "docker/docker.h"

#include <stdio.h>
#include <string.h>

#include "base/log.h"
#include "docker/containers_json.h"
#include "docker/engine_pipes.h"
#include "docker/pipe_http.h"

#define LIST_TIMEOUT_MS 1000
#define STOP_TIMEOUT_MS 15000

// Sends request to each engine pipe in turn until one is served.
static PipeResult ask_engine(const char *request, int timeout_ms, HttpResponse *out) {
  wchar_t pipes[ENGINE_PIPES_MAX][260];
  int n = engine_pipes(pipes);
  PipeResult r = PIPE_NO_SERVER;
  for (int i = 0; i < n && r == PIPE_NO_SERVER; i++) {
    r = pipe_http_exchange(pipes[i], request, strlen(request), timeout_ms, out);
  }
  return r;
}

bool docker_list(RawScan *raw) {
  static const char REQUEST[] =
      "GET /containers/json HTTP/1.1\r\nHost: docker\r\nUser-Agent: egret\r\nConnection: close\r\n\r\n";
  HttpResponse resp = {0};
  bool ok = ask_engine(REQUEST, LIST_TIMEOUT_MS, &resp) == PIPE_OK && resp.status == 200;
  if (ok) {
    int n = docker_parse_containers(resp.body.data, resp.body.len, &raw->containers);
    ok = n >= 0;
    raw->container_count = ok ? n : 0;
    log_debug("docker", "%d running containers", raw->container_count);
  }
  http_response_free(&resp);
  return ok;
}

// Container ids are hex; anything else never reaches the request line.
static bool valid_id(const char *id) {
  size_t n = strspn(id, "0123456789abcdef");
  return n > 0 && id[n] == 0 && n <= 64;
}

DockerResult docker_stop(const char *id) {
  if (!valid_id(id)) return DOCKER_FAILED;
  char request[256];
  snprintf(request, sizeof request,
           "POST /containers/%s/stop?t=10 HTTP/1.1\r\nHost: docker\r\nUser-Agent: egret\r\n"
           "Content-Length: 0\r\nConnection: close\r\n\r\n",
           id);
  HttpResponse resp = {0};
  PipeResult r = ask_engine(request, STOP_TIMEOUT_MS, &resp);
  int status = resp.status;
  http_response_free(&resp);
  log_info("docker", "stop %.12s: pipe %d, status %d", id, (int)r, status);
  if (r == PIPE_NO_SERVER) return DOCKER_NO_ENGINE;
  if (r != PIPE_OK) return DOCKER_FAILED;
  if (status == 204) return DOCKER_OK;
  if (status == 304 || status == 404) return DOCKER_GONE;
  return DOCKER_FAILED;
}
