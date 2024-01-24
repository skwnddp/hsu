#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#pragma region INITIAL_DEFINE_SETTINGS
// 창 크기 정의
#define WIDTH 40
#define HEIGHT 30
//키 입력 정의
#define UP  0x48
#define DOWN 0x50
#define LEFT 0x4b
#define RIGHT 0x4d
#define UP2		'w'
#define UP3		'W'
#define DOWN2	's'
#define LEFT2	'a'
#define LEFT3	'A'
#define RIGHT2	'd'
#define RIGHT3	'd'
#define ESC 0x1b
#define SPECIAL1 0xe0 // 특수키는 0xe0 + key 값으로 구성된다.
#define SPECIAL2 0x00 // keypad 경우 0x00 + key 로 구성된다.
// 색상 정의
#define BLACK	0
#define BLUE1	1
#define GREEN1	2
#define CYAN1	3
#define RED1	4
#define MAGENTA1 5
#define YELLOW1	6
#define GRAY1	7 // ~ 진한 계열
#define GRAY2	8 // 밝은 계열 ~
#define BLUE2	9
#define GREEN2	10
#define CYAN2	11
#define RED2	12
#define MAGENTA2 13
#define YELLOW2	14
#define WHITE	15
#pragma endregion

int hidden_index; // Hidden 화면 번호 0 or 1
HANDLE scr_handle[2]; // 화면 버퍼 변수
long long frame_count = 0;

void removeCursor(void)
{
	CONSOLE_CURSOR_INFO curInfo;
	GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
	curInfo.bVisible = 0;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
}

//버퍼 구현
void scr_init()
{
	int i;
	CONSOLE_CURSOR_INFO cci;

	cci.dwSize = 1;
	cci.bVisible = FALSE;
	// 화면 버퍼 2개를 만든다.
	for (i = 0; i < 2; i++) {
		scr_handle[i] = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
		// 커서를 숨긴다.
		SetConsoleCursorInfo(scr_handle[i], &cci);
	}
	// 0번 화면이 default
	SetConsoleActiveScreenBuffer(scr_handle[0]);
	hidden_index = 1; // 1번 화면이 Hidden Screen
}
void ChangeScreenSize(HANDLE hnd, COORD NewSize)
{
	//HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	SMALL_RECT DisplayArea = { 0, 0, 0, 0 };
	CONSOLE_SCREEN_BUFFER_INFO SBInfo;
	GetConsoleScreenBufferInfo(hnd, &SBInfo);
	DisplayArea.Bottom = NewSize.Y;
	DisplayArea.Right = NewSize.X;
	SetConsoleWindowInfo(hnd, 1, &DisplayArea);
}
void ChangeBuffSize(HANDLE hnd, COORD NewSize)
{
	SetConsoleScreenBufferSize(hnd, NewSize);
}
//처음 화면으로 감
void scr_switch()
{
	// hidden 을 active 로 변경한다.
	SetConsoleActiveScreenBuffer(scr_handle[hidden_index]);
	// active를 hidden으로 변경한다.
	hidden_index = !hidden_index; // 0 <-> 1 toggle
}
void scr_clear()
{
	COORD Coor = { 0, 0 };
	DWORD dw;
	// hidden screen를 clear한다.
	// WIDTH*2 * HEIGHT 값은 [속성]에서 설정한 값과 정확히 같아야 한다.
	// 즉, 화면 속성에서 너비(W)=80, 높이(H)=25라면 특수 문자는 2칸씩 이므로 WIDTH=40, HEIGHT=25이다.
	FillConsoleOutputCharacter(scr_handle[hidden_index], ' ', WIDTH * 2 * HEIGHT, Coor, &dw);
}
void scr_release()
{
	CloseHandle(scr_handle[0]);
	CloseHandle(scr_handle[1]);
}

//텍스트 컬러
void textcolor(int fg_color, int bg_color)
{
	SetConsoleTextAttribute(scr_handle[hidden_index], fg_color | bg_color << 4);
}

// 화면 지우기고 원하는 배경색으로 설정한다.
void cls(int text_color, int bg_color)
{
	char cmd[100];
	system("cls"); //화면 크기 강제로 조정한다.
	sprintf(cmd, "mode con: cols=%d lines=%d", WIDTH * 2, HEIGHT);
	system(cmd);
	sprintf(cmd, "COLOR %x%x", bg_color, text_color);
	system(cmd);
}
void gotoxy(int x, int y)
{
	DWORD dw;
	COORD CursorPosition = { x, y };
	// hidden screen에 gotoxy
	SetConsoleCursorPosition(scr_handle[hidden_index], CursorPosition);
}
void gotoxy2(int x, int y) //내가 원하는 위치로 커서 이동
{
	COORD pos = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);// WIN32API 함수입니다. 이건 알필요 없어요
}
void printscr(char* str)
{
	DWORD dw;
	// hidden screen에 gotoxy 되었다고 가정하고 print
	WriteFile(scr_handle[hidden_index], str, strlen(str), &dw, NULL);
}
//printxy 이게 진짜 출력임...
void printxy(int x, int y, char* str)
{
	DWORD dw;
	COORD CursorPosition = { x, y };
	// hidden screen에 gotoxy + print
	SetConsoleCursorPosition(scr_handle[hidden_index], CursorPosition);
	WriteFile(scr_handle[hidden_index], str, strlen(str), &dw, NULL);
}

// 테스트용 map
int map[HEIGHT][WIDTH * 2]; // 가로 길이 늘려서 강제 개행되는 블록 안 보이게~

// DrawMap()은 Hidden screen에 그려준다.
void DrawMap()
{
	int x, y, color = 0;
	scr_clear();
	for (y = 0; y < HEIGHT; y++) {
		for (x = 0; x < WIDTH; x++) {
			if (map[y][x] != 0)
				gotoxy(x * 2, y);
			switch (map[y][x]) {
			case 1://각종 블록, 플레이어 색깔
				textcolor(BLACK, WHITE);
				printscr(" ");
				break;
			case 2:
				textcolor(BLUE1, WHITE);
				printscr("■");
				break;
			case 3:
				textcolor(GREEN1, WHITE);
				printscr("■");
				break;
			case 4:
				textcolor(CYAN1, WHITE);
				printscr("■");
				break;
			case 5:		//RED1 패스하고 2번
				textcolor(RED2, WHITE);
				printscr("■");
				break;
			case 6:		//MAGENTA1 패스하고 2번
				textcolor(MAGENTA2, WHITE);
				printscr("■");
				break;
			case 7:
				textcolor(YELLOW1, WHITE);
				printscr("■");
				break;
			case 8:
				textcolor(GRAY1, WHITE);
				printscr("■");
				break;
			case 9:
				textcolor(GRAY2, WHITE);
				printscr("■");
				break;
			case 10:
				textcolor(BLUE2, WHITE);
				printscr("■");
				break;
			case 11:
				textcolor(GREEN2, WHITE);
				printscr("■");
				break;
			case 12:
				textcolor(CYAN2, WHITE);
				printscr("■");
				break;
			case 13:
				textcolor(RED2, WHITE); // 가시 히트 모션
				printscr("●");
				break;
			case 14:
				textcolor(GREEN1, WHITE); // 캐릭터 몸통
				printscr("●");
				break;
			case 15://15 ~ 20 여유분 
				textcolor(RED2, GRAY1); // 체력
				printscr("♡");
				break;
			case 16:
				textcolor(RED2, GRAY1);
				printscr("♥");
				break;
			case 17:
				textcolor(BLUE2, WHITE); // 스타 먹었을 때 쉴드 체력
				printscr("♥");
				break;
			case 18:
				textcolor(MAGENTA2, YELLOW2); // 프로그레스바 체크포인트 클로버
				printscr("♣");
				break;
			case 19:
				textcolor(CYAN1, WHITE); // 프로그레스바 대가리
				printscr("▶");
				break;
			case 20: //블록 위 아이템 텍스쳐 깜빡임
				if ((frame_count / 5) % 2 == 1) {
					textcolor(GREEN1, WHITE);
					printscr("▲");
					break;
				}
				else {
					textcolor(GREEN2, WHITE);
					printscr("▲");
					break;
				}
			case 21:
				if ((frame_count / 5) % 2 == 1) {
					textcolor(RED1, WHITE);
					printscr("♥");
					break;
				}
				else {
					textcolor(RED2, WHITE);
					printscr("♥");
					break;
				}
			case 22:
				if ((frame_count / 5) % 2 == 1) {
					textcolor(YELLOW1, WHITE);
					printscr("★");
					break;
				}
				else {
					textcolor(YELLOW2, WHITE);
					printscr("★");
					break;
				}
			case 30: //슬라이드 미니맵 동그라미 도트 깜빡임
				if ((frame_count / 5) % 2 == 1) {
					textcolor(GREEN1, WHITE);
					printscr("●");
					break;
				}
				else {
					textcolor(GREEN2, WHITE);
					printscr("●");
					break;
				}
			case 31:
				if ((frame_count / 5) % 2 == 1) {
					textcolor(GRAY2, WHITE);
					printscr("●");
					break;
				}
				else {
					textcolor(GRAY1, WHITE);
					printscr("●");
					break;
				}
			case 32:// 슬라이드 배경 깜빡임
				if ((frame_count / 10) % 2 == 1) {
					textcolor(WHITE, GRAY1);
					printscr("■");
					break;
				}
				else {
					textcolor(GRAY1, WHITE);
					printscr("■");
					break;
				}
			case 40:// 클리어 무지개 효과
				color = ((frame_count / 320) % 16);
				textcolor(color, color % 16 + 10);
				printscr("■");
				break;
			case 41:// 게임오버 그레이
				color = (frame_count / 160) % 2 + 7;
				textcolor(color, BLACK);
				printscr("■");
				break;
			}
		}
	}
}
void map_clear() {
	for (int i = 0; i < WIDTH; i++) {
		for (int j = 0; j < HEIGHT; j++) {
			map[j][i] = 0;
		}
	}
}

// 도트 픽셀 : 맵은 미니맵처럼, 나머지는 글자처럼 구현
void dot_map(int lv1, int lv2, int lv3, int lv4, int lv5, int dot_num) {
	//정방향 3개, 역방향 2개, 기다란거 1개 & 전체 스테이지 갯수 미리보기 효과
	int dot_y = 18, dot_x = 5;
	//정방향 박스 3개
	for (int i = 0; i < 3; i++) {
		map[dot_y + 0][dot_x + i * 14 + 0] = dot_num;
		map[dot_y + 0][dot_x + i * 14 + 1] = dot_num;
		map[dot_y + 0][dot_x + i * 14 + 2] = dot_num;
		//2행의 가로 3칸
		map[dot_y + 1][dot_x + i * 14 + 0] = dot_num;
		if (i == 0)
			map[dot_y + 1][dot_x + i * 14 + 1] = lv1;
		else if (i == 1)
			map[dot_y + 1][dot_x + i * 14 + 1] = lv3;
		else
			map[dot_y + 1][dot_x + i * 14 + 1] = lv5;
		map[dot_y + 1][dot_x + i * 14 + 2] = dot_num;
		//3행의 가로 3칸
		map[dot_y + 2][dot_x + i * 14 + 0] = dot_num;
		map[dot_y + 2][dot_x + i * 14 + 1] = dot_num;
		map[dot_y + 2][dot_x + i * 14 + 2] = dot_num;
		//4행의 가로 3칸	
		map[dot_y + 3][dot_x + i * 14 + 2] = dot_num;
	}
	//역방향 박스 2개
	dot_y = 23, dot_x = 12;
	for (int i = 0; i < 2; i++) {
		//1행의 가로 3칸
		map[dot_y + 0][dot_x + i * 14 + 2] = dot_num;
		//2행의 가로 3칸
		map[dot_y + 1][dot_x + i * 14 + 0] = dot_num;
		map[dot_y + 1][dot_x + i * 14 + 1] = dot_num;
		map[dot_y + 1][dot_x + i * 14 + 2] = dot_num;
		//3행의 가로 3칸
		map[dot_y + 2][dot_x + i * 14 + 0] = dot_num;
		if (i == 0)
			map[dot_y + 2][dot_x + i * 14 + 1] = lv2;
		else
			map[dot_y + 2][dot_x + i * 14 + 1] = lv4;
		map[dot_y + 2][dot_x + i * 14 + 2] = dot_num;
		//4행의 가로 3칸	
		map[dot_y + 3][dot_x + i * 14 + 0] = dot_num;
		map[dot_y + 3][dot_x + i * 14 + 1] = dot_num;
		map[dot_y + 3][dot_x + i * 14 + 2] = dot_num;
	}
	//기다란거 1개
	for (int i = 0; i < WIDTH - 4; i++) {
		map[dot_y - 1][i] = dot_num;
	}

}
//단어 도트의 x, y 피라미터
void dot_jump(int dot_x, int dot_y, int dot_num) {
	//J
	//1행의 가로 3칸
	map[dot_y + 0][dot_x + 1] = dot_num;
	map[dot_y + 0][dot_x + 2] = dot_num;
	//2행의 가로 3칸
	map[dot_y + 1][dot_x + 2] = dot_num;
	//3행의 가로 3칸
	map[dot_y + 2][dot_x + 2] = dot_num;
	//4행의 가로 3칸	
	map[dot_y + 3][dot_x + 2] = dot_num;
	//5행의 가로 3칸	
	map[dot_y + 4][dot_x + 0] = dot_num;
	map[dot_y + 4][dot_x + 1] = dot_num;
	map[dot_y + 4][dot_x + 2] = dot_num;

	//U
	//1행의 가로 3칸
	map[dot_y + 0][dot_x + 4] = dot_num;
	map[dot_y + 0][dot_x + 6] = dot_num;
	//2행의 가로 3칸
	map[dot_y + 1][dot_x + 4] = dot_num;
	map[dot_y + 1][dot_x + 6] = dot_num;
	//3행의 가로 3
	map[dot_y + 2][dot_x + 4] = dot_num;
	map[dot_y + 2][dot_x + 6] = dot_num;
	//4행의 가로 3칸	
	map[dot_y + 3][dot_x + 4] = dot_num;
	map[dot_y + 3][dot_x + 6] = dot_num;
	//5행의 가로 3칸	
	map[dot_y + 4][dot_x + 4] = dot_num;
	map[dot_y + 4][dot_x + 5] = dot_num;
	map[dot_y + 4][dot_x + 6] = dot_num;

	//M
	//1행의 가로 3칸
	map[dot_y + 0][dot_x + 8] = dot_num;
	map[dot_y + 0][dot_x + 9] = dot_num;
	map[dot_y + 0][dot_x + 10] = dot_num;
	//2행의 가로 3칸
	map[dot_y + 1][dot_x + 8] = dot_num;
	map[dot_y + 1][dot_x + 9] = dot_num;
	map[dot_y + 1][dot_x + 10] = dot_num;
	//3행의 가로 3칸
	map[dot_y + 2][dot_x + 8] = dot_num;
	map[dot_y + 2][dot_x + 10] = dot_num;
	//4행의 가로 3칸	
	map[dot_y + 3][dot_x + 8] = dot_num;
	map[dot_y + 3][dot_x + 10] = dot_num;
	//5행의 가로 3칸	
	map[dot_y + 4][dot_x + 8] = dot_num;
	map[dot_y + 4][dot_x + 10] = dot_num;

	//P
	//1행의 가로 3칸
	map[dot_y + 0][dot_x + 12] = dot_num;
	map[dot_y + 0][dot_x + 13] = dot_num;
	map[dot_y + 0][dot_x + 14] = dot_num;
	//2행의 가로 3칸
	map[dot_y + 1][dot_x + 12] = dot_num;
	map[dot_y + 1][dot_x + 14] = dot_num;
	//3행의 가로 3칸
	map[dot_y + 2][dot_x + 12] = dot_num;
	map[dot_y + 2][dot_x + 13] = dot_num;
	map[dot_y + 2][dot_x + 14] = dot_num;
	//4행의 가로 3칸	
	map[dot_y + 3][dot_x + 12] = dot_num;
	//5행의 가로 3칸	
	map[dot_y + 4][dot_x + 12] = dot_num;
}
void dot_level(int dot_x, int dot_y, int dot_num) {
	//알파벳 L
	map[dot_y + 0][dot_x + 0] = dot_num;

	//2행의 가로 3칸
	map[dot_y + 1][dot_x + 0] = dot_num;

	//3행의 가로 3칸
	map[dot_y + 2][dot_x + 0] = dot_num;

	//4행의 가로 3칸	
	map[dot_y + 3][dot_x + 0] = dot_num;

	//5행의 가로 3칸	
	map[dot_y + 4][dot_x + 0] = dot_num;
	map[dot_y + 4][dot_x + 1] = dot_num;
	map[dot_y + 4][dot_x + 2] = dot_num;

	//알파벳 E
	map[dot_y + 0][dot_x + 4] = dot_num;
	map[dot_y + 0][dot_x + 5] = dot_num;
	map[dot_y + 0][dot_x + 6] = dot_num;
	//2행의 가로 3칸
	map[dot_y + 1][dot_x + 4] = dot_num;

	//3행의 가로 3칸
	map[dot_y + 2][dot_x + 4] = dot_num;
	map[dot_y + 2][dot_x + 5] = dot_num;
	map[dot_y + 2][dot_x + 6] = dot_num;
	//4행의 가로 3칸	
	map[dot_y + 3][dot_x + 4] = dot_num;

	//5행의 가로 3칸	
	map[dot_y + 4][dot_x + 4] = dot_num;
	map[dot_y + 4][dot_x + 5] = dot_num;
	map[dot_y + 4][dot_x + 6] = dot_num;

	//알파벳 V
	map[dot_y + 0][dot_x + 8] = dot_num;

	map[dot_y + 0][dot_x + 10] = dot_num;
	//2행의 가로 3칸
	map[dot_y + 1][dot_x + 8] = dot_num;

	map[dot_y + 1][dot_x + 10] = dot_num;
	//3행의 가로 3칸
	map[dot_y + 2][dot_x + 8] = dot_num;

	map[dot_y + 2][dot_x + 10] = dot_num;
	//4행의 가로 3칸	
	map[dot_y + 3][dot_x + 8] = dot_num;

	map[dot_y + 3][dot_x + 10] = dot_num;
	//5행의 가로 3칸	

	map[dot_y + 4][dot_x + 9] = dot_num;

	//알파벳 E
	map[dot_y + 0][dot_x + 12] = dot_num;
	map[dot_y + 0][dot_x + 13] = dot_num;
	map[dot_y + 0][dot_x + 14] = dot_num;
	//2행의 가로 3칸
	map[dot_y + 1][dot_x + 12] = dot_num;

	//3행의 가로 3칸
	map[dot_y + 2][dot_x + 12] = dot_num;
	map[dot_y + 2][dot_x + 13] = dot_num;
	map[dot_y + 2][dot_x + 14] = dot_num;
	//4행의 가로 3칸	
	map[dot_y + 3][dot_x + 12] = dot_num;

	//5행의 가로 3칸	
	map[dot_y + 4][dot_x + 12] = dot_num;
	map[dot_y + 4][dot_x + 13] = dot_num;
	map[dot_y + 4][dot_x + 14] = dot_num;

	//알파벳 L
	map[dot_y + 0][dot_x + 16] = dot_num;

	//2행의 가로 3칸
	map[dot_y + 1][dot_x + 16] = dot_num;

	//3행의 가로 3칸
	map[dot_y + 2][dot_x + 16] = dot_num;

	//4행의 가로 3칸	
	map[dot_y + 3][dot_x + 16] = dot_num;

	//5행의 가로 3칸	
	map[dot_y + 4][dot_x + 16] = dot_num;
	map[dot_y + 4][dot_x + 17] = dot_num;
	map[dot_y + 4][dot_x + 18] = dot_num;
}
void dot_clear(int dot_x, int dot_y, int dot_num) {
	//알파벳 C
	map[dot_y + 0][dot_x + 0] = dot_num;
	map[dot_y + 0][dot_x + 1] = dot_num;
	map[dot_y + 0][dot_x + 2] = dot_num;
	//2행의 가로 3칸
	map[dot_y + 1][dot_x + 0] = dot_num;

	//3행의 가로 3칸
	map[dot_y + 2][dot_x + 0] = dot_num;

	//4행의 가로 3칸	
	map[dot_y + 3][dot_x + 0] = dot_num;

	//5행의 가로 3칸	
	map[dot_y + 4][dot_x + 0] = dot_num;
	map[dot_y + 4][dot_x + 1] = dot_num;
	map[dot_y + 4][dot_x + 2] = dot_num;

	//알파벳 L
	map[dot_y + 0][dot_x + 4] = dot_num;

	//2행의 가로 3칸
	map[dot_y + 1][dot_x + 4] = dot_num;

	//3행의 가로 3칸
	map[dot_y + 2][dot_x + 4] = dot_num;

	//4행의 가로 3칸	
	map[dot_y + 3][dot_x + 4] = dot_num;

	//5행의 가로 3칸	
	map[dot_y + 4][dot_x + 4] = dot_num;
	map[dot_y + 4][dot_x + 5] = dot_num;
	map[dot_y + 4][dot_x + 6] = dot_num;

	//알파벳 E
	map[dot_y + 0][dot_x + 8] = dot_num;
	map[dot_y + 0][dot_x + 9] = dot_num;
	map[dot_y + 0][dot_x + 10] = dot_num;
	//2행의 가로 3칸
	map[dot_y + 1][dot_x + 8] = dot_num;

	//3행의 가로 3칸
	map[dot_y + 2][dot_x + 8] = dot_num;
	map[dot_y + 2][dot_x + 9] = dot_num;
	map[dot_y + 2][dot_x + 10] = dot_num;
	//4행의 가로 3칸	
	map[dot_y + 3][dot_x + 8] = dot_num;

	//5행의 가로 3칸	
	map[dot_y + 4][dot_x + 8] = dot_num;
	map[dot_y + 4][dot_x + 9] = dot_num;
	map[dot_y + 4][dot_x + 10] = dot_num;

	//알파벳 A
	map[dot_y + 0][dot_x + 12] = dot_num;
	map[dot_y + 0][dot_x + 13] = dot_num;
	map[dot_y + 0][dot_x + 14] = dot_num;
	//2행의 가로 3칸
	map[dot_y + 1][dot_x + 12] = dot_num;

	map[dot_y + 1][dot_x + 14] = dot_num;
	//3행의 가로 3칸
	map[dot_y + 2][dot_x + 12] = dot_num;
	map[dot_y + 2][dot_x + 13] = dot_num;
	map[dot_y + 2][dot_x + 14] = dot_num;
	//4행의 가로 3칸	
	map[dot_y + 3][dot_x + 12] = dot_num;
	map[dot_y + 3][dot_x + 14] = dot_num;
	//5행의 가로 3칸	
	map[dot_y + 4][dot_x + 12] = dot_num;
	map[dot_y + 4][dot_x + 14] = dot_num;

	//알파벳 R
	map[dot_y + 0][dot_x + 16] = dot_num;
	map[dot_y + 0][dot_x + 17] = dot_num;
	//2행의 가로 3칸
	map[dot_y + 1][dot_x + 16] = dot_num;
	map[dot_y + 1][dot_x + 18] = dot_num;
	//3행의 가로 3칸
	map[dot_y + 2][dot_x + 16] = dot_num;
	map[dot_y + 2][dot_x + 17] = dot_num;
	//4행의 가로 3칸	
	map[dot_y + 3][dot_x + 16] = dot_num;
	map[dot_y + 3][dot_x + 18] = dot_num;
	//5행의 가로 3칸	
	map[dot_y + 4][dot_x + 16] = dot_num;
	map[dot_y + 4][dot_x + 18] = dot_num;
}
void dot_gameover(int dot_x, int dot_y, int dot_num) {
	//알파벳 G
	map[dot_y + 0][dot_x + 0] = dot_num;
	map[dot_y + 0][dot_x + 1] = dot_num;
	map[dot_y + 0][dot_x + 2] = dot_num;
	//2행의 가로 3칸
	map[dot_y + 1][dot_x + 0] = dot_num;

	//3행의 가로 3칸
	map[dot_y + 2][dot_x + 0] = dot_num;

	//4행의 가로 3칸	
	map[dot_y + 3][dot_x + 0] = dot_num;
	map[dot_y + 3][dot_x + 2] = dot_num;

	//5행의 가로 3칸	
	map[dot_y + 4][dot_x + 0] = dot_num;
	map[dot_y + 4][dot_x + 1] = dot_num;
	map[dot_y + 4][dot_x + 2] = dot_num;

	//알파벳 A
	map[dot_y + 0][dot_x + 4] = dot_num;
	map[dot_y + 0][dot_x + 5] = dot_num;
	map[dot_y + 0][dot_x + 6] = dot_num;

	//2행의 가로 3칸
	map[dot_y + 1][dot_x + 4] = dot_num;
	map[dot_y + 1][dot_x + 6] = dot_num;

	//3행의 가로 3칸
	map[dot_y + 2][dot_x + 4] = dot_num;
	map[dot_y + 2][dot_x + 5] = dot_num;
	map[dot_y + 2][dot_x + 6] = dot_num;

	//4행의 가로 3칸	
	map[dot_y + 3][dot_x + 4] = dot_num;
	map[dot_y + 3][dot_x + 6] = dot_num;

	//5행의 가로 3칸	
	map[dot_y + 4][dot_x + 4] = dot_num;
	map[dot_y + 4][dot_x + 6] = dot_num;

	//알파벳 M
	map[dot_y + 0][dot_x + 8] = dot_num;
	map[dot_y + 0][dot_x + 10] = dot_num;
	//2행의 가로 3칸
	map[dot_y + 1][dot_x + 8] = dot_num;
	map[dot_y + 1][dot_x + 9] = dot_num;
	map[dot_y + 1][dot_x + 10] = dot_num;
	//3행의 가로 3칸
	map[dot_y + 2][dot_x + 8] = dot_num;
	map[dot_y + 2][dot_x + 10] = dot_num;
	//4행의 가로 3칸	
	map[dot_y + 3][dot_x + 8] = dot_num;
	map[dot_y + 3][dot_x + 10] = dot_num;
	//5행의 가로 3칸	
	map[dot_y + 4][dot_x + 8] = dot_num;
	map[dot_y + 4][dot_x + 10] = dot_num;

	//알파벳 E 
	map[dot_y + 0][dot_x + 12] = dot_num;
	map[dot_y + 0][dot_x + 13] = dot_num;
	map[dot_y + 0][dot_x + 14] = dot_num;
	//2행의 가로 3칸
	map[dot_y + 1][dot_x + 12] = dot_num;

	//3행의 가로 3칸
	map[dot_y + 2][dot_x + 12] = dot_num;
	map[dot_y + 2][dot_x + 13] = dot_num;
	map[dot_y + 2][dot_x + 14] = dot_num;
	//4행의 가로 3칸	
	map[dot_y + 3][dot_x + 12] = dot_num;

	//5행의 가로 3칸	
	map[dot_y + 4][dot_x + 12] = dot_num;
	map[dot_y + 4][dot_x + 13] = dot_num;
	map[dot_y + 4][dot_x + 14] = dot_num; //반 칸 띄우기___

	//알파벳 O
	map[dot_y + 0][dot_x + 18] = dot_num;
	map[dot_y + 0][dot_x + 19] = dot_num;
	map[dot_y + 0][dot_x + 20] = dot_num;
	//2행의 가로 3칸
	map[dot_y + 1][dot_x + 18] = dot_num;
	map[dot_y + 1][dot_x + 20] = dot_num;
	//3행의 가로 3칸
	map[dot_y + 2][dot_x + 18] = dot_num;
	map[dot_y + 2][dot_x + 20] = dot_num;
	//4행의 가로 3칸	
	map[dot_y + 3][dot_x + 18] = dot_num;
	map[dot_y + 3][dot_x + 20] = dot_num;
	//5행의 가로 3칸	
	map[dot_y + 4][dot_x + 18] = dot_num;
	map[dot_y + 4][dot_x + 19] = dot_num;
	map[dot_y + 4][dot_x + 20] = dot_num;

	//알파벳 V
	map[dot_y + 0][dot_x + 22] = dot_num;
	map[dot_y + 0][dot_x + 24] = dot_num;
	//2행의 가로 3칸
	map[dot_y + 1][dot_x + 22] = dot_num;
	map[dot_y + 1][dot_x + 24] = dot_num;
	//3행의 가로 3칸
	map[dot_y + 2][dot_x + 22] = dot_num;
	map[dot_y + 2][dot_x + 24] = dot_num;
	//4행의 가로 3칸	
	map[dot_y + 3][dot_x + 22] = dot_num;
	map[dot_y + 3][dot_x + 24] = dot_num;
	//5행의 가로 3칸	
	map[dot_y + 4][dot_x + 23] = dot_num;

	//알파벳 E
	map[dot_y + 0][dot_x + 26] = dot_num;
	map[dot_y + 0][dot_x + 27] = dot_num;
	map[dot_y + 0][dot_x + 28] = dot_num;
	//2행의 가로 3칸
	map[dot_y + 1][dot_x + 26] = dot_num;
	//3행의 가로 3칸
	map[dot_y + 2][dot_x + 26] = dot_num;
	map[dot_y + 2][dot_x + 27] = dot_num;
	map[dot_y + 2][dot_x + 28] = dot_num;
	//4행의 가로 3칸
	map[dot_y + 3][dot_x + 26] = dot_num;
	//5행의 가로 3칸
	map[dot_y + 4][dot_x + 26] = dot_num;
	map[dot_y + 4][dot_x + 27] = dot_num;
	map[dot_y + 4][dot_x + 28] = dot_num;

	//알파벳 R
	map[dot_y + 0][dot_x + 30] = dot_num;
	map[dot_y + 0][dot_x + 31] = dot_num;
	//2행의 가로 3칸
	map[dot_y + 1][dot_x + 30] = dot_num;
	map[dot_y + 1][dot_x + 32] = dot_num;
	//3행의 가로 3칸
	map[dot_y + 2][dot_x + 30] = dot_num;
	map[dot_y + 2][dot_x + 31] = dot_num;
	//4행의 가로 3칸
	map[dot_y + 3][dot_x + 30] = dot_num;
	map[dot_y + 3][dot_x + 32] = dot_num;
	//5행의 가로 3칸
	map[dot_y + 4][dot_x + 30] = dot_num;
	map[dot_y + 4][dot_x + 32] = dot_num;
}
//숫자 도트의 y, x, 색깔 피라미터
void dot_1(int dot_x, int dot_y, int dot_num) {
	//1행의 가로 3칸
	map[dot_y + 0][dot_x + 1] = dot_num;

	//2행의 가로 3칸
	map[dot_y + 1][dot_x + 0] = dot_num;
	map[dot_y + 1][dot_x + 1] = dot_num;

	//3행의 가로 3칸
	map[dot_y + 2][dot_x + 1] = dot_num;

	//4행의 가로 3칸	
	map[dot_y + 3][dot_x + 1] = dot_num;

	//5행의 가로 3칸	
	map[dot_y + 4][dot_x + 0] = dot_num;
	map[dot_y + 4][dot_x + 1] = dot_num;
	map[dot_y + 4][dot_x + 2] = dot_num;
}
void dot_2(int dot_x, int dot_y, int dot_num) {
	//1행의 가로 3칸
	map[dot_y + 0][dot_x + 0] = dot_num;
	map[dot_y + 0][dot_x + 1] = dot_num;
	map[dot_y + 0][dot_x + 2] = dot_num;
	//2행의 가로 3칸
	map[dot_y + 1][dot_x + 2] = dot_num;
	//3행의 가로 3칸
	map[dot_y + 2][dot_x + 0] = dot_num;
	map[dot_y + 2][dot_x + 1] = dot_num;
	map[dot_y + 2][dot_x + 2] = dot_num;
	//4행의 가로 3칸	
	map[dot_y + 3][dot_x + 0] = dot_num;

	//5행의 가로 3칸	
	map[dot_y + 4][dot_x + 0] = dot_num;
	map[dot_y + 4][dot_x + 1] = dot_num;
	map[dot_y + 4][dot_x + 2] = dot_num;
}
void dot_3(int dot_x, int dot_y, int dot_num) {
	//1행의 가로 3칸
	map[dot_y + 0][dot_x + 0] = dot_num;
	map[dot_y + 0][dot_x + 1] = dot_num;
	map[dot_y + 0][dot_x + 2] = dot_num;
	//2행의 가로 3칸
	map[dot_y + 1][dot_x + 2] = dot_num;
	//3행의 가로 3칸
	map[dot_y + 2][dot_x + 0] = dot_num;
	map[dot_y + 2][dot_x + 1] = dot_num;
	map[dot_y + 2][dot_x + 2] = dot_num;
	//4행의 가로 3칸	
	map[dot_y + 3][dot_x + 2] = dot_num;
	//5행의 가로 3칸	
	map[dot_y + 4][dot_x + 0] = dot_num;
	map[dot_y + 4][dot_x + 1] = dot_num;
	map[dot_y + 4][dot_x + 2] = dot_num;
}
void dot_4(int dot_x, int dot_y, int dot_num) {
	//1행의 가로 3칸
	map[dot_y + 0][dot_x + 0] = dot_num;
	map[dot_y + 0][dot_x + 2] = dot_num;
	//2행의 가로 3칸
	map[dot_y + 1][dot_x + 0] = dot_num;
	map[dot_y + 1][dot_x + 2] = dot_num;
	//3행의 가로 3칸
	map[dot_y + 2][dot_x + 0] = dot_num;
	map[dot_y + 2][dot_x + 1] = dot_num;
	map[dot_y + 2][dot_x + 2] = dot_num;
	//4행의 가로 3칸	

	map[dot_y + 3][dot_x + 2] = dot_num;
	//5행의 가로 3칸	

	map[dot_y + 4][dot_x + 2] = dot_num;
}
void dot_5(int dot_x, int dot_y, int dot_num) {
	//1행의 가로 3칸
	map[dot_y + 0][dot_x + 0] = dot_num;
	map[dot_y + 0][dot_x + 1] = dot_num;
	map[dot_y + 0][dot_x + 2] = dot_num;
	//2행의 가로 3칸
	map[dot_y + 1][dot_x + 0] = dot_num;

	//3행의 가로 3칸
	map[dot_y + 2][dot_x + 0] = dot_num;
	map[dot_y + 2][dot_x + 1] = dot_num;
	map[dot_y + 2][dot_x + 2] = dot_num;
	//4행의 가로 3칸	
	map[dot_y + 3][dot_x + 2] = dot_num;
	//5행의 가로 3칸	
	map[dot_y + 4][dot_x + 0] = dot_num;
	map[dot_y + 4][dot_x + 1] = dot_num;
	map[dot_y + 4][dot_x + 2] = dot_num;
}

// 화면 슬라이드 & 클리어 효과
void block_slide(int delay, int nx, int ny,
	int lv1, int lv2, int lv3, int lv4, int lv5, int dot_num)
{	
	//화면 딜레이, 배열 내 제곱 상수 nx ny, 현재 레밸~클리어 여부 DrawMap 숫자, 배경 맵 숫자
	unsigned char key_slide = 0;
	int clear_x = 0, clear_y = 0, dx = 1, dy = 1;
	//화면 그리기
	while (1) {
		scr_switch();
		for (int i = 0; i < WIDTH; i++)
		{
			clear_x = (clear_x ^ nx) % WIDTH;
			clear_y = (i ^ ny) % HEIGHT;
			map[clear_y][clear_x] = dot_num;
		}
		clear_x += nx, clear_y += ny;
		for (int i = 0; i < WIDTH; i++)
		{
			clear_x = (clear_x ^ nx) % WIDTH;
			clear_y = (i ^ ny) % HEIGHT;
			map[clear_y][clear_x] = 0;
		}
		if (clear_x == WIDTH) {
			clear_x = 0;
		}

		if (kbhit()) {
			key_slide = getch();
			if (key_slide == UP || key_slide == UP2) {
				frame_count = 0;
				break;
			}
		}
		dot_level(4, 4, 3);
		//레밸 옆에 숫자 출력
		dot_map(lv1, lv2, lv3, lv4, lv5, 10);
		if (lv5 == 31) {
			DrawMap();
			textcolor(BLACK, YELLOW2);
			printxy(8, 10, "클리어 문구로 넘어갑니다");
		}
		else if (lv5 == 30) {
			dot_5(24, 4, 7);
			DrawMap();
			textcolor(CYAN1, YELLOW2);
			printxy(8, 10, "★ 스타 획득 시 쉴드를 획득합니다");
		}
		else if (lv4 == 30) {
			dot_4(24, 4, 6);
			DrawMap();
			textcolor(CYAN1, YELLOW2);
			printxy(8, 10, "★ 스타 획득 시 쉴드를 획득합니다");
		}
		else if (lv3 == 30) {
			dot_3(24, 4, 5);
			DrawMap();
			textcolor(GREEN1, YELLOW2);
			printxy(8, 10, "▲ 가시는 밟아도 없어지지 않습니다");
			textcolor(RED2, YELLOW2);
			printxy(8, 11, "♥ 하트는 획득 시 사라집니다");
		}
		else if (lv2 == 30) {
			dot_2(24, 4, 4);
			DrawMap();
			textcolor(GREEN1, YELLOW2);
			printxy(8, 10, "▲ 가시는 밟아도 없어지지 않습니다");
			textcolor(RED2, YELLOW2);
			printxy(8, 11, "♥ 하트는 획득 시 사라집니다");
		}
		else if (lv1 == 30) {
			dot_1(24, 4, 2);
			DrawMap();
			textcolor(BLUE2, YELLOW2);
			printxy(8, 10, "■ 블록을 밟으면 자동으로 점프합니다");
		}
		textcolor(MAGENTA2, YELLOW2);
		printxy(16, 28, "▲ 방향키 ↑ 혹은 W 를 입력하면 진행합니다 ... ▲");
		Sleep(delay);
		frame_count++;
	}
	//화면 사라지기
	clear_x = 0; clear_y = 0;
	while (1) {
		scr_switch();
		for (int i = 0; i < WIDTH; i++)
		{
			clear_x = (clear_x ^ nx) % WIDTH;
			clear_y = (i ^ ny) % HEIGHT;
			map[clear_y][clear_x] = 0;
		}
		clear_x += nx, clear_y += ny;
		for (int i = 0; i < WIDTH; i++)
		{
			clear_x = (clear_x ^ nx) % WIDTH;
			clear_y = (i ^ ny) % HEIGHT;
			map[clear_y][clear_x] = 0;
		}
		if (clear_x == 0)
		{
			Sleep(200);
			//scr_release();
			//scr_clear();
			break;
		}
		DrawMap();
		Sleep(1);
	}
}

//프로그레스바
int progress1 = 0;
int progressbar1_return = 0;
void progressbar1(int progress_delay) {

	for (int i = 0; i < WIDTH; i++) { //배경 그리기
		map[HEIGHT - 1][i] = 0;
	}
	map[HEIGHT - 1][WIDTH / 2] = 18; // 체크 포인트스럽게
	for (int i = 0; i < progress1; i++) {
		map[HEIGHT - 1][i] = 4;
	}
	map[HEIGHT - 1][progress1] = 19; // 머리 부분

	if (frame_count % progress_delay == 0) progress1++;
	if (progress1 == WIDTH) {
		for (int i = 0; i < progress1; i++) {
			map[HEIGHT - 1][i] = 0;
		}
		progress1 = 0; // 0칸으로 초기화
		progressbar1_return = 1; // 만땅이므로 1 리턴
	}
}

char score[100]; // 점수 구현 char
int player1_score = 0; // 캐릭터 점수 int
int max_hp = 5, hp = 5, max_shield_hp = 3, shield_hp = 0;
void hp_init_player1() {
	for (int i = 0; i < max_hp; i++) { // 빈 하트
		map[1][5 + i] = 15;
	}
	for (int i = 0; i < max_shield_hp; i++) { // 쉴드 하트 지우기
		map[1][10 + i] = 0;
	}
}
void hp_player1() {
	if (hp > 5) hp = 5;
	if (hp == -1) hp = 0;
	for (int i = 0; i < hp; i++) { // 채워진 하트
		map[1][5 + i] = 16;
	}
	for (int i = 0; i < shield_hp; i++) { // 스타 먹으면 블루 하트(쉴드) 생성
		map[1][10 + i] = 17;
	}
}

// 무브 캐릭터
int player1_y = 3, player1_x = 20, player1_len = 3, player1_color=14;
void reset_player1() {
	player1_y = 2;
	player1_x = 20;
}
void init_player1()
{
	for (int i = 0; i < player1_len; i++) {
		map[player1_y + i][player1_x] = 0;
	}
}
void move_player1(dx, dy)
{
	for (int i = 0; i < player1_len; i++) {
		map[player1_y - i][player1_x - dx] = 0;
	}
	player1_y += dy;

	if (dx < 0) {
		player1_x--;
		if (player1_x == -1) player1_x = 0;
		if (player1_x == WIDTH) player1_x = WIDTH - 1;
		dx = -1;
	}
	else if (dx > 0) {
		//dx = 0 이면 가만히 잇어야 됨
		player1_x++;
		if (player1_x == -1) player1_x = 0;
		if (player1_x == WIDTH) player1_x = WIDTH - 1;
		dx = 1;
	}
	/*if (dy < 0) {}*/
	if (player1_y == HEIGHT - 1) {
		player1_y = player1_len - 1;
		if (shield_hp != 0) shield_hp--;
		else hp--;
		hp_init_player1();
		hp_player1();
	}
	//else {
	//	if (player1_y == HEIGHT - 1) player1_y = 0;
	//}
	for (int i = 0; i < player1_len; i++) {
		map[player1_y - i][player1_x - dx] = player1_color;
	}
}

// 캐릭터 점프
int thorn_tmp = 0;
void jump_player1() {
	if (thorn_tmp == 0) { // 가시가 아닐때~
		textcolor(BLUE2, WHITE); gotoxy(14, 3); printscr(" + 100");
		//textcolor(BLUE2, WHITE); gotoxy((player1_x + 2) * 2, player1_y -2); printscr("JUMP!!");
	} //점프 마다 점수 증가 이펙트
	else {
		thorn_tmp = 0; // 가시는 점수 증가 안함
		player1_color = 13; // 히트 모션
	}

	//에러방지 천장 점프 차등
	if (player1_y < 11) move_player1(0, -player1_y + player1_len - 1);
	else move_player1(0, -8);
	player1_score += 100; // 블록 점프하면 점수 먹기
}

//일정 프레임 카운트 마다 블록 x 좌표를 간격두기
int frame_devide = 0;
int block_x_rand() {
	if (frame_count % WIDTH == 0) frame_devide += 10;
	if (frame_devide >= WIDTH / 2) frame_devide = 0;
	return WIDTH + (frame_devide);
}

//무브 블록 설정값, Ctrl + M + M 으로 열고 닫기
#pragma region blue1   긴 파랑 1
int blue1_len = 10, blue1_y_rand = 8;
//블록의 길이 len, y축 랜덤 생성 rand
int blue1_y = 8;
int blue1_x = 32;
void init_blue1()
{
	
	blue1_y = blue1_y_rand;
	blue1_x = block_x_rand();

	for (int i = 0; i <= blue1_len; i++) {
		map[blue1_y][blue1_x + i] = 0;
	}
}
void move_blue1(dx, dy)
{
	for (int i = 0; i <= blue1_len; i++) {
		map[blue1_y][blue1_x + i] = 0;
	}
	blue1_x += dx;
	if (blue1_x <= -blue1_len) {
		blue1_len = rand() % 2 + 8;
		blue1_y_rand = rand() % (HEIGHT / 2) + (HEIGHT / 3);
		init_blue1();
	}
	for (int i = 0; i <= blue1_len; i++) {
		map[blue1_y][blue1_x + i] = 10;
	}
	//맵 넘어가서 위로 생성되는 허수 블록 안보이게 
	//for (int i = 0; i <= blue1_len; i++) {
	//	map[blue1_y - 1][WIDTH - i - 1] = 0;
	//}
}
void hit_blue1() {
	if (player1_y == blue1_y - 1 && //일반 블록 1, 2, 3을 밟을 때
		blue1_x <= player1_x && player1_x <= blue1_x + blue1_len)
	{
		//textcolor(BLUE1, WHITE);
		//printxy(60, 3, "CRUSHED");
		//점프 구현
		jump_player1();
	}
	//else {
	//	textcolor(BLUE1, WHITE);
	//	printxy(60, 3, "정상    ");
	//}
}
#pragma endregion

#pragma region blue1_2 긴 파랑 2
int blue1_2_len = 10, blue1_2_y_rand = 8;
//블록의 길이 len, y축 랜덤 생성 rand
int blue1_2_y = 12;
int blue1_2_x = 16;
void init_blue1_2()
{
	
	blue1_2_y = blue1_2_y_rand;
	blue1_2_x = block_x_rand();

	for (int i = 0; i <= blue1_2_len; i++) {
		map[blue1_2_y][blue1_2_x + i] = 0;
	}
}
void move_blue1_2(dx, dy)
{
	for (int i = 0; i <= blue1_2_len; i++) {
		map[blue1_2_y][blue1_2_x + i] = 0;
	}
	blue1_2_x += dx;
	if (blue1_2_x <= -blue1_2_len) {
		blue1_2_len = rand() % 2 + 8;
		blue1_2_y_rand = rand() % (HEIGHT / 2) + (HEIGHT / 3);
		init_blue1_2();
	}
	for (int i = 0; i <= blue1_2_len; i++) {
		map[blue1_2_y][blue1_2_x + i] = 10;
	}
}
void hit_blue1_2() {
	if (player1_y == blue1_2_y - 1 && //일반 블록 1, 2, 3을 밟을 때
		blue1_2_x <= player1_x && player1_x <= blue1_2_x + blue1_2_len)
	{
		//점프 구현
		jump_player1();
	}
}
#pragma endregion

#pragma region blue1_3 긴 파랑 3
int blue1_3_len = 10, blue1_3_y_rand = 8;
//블록의 길이 len, y축 랜덤 생성 rand
int blue1_3_y = 20;
int blue1_3_x = 4;

void init_blue1_3()
{
	
	blue1_3_y = blue1_3_y_rand;
	blue1_3_x = block_x_rand();

	for (int i = 0; i <= blue1_3_len; i++) {
		map[blue1_3_y][blue1_3_x + i] = 0;
	}
}
void move_blue1_3(dx, dy)
{
	for (int i = 0; i <= blue1_3_len; i++) {
		map[blue1_3_y][blue1_3_x + i] = 0;
	}
	blue1_3_x += dx;
	if (blue1_3_x <= -blue1_3_len) {
		blue1_3_len = rand() % 2 + 8;
		blue1_3_y_rand = rand() % (HEIGHT / 2) + (HEIGHT / 3);
		init_blue1_3();
	}
	for (int i = 0; i <= blue1_3_len; i++) {
		map[blue1_3_y][blue1_3_x + i] = 10;
	}
}
void hit_blue1_3() {
	if (player1_y == blue1_3_y - 1 && //일반 블록 1, 2, 3을 밟을 때
		blue1_3_x <= player1_x && player1_x <= blue1_3_x + blue1_3_len)
	{
		jump_player1();
	}
}
#pragma endregion

#pragma region blue2   중간 파랑 1
int blue2_len = 8, blue2_y_rand = 8;
//블록의 길이 len, y축 랜덤 생성 rand
int blue2_y = 10;
int blue2_x = WIDTH - 1;

void init_blue2()
{
	
	blue2_y = blue2_y_rand;
	blue2_x = block_x_rand();

	for (int i = 0; i <= blue2_len; i++) {
		map[blue2_y][blue2_x + i] = 0;
	}
}
void move_blue2(dx, dy)
{
	//A = rand() % 10;
	for (int i = 0; i <= blue2_len; i++) {
		map[blue2_y][blue2_x + i] = 0;
	}
	blue2_x += dx;
	if (dx > 0) {
		if (blue2_x == WIDTH)
			blue2_x = 0;
	}
	else {
		if (blue2_x <= -blue2_len) {
			//blue2_y = -1;
			blue2_len = rand() % 2 + 4;
			blue2_y_rand = rand() % (HEIGHT / 2) + (HEIGHT / 3);
			init_blue2();
		}
	}
	for (int i = 0; i <= blue2_len; i++) {
		map[blue2_y][blue2_x + i] = 10;
	}
}
void hit_blue2() {
	if (player1_y == blue2_y - 1 &&
		blue2_x <= player1_x && player1_x <= blue2_x + blue2_len)
	{
		//점프 구현
		jump_player1();
	}
}
#pragma endregion

#pragma region blue2_2 중간 파랑 2
int blue2_2_len = 8, blue2_2_y_rand = 8;
//블록의 길이 len, y축 랜덤 생성 rand
int blue2_2_y = 10;
int blue2_2_x = WIDTH - 1;

void init_blue2_2()
{
	
	blue2_2_y = blue2_2_y_rand;
	blue2_2_x = block_x_rand();

	for (int i = 0; i <= blue2_2_len; i++) {
		map[blue2_2_y][blue2_2_x + i] = 0;
	}
}
void move_blue2_2(dx, dy)
{
	//A = rand() % 10;
	for (int i = 0; i <= blue2_2_len; i++) {
		map[blue2_2_y][blue2_2_x + i] = 0;
	}
	blue2_2_x += dx;
	if (dx > 0) {
		if (blue2_2_x == WIDTH)
			blue2_2_x = 0;
	}
	else {
		if (blue2_2_x <= -blue2_2_len) {
			blue2_2_len = rand() % 2 + 4;
			blue2_2_y_rand = rand() % (HEIGHT / 2) + (HEIGHT / 3);
			init_blue2_2();
		}
	}
	for (int i = 0; i <= blue2_2_len; i++) {
		map[blue2_2_y][blue2_2_x + i] = 10;
	}
}
void hit_blue2_2() {
	if (player1_y == blue2_2_y - 1 &&
		blue2_2_x <= player1_x && player1_x <= blue2_2_x + blue2_2_len)
	{
		//점프 구현
		jump_player1();
	}
}
#pragma endregion

#pragma region blue2_3 중간 파랑 3
int blue2_3_len = 4, blue2_3_y_rand = 8;
//블록의 길이 len, y축 랜덤 생성 rand
int blue2_3_y = 10;
int blue2_3_x = WIDTH - 1;

void init_blue2_3()
{
	
	blue2_3_y = blue2_3_y_rand;
	blue2_3_x = block_x_rand();

	for (int i = 0; i <= blue2_3_len; i++) {
		map[blue2_3_y][blue2_3_x + i] = 0;
	}
}
void move_blue2_3(dx, dy)
{
	//A = rand() % 10;
	for (int i = 0; i <= blue2_3_len; i++) {
		map[blue2_3_y][blue2_3_x + i] = 0;
	}
	blue2_3_x += dx;

	if (dx > 0) {
		if (blue2_3_x == WIDTH)
			blue2_3_x = 0;
	}
	else {
		if (blue2_3_x <= -blue2_3_len) {
			blue2_3_len = rand() % 2 + 4;
			blue2_3_y_rand = rand() % (HEIGHT / 2) + (HEIGHT / 3);
			init_blue2_3();
		}
	}
	for (int i = 0; i <= blue2_3_len; i++) {
		map[blue2_3_y][blue2_3_x + i] = 10;
	}
}
void hit_blue2_3() {
	if (player1_y == blue2_3_y - 1 &&
		blue2_3_x <= player1_x && player1_x <= blue2_3_x + blue2_3_len)
	{
		//점프 구현
		jump_player1();
	}
}
#pragma endregion

#pragma region thorn1 가시1 (하트 차감)
int thorn1_len = 5, thorn1_y_rand = 8, thorn1_thorn_rand = 3;
//A는 블록 길이, B는 y축 생성 위치
int thorn1_y = 12;
int thorn1_x = 12;

void init_thorn1()
{
	
	thorn1_y = thorn1_y_rand;
	thorn1_x = block_x_rand();;

	for (int i = 0; i <= thorn1_len; i++) {
		map[thorn1_y][thorn1_x + i] = 0;
	}
	map[thorn1_y - 1][thorn1_x + thorn1_thorn_rand] = 0;
}
void move_thorn1(dx, dy)
{
	//A = rand() % 10;
	for (int i = 0; i <= thorn1_len; i++) {
		map[thorn1_y][thorn1_x + i] = 0;
	}
	map[thorn1_y - 1][thorn1_x + thorn1_thorn_rand] = 0;

	thorn1_x += dx;
	if (dx > 0) {
		if (thorn1_x == WIDTH)
			thorn1_x = 0;
	}
	else {
		if (thorn1_x <= -thorn1_len) {
			thorn1_len = rand() % 3 + 5;
			thorn1_y_rand = rand() % (HEIGHT / 2) + (HEIGHT / 3);
			thorn1_thorn_rand = rand() % (thorn1_len - 1) + 1;
			init_thorn1();
		}
	}
	for (int i = 0; i <= thorn1_len; i++) {
		map[thorn1_y][thorn1_x + i] = 11;
	}
	map[thorn1_y - 1][thorn1_x + thorn1_thorn_rand] = 20;
}
void hit_thorn1() {
	//캐릭터의 몸통 y축 범위와 가시 x축 닿음
	if (player1_x == thorn1_x + thorn1_thorn_rand &&
		player1_y - player1_len <= thorn1_y - 1 && thorn1_y - 1 <= player1_y)
	{
		//textcolor(GREEN2, WHITE);
		//printxy(60, 3, "CRUSHED");
		thorn_tmp++;
		jump_player1();
		if (shield_hp != 0) shield_hp--;
		else hp--;
		hp_init_player1();
		Sleep(10);
	}
	//가시 x축을 제외한 범위에서 캐릭터가 블록을 밟을 때
	else if (thorn1_x <= player1_x && player1_x <= thorn1_x + thorn1_len &&
		player1_y == thorn1_y - 1 && player1_x != thorn1_x + thorn1_thorn_rand) {
		//textcolor(GREEN2, WHITE);
		//printxy(60, 3, "정상    ");
		//점프 구현
		jump_player1();
	}
}
#pragma endregion

#pragma region thorn2 가시2 (하트 차감)
int thorn2_len = 8, thorn2_y_rand = 8, thorn2_thorn_rand = 2;
//A는 블록 길이, B는 y축 생성 위치
int thorn2_y = 20;
int thorn2_x = 20;

void init_thorn2()
{
	
	thorn2_y = thorn2_y_rand;
	thorn2_x = block_x_rand();;

	for (int i = 0; i <= thorn2_len; i++) {
		map[thorn2_y][thorn2_x + i] = 0;
	}
	map[thorn2_y - 1][thorn2_x + thorn2_thorn_rand] = 0;
}
void move_thorn2(dx, dy)
{
	//A = rand() % 10;
	for (int i = 0; i <= thorn2_len; i++) {
		map[thorn2_y][thorn2_x + i] = 0;
	}
	map[thorn2_y - 1][thorn2_x + thorn2_thorn_rand] = 0;

	thorn2_x += dx;
	if (dx > 0) {
		if (thorn2_x == WIDTH)
			thorn2_x = 0;
	}
	else {
		if (thorn2_x <= -thorn2_len) {
			thorn2_len = rand() % 2 + 3;
			thorn2_y_rand = rand() % (HEIGHT / 2) + (HEIGHT / 3);
			thorn2_thorn_rand = rand() % (thorn2_len - 1) + 1;
			init_thorn2();
		}
	}
	for (int i = 0; i <= thorn2_len; i++) {
		map[thorn2_y][thorn2_x + i] = 11;
	}
	map[thorn2_y - 1][thorn2_x + thorn2_thorn_rand] = 20;
}
void hit_thorn2() {
	//캐릭터의 몸통 y축 범위와 가시 x축 닿음
	if (player1_x == thorn2_x + thorn2_thorn_rand &&
		player1_y - player1_len <= thorn2_y - 1 && thorn2_y - 1 <= player1_y)
	{
		thorn_tmp++;
		jump_player1();
		if (shield_hp != 0) shield_hp--;
		else hp--;
		hp_init_player1();
		Sleep(10);
	}
	//가시 x축을 제외한 범위에서 캐릭터가 블록을 밟을 때
	else if (thorn2_x <= player1_x && player1_x <= thorn2_x + thorn2_len &&
		player1_y == thorn2_y - 1 && player1_x != thorn2_x + thorn2_thorn_rand) 
	{
		jump_player1();
	}
}
#pragma endregion

#pragma region thorn3 가시3 (하트 차감)
int thorn3_len = 8, thorn3_y_rand = 8, thorn3_thorn_rand = 2;
//A는 블록 길이, B는 y축 생성 위치
int thorn3_y = 10;
int thorn3_x = 20;

void init_thorn3()
{
	
	thorn3_y = thorn3_y_rand;
	thorn3_x = block_x_rand();;

	for (int i = 0; i <= thorn3_len; i++) {
		map[thorn3_y][thorn3_x + i] = 0;
	}
	map[thorn3_y - 1][thorn3_x + thorn3_thorn_rand] = 0;
}
void move_thorn3(dx, dy)
{
	//A = rand() % 10;
	for (int i = 0; i <= thorn3_len; i++) {
		map[thorn3_y][thorn3_x + i] = 0;
	}
	map[thorn3_y - 1][thorn3_x + thorn3_thorn_rand] = 0;

	thorn3_x += dx;
	if (dx > 0) {
		if (thorn3_x == WIDTH)
			thorn3_x = 0;
	}
	else {
		if (thorn3_x <= -thorn3_len) {
			thorn3_len = rand() % 2 + 3;
			thorn3_y_rand = rand() % (HEIGHT / 2) + (HEIGHT / 3);
			thorn3_thorn_rand = rand() % (thorn3_len - 1) + 1;
			init_thorn3();
		}
	}
	for (int i = 0; i <= thorn3_len; i++) {
		map[thorn3_y][thorn3_x + i] = 11;
	}
	map[thorn3_y - 1][thorn3_x + thorn3_thorn_rand] = 20;
}
void hit_thorn3() {
	//캐릭터의 몸통 y축 범위와 가시 x축 닿음
	if (player1_x == thorn3_x + thorn3_thorn_rand &&
		player1_y - player1_len <= thorn3_y - 1 && thorn3_y - 1 <= player1_y)
	{
		thorn_tmp++;
		jump_player1();
		if (shield_hp != 0) shield_hp--;
		else hp--;
		hp_init_player1();
		Sleep(10);
	}
	//가시 x축을 제외한 범위에서 캐릭터가 블록을 밟을 때
	else if (thorn3_x <= player1_x && player1_x <= thorn3_x + thorn3_len &&
		player1_y == thorn3_y - 1 && player1_x != thorn3_x + thorn3_thorn_rand) 
	{
		jump_player1();
	}
}
#pragma endregion

#pragma region heart1 하트1 (하트 증가)
int heart1_len = 8, heart1_y_rand = 8, heart1_heart_rand = 2;
int heart1_num = 21;
int heart1_y = 12;
int heart1_x = 30;

void init_heart1()
{
	heart1_y = heart1_y_rand;
	heart1_x = block_x_rand();

	if (frame_count % 20 == 0) { // 20 배수 단위일 때 생성
		for (int i = 0; i <= heart1_len; i++) {
			map[heart1_y][heart1_x + i] = 0;
		}
		map[heart1_y - 1][heart1_x + 2] = 0;
	}
	else {
		heart1_x = -10; // 아니면 대기 시키기
	}

}
void move_heart1(dx, dy)
{
	//A = rand() % 10;
	for (int i = 0; i <= heart1_len; i++) {
		map[heart1_y][heart1_x + i] = 0;
	}
	map[heart1_y - 1][heart1_x + heart1_heart_rand] = 0;

	heart1_x += dx;
	if (dx > 0) {
		if (heart1_x == WIDTH)
			heart1_x = 0;
	}
	else {
		if (heart1_x <= -heart1_len) {
			heart1_len = rand() % 3 + 2;
			heart1_y_rand = rand() % (HEIGHT / 2) + (HEIGHT / 3);
			heart1_heart_rand = rand() % (heart1_len - 1) + 1;
			heart1_num = 21;
			init_heart1();
		}
	}
	for (int i = 0; i <= heart1_len; i++) {
		map[heart1_y][heart1_x + i] = 5;
	}
	heart1_num = (heart1_num == 21) ? 21 : 0;
	map[heart1_y - 1][heart1_x + heart1_heart_rand] = heart1_num;
}
void hit_heart1() {
	//캐릭터의 몸통 y축 범위와 하트 x축 닿음
	if (player1_x == heart1_x + heart1_heart_rand &&
		player1_y - player1_len <= heart1_y - 1 && heart1_y - 1 <= player1_y &&
		heart1_num == 21)
	{
		hp++;
		hp_init_player1();
		heart1_num = 0;
		player1_score += 1000; // 점수 보너스
		textcolor(RED2, WHITE); gotoxy(14, 4); printscr(" + 1000"); //점프 마다 점수 증가 이펙트
		Sleep(10);
	}
	//하트 x축을 제외한 범위에서 캐릭터가 블록을 밟을 때
	if (heart1_x <= player1_x && player1_x <= heart1_x + heart1_len &&
		player1_y == heart1_y - 1/* && player1_x != heart1_x + heart1_heart_rand*/) 
	{
		//점프 구현
		jump_player1();
	}
}
#pragma endregion

#pragma region start1 별1 (쉴드 / 추가 체력 생성)
int star1_len = 5, star1_y_rand = 8, star1_star_rand = 2;
int star1_num = 22;
int star1_y = 10;
int star1_x = 20;

void init_star1()
{
	
	star1_y = star1_y_rand;
	star1_x = block_x_rand();;

	if (frame_count % 30 == 0) { // 30 배수 단위일 때 생성
		for (int i = 0; i <= star1_len; i++) {
			map[star1_y][star1_x + i] = 0;
		}
		map[star1_y - 1][star1_x + star1_star_rand] = 0;
	}
	else {
		star1_x = -10; // 아니면 대기 시키기
	}
}
void move_star1(dx, dy)
{
	//A = rand() % 10;
	for (int i = 0; i <= star1_len; i++) {
		map[star1_y][star1_x + i] = 0;
	}
	map[star1_y - 1][star1_x + star1_star_rand] = 0;

	star1_x += dx;
	if (dx > 0) {
		if (star1_x == WIDTH)
			star1_x = 0;
	}
	else {
		if (star1_x <= -star1_len) {
			star1_len = rand() % 2 + 2;
			star1_y_rand = rand() % (HEIGHT / 2) + (HEIGHT / 3);
			star1_star_rand = rand() % (star1_len - 1) + 1;
			star1_num = 22;
			init_star1();
		}
	}
	for (int i = 0; i <= star1_len; i++) {
		map[star1_y][star1_x + i] = 7;
	}
	star1_num = (star1_num == 22) ? 22 : 0;
	map[star1_y - 1][star1_x + star1_star_rand] = star1_num;
}
void hit_star1() {
	//캐릭터의 몸통 y축 범위와 스타 x축 닿음
	if (player1_x == star1_x + star1_star_rand &&
		player1_y - player1_len <= star1_y - 1 && star1_y - 1 <= player1_y &&
		star1_num == 22)
	{
		shield_hp = 3;
		star1_num = 0;
		player1_score += 5000; // 점수 보너스
		textcolor(YELLOW1, WHITE); gotoxy(14, 4); printscr(" + 5000"); //점프 마다 점수 증가 이펙트
		Sleep(10);
	}
	//스타 x축을 제외한 범위에서 캐릭터가 블록을 밟을 때
	else if (star1_x <= player1_x && player1_x <= star1_x + star1_len &&
		player1_y == star1_y - 1 && player1_x != star1_x + star1_star_rand) {
		//점프 구현
		jump_player1();
	}
}
#pragma endregion

// 타이틀 화면
void title() {
	unsigned char key_title = 0;
	int color = 0;
	scr_clear();
	scr_init();
		
	while (1) {
		textcolor(BLUE2, WHITE);
		printxy(21, 15, "┌──────────────────────────────────┐ ");
		printxy(21, 16, "│ ←→, A D키로 캐릭터를 조작하세요│ ");
		printxy(21, 17, "│ 블록 위  가시 ▲ 를 피하고       │ ");
		printxy(21, 18, "│ 하트 ♥나 무적 ★을 획득하면서	│ ");
		printxy(21, 19, "│ 블록을 점프해 끝까지 달려보세요  │ ");
		printxy(21, 20, "└──────────────────────────────────┘ ");
		printxy(40, 7, "&");

		textcolor(color % 8, WHITE);
		printxy(23, 27, "< 아무키나 눌러서 게임 시작... >");

		scr_switch();
		if (frame_count % 4 == 0) {
			move_blue1(-1, 0);
			move_heart1(-1, 0);
			move_thorn1(-1, 0);
			move_star1(-1, 0);
		}
		if (frame_count % 32 == 0) {
			color++;
			if (color == 16) color = 0;
		}
		if (kbhit()) {
			key_title = getch();
			if (key_title == UP || key_title == UP2) {
				getch();
			}
			dot_jump(3, 8, 0);
			dot_jump(22, 2, 0);
			frame_count = 0;
			break;
		}
		dot_jump(3, 9, 8);
		dot_jump(2, 8, 4);
		dot_jump(23, 1, 8);
		dot_jump(22, 2, 10);
		DrawMap();
		Sleep(1);
		frame_count++;
	}
}

long long count = 1; //지연 시간, 첫 카운트 다운 변수
//매 레밸마다 카운트 다운(갑자기 시작, 돌연사 방지)
void countdown(int count_level) {
	if (count == count_level) {
		dot_3(18, 10, 10);
		DrawMap();
		scr_switch();
		Sleep(500);
		dot_3(18, 10, 0);

		dot_2(18, 10, 11);
		DrawMap();
		scr_switch();
		Sleep(500);
		dot_2(18, 10, 0);

		dot_1(18, 10, 12);
		DrawMap();
		scr_switch();
		Sleep(500);
		dot_1(18, 10, 0);

		Sleep(200);
		count++;
	}
}

//Sleep(짝수), 프레임카운트, 프로그레스바 세밀 조정
void level_1(delay, framecount_dalay, progress_delay) {
	//블루1 3개, 블루2 3개
	hit_blue1();
	hit_blue1_2();
	hit_blue1_3();
	hit_blue2();
	hit_blue2_2();
	hit_blue2_3();

	scr_switch();
	//프레임 카운드 당 블록 움직임
	//최상단에 올리면 len - 1 끝점을 안밟지만 점프가 부자연스러움
	//그냥 두면 끝점 에러가 뜨는 딜레마가...
	if (frame_count % framecount_dalay == 0) 
	{ //끝으로 갈수록 최상단으로 출력됨
		move_blue1(-1, 0);
		move_blue1_2(-1, 0);
		move_blue1_3(-1, 0);
		move_blue2(-1, 0);
		move_blue2_2(-1, 0);
		move_blue2_3(-1, 0);
		
		progressbar1(progress_delay);
		move_player1(0, 1);
	}
	move_player1(0, 0);
	hp_init_player1();
	hp_player1();

	//3 2 1 카운트 다운 실행 이후 그리기
	countdown(1);
	DrawMap();

	//블록 맵 -> 텍스트 순서로 넣으면 그나마 ㄱㅊ
	textcolor(RED2, GRAY1);
	printxy(2, 1, "HP    : ");
	textcolor(BLUE2, GRAY1);
	printxy(2, 2, "");
	sprintf(score, "SCORE : %10d", player1_score); gotoxy(2, 2); printscr(score);
	textcolor(BLUE2, GRAY1);
	printxy(16, HEIGHT - 2, "▲▲▲ 바닥에 닿으면 하트가 1 차감됩니다 ▲▲▲");
	textcolor(BLACK, GRAY1);
	printxy(0, HEIGHT - 2, "[ Level 1 ]");
	textcolor(BLACK, GRAY1);
	printxy(WIDTH * 2 - 12, HEIGHT - 2, " [ Level 2 ]");

	Sleep(delay);
	frame_count++;
}

void level_2(delay, framecount_dalay, progress_delay){
	//가시 1개, 하트 1개, 블루1 3개, 블루2 2개
	hit_thorn1();
	hit_heart1();

	hit_blue1();
	hit_blue1_2();
	hit_blue1_3();

	hit_blue2();
	hit_blue2_2();

	scr_switch();
	//프레임 카운드 당 블록 움직임
	//최상단에 올리면 len - 1 끝점을 안밟지만 점프가 부자연스러움
	//그냥 두면 끝점 에러가 뜨는 딜레마가...
	if (frame_count % framecount_dalay == 0) {
		//끝으로 갈수록 최상단으로 출력됨
		int range1 = rand() % 99 + 1;
		int range2 = rand() % 1 + 1;
		if (range1 < 50) {
			switch (range2)
			{
			case 1:move_thorn1(-1, 0);
			case 2:move_heart1(-1, 0);
			}
		}
		move_blue1(-1, 0); move_blue1_2(-1, 0); move_blue1_3(-1, 0);
		move_blue2(-1, 0); move_blue2_2(-1, 0);

		progressbar1(progress_delay);
		move_player1(0, 1);
	}		
	//블록 짤림 방지 0칸 이동
	move_heart1(0, 0); move_thorn1(0, 0);
	move_player1(0, 0);

	countdown(2);
	hp_init_player1();
	hp_player1();
	DrawMap(); //DrwaMap -> 텍스트 순서로 넣으면 그나마 ㄱㅊ

	textcolor(RED2, GRAY1);
	printxy(2, 1, "HP    : ");
	textcolor(BLUE2, GRAY1);
	sprintf(score, "SCORE : %10d", player1_score); gotoxy(2, 2); printscr(score);
	textcolor(BLUE2, GRAY1);
	printxy(16, HEIGHT - 2, "▲▲▲ 바닥에 닿으면 하트가 1 차감됩니다 ▲▲▲");

	textcolor(BLACK, GRAY1);
	printxy(0, HEIGHT - 2, "[ Level 2]");
	textcolor(BLACK, GRAY1);
	printxy(WIDTH * 2 - 12, HEIGHT - 2, " [ Level 3 ]");

	Sleep(delay);
	player1_color = 14; // 가시 히트 이후 플레이어 색상 원복
	frame_count++;
}

void level_3(delay, framecount_dalay, progress_delay){
	//가시 2개, 하트 1개, 블루1 2개, 블루2 2개
	hit_thorn1();
	hit_thorn2();
	hit_heart1();
	
	hit_blue1();
	hit_blue1_2();
	hit_blue2();
	hit_blue2_2();

	scr_switch();
	//프레임 카운드 당 블록 움직임
	//최상단에 올리면 len - 1 끝점을 안밟지만 점프가 부자연스러움
	//그냥 두면 끝점 에러가 뜨는 딜레마가...
	if (frame_count % framecount_dalay == 0) {
		//끝으로 갈수록 최상단으로 출력됨
		int range1 = rand() % 99 + 1;
		int range2 = rand() % 2 + 1;
		if (range1 < 70) {
			switch (range2)
			{
			case 1:move_thorn1(-1, 0);
			case 2:move_thorn2(-1, 0);
			}
		}
		move_blue1(-1, 0); move_blue1_2(-1, 0);
		move_blue2(-1, 0); move_blue2_2(-1, 0);
		move_heart1(-1, 0);

		progressbar1(progress_delay);
		move_player1(0, 1);
	}
	//블록 짤림 방지 0칸 이동
	move_blue1(0, 0); move_blue1_2(0, 0); move_blue2(0, 0); move_blue2_2(0, 0);
	move_heart1(0, 0); move_thorn1(0, 0); move_thorn2(0, 0);
	move_player1(0, 0);

	countdown(3);
	hp_init_player1();
	hp_player1();
	DrawMap(); //블록 맵 -> 텍스트 순서로 넣으면 그나마 ㄱㅊ

	textcolor(RED2, GRAY1);
	printxy(2, 1, "HP    : ");
	textcolor(BLUE2, GRAY1);
	sprintf(score, "SCORE : %10d", player1_score); gotoxy(2, 2); printscr(score);
	textcolor(BLUE2, GRAY1);
	printxy(16, HEIGHT - 2, "▲▲▲ 바닥에 닿으면 하트가 1 차감됩니다 ▲▲▲");

	textcolor(BLACK, GRAY1);
	printxy(0, HEIGHT - 2, "[ Level 3]");
	textcolor(BLACK, GRAY1);
	printxy(WIDTH * 2 - 12, HEIGHT - 2, " [ Level 4 ]");

	Sleep(delay);
	player1_color = 14; // 가시 히트 이후 플레이어 색상 원복
	frame_count++;
}

void level_4(delay, framecount_dalay, progress_delay){	
	//가시 3개, 스타 1개, 하트 1개, 블루1 1개, 블루2 2개
	hit_thorn1();
	hit_thorn2();
	hit_thorn3();
	hit_star1();
	hit_heart1();
	
	hit_blue1();
	hit_blue2();
	hit_blue2_2();

	scr_switch();
	//프레임 카운드 당 블록 움직임
	//최상단에 올리면 len - 1 끝점을 안밟지만 점프가 부자연스러움
	//그냥 두면 끝점 에러가 뜨는 딜레마가...
	if (frame_count % framecount_dalay == 0) {
		//끝으로 갈수록 최상단으로 출력됨
		int range1 = rand() % 99 + 1;
		int range2 = rand() % 3 + 1;
		if (range1 < 80) {
			switch (range2)
			{
			case 1:move_thorn1(-1, 0);
			case 2:move_thorn2(-1, 0);
			case 3:move_thorn3(-1, 0);
			}
		}
		move_heart1(-2, 0);
		move_star1(-2, 0);
		move_blue1(-2, 0);
		move_blue2(-1, 0); move_blue2_2(-1, 0);

		progressbar1(progress_delay);
		move_player1(0, 1);
	}
	//블록 짤림 방지 0칸 이동
	move_blue1(0, 0); move_blue2(0, 0); move_blue2_2(0, 0);
	move_heart1(0, 0); move_star1(0, 0);
	move_thorn1(0, 0); move_thorn2(0, 0); move_thorn3(0, 0);
	move_player1(0, 0);

	countdown(4);
	hp_init_player1();
	hp_player1();
	DrawMap(); //블록 맵 -> 텍스트 순서로 넣으면 그나마 ㄱㅊ

	textcolor(RED2, GRAY1);
	printxy(2, 1, "HP    : ");
	textcolor(BLUE2, GRAY1);
	sprintf(score, "SCORE : %10d", player1_score); gotoxy(2, 2); printscr(score);
	textcolor(BLUE2, GRAY1);
	printxy(16, HEIGHT - 2, "▲▲▲ 바닥에 닿으면 하트가 1 차감됩니다 ▲▲▲");

	textcolor(BLACK, GRAY1);
	printxy(0, HEIGHT - 2, "[ Level 4]");
	textcolor(BLACK, GRAY1);
	printxy(WIDTH * 2 - 12, HEIGHT - 2, " [ Level 5 ]");

	Sleep(delay);
	player1_color = 14; // 가시 히트 이후 플레이어 색상 원복
	frame_count++;
}

void level_5(delay, framecount_dalay, progress_delay){
	//가시 3개, 하트 1개, 블루2 2개
	hit_thorn1();
	hit_thorn2();
	hit_thorn3();
	hit_star1();
	hit_heart1();

	hit_blue2();
	hit_blue2_2();

	scr_switch();
	if (frame_count % framecount_dalay == 0) {
		//끝으로 갈수록 최상단으로 출력됨
		int range1 = rand() % 99 + 1;
		int range2 = rand() % 3 + 1;
		if (range1 < 90) {
			switch (range2)
			{
			case 1:move_thorn1(-2, 0);
			case 2:move_thorn2(-2, 0);
			case 3:move_thorn3(-1, 0);
			}
		}
		move_blue2(-2, 0); move_blue2_2(-2, 0);
		move_heart1(-2, 0);
		move_star1(-4, 0);

		progressbar1(progress_delay);
		move_player1(0, 1);
	}
	//블록 짤림 방지 0칸 이동
	move_blue2(0, 0);
	move_heart1(0, 0); move_star1(0, 0);
	move_thorn1(0, 0); move_thorn2(0, 0); move_thorn3(0, 0);
	move_player1(0, 0);

	countdown(5);
	hp_init_player1();
	hp_player1();
	DrawMap(); //블록 맵 -> 텍스트 순서로 넣으면 그나마 ㄱㅊ

	textcolor(RED2, GRAY1);
	printxy(2, 1, "HP    : ");
	textcolor(BLUE2, GRAY1);
	sprintf(score, "SCORE : %10d", player1_score); gotoxy(2, 2); printscr(score);
	textcolor(BLUE2, GRAY1);
	printxy(16, HEIGHT - 2, "▲▲▲ 바닥에 닿으면 하트가 1 차감됩니다 ▲▲▲");

	textcolor(BLACK, GRAY1);
	printxy(0, HEIGHT - 2, "[ Level 5]");
	textcolor(BLACK, GRAY1);
	printxy(WIDTH * 2 - 12, HEIGHT - 2, " [ FINISH ]");

	Sleep(delay);
	player1_color = 14; // 가시 히트 이후 플레이어 색상 원복
	frame_count++;
}

//레밸 재시작 or 클리어 시 특정 변수 및 환경 초기화
void gameover_title() {//타이틀 이동시 체력5,쉴드0, 점수0 321카운트다운, 프레임카운트, 점수 초기화
	hp = 5, shield_hp = 0, player1_score = 0; progress1 = 0, progressbar1_return = 0, count = 1, frame_count = 0;
	reset_player1();
}
void gamevoer_retry() {//타이틀 이동시 체력2, 321카운트다운, 점수 초기화
	hp = 2, progressbar1_return = 0, count--;
	if (progress1 >= WIDTH / 2) progress1 = WIDTH / 2; // 체크 포인트
	else progress1 = 0;
	reset_player1();
}

//게임 오버 or 클리어 문구 띄우기
void info_gameover() {
	if (frame_count % 2 == 0) {
		dot_gameover(WIDTH / 12, HEIGHT / 3, 41);
	}
	textcolor(BLACK, GRAY1);
	printxy(18, 20, "타이틀 화면(T)... 다시 도전(R)... 끝내기(Q)...");
	textcolor(YELLOW2, MAGENTA2);
	printxy(16, 24, "다시 도전 시 ♥ 체력 2칸이 부여되고");
	printxy(16, 25, "처음 혹은 ♣ 체크 포인트 부터 시작합니다...");
	scr_switch();
	DrawMap();
	frame_count++;
}
void info_clear() {
	if (frame_count % 2 == 0) {
		dot_clear(WIDTH / 4, HEIGHT / 3, 40);
	}
	//체력, 프레임카운트, 카운트다운, 점수 초기화
	printxy(24, 20, "타이틀 화면(T)... 끝내기(Q)...");
	scr_switch();
	DrawMap();
	frame_count++;
}

//키 입력 (p를 입력하면 치트키처럼 프로그레스를 늘림)
int keypress() {
	unsigned char last_ch, ch;
	// 특수키 0xe0 을 입력받으려면 unsigned char 로 선언해야 함
	int keep_moving = 0;
	ch = UP;
	//last_ch = UP;
	if (kbhit() == 1) {  // 키보드가 눌려져 있으면
		char c1;
		c1 = getch(); // key 값을 읽는다
		if (c1 == '\0') // VS 의 NULL 값은 Error이다.
			//continue;
			return 1;
		//continue를 쓰기 위한 편법
		else
			ch = c1; // NULL이 아니면 정상 입력이다.
		// ESC 누르면 프로그램 종료
		if (ch == ESC)
			exit(0);
		if (ch == SPECIAL1) { // 만약 특수키
			// 예를 들어 UP key의 경우 0xe0 0x48 두개의 문자가 들어온다.
			ch = getch();
			switch (ch) {
			case LEFT:
				last_ch = ch;
				keep_moving = 1;
				break;
			case RIGHT:
				last_ch = ch;
				keep_moving = 1;
				break;
			default: // 방향키가 아니면 멈춘다 1이면 계속 진행
				keep_moving = 0;
				break;
			}
		}
		else {
			// 특수 문자가 아니지만 방향키 대신 AWSD 사용하는 경우 처리
			switch (ch) {
			case 'P': last_ch = 'P';
				keep_moving = 1;
				break;
			case 'p': last_ch = 'p';
				keep_moving = 1;
				break;
			case RIGHT2: last_ch = RIGHT;
				keep_moving = 1;
				break;
			case LEFT2: last_ch = LEFT;
				keep_moving = 1;
				break;
			default: // 방향이 아니면 멈춘다 --> 1이면 계속 진행
				keep_moving = 0;
				break;
			}
		}
	}
	if (keep_moving) { // 움직이고 있으면	
		switch (last_ch) {
		case 'P':  // 치트키처럼 게임 진행 빠르게
			if (progress1 >= WIDTH - 5) progress1 = WIDTH - 5;
			progress1++;
			break;
		case 'p':// 치트키처럼 게임 진행 빠르게
			if (progress1 >= WIDTH - 5) progress1 = WIDTH - 5;
			progress1++;
			break;
		case LEFT:
			if (frame_count % 2 == 0) {
				move_player1(-1, 0);
				for (int i = 0; i < player1_len; i++) {
					map[player1_y - i][player1_x + 1] = 0;
				}
			}
			break;
		case RIGHT:
			if (frame_count % 2 == 0) {
				move_player1(1, 0);
				for (int i = 0; i < player1_len; i++) {
					map[player1_y - i][player1_x - 1] = 0;
				}
			}
			break;
		}
	}
}
//좌우 블록의 대가리는 좌측, 상하 블록의 대가리는 하단
void main() {
	unsigned char ch, last_ch, key_main = 0;
	int keep_moving = 0;
	ch = UP, last_ch = UP;
	srand(time(NULL));
	cls(BLACK, WHITE); // 콘솔 크기 조정
	
	//타이틀 화면
title_line:
	scr_init();
	title();
	
	//while문 괄호 안 숫자 true 값은 레밸을 직관적으로 표기
	///////////////////////////////////////////////////////
	block_slide(40, 1, 2, 30, 0, 0, 0, 0, 32);
level_1_line:
	scr_init();
	while (1) {
		if (keypress() == 1) continue;
		keypress();
		level_1(80, 2, 8);
		if (hp == 0) { // 체력 없으면
			while (1)
			{
				info_gameover();
				if (kbhit() == 1) {  // 키보드가 눌려져 있으면
					key_main = getch(); // key_main 값을 읽는다
					if (key_main == 'Q' || key_main == 'q') exit(0);
				}
				if (key_main == 'T' || key_main == 't') { // 타이틀로
					key_main = 0;
					textcolor(BLUE2, WHITE); // 강제로 배경 색을 바꿈(scr_claer X)
					printxy(0, 0, "");
					scr_switch();
					map_clear();
					DrawMap();
					gameover_title();
					goto title_line;
				}
				if (key_main == 'R' || key_main == 'r') { // 다시 시작
					key_main = 0;
					textcolor(BLUE2, WHITE); // 강제로 배경 색을 바꿈(scr_claer X)
					printxy(0, 0, "");
					scr_switch();
					map_clear();
					DrawMap();
					gamevoer_retry();
					goto level_1_line;
				}
			}
		}
		if (progressbar1_return == 1) { // 프로그레스바 끝나면
			progressbar1_return = 0;
			reset_player1();
			break;
		}
	}

	block_slide(40, 1, 2, 31, 30, 0, 0, 0, 32);
level_2_line:
	scr_init();
	while (2) {
		keypress();
		if (keypress() == 1) continue;
		level_2(80, 2, 8);
		if (hp == 0) { // 체력 없으면
			while (2)
			{
				info_gameover();
				if (kbhit() == 1) {  // 키보드가 눌려져 있으면
					key_main = getch(); // key_main 값을 읽는다
					if (key_main == 'Q' || key_main == 'q') exit(0);
				}
				if (key_main == 'T' || key_main == 't') { // 타이틀로
					key_main = 0;
					textcolor(BLUE2, WHITE); // 강제로 배경 색을 바꿈(scr_claer X)
					printxy(0, 0, "");
					scr_switch();
					map_clear();
					DrawMap();
					gameover_title();
					goto title_line;
				}
				if (key_main == 'R' || key_main == 'r') { // 다시 시작
					key_main = 0;
					textcolor(BLUE2, WHITE); // 강제로 배경 색을 바꿈(scr_claer X)
					printxy(0, 0, "");
					scr_switch();
					map_clear();
					DrawMap();
					gamevoer_retry();
					goto level_2_line;
				}
			}
		}
		if (progressbar1_return == 1) {
			progressbar1_return = 0;
			reset_player1();
			break;
		}
	}

	block_slide(40, 1, 2, 31, 31, 30, 0, 0, 32);
level_3_line:
	scr_init();
	while (3) {
		keypress();
		if (keypress() == 1) continue;
		level_3(60, 2, 8);
		if (hp == 0) { // 체력 없으면
			while (1)
			{
				info_gameover();
				if (kbhit() == 1) {  // 키보드가 눌려져 있으면
					key_main = getch(); // key_main 값을 읽는다
					if (key_main == 'Q' || key_main == 'q') exit(0);
				}
				if (key_main == 'T' || key_main == 't') { // 타이틀로
					key_main = 0;
					textcolor(BLUE2, WHITE); // 강제로 배경 색을 바꿈(scr_claer X)
					printxy(0, 0, "");
					scr_switch();
					map_clear();
					DrawMap();
					gameover_title();
					goto title_line;
				}
				if (key_main == 'R' || key_main == 'r') { // 다시 시작
					key_main = 0;
					textcolor(BLUE2, WHITE); // 강제로 배경 색을 바꿈(scr_claer X)
					printxy(0, 0, "");
					scr_switch();
					map_clear();
					DrawMap();
					gamevoer_retry();
					goto level_3_line;
				}
			}
		}
		if (progressbar1_return == 1) {
			progressbar1_return = 0;
			reset_player1();
			break;
		}
	}

	block_slide(20, 1, 2, 31, 31, 31, 30, 0, 32);
level_4_line:
	scr_init();
	while (4) {
		keypress();
		if (keypress() == 1) continue;
		level_4(50, 2, 8);
		if (hp == 0) { // 체력 없으면
			while (1)
			{
				info_gameover();
				if (kbhit() == 1) {  // 키보드가 눌려져 있으면
					key_main = getch(); // key_main 값을 읽는다
					if (key_main == 'Q' || key_main == 'q') exit(0);
				}
				if (key_main == 'T' || key_main == 't') { // 타이틀로
					key_main = 0;
					textcolor(BLUE2, WHITE); // 강제로 배경 색을 바꿈(scr_claer X)
					printxy(0, 0, "");
					map_clear();
					DrawMap();
					scr_switch();
					gameover_title();
					goto title_line;
				}
				if (key_main == 'R' || key_main == 'r') { // 다시 시작
					key_main = 0;
					textcolor(BLUE2, WHITE); // 강제로 배경 색을 바꿈(scr_claer X)
					printxy(0, 0, "");
					scr_switch();
					map_clear();
					DrawMap();
					gamevoer_retry();
					goto level_4_line;
				}
			}
		}
		if (progressbar1_return == 1) {
			progressbar1_return = 0;
			reset_player1();
			break;
		}
	}

	block_slide(20, 1, 2, 31, 31, 31, 31, 30, 32);
level_5_line:
	scr_init();
	while (5) {
		keypress();
		if (keypress() == 1) continue;
		level_5(40, 2, 10);
		if (hp == 0) { // 체력 없으면
			while (1)
			{
				info_gameover();
				if (kbhit() == 1) {  // 키보드가 눌려져 있으면
					key_main = getch(); // key_main 값을 읽는다
					if (key_main == 'Q' || key_main == 'q') exit(0);
				}
				if (key_main == 'T' || key_main == 't') { // 타이틀로
					key_main = 0;
					textcolor(BLUE2, WHITE); // 강제로 배경 색을 바꿈(scr_claer X)
					printxy(0, 0, "");
					scr_switch();
					map_clear();
					DrawMap();
					gameover_title();
					goto title_line;
				}
				if (key_main == 'R' || key_main == 'r') { // 다시 시작
					key_main = 0;
					textcolor(BLUE2, WHITE); // 강제로 배경 색을 바꿈(scr_claer X)
					printxy(0, 0, "");
					scr_switch();
					map_clear();
					DrawMap();
					gamevoer_retry();
					goto level_5_line;
				}
			}
		}
		if (progressbar1_return == 1) {
			progressbar1_return = 0;
			reset_player1();
			break;
		}
	}

	block_slide(20, 1, 2, 31, 31, 31, 31, 31, 32);
	while (100) { // 클리어 화면
		info_clear();
		if (kbhit() == 1) {  // 키보드가 눌려져 있으면
			key_main = getch(); // key_main 값을 읽는다
			if (key_main == 'Q' || key_main == 'q') exit(0);
		}
		if (key_main == 'T' || key_main == 't') {
			key_main = 0;
			textcolor(BLUE2, WHITE); // 강제로 배경 색을 바꿈(scr_claer X)
			printxy(0, 0, "");
			gameover_title();
			scr_switch();
			dot_clear(WIDTH / 4, HEIGHT / 3, 0); // clear 블록 지우기
			DrawMap();
			goto title_line;
		}
	}
}
