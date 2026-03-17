#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define cardId_side 8//7位数字，一位'\0'
#define cardId_Min (pow(10,cardId_side-1)+1) 
#define cardId_Max (pow(10,cardId_side)-1)

#define Max_cardUserName_side 7

#define _STR(N) #N // 转字符串
#define STR(N) _STR(N) //宏展开遇到#或者##会停止展开

typedef struct Card
{
	char cardId[cardId_side];
	double balance;// 余额
	char status;// 用户状态：'0'正常；'1'冻结。
	char cardUserName[];// 柔性数组适应用户id长度
}User;

typedef struct CardList
{
	struct CardList* next;
	struct Card data;
}Node,*List;

//void Scan(FILE* pf);// 读取文件
void WelcomeMenu(void);
void UserMenu(void);
void AdminMenu(void);
int FindId(char* findcardId,List cardList);// 查找userId是否存在：0，存在；1，不存在。
int FindName(char* findcardName, List cardList);
//void Print(FILE* pf);// 存储文件后关闭
int Add(List tail, List cardList);//添加饭卡信息 返回值：0，正常添加；1，因账号已存在而退出。




int main(void)
{
	srand((unsigned int)time(NULL));
	//FILE* pf = fopen("card.dat", "ab+");

	Node* HEAD = (Node*)malloc(sizeof(Node));//头节点便于操作
	HEAD->next = NULL;
	HEAD->data.balance = 0;
	HEAD->data.status = 0;
	strcpy(HEAD->data.cardId,"0000000");
	strcpy(HEAD->data.cardUserName, "头节点");

	List cardList = HEAD;
	List tail = HEAD;

	//Scan(pf);
	printf("-------------------------------\n");
	printf("\t欢迎使用本系统~\n");
	printf("\t请先登录\n");
	int opt;
	do
	{
		WelcomeMenu();
		scanf("%d", &opt);
		switch (opt)
		{
			char cardId[cardId_side];
		case 1:
			UserMenu();
			scanf("%s", cardId);
			Find(cardId,cardList);
			break;
		case 2:
			AdminMenu();
			break;
		default:
			if (!opt)
			{
				printf("感谢使用本软件~\n");
				//print(pf);
			}
			else printf("选择不合法，请重新输入。\n");
			break;
		}
		printf("\n");
	} while (opt);
	return 0;
}

//void Scan(FILE* pf)
//{
//	
//
//}

void WelcomeMenu(void)
{
	printf("-------------------------------\n");
	printf("\t请选择登录种类\n");
	printf("\t1 普通用户登录\n");
	printf("\t2 管理员登录\n");
	printf("-------------------------------\n");
	printf("\t如需退出请按 0\n");
	printf("-------------------------------\n");
}

void UserMenu(void)
{
	printf("-------------------------------\n");
	printf("\t请输入您的饭卡号\n");
	printf("-------------------------------\n");
	printf("\t如需退出请按 0\n");
	printf("-------------------------------\n");
}

void AdminMenu(void)
{
	printf("-------------------------------\n");
	printf("\t请输入您的管理员密钥\n");
	printf("\t默认为admin\n");
	printf("-------------------------------\n");
	printf("\t如需退出请按 0\n");
	printf("-------------------------------\n");
}

int FindId(char* findcardId,List cardList)
{
	List current = cardList->next;

	while (current)
	{
		if (!stecmp(findcardId, current->data.cardId))
		{
			current = current->next;
		}
		else
			return 0;
	}
	return 1;
}

int FindName(char* findcardName, List cardList)
{
	List current = cardList->next;

	while (current)
	{
		if (!stecmp(findcardName, current->data.cardUserName))
		{
			current = current->next;
		}
		else
			return 0;
	}
	return 1;
}

//void Print(FILE* pf)
//{
//
//	fclose(pf);
//	pf = NULL;
//}

int Add(List tail,List cardList)
{
	char name[2 * Max_cardUserName_side];
	while (1)
	{
		scanf("%"STR(2 * Max_cardUserName_side)"s", name);
		char ch;
		while (ch = getchar() != '\n');//清空缓冲区
		if (strlen(name) > Max_cardUserName_side + 1)
			break;
		else
		{
			printf("该姓名过于长，请重新输入吧\n");
		}
	}
	if (!FindName(name,cardList))
	{
		printf("该账号已存在。添加进程关闭。\n");
		return 1;
	}
	Node* user = (Node*)malloc(sizeof(Node) + strlen(name));
	strcpy(user->data.cardUserName, name);
	user->data.balance = 0.0;
	user->data.status = 0;
	int intId = rand()% (int)(cardId_Max-cardId_Min+1) + cardId_Min;
	/*
	char strId[cardId_side];
	sprintf(strId,"%d",intId);//随机数转字符串
	strcpy(user->node.cardId, strId);
	*/
	strcpy(user->data.cardId, STR(intId));
	tail->next = user;
	tail = user;
	user->next = NULL;
}