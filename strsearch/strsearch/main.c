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
	printf("���ڿ��� Ž���� ������ �̸��� �Է��ϼ���. \n"); //ANSI ���ڵ��� ���� �ʿ�
	scanf("%s", file);

	int fileresult = html_to_text(file);

	if (fileresult == 0) {
		printf("�������� Ž���� �ܾ �Է��ϼ���. \n");
		scanf("%s", find);

		find_in_txt(find);
	}

	search_continue(file, find, fileresult);

	return 0;
}

void search_continue(char* file, char* find, int fileresult) {
	/* ���� ������ ���� ����ؼ� ���� ������ ����
	*/
	while (1) {
		printf("'1'�� ���� �ٸ� ���ڿ��� Ž��, '2'�� ���� ���ο� ������ Ž��, '0'�� ���� ���� \n");
		int input = getch();

		switch (input) {
		case '1':
			if (fileresult == 0) {
				printf("�������� Ž���� �ܾ �Է��ϼ���. \n");
				scanf("%s", find);

				find_in_txt(find);
			}
			else printf("Ž���� ������ ã�� �� �����ϴ�.");
			break;

		case '2':
			printf("���ڿ��� Ž���� ������ �̸��� �Է��ϼ���. \n");
			scanf("%s", file);

			int fileresult = html_to_text(file);
			break;

		case '0':
			exit(0);
			break;

		default:
			printf("�߸� �Է��ϼ̽��ϴ�.");
			break;
		}

		printf("\n");
	}
}

int html_to_text(char* name) {
	/* ���ϸ��� �μ��� �Ͽ� HTML ������ �ҷ��� �� �ؽ�Ʈ ���Ϸ� ��ȯ�ϴ� �Լ�.
	*/
	FILE* html, * txt;
	char buffer[1000];

	printf("������ Ȯ���߽��ϴ�. \n");

	html = fopen(name, "r");
	txt = fopen("test.txt", "w");
	if (html == NULL) {
		printf("������ ã�� �� �����ϴ�.");
		return 1;
	} // test.txt���� ���� �� html���� �ҷ���

	int body = 0; // <body> �±��� �߻� ���� Ȯ�ο�

	while (!feof(html)) {
		fgets(buffer, 1000, html);

		if (strstr(buffer, "<body") != NULL) body = 1; // <body�� ���� ��� Ȯ�� ����

		if (body) {
			int linelength = 0;
			int maxpos = buffer + strlen(buffer); // ������ �ִ� ��ġ Ȯ��
			char* start = buffer;
			char* lastpos = buffer; // �ֱ� ��ġ ����

			int pos = strcspn(buffer, "<");
			lastpos += pos;
			linelength += lastpos - start;

			print_to_txt(start, lastpos, txt); // ù < ���ڰ� ������ ���� ���ڸ� test.txt�� ���
			
			while(lastpos < maxpos) {
				pos = strcspn(lastpos, ">");
				lastpos += pos;
				start = lastpos + 1;
				
				pos = strcspn(lastpos, "<"); 
				lastpos += pos;

				linelength += lastpos - start;
				if(linelength > 1 || *(start + 1) != '\0') print_to_txt(start, lastpos, txt); /* > ���ڿ� < ���� ���� ���ڸ� test.txt�� ���
																			   ���� ��ü�� ���� ���ڸ� �ִ� ��� ���� ���� */
			}
		}

		if (strstr(buffer, "</body") != NULL) body = 1; // </body�� ���� ��� Ȯ�� ����

	}

	fclose(html);
	fclose(txt);

	return 0;
}

void print_to_txt(char* start, char* lastpos, FILE* txt) {
	/* start���� lastpos ���� ���������� �����͸� �ؽ�Ʈ�� ���
	*/

	while (start < lastpos) {
		if (*start < 0 && start < lastpos - 1) {
			//ASCII ���� ���� ���ڰ� ���� ���(�ѱ��� ���)
			char letter[3] = { start[0], start[1], '\0' };

			fputs(letter, txt);
			start += 2;
		}
		else {
			//ASCII ���� ���� ���ڰ� �ִ� ���(������ ���)
			fputc(*start, txt);
			start++;
		}
	}
};

void find_in_txt(char* find) {
	/* test.txt���Ͽ��� ���ϴ� �ܾ �˻� �� �����ϴ� ���� ���
	*/
	FILE* txt = fopen("test.txt", "r");
	char buffer[200]; // ���� ���̴� �ѱ� 100�ڷ� ����, ������ 100�� �̻��� ��� ���� �������� �ѱ�
	int previous = 0;
	fgets(buffer, 200, txt);
	int now = ftell(txt);
	int found = 1;

	if (strstr(buffer, find) != NULL) {
		printf("%d��° ��� \n", found++);
		print_from_txt(previous, txt); // ù ���忡 �ܾ ���Ե� ���
	}

	while (!feof(txt)) {
		previous = now;
		fgets(buffer, 200, txt);
		now = ftell(txt);

		if (strstr(buffer, find) != NULL) {
			printf("%d��° ��� \n", found++);
			print_from_txt(previous, txt);
		}
	}

	if (found == 1) printf("�˻� ����� �����ϴ�.");
	fclose(txt);
}

void print_from_txt(char* start, FILE* txt) {
	/* start ��ġ���� 3���� ���� ���
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