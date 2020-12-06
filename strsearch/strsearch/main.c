#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>

void search_continue(char*, char*, int);
int html_to_text(char*);
void print_to_txt(char*, char*, FILE*);
void find_in_txt(char*);
void print_from_txt(char*, FILE*);

int main(void) {
	char file[100];
	char find[100];
	printf("문자열을 탐색할 문서의 이름을 입력하세요. \n"); //ANSI 인코딩된 파일 필요
	scanf("%s", file);

	int fileresult = html_to_text(file);

	if (fileresult == 0) {
		printf("문서에서 탐색할 단어를 입력하세요. \n");
		scanf("%s", find);

		find_in_txt(find);
	}

	search_continue(file, find, fileresult);

	return 0;
}

void search_continue(char* file, char* find, int fileresult) {
	/* 무한 루프를 통해 계속해서 다음 진행을 선택
	*/
	while (1) {
		printf("'1'을 눌러 다른 문자열을 탐색, '2'를 눌러 새로운 문서를 탐색, '0'을 눌러 종료 \n");
		int input = getch();

		switch (input) {
		case '1':
			if (fileresult == 0) {
				printf("문서에서 탐색할 단어를 입력하세요. \n");
				scanf("%s", find);

				find_in_txt(find);
			}
			else printf("탐색할 문서를 찾을 수 없습니다.");
			break;

		case '2':
			printf("문자열을 탐색할 문서의 이름을 입력하세요. \n");
			scanf("%s", file);

			int fileresult = html_to_text(file);
			break;

		case '0':
			exit(0);
			break;

		default:
			printf("잘못 입력하셨습니다.");
			break;
		}

		printf("\n");
	}
}

int html_to_text(char* name) {
	/* 파일명을 인수로 하여 HTML 파일을 불러온 후 텍스트 파일로 변환하는 함수.
	*/
	FILE* html, * txt;
	char buffer[1000];

	printf("파일을 확인했습니다. \n");

	html = fopen(name, "r");
	txt = fopen("test.txt", "w");
	if (html == NULL) {
		printf("파일을 찾을 수 없습니다.");
		return 1;
	} // test.txt파일 생성 및 html파일 불러옴

	int body = 0; // <body> 태그의 발생 여부 확인용

	while (!feof(html)) {
		fgets(buffer, 1000, html);

		if (strstr(buffer, "<body") != NULL) body = 1; // <body가 나온 경우 확인 시작

		if (body) {
			int linelength = 0;
			int maxpos = buffer + strlen(buffer); // 버퍼의 최대 위치 확인
			char* start = buffer;
			char* lastpos = buffer; // 최근 위치 저장

			int pos = strcspn(buffer, "<");
			lastpos += pos;
			linelength += lastpos - start;

			print_to_txt(start, lastpos, txt); // 첫 < 글자가 나오기 이전 문자를 test.txt에 출력
			
			while(lastpos < maxpos) {
				pos = strcspn(lastpos, ">");
				lastpos += pos;
				start = lastpos + 1;
				
				pos = strcspn(lastpos, "<"); 
				lastpos += pos;

				linelength += lastpos - start;
				if(linelength > 1 || *(start + 1) != '\0') print_to_txt(start, lastpos, txt); /* > 글자와 < 글자 사이 문자를 test.txt에 출력
																			   문장 전체에 개행 문자만 있는 경우 개행 생략 */
			}
		}

		if (strstr(buffer, "</body") != NULL) body = 1; // </body가 나온 경우 확인 종료

	}

	fclose(html);
	fclose(txt);

	return 0;
}

void print_to_txt(char* start, char* lastpos, FILE* txt) {
	/* start에서 lastpos 이전 범위까지의 데이터를 텍스트로 출력
	*/

	while (start < lastpos) {
		if (*start < 0 && start < lastpos - 1) {
			//ASCII 범위 내에 문자가 없는 경우(한글인 경우)
			char letter[3] = { start[0], start[1], '\0' };

			fputs(letter, txt);
			start += 2;
		}
		else {
			//ASCII 범위 내에 문자가 있는 경우(영문인 경우)
			fputc(*start, txt);
			start++;
		}
	}
};

void find_in_txt(char* find) {
	/* test.txt파일에서 원하는 단어를 검색 후 포함하는 문장 출력
	*/
	FILE* txt = fopen("test.txt", "r");
	char buffer[200]; // 문장 길이는 한글 100자로 제한, 문장이 100자 이상인 경우 다음 문장으로 넘김
	int previous = 0;
	fgets(buffer, 200, txt);
	int now = ftell(txt);
	int found = 1;

	if (strstr(buffer, find) != NULL) {
		printf("%d번째 결과 \n", found++);
		print_from_txt(previous, txt); // 첫 문장에 단어가 포함된 경우
	}

	while (!feof(txt)) {
		previous = now;
		fgets(buffer, 200, txt);
		now = ftell(txt);

		if (strstr(buffer, find) != NULL) {
			printf("%d번째 결과 \n", found++);
			print_from_txt(previous, txt);
		}
	}

	if (found == 1) printf("검색 결과가 없습니다.");
	fclose(txt);
}

void print_from_txt(char* start, FILE* txt) {
	/* start 위치부터 3개의 문장 출력
	*/
	char result[200];

	fseek(txt, start, SEEK_SET);
	for (int i = 0; i < 3; i++) {
		fgets(result, 200, txt);
		puts(result);
	}

	printf("\n");

	fseek(txt, start, SEEK_SET);
	fgets(result, 200, txt);
};