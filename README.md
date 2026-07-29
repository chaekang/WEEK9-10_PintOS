# PintOS Threads & User Program

> x86-64 교육용 운영체제 Pintos의 스레드 스케줄링과 사용자 프로그램 실행 환경을 구현한 프로젝트

이 저장소는 KAIST Pintos의 Threads와 User Program 과제를 다룹니다. 타이머 기반 스레드 제어, 우선순위 스케줄링, MLFQS, 프로세스 생명주기와 시스템 콜을 구현하며 커널의 실행 흐름과 동기화 구조를 학습했습니다.

## 프로젝트 범위

| 영역 | 구현 내용 |
| --- | --- |
| Threads | Alarm Clock, Priority Scheduling, Priority Donation, MLFQS |
| User Program | Argument Passing, System Call, Process `exec`·`wait`·`exit` |
| File I/O | 파일 생성·열기·읽기·쓰기·닫기와 파일 디스크립터 관리 |
| Environment | C, QEMU, GCC, GNU Make, Docker |

## 핵심 구현

### CPU 사용 이력 기반 동적 우선순위 스케줄링

- 모든 스레드를 실행 상태와 관계없이 추적하고 `recent_cpu`, `load_avg`, `nice`를 이용해 우선순위를 계산했습니다.
- CPU 사용 이력은 1초마다, 우선순위는 4 tick마다 갱신하고 계산 직후 ready queue를 다시 정렬했습니다.
- 동일 조건의 작업에는 CPU를 균등하게 배분하고, `nice` 값이 높을수록 실행 기회가 단계적으로 줄어들도록 구성했습니다.
- 잠금 대기 중인 스레드도 CPU 사용 이력이 감쇠해 잠금 해제 후 실제 부하에 맞는 우선순위를 회복하도록 했습니다.

### 우선순위 기반 동기화

- semaphore, lock, condition variable의 대기열을 우선순위 순서로 관리했습니다.
- 중첩된 lock 대기에서는 우선순위가 연쇄적으로 전달되도록 donation 경로를 구성했습니다.
- lock 해제 시 해당 lock의 donation만 제거하고, 남은 donation과 원래 우선순위를 기준으로 현재 우선순위를 복구했습니다.

### 프로세스와 시스템 콜

- 사용자 프로그램의 인자를 ABI에 맞게 stack에 배치하고 ELF 실행 흐름을 연결했습니다.
- `exec`, `wait`, `exit`의 프로세스 실행·대기·종료 흐름과 상태 전달을 구현했습니다.
- 사용자 주소를 검증한 뒤 파일 시스템 호출을 수행하고, 프로세스별 파일 디스크립터의 생명주기를 관리했습니다.

## 검증

Pintos가 제공하는 Threads 테스트를 기준으로 Alarm Clock부터 MLFQS까지 검증했습니다. 프로젝트 회고와 팀 작업 방식은 [`Presentation/Week9_Presentation.md`](./Presentation/Week9_Presentation.md)에서 확인할 수 있습니다.

## 실행

```bash
cd pintos/threads
make
cd build
pintos -- run alarm-multiple
```

User Program 테스트는 다음과 같이 실행합니다.

```bash
cd pintos/userprog
make
cd build
make check
```

## 저장소 구조

```text
.
├── pintos/
│   ├── threads/       # 스레드와 스케줄러
│   ├── userprog/      # 프로세스와 시스템 콜
│   ├── filesys/       # 파일 시스템
│   └── tests/         # Pintos 테스트
└── Presentation/      # 팀 회고와 발표 자료
```

