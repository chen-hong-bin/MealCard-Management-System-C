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
#define Max_AdminPassward_side 7
#define Min_AdminPassward_side 3

#define _STR(N) #N // 转字符串
#define STR(N) _STR(N) //宏展开遇到#或者##会停止展开

typedef struct Card
{
	char cardId[cardId_side];
	double balance;// 余额
	double add_balance;// 待加入的余额
	char status;// 用户状态：'0'正常；'1'冻结。
	char cardUserName[];// 柔性数组适应用户id长度
}User;

typedef struct CardList
{
	struct CardList* last;
	struct CardList* next;
	struct Card data;
}Node,*List;

struct BalanceCard
{
	struct BalanceCard* next;
	char cardUserName[];
};

struct DestoryCard
{
	struct DestoryCard* next;
	char cardUserName[];
};

//void Scan(FILE* pf);// 读取文件
void WelcomeMenu(void);
void UserMenu(int process);
void AdminMenu(int process);
//int FindId(char* findcardId,List cardList);// 查找userId是否存在：0，存在；1，不存在。
//int FindName(char* findcardName, List cardList);
List FindId(char* findcardId, List cardList);// 查找userId是否存在：NULL，不存在；返回地址值，存在。
List FindName(char* findcardName, List cardList);
int Add(List tail, List cardList);//添加饭卡信息 返回值：0，正常添加；1，因账号已存在而退出。
int DestoryId(char* cardName, List cardList);// 删除某饭卡信息。0，删除成功；1，因不存在而删除失败。
int DestoryName(char* cardName,List cardList);
double Balance(char* cardName, List cardList);
struct DestroyCard* FindStoryCard(char* findcardname, struct DestotyCard* DestoryCardList);
//void Print(FILE* pf);// 存储文件后关闭




int main(void)
{
	char AdminPassward[Max_AdminPassward_side] = "admin";

	srand((unsigned int)time(NULL));
	//FILE* pf = fopen("card.dat", "ab+");

	Node* HEAD = (Node*)malloc(sizeof(Node));//头节点便于操作
	HEAD->next = NULL;
	HEAD->last = NULL;
	HEAD->data.balance = 0;
	HEAD->data.add_balance = 0;
	HEAD->data.status = 0;
	strcpy(HEAD->data.cardId,"0000000");

	List cardList = HEAD;
	List tail = HEAD;

	struct BalanceCard* BalanceHEAD = (struct BalanceCard*)malloc(sizeof(struct BalanceCard));
	BalanceHEAD->next = NULL;

	struct BalanceCard* BalanceCardList = BalanceHEAD;
	struct BalanceCard* tailBalanceCard = BalanceHEAD;

	struct DestoryCard* DestoryHEAD = (struct DestoryCard*)malloc(sizeof(struct DestoryCard));
	DestoryHEAD->next = NULL;

	struct DestoryCard* DestoryCardList = DestoryHEAD;
	struct DestoryCard* tailDestoryCard = DestoryHEAD;

	//Scan(pf);
	printf("-------------------------------\n");
	printf("\t欢迎使用本系统~\n");
	printf("\t请先登录\n");
	char opt[2];
	do
	{
		WelcomeMenu();
		scanf("%1s", opt[0]);
		switch (opt[0])
		{
		case '1':
			UserMenu(0);
			char cardName[Max_cardUserName_side];
			scanf("%s", cardName);
			List user = FindName(cardName, cardList);
			if(! user)
			{
				printf("该用户不存在，请联系管理员\n");
			}
			else 
			{
				if (user->data.status = '1')
				{
					printf("该卡已冻结，请联系管理员");
					opt[0] = '0';
					break;
				}
				UserMenu(1);
				scanf("%1s", &opt[0]);
				switch (opt[0])
				{
				case '1':
					printf("您的余额>%.2lf\n", Balance(cardName, cardList));
					break;
				case '2':
					printf("请输入您的充值金额\n");
					printf("精确到小数点后两位\n");
					double add_balance;
					do {
						scanf("%.2lf", add_balance);
						if (add_balance <= 0)
						{
							printf("必须为非负数,请重新输入\n");
						}
					} while (add_balance < 0);
					if (add_balance > 0)
					{
						user->data.add_balance = add_balance;
						struct BalanceCard* item = (struct BalanceCard*)malloc(sizeof(struct BalanceCard) + srtlen(user->data.cardUserName));
						strcpy(item->cardUserName, user->data.cardUserName);
						item->next = NULL;
						tailBalanceCard->next = item;
					}
					break;
				case '3':
					printf("请输入您的消费金额\n");
					printf("精确到小数点后两位\n");
					double consume;
					do {
						scanf("%.2lf", consume);
						if (consume <= 0)
						{
							printf("必须为非负数,请重新输入\n");
						}
					} while ((consume <= 0));
					if (user->data.balance - consume < 0)
					{
						printf("余额不足，消费失败\n");
					}
					else
					{
						printf("消费成功\n");
						user->data.balance -= consume;
					}
					break;
				case '4':
					printf("您确定要注销吗\n确定请按 1\n");
					printf("如需退出请按 0");
					scanf("%1s", &opt[0]);
					if (opt[0] = '1')
					{
						if (user->data.balance != 0)
						{
							printf("申请注销失败，您的余额不为0\n");
						}
						else if (user->data.add_balance != 0)
						{
							printf("申请注销失败，您还有充值未到账\n");
						}
						else if (FindDestoryCardName(user->data.cardUserName, DestoryCardList))
						{
							printf("请勿重复提交注销请求，谢谢\n");
						}
						else
						{
							struct DestoryCard* item = (struct DestoryCard*)malloc(sizeof(struct DestoryCard) + srtlen(user->data.cardUserName));
							strcpy(item->cardUserName, user->data.cardUserName);
							item->next = NULL;
							tailDestoryCard->next = item;
						}
					}
					break;
				default:
					if (opt[0] != '0')
					{
						printf("选择不合法，请重新输入。\n");
					}
					break;
				}
			}
			break;

		case '2':
			AdminMenu(0);
			char adminPassward[Max_AdminPassward_side];
			while (1)
			{
				scanf("%s", adminPassward);
				if (!strcmp(adminPassward, "0"))
				{
					opt[0] = '0';
					break;
				}
				if (strcmp(adminPassward, AdminPassward))
				{
					printf("密钥错误，请重新输入。\n");
				}
				else
				{
					AdminMenu(1);
					scanf("%1s", &opt[0]);
					switch (opt[0])
					{
					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
					default:
						if (opt[0] != '0')
						{
							printf("选择不合法，请重新输入。\n");
						}
						break;
					}
				}
			}
			break;

		default:
			if (opt[0] != '0')
			{
				printf("选择不合法，请重新输入。\n");
			}
			break;
		}
		printf("\n");
		if (opt[0] = '0')
		{
			printf("感谢使用本软件~\n");
			break;
		}
	} while (opt[0] != '0');
	// Print(pf);
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

void UserMenu(int process)
{
	switch (process)
	{
	case 0:
		printf("-------------------------------\n");
		printf("\t请输入您的饭卡号\n");
		printf("-------------------------------\n");
		printf("\t如需退出请按 0\n");
		printf("-------------------------------\n");
		break;
	case 1:
		printf("-------------------------------\n");
		printf("\t请选择您需要办理的业务\n");
		printf("-------------------------------\n");
		printf("\t1 查询余额\n");
		printf("\t2 充值\n");
		printf("\t3 使用饭卡\n");
		printf("\t4 注销饭卡\n");
		printf("\t0 退出\n");
		printf("-------------------------------\n");
		break;
	}
}

void AdminMenu(int process)
{
	switch (process)
	{
	case 0:
		printf("-------------------------------\n");
		printf("\t请输入您的管理员密钥\n");
		printf("\t默认为admin\n");
		printf("-------------------------------\n");
		printf("\t如需退出请按 0\n");
		printf("-------------------------------\n");
		break;
	case 1:
		printf("-------------------------------\n");
		printf("\t请选择您要进行的操作\n");
		printf("-------------------------------\n");
		printf("\t1 处理充值请求\n");
		printf("\t2 处理注销请求\n");
		printf("\t3 查看所有饭卡信息\n");
		printf("\t4 添加饭卡\n");
		printf("\t5 解冻与冻结\n");
		printf("\t0 退出\n");
		printf("-------------------------------\n");
		break;
	}
}
//
//int FindId(char* findcardId,List cardList)
//{
//	List current = cardList->next;
//
//	while (current)
//	{
//		if (!stecmp(findcardId, current->data.cardId))
//		{
//			current = current->next;
//		}
//		else
//			return 0;
//	}
//	return 1;
//}
//
//int FindName(char* findcardName, List cardList)
//{
//	List current = cardList->next;
//
//	while (current)
//	{
//		if (!stecmp(findcardName, current->data.cardUserName))
//		{
//			current = current->next;
//		}
//		else
//			return 0;
//	}
//	return 1;
//}

List FindId(char* findcardId, List cardList)
{
	List current = cardList->next;

	while (current)
	{
		if (!strcmp(findcardId, current->data.cardId))
		{
			current = current->next;
		}
		else
			return current;
	}
	return NULL;
}

List FindName(char* findcardname, List cardList)
{
	List current = cardList->next;

	while (current)
	{
		if (!strcmp(findcardname, current->data.cardUserName))
		{
			current = current->next;
		}
		else
			return current;
	}
	return NULL;
}

double Balance(char* cardName, List cardList)
{
	List item = FindName(cardName,cardList);
	return item->data.balance;
}

int Add(List tail,List cardList)
{
	char name[2 * Max_cardUserName_side];
	printf("请输入姓名。一经建立无法修改。\n");
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
	if (FindName(name,cardList))
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
	user->last = tail;
	tail->next = user;
	tail = user;
	user->next = NULL;
}

int DestoryId(char* cardId, List cardList)
{
	List item0 = FindId(cardId, cardList);
	if (item0)
	{
		printf("该账号不存在。删除进程关闭。\n");
		return 1;
	}
	List item1 = item0;
	List item2 = item1;
	item1 = item0->last;
	item2 = item0->next;
	item1->next = item0->next;
	item2->last = item0->last;
	free(item0);
}


int DestoryName(char* cardName, List cardList)
{
	List item0 = FindName(cardName, cardList);
	if (item0)
	{
		printf("该账号不存在。删除进程关闭。\n");
		return 1;
	}
	List item1 = item0;
	List item2 = item1;
	item1 = item0->last;
	item2 = item0->next;
	item1->next = item0->next;
	item2->last = item0->last;
	free(item0);
}

struct DestroyCard* FindStoryCard(char* findcardname, struct DestoryCard* DestoryCardList)
{
	struct DestoryCard* current = DestoryCardList->next;

	while (current)
	{
		if (!strcmp(findcardname, current->cardUserName))
		{
			current = current->next;
		}
		else
			return current;
	}
	return NULL;
}

//void Print(FILE* pf)
//{
//
//	fclose(pf);
//	pf = NULL;
//}