# [MBTI] Signal

> 작성일자: 2020년 11월 22일

> 김상재 (hestarium@konkuk.ac.kr)

간단하게 사용방법만 적어놓고 이 프로젝트를 종료합니다.

## 1. 옵션 적용 방법

1. mbti/CMakeLists.txt 에 다음과 같은 문장을 수정하여 적용하면 됨.

    ```cmake
    add_definitions(-DUSER_EXEC)
    ```

    해당 문구는 gcc의 -D 옵션과 동일하게 사용하면 됨.


## 2. 옵션 종류

1. USER_EXEC
- USER_EXEC이 적용되어있을 경우 5초 이후에 자동으로 실험이 시작됨.
- USER_EXEC이 미적용되어있을 경우 부모 Process에 SIGCONT를 줄 때 실험이 시작됨.

2. __DEBUGMSG, __ESSENTIAL
- 추가적인 Log 출력에 대한 Option

3. CASE12, CASEXX
- XX에 숫자가 들어가며 각 System Test의 성공실패여부를 판단할 수 있는 출력문이 출력됨.

