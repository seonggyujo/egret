# egret 구조

PC에서 리슨 중인 TCP 포트를 보여 주고 그 포트를 연 프로그램을 끄는 트레이 앱이다. 순수 C(C11)로 쓰고
Win32, Direct2D, DirectWrite만 쓴다. 외부 라이브러리는 없다. 한 파일은 한 가지 일만 맡고 약 150줄을 넘지
않는다. 각 파일 맨 위에 맡은 일을 적는다.

같은 소스에서 프로그램 두 개가 나온다.

- `egret.exe`: 트레이 앱. Release는 창 프로그램, Debug는 콘솔 프로그램(로그가 보인다). CLI 인자를 받으면
  띄운 터미널의 콘솔을 빌려 CLI로 돈다. `--admin-stop`이면 UAC로 다시 실행된 관리자 도우미다.
- `egret.com`: CLI. 확장자 없이 `egret`을 치면 cmd와 PowerShell은 PATHEXT 순서대로 `.com`을 먼저 고른다.
  인자 없이 실행하면 옆의 `egret.exe`를 띄우고 끝난다.

## 의존 방향

```
main.c     → ui/  → actions/ → docker/ → model/ → base/
main_cli.c → cli/ ↗          → app/    ↗
                  → scan/ → probe/  → model/
                          → docker/
             ui/ → render/ → base/
             ui/, cli/ → app/(설정, 문구, 테마, 단축키 글자, 시작 시 실행, PATH) → base/
```

- 창(HWND)과 Windows 메시지는 `ui/`만 안다. 아래 모듈은 `ui/`를 모른다.
- `model/`은 Win32를 모르는 순수 계산이다. `probe/`와 `docker/`는 `model/raw.h`의 데이터 모양으로 결과를 채우고,
  `model/rows.h`가 그것을 줄(행)로 바꾼다. 테스트(`tests/`)는 창 없이 `egret_core` 라이브러리만으로 빌드된다.
- 느린 일은 `ui/`가 다른 스레드에서 돌린다. 스캔은 `ui/scanner.c`(2초마다, 팝업을 열거나 끈 뒤에는 바로),
  끄기는 `ui/stopper.c`(컨테이너는 10초, 서비스는 20초, UAC는 사용자가 누를 때까지 걸릴 수 있다).
- Direct2D와 DirectWrite 타입은 `render/` 안에만 있다. 밖에는 `canvas.h`, `path.h`, `ui_text.h`의 불투명 API만
  나간다.
- `*_parts.h`, `rows_parts.h`, `stop_parts.h`, `cli_parts.h`, `pipe_http.h`, `engine_pipes.h`, `process_cwd.h`,
  `strings_tables.h`, `canvas_d2d.h`, `fonts.h`는 같은 폴더(와 테스트) 안에서만 include한다.

## `base/` 공용 도구

| 경로 | 역할 |
| --- | --- |
| `log.c/h` | 레벨별 로그(stderr, 콘솔이면 UTF-16). `EGRET_LOG`(error, warn, info, debug, trace)로 레벨 변경 |
| `mem.c/h` | NULL을 돌려주지 않는 할당 |
| `buf.c/h` | 늘어나는 바이트 버퍼(도커 응답) |
| `utf.c/h` | UTF-8과 UTF-16 변환 |
| `args.c/h` | 명령줄을 Windows 규칙으로 나눠 UTF-8로 |
| `clock.c/h` | 지금 시각(FILETIME 단위, 프로세스 시작 시각과 같은 단위) |
| `box.h` | 정수 픽셀 사각형과 점 포함 검사 |
| `names.c/h` | 프로그램 이름과 경로를 ASCII 대소문자 구분 없이 비교, 접두, 포함, 폴더 안인지 |
| `json.c/h`, `json_parse.c`, `json_scalars.c`, `json_reader.h` | 엄격한 JSON 읽기(RFC 8259, heron-c에서 가져와 읽기만 남김) |

## `model/` 줄 만들기 (순수 계산)

| 경로 | 역할 |
| --- | --- |
| `raw.c/h` | 한 번의 조사 결과: 리슨 소켓, 프로세스 스냅샷, 리슨 프로세스의 경로와 시작 시각과 작업 폴더, 서비스, 컨테이너 |
| `listener.c/h` | 줄 하나(포트, PID, 종류, v4/v6, LAN, heron-c, 이름, 폴더, 서비스, 컨테이너, 시작 시각)와 줄 목록 |
| `rows.c/h`, `rows_classify.c`, `rows_parts.h` | 조사 결과를 줄로: (포트, PID)마다 한 줄로 v4/v6 합치기, 0.0.0.0이나 ::면 LAN, 도커 전달 프로세스가 연 포트는 그 컨테이너, `wslrelay.exe`는 WSL, PID 0/4와 Windows 폴더 프로그램은 시스템, 열 수 없는 프로세스는 자신이나 부모가 Windows 폴더 밖 프로그램의 서비스면 서비스(로컬 DB), 아니면 시스템. 조상에 `heron-c.exe`가 있으면 heron-c 표시. 작업 폴더가 Windows 폴더 안이면 프로그램 폴더를 보인다 |
| `filter.c/h` | 기본으로 보이는 줄(시스템과 목록에서 뺀 프로그램 제외), 검색어(포트 앞자리, 이름, 폴더, 서비스, 이미지) |
| `text.c/h` | 실행 시간("41s", "2h 13m", "3d 4h"), 홈 폴더를 `~`로 줄인 경로 |

## `probe/` 이 PC 조사

| 경로 | 역할 |
| --- | --- |
| `sockets.c/h` | `GetExtendedTcpTable(TCP_TABLE_OWNER_PID_LISTENER)`로 IPv4와 IPv6 리슨 소켓과 PID |
| `processes.c/h` | Toolhelp 스냅샷: 모든 프로세스의 PID, 부모, 실행 파일 이름(권한 없이도 읽힌다) |
| `process_info.c/h` | 프로세스 열기 두 번: 제한된 조회 권한으로 경로와 시작 시각, 메모리 읽기 권한으로 작업 폴더 |
| `process_cwd.c/h` | PEB에서 `CurrentDirectory.DosPath` 읽기(x64와 WOW64 32비트) |
| `services.c/h` | 리슨 프로세스(와 부모)에서 도는 서비스와 그 실행 파일 경로(서비스 관리자) |
| `service_program.c/h` | 서비스 명령줄에서 실행 파일 경로 잘라 내기(따옴표 있든 없든) |

## `docker/` Docker Engine API

| 경로 | 역할 |
| --- | --- |
| `engine_pipes.c/h` | 시도할 파이프 순서: `DOCKER_HOST`(npipe://), `\\.\pipe\docker_engine`, `\\.\pipe\dockerDesktopLinuxEngine` |
| `pipe_http.c/h` | named pipe 위 HTTP/1.1, 겹친 I/O와 마감 시간(엔진이 멈춰도 스캔이 늦어지지 않게) |
| `http.c/h` | HTTP 응답 해석: 상태, 헤더, Content-Length, chunked, 끝까지 |
| `containers_json.c/h` | `GET /containers/json`을 컨테이너 목록으로(이름, 이미지, 생성 시각, 공개 TCP 포트) |
| `docker.c/h` | 컨테이너 목록과 멈추기(`POST /containers/{id}/stop?t=10`). 엔진이 없으면 조용히 건너뛴다 |

## `scan/`

| 경로 | 역할 |
| --- | --- |
| `scan.c/h` | 한 번 조사: 소켓, 스냅샷, 리슨 프로세스 정보, 서비스, 도커, 그다음 `model/rows.h`. 어느 스레드에서나 부를 수 있다 |

## `actions/` 끄기와 열기

| 경로 | 역할 |
| --- | --- |
| `stop.c/h`, `stop_parts.h` | 종류별로 끄는 방법 고르기: 시스템과 WSL은 막고, 컨테이너는 도커로, 서비스는 서비스 관리자로, 나머지는 프로세스 종료 |
| `stop_process.c` | `TerminateProcess`. 그 전에 같은 프로그램인지(이름, 시작 시각) 확인해서 PID 재사용을 피한다. 3초 기다림 |
| `stop_service.c` | 서비스 멈추기(관리자 권한이 없으면 거부되어 "관리자 권한 필요") |
| `stop_admin.c` | UAC로 `egret.exe --admin-stop <pid> [서비스]`를 실행하고 결과를 종료 코드로 받기, 그리고 그 관리자 쪽 |
| `open.c/h` | `http://localhost:<포트>`를 기본 브라우저로, 폴더를 탐색기로 |

## `app/` 설정과 문구

| 경로 | 역할 |
| --- | --- |
| `prefs.c/h` | `%APPDATA%\egret\settings.ini`: 테마, 언어, 단축키, 전부 보기, 목록에서 뺀 프로그램 |
| `strings.c/h`, `strings_ko.c`, `strings_en.c`, `strings_tables.h` | 화면과 CLI의 글자 전부(한국어, 영어). 고른 적 없으면 Windows 표시 언어 |
| `theme.c/h` | 라이트와 다크 팔레트, 시스템 따라가기, 작업 표시줄 밝기(트레이 아이콘용) |
| `hotkey.c/h` | 단축키 글자("Ctrl+Alt+P")와 `RegisterHotKey` 값 사이 변환 |
| `hidden.c/h` | 목록에서 뺀 프로그램(exe 이름) |
| `autostart.c/h` | Windows 시작 시 실행: `HKCU\...\Run`의 `egret` 값(`egret.exe --tray`) |
| `user_path.c/h`, `path_list.c/h` | 사용자 PATH(`HKCU\Environment`)에 egret 폴더 넣고 빼기, `WM_SETTINGCHANGE` 알림 |
| `exe_path.c/h` | 이 프로그램과 옆의 `egret.exe` 경로 |
| `version.h` | 버전 |

## `cli/`

| 경로 | 역할 |
| --- | --- |
| `cli.c/h`, `cli_parts.h` | 인자를 보고 할 일 고르기, `egret.exe`일 때 터미널 콘솔 빌리기 |
| `args.c/h` | 인자 읽기(`--list`, `--all`, `-y`, 포트 1~65535) |
| `out.c/h` | 콘솔이면 UTF-16, 파이프나 파일이면 UTF-8로 쓰고 읽기(PowerShell이 붙이는 BOM은 버린다) |
| `columns.c/h` | 표 칸 폭(한글은 두 칸) |
| `list.c` | `--list` 표: PORT PID PROCESS UP FOLDER TAGS |
| `stop_ports.c` | 포트 끄기: 대상 보여 주기, `-y`가 없으면 묻기, 끄기, 결과. 종료 코드 0, 1(일부 실패), 2(인자 오류나 리슨 없음), 3(취소) |

## `render/` 그리기 (Direct2D, DirectWrite)

| 경로 | 역할 |
| --- | --- |
| `canvas.c/h`, `canvas_d2d.h` | GDI DC에 묶는 Direct2D 렌더 타깃: 창의 paint DC(자식 창인 검색 입력란 위를 덮지 않는다) 또는 투명 비트맵(트레이 아이콘). 화면 밖에서 그려 한 번에 옮긴다 |
| `shapes.c` | 사각형, 둥근 사각형, 원, 자르기 |
| `path.c/h` | 숫자로 적은 벡터 경로(이동, 직선, 3차 곡선, 원호, 닫기) 채우기와 둥근 선 그리기 |
| `fonts.c/h` | DirectWrite 팩토리 |
| `ui_text.c/h` | 한 줄 글자, 세로 가운데, 넘치면 말줄임(heron-c에서 가져옴) |

## `ui/` 트레이와 팝업

| 경로 | 역할 |
| --- | --- |
| `window.c/h` | 팝업 창 클래스와 만들기, 메시지를 담당 파일로 넘기는 스위치 |
| `app.c/h` | 앱 상태(DPI에 맞춘 크기와 글꼴, 줄, 보이는 줄, 선택, 확인 대기, 트레이, 단축키), 상태 표시줄 알림 |
| `single.c/h` | 한 번만 실행: 두 번째 실행은 떠 있는 egret에 팝업을 띄우라고 하고 끝난다. Debug는 이름을 따로 쓴다 |
| `popup.c/h` | 마우스가 있는 화면의 작업 영역 오른쪽 아래에 내용만큼의 높이로 띄우기, 포커스를 잃거나 Esc면 숨기기, Windows 11 둥근 모서리와 그림자, 모니터 DPI |
| `tray.c/h`, `tray_icon.c/h` | 트레이 아이콘(서버 개수를 노란 사각형에 숫자로, 0개면 백로 마크), 툴팁, 왼쪽 클릭 토글, 오른쪽 클릭 메뉴, 알림 |
| `hotkey_reg.c/h` | 전역 단축키 등록, 바꾸기, 새 조합을 받는 동안 잠시 해제 |
| `scanner.c/h` | 스캔 스레드 |
| `stopper.c/h` | 끄기 스레드(한 번에 하나), 결과는 상태 표시줄이나(팝업이 닫혔으면) 알림으로 |
| `rows_view.c/h` | 보이는 줄 거르기, 스캔이 바뀌어도 같은 포트와 PID에 선택 유지, 트레이 개수 |
| `search.c/h` | 검색 입력란(Windows EDIT 컨트롤, IME와 붙여넣기가 그대로 된다). 포커스를 갖고 목록 키를 넘긴다 |
| `keys.c/h` | 위아래로 고르기, Enter 브라우저(확인 중이면 끄기), Del 두 번 끄기, Esc 취소, 검색 지우기, 숨기기 |
| `mouse.c/h`, `list_input.c/h` | 무엇 위에 있는지, 클릭, 더블클릭(브라우저), 오른쪽 클릭 메뉴, 휠 |
| `row_actions.c/h`, `row_menu.c` | 브라우저, 폴더, 두 번 눌러 끄기(노란 확인 줄, 3초 지나면 취소), 서비스는 UAC, 오른쪽 클릭 메뉴(포트 복사, 목록에서 빼기) |
| `layout.c/h` | 목록 화면의 상자들(그리기, 마우스, 툴팁이 같이 쓴다) |
| `paint.c/h`, `list_paint.c/h`, `row_paint.c`, `row_texts.c/h`, `status.c/h` | 그리기: 머리(백로 마크, 검색, 톱니), 열 제목, 줄(포트, 프로세스, 폴더, 표시, 실행 시간, 또는 버튼, 또는 노란 확인), 빈 화면, 스크롤 막대, 상태 표시줄과 링크 |
| `widgets.c/h`, `icons.c/h` | 아이콘 버튼, 글자 버튼, 스위치, 선택 묶음, 칩, 링크. 아이콘과 백로 마크는 SVG에서 옮긴 벡터 경로(글꼴 기호나 이모지 없음) |
| `settings_layout.c/h`, `settings_paint.c/h`, `settings_input.c/h` | 설정 화면: 테마, 언어, 단축키(눌러서 바꾸기), Windows 시작 시 실행, 터미널에서 egret 명령 쓰기, 목록에서 뺀 프로그램 |
| `tooltip.c/h` | 아이콘 버튼, LAN과 WSL과 heron-c 표시, 잘린 폴더의 전체 경로 |
| `clipboard.c/h` | 포트 번호 복사 |

## 테스트 `tests/`

`egret-tests.exe`는 `egret_core`(창 없는 모듈)만으로 빌드한다.

| 경로 | 역할 |
| --- | --- |
| `check.h`, `test_main.c` | 작은 테스트 틀(heron-c와 같음) |
| `raw_fixture.h` | 손으로 만드는 조사 결과 |
| `test_rows.c` | v4/v6 합치기, LAN, 정렬, heron-c, 시스템, 서비스(postgres 사례), 컨테이너와 WSL, Windows 폴더 안 작업 폴더 |
| `test_filter_text.c` | 보이는 줄, 검색, 실행 시간, 짧은 경로 |
| `test_docker.c` | HTTP 해석, 컨테이너 JSON, 엔진 파이프 이름 |
| `test_app.c` | 단축키 글자, 목록에서 뺀 프로그램, PATH 목록 고치기 |
| `test_cli.c` | 인자, 칸 폭, 서비스 실행 파일 경로 |

## 로그

- Debug 빌드는 콘솔 프로그램이라 띄운 터미널에 로그가 나온다. 기본 레벨은 debug.
- Release는 창 프로그램이고 기본 레벨은 warn. CLI는 error만(출력이 섞이지 않게). `EGRET_LOG`로 바꾼다.
