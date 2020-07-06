# Microbench
실시간 시스템 콜 분야별 측정

# 측정 분야
- Signal: 김상재

- Mutex/Semaphore: 이종빈, 장서연
 
- IPC: 박성준
  application code compile <br>
  ``` $gcc -o <file_name> <file_name>.c -lrt ``` <br>
    (Signal을 제외한 나머지 환경은 Single Producer Single Consumer 환경에서 측정 진행)

# 측정 환경
- OS: Ubuntu 20.04 LTS
- Kernel Version: 5.4.0-37-generic
- CPU Core/Threads: 2-Cores, 4-Threads

# Git 사용 규칙
- 각자 알아서 브랜치 만들어서 작업하고 master로 push ㄱ
- 그래도 commit message 알아볼수 있게는 만들기
