SkyOS++
----------------

SkyOS\++은 'C++로 나만의 운영체제 개발하기'에서 소개한 SkyOS32의  
바통을 이어받아 개발진행중인 프로젝트입니다.  
그래서 이 저장소에 오신분은 SkyOS32에 대해 이미 알고 있다고 가정합니다.  

프로젝트 목표
-------

개발을 WIN32에서 진행하기 위해 하드웨어 종속적인 부분과 순수 프로그래밍 영역 분리  
WIN32 / SKYOS++ 두 플랫폼에서 동시에 동작할 수 있는 커널 시스템 개발  

개발방법
-------
SKYOS32 프로젝트  
WIN32로 개발할시에는 전처리기에 SKY_EMULATOR를 추가한다.  
에뮬레이터나 실기에 배포할시에는 SKY_EMULATOR 전처리기를 빼고 빌드한다.  

LUA5 프로젝트  
WIN32로 개발할시에는 전처리기에 SKY_DLL;LUA_BUILD_AS_DLL을 추가한다.  
에뮬레이터나 실기에 배포할시에는 SKY_DLL;LUA_BUILD_AS_DLL을 빼고 빌드한다.  

가상 이미지 파일
-------
[구글드라이브](https://drive.google.com/drive/folders/1uq7IkAAjByu1663FjnKhTRfYMb0CV8Or?usp=sharing)

WIN32 디버깅
-------
![Alt text](./Img/win32_dev.jpg)