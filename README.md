# egret

PC에서 리슨 중인 서버(dev 서버, 로컬 DB, 도커 컨테이너)를 한눈에 보고 바로 끄는 Windows 트레이 앱입니다.
순수 C(C11)로 썼고 Win32, Direct2D, DirectWrite만 씁니다. 외부 라이브러리는 없습니다.

## 내려받기

[Releases](https://github.com/seonggyujo/egret/releases)에서 zip을 받아 아무 폴더에 풀고 `egret.exe`를 실행합니다.
설치 파일은 없습니다. `egret.exe`(트레이 앱)와 `egret.com`(CLI)은 같은 폴더에 둡니다.
Windows 10 1703 이상(x64)이 필요하고, Windows 11에서 확인했습니다.
서명하지 않은 exe라 처음 실행할 때 SmartScreen이 경고하면 "추가 정보"에서 "실행"을 누릅니다.

## 빌드

필요한 것: MinGW-w64 gcc(UCRT), CMake, Ninja. 예를 들어 Strawberry Perl에 들어 있는 gcc 13으로 빌드합니다.

```sh
cmake -S . -B build -G Ninja                       # Debug: egret.exe가 콘솔 프로그램이라 로그가 보임
cmake --build build
./build/egret-tests.exe                            # 단위 테스트

cmake -S . -B build-release -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build-release                        # egret.exe(트레이 앱)와 egret.com(CLI)
```

`egret.exe`와 `egret.com`은 같은 폴더에 둡니다. `.c` 파일을 추가하면 CMake를 다시 실행해서 목록을 갱신합니다.
clangd는 `build/compile_commands.json`과 `.clangd`(MinGW 대상 지정)를 읽습니다.

## 쓰기

`egret.exe`를 실행하면 트레이에 상주하고 팝업이 한 번 뜹니다. 이미 떠 있으면 그 팝업을 띄우고 끝납니다.

- **Ctrl+Alt+P**(설정에서 바꿈)나 트레이 아이콘 클릭으로 팝업을 열고 닫습니다. Esc나 다른 곳을 누르면 숨습니다.
- 팝업 맨 위는 검색창입니다. 포트, 이름, 폴더로 거릅니다. `51`을 치면 5173만 남습니다.
- 줄마다 포트, 프로세스, 실행 폴더, 표시, 실행 시간이 나옵니다. 같은 포트와 PID의 IPv4, IPv6는 한 줄입니다.
  - `heron-c`: heron-c 터미널에서 띄운 서버
  - `docker`: 도커 컨테이너가 공개한 포트(컨테이너 이름과 이미지)
  - `서비스`: Windows 서비스(로컬 DB 등). 끄려면 관리자 권한이 필요해서 UAC 창이 뜹니다
  - `LAN`: 0.0.0.0이나 ::에 열려 있어 같은 네트워크의 다른 기기도 접속할 수 있는 포트
  - `WSL`: WSL 안의 서버를 넘겨 주는 `wslrelay.exe`. 끄면 그 배포판의 포워딩이 전부 끊겨서 막아 둡니다
- 마우스를 올린 줄에 브라우저로 열기(`http://localhost:포트`), 폴더 열기, 끄기 버튼이 나옵니다.
- 끄기는 두 번 누릅니다. 처음 누르면 줄이 노랗게 바뀌고, 한 번 더 누르면 꺼집니다. 3초 안에 안 누르면 취소됩니다.
- 시스템 포트(PID 0과 4, Windows 폴더 안 프로그램, 열 수 없는 프로세스)는 기본으로 숨깁니다. 아래 "보기"로 전부 봅니다.
- 줄을 오른쪽 클릭하면 포트 번호 복사, 목록에서 빼기(그 프로그램을 계속 숨김)가 있습니다.
- 트레이 아이콘에 기본으로 보이는 서버 개수가 나옵니다(2초마다 갱신).

| 키 | 동작 |
| --- | --- |
| 위, 아래, PageUp, PageDown | 줄 고르기 |
| Enter | 고른 줄을 브라우저로 열기, 노란 줄이면 끄기 |
| Del | 고른 줄 끄기(두 번) |
| Esc | 끄기 취소, 그다음 검색 지우기, 그다음 숨기기 |

Windows 11은 새 트레이 아이콘을 작업 표시줄 `^` 안에 넣습니다. 늘 보이게 하려면 설정 > 개인 설정 > 작업 표시줄 >
기타 시스템 트레이 아이콘에서 egret을 켭니다.

## CLI

```sh
egret --list            # 리슨 중인 포트 (--all: 시스템 포트까지)
egret 3000 5173         # 그 포트를 연 프로그램 끄기, 먼저 물어봄
egret 3000 -y           # 묻지 않고 끄기
egret --help
```

출력은 UTF-8입니다(콘솔에는 UTF-16으로 써서 코드 페이지와 상관없이 한글이 보입니다).
종료 코드: 0 완료, 1 일부를 끄지 못함, 2 인자 오류나 리슨 중인 프로그램 없음, 3 취소.
어느 폴더에서나 `egret`을 쓰려면 설정의 "터미널에서 egret 명령 쓰기"를 켭니다(사용자 PATH에 egret 폴더를 넣음).

## 설정

톱니 버튼으로 엽니다. `%APPDATA%\egret\settings.ini`에 저장합니다.

- 테마: 시스템(Windows 앱 모드를 따름), 라이트, 다크
- 언어: 한국어, English(고른 적 없으면 Windows 표시 언어)
- 열기 단축키: 누른 뒤 새 조합을 누름. 다른 프로그램이 쓰고 있으면 알려 줍니다
- Windows 시작할 때 실행
- 터미널에서 egret 명령 쓰기
- 목록에서 뺀 프로그램: x로 다시 보이게

구조는 [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md)에 있습니다.

## 라이선스

MIT. [LICENSE](LICENSE)를 보세요.
