// Choosing how to stop a row: by its kind.
#include "actions/stop.h"

#include "actions/stop_parts.h"
#include "docker/docker.h"

bool stop_needs_admin(const Listener *l) { return l->kind == KIND_SERVICE; }

static StopResult from_docker(DockerResult r) {
  switch (r) {
    case DOCKER_OK: return STOP_OK;
    case DOCKER_GONE: return STOP_GONE;
    case DOCKER_NO_ENGINE: return STOP_NO_ENGINE;
    default: return STOP_FAILED;
  }
}

StopResult stop_listener(const Listener *l) {
  switch (l->kind) {
    case KIND_SYSTEM:
    case KIND_WSL: return STOP_BLOCKED;
    case KIND_CONTAINER: return from_docker(docker_stop(l->container_id));
    case KIND_SERVICE: return l->service[0] ? stop_service(l->service) : stop_process(l->pid, l->exe, l->started);
    default: return stop_process(l->pid, l->exe, l->started);
  }
}
