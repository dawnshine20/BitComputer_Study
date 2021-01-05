﻿#define _CRT_SECURE_NO_WARNINGS 
#include <stdio.h>
#include <stdlib.h>
#include<string.h>

#define HEADERSIZE 250
#define INTBYTESIZE 4
#define SHORTBYTESIZE 2
typedef struct
{
	int width;
	int height;
	int pictureNumber;
}INFORMATION;

int main()
{
	INFORMATION jsfInfo;
	memset(&jsfInfo, 0, sizeof(INFORMATION));    // p1을 구조체 크기만큼 0으로 설정

	FILE* fpFile = NULL;
	FILE* fpFile1 = NULL;
	int currentPos = 0;

	fpFile = fopen("0000.JSF", "rb");
	if (fpFile == NULL)
	{
		printf("읽기 실패");
		return 0;
	} // 파일 열기에 실패하면

	fseek(fpFile, 0, SEEK_SET); // 첫번째 파일포인터

	fseek(fpFile, HEADERSIZE, SEEK_CUR);                    //처음 위치에서 헤더사이즈byte 뒤로 설정
	printf("현재 위치는 : %ld\n", ftell(fpFile));			//fseek 후 파일 포인터 위치 확인

	fread(&jsfInfo.pictureNumber, sizeof(long), 1, fpFile);
	printf("그림의 갯수는 : %d개 입니다.\n", jsfInfo.pictureNumber);

	fseek(fpFile, 26, SEEK_CUR);// 예약어 무시

	fread(&jsfInfo.width, sizeof(long), 1, fpFile);
	printf("압축해상도 size는 : %d입니다.\n", jsfInfo.width);

	fread(&jsfInfo.width, sizeof(short), 1, fpFile);
	printf("가로 size는 : %d입니다.\n", jsfInfo.width);
	fread(&jsfInfo.height, sizeof(short), 1, fpFile);
	printf("세로 size는 : %d입니다.\n\n", jsfInfo.height);


	fpFile1 = fopen("sample.txt", "w");
	
	//--------------------------------------
	int heightCount = 0;
	//while (jsfInfo.height != heightCount)
	while (jsfInfo.height != 1)
	{
		int location = ftell(fpFile);
		heightCount++;

		short idNumber = 0; // 식별번호

		short setCount = 0; // 세트갯수
		short spaceCount = 0; // 공백갯수
		short pixelCount = 0; // 그림갯수

		fread(&idNumber, sizeof(short), 1, fpFile);
		printf("시작 식별번호는 : %d입니다.\n", idNumber);

		if (idNumber == 0x0000)
		{
			for (int i = 0; i < jsfInfo.width; i++)
			{
				fprintf(fpFile1, "%c%c", '-', '-');
			}
			fprintf(fpFile1, "\n");
		}
		else if (idNumber == 0x0001)
		{
			for (int i = 0; i < jsfInfo.width; i++)
			{
				fseek(fpFile, sizeof(short), SEEK_CUR);
				fprintf(fpFile1, "%c%c", '0', '0');
			}
			fprintf(fpFile1, "\n");
		}
		else // 공백과 데이터 공존
		{
			int temp = 0;
			setCount = idNumber - 1;

			for (int i = 0; i < setCount; i++)
			{
				int location = ftell(fpFile);
				fread(&spaceCount, sizeof(short), 1, fpFile);//공백갯수
				printf("공백 갯수는 : %d입니다.\n", spaceCount);
				fread(&pixelCount, sizeof(short), 1, fpFile);//데이터갯수
				printf("픽셀 갯수는 : %d입니다.\n", pixelCount);

				temp += spaceCount;
				temp += pixelCount;

				for (int j = 0; j < spaceCount; j++)
				{
					fseek(fpFile, sizeof(short), SEEK_CUR);
					fprintf(fpFile1, "%c%c", '-', '-');
				}
				for (int j = 0; j < pixelCount; j++)
				{
					fseek(fpFile, sizeof(short), SEEK_CUR);
					fprintf(fpFile1, "%c%c", '0', '0');
				}
			}

			int rest = jsfInfo.width - temp - 1;
			for (int i = 0; i < rest; i++)
			{
				fseek(fpFile, sizeof(short), SEEK_CUR);
				fprintf(fpFile1, "%c%c", '-', '-');
			}

			fprintf(fpFile1, "\n");
		}
	}
	

	fclose(fpFile1);    // 텍스트 파일 닫기
	fclose(fpFile);    // 이미지 파일 닫기

	return 0;
}

//int linenumber = 0;
//while (linenumber != jsfInfo.height)
//{
//
//	int setCount = 0;
//	int spacePixelCnt = 0;
//	int dotPixelCnt = 0;
//	int twoByteCnt = 0;
//	//식별번호
//	fseek(fpFile, SHORTBYTESIZE, SEEK_CUR);
//	fread(&setCount, sizeof(short), 1, fpFile);
//
//	//0000 공백, 0001 모두 색상, 그 외에는 모두 공백 색상 존재
//	if (setCount != 0x0000 && setCount != 0x0001)
//	{
//		int setLoopCnt = 0;
//		while (1)
//		{
//			//공백 갯수
//
//			fseek(fpFile, SHORTBYTESIZE, SEEK_CUR);
//			fread(&spacePixelCnt, sizeof(short), 1, fpFile);
//			//점의 갯수
//			fseek(fpFile, SHORTBYTESIZE, SEEK_CUR);
//			fread(&dotPixelCnt, sizeof(short), 1, fpFile);
//
//			for (int i = 0; i < spacePixelCnt; i++)
//			{
//				twoByteCnt++;
//				fseek(fpFile, SHORTBYTESIZE, SEEK_CUR);
//
//				fprintf(fpFile1, "%c%c", '-', '-');    // 텍스트 파일에 문자 2BYTE 출력
//
//			}
//			for (int i = 0; i < dotPixelCnt; i++)
//			{
//				twoByteCnt++;
//				fseek(fpFile, SHORTBYTESIZE, SEEK_CUR);
//
//				fprintf(fpFile1, "%c%c", 'O', 'O');    // 텍스트 파일에 문자 2BYTE 출력
//			}
//			setLoopCnt++;
//
//			//총 세트 카운트와 세트의 반복횟수가 같으면 나머지도 마저 처리한다.
//			if (setCount - 1 == setLoopCnt)
//			{
//				while (1)
//				{
//					twoByteCnt++;
//					fseek(fpFile, SHORTBYTESIZE, SEEK_CUR);
//
//					fprintf(fpFile1, "%c%c", '-', '-');    // 텍스트 파일에 문자 2BYTE 출력
//					if (twoByteCnt == jsfInfo.height - 1)
//					{
//						break;
//					}
//				}
//			}
//			//한줄 다 읽었으면 루프문 탈출
//			if (twoByteCnt == jsfInfo.height - 1)
//			{
//				break;
//			}
//		}
//	}
//
//
//	if (setCount == 0x0000)
//	{
//		for (int i = 0; i < jsfInfo.width; i++)
//		{
//			twoByteCnt++;
//			fprintf(fpFile1, "%c%c", '-', '-');    // 텍스트 파일에 문자 2BYTE 출력
//		}
//	}
//
//	if (setCount == 0x0001)
//	{
//		//점의 갯수
//		fseek(fpFile, SHORTBYTESIZE, SEEK_CUR);
//		fread(&dotPixelCnt, sizeof(short), 1, fpFile);
//
//		for (int i = 0; i < jsfInfo.width; i++)
//		{
//			twoByteCnt++;
//			fseek(fpFile, SHORTBYTESIZE, SEEK_CUR);
//
//			fprintf(fpFile1, "%c%c", 'O', 'O');    // 텍스트 파일에 문자 2BYTE 출력
//		}
//	}
//
//	linenumber++;
//	fprintf(fpFile1, "\n");    // 가로 픽셀 저장이 끝났으면 줄바꿈을 해줌
//
//}