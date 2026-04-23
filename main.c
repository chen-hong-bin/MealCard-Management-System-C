#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
void FixGbk(void) {
    SetConsoleOutputCP(936);
    SetConsoleCP(936);
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define cardId_side 8//7位数字，一位'\0'
#define cardId_Min (pow(10,cardId_side-2)+1) 
#define cardId_Max (pow(10,cardId_side-1)-1)

#define Max_cardUserName_side 11
#define Max_AdminPassward_side 7
#define Min_AdminPassward_side 3

#define _STR(N) #N // 转字符串
#define STR(N) _STR(N) //宏展开遇到#或者##会停止展开
#define STATUS(N) N=='1'?"冻结":"正常" 

typedef struct Card
{
	char cardId[cardId_side];
	double balance;// 余额
	double add_balance;// 待加入的余额
	int namelength;
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
	char cardUserId[cardId_side];
};

struct DestoryCard
{
	struct DestoryCard* next;
	char cardUserId[cardId_side];
};

void WelcomeMenu(void);
void UserMenu(int process);
void AdminMenu(int process);
List FindId(char* findcardId, List cardList);// 查找userId是否存在：NULL，不存在；返回地址值，存在。
List FindName(char* findcardName, List cardList);
int Add(List tail, List cardList,unsigned int* length);//添加饭卡信息 返回值：0，正常添加；1，因账号已存在而退出。
int DestoryName(char* cardName,List cardList);// 删除某饭卡信息。0，删除成功；1，因不存在而删除失败。
struct DestoryCard* FindDestoryCardName(char* findcardname, struct DestoryCard* DestoryCardList);
void ChargBalanceCard(struct BalanceCard** BalanceCardList, List cardList);
void HandleDestory(struct DestoryCard** DesHead, List cardList, unsigned int* length);
void View(List cardList);




int main(void)
{
	FixGbk();
	printf("温馨提示：如果没有正常关闭，更改的信息是不会保存的哟~\n");
	char AdminPassward[Max_AdminPassward_side];
	strcpy(AdminPassward, "admin");
	int flag = 1;//判断源文件是否存在
	srand((unsigned int)time(NULL));
	FILE* pf = fopen("card.dat", "ab+");
	if (pf == NULL)
	{
		pf = fopen("card.dat", "wb+");
		flag = 0;
	}
	if (pf == NULL)
	{
		printf("创建失败，程序退出\n");
		return 1;
	}
	unsigned int ListCardLength = 0, BalanceCardLength = 0, DestoryCardLength = 0, adminLength = 0;

	Node* HEAD = (Node*)malloc(sizeof(Node));//头节点便于操作
	HEAD->next = NULL;
	HEAD->last = NULL;
	HEAD->data.balance = 0;
	HEAD->data.add_balance = 0;
	HEAD->data.status = 0;
	HEAD->data.namelength = 0;
	strcpy(HEAD->data.cardId,"0000000");

	List cardList = HEAD;
	List tail = cardList;

	struct BalanceCard* BalanceHEAD = (struct BalanceCard*)malloc(sizeof(struct BalanceCard));
	BalanceHEAD->next = NULL;

	struct BalanceCard* BalanceCardList = BalanceHEAD;
	struct BalanceCard* tailBalanceCard = BalanceHEAD;

	struct DestoryCard* DestoryHEAD = (struct DestoryCard*)malloc(sizeof(struct DestoryCard));
	DestoryHEAD->next = NULL;

	struct DestoryCard* DestoryCardList = DestoryHEAD;
	struct DestoryCard* tailDestoryCard = DestoryHEAD;

	if (flag)
	{
		fread(&adminLength, sizeof(unsigned int), 1, pf);
		fread(AdminPassward, sizeof(char) * (adminLength + 1), 1, pf);
		fread(&ListCardLength, sizeof(unsigned int), 1, pf);
		fread(&BalanceCardLength, sizeof(unsigned int), 1, pf);
		fread(&DestoryCardLength, sizeof(unsigned int), 1, pf);
		fread(&HEAD->next, sizeof(HEAD->next), 1, pf);
		fread(&BalanceHEAD->next, sizeof(BalanceHEAD->next), 1, pf);
		fread(&DestoryHEAD->next, sizeof(DestoryHEAD->next), 1, pf);

		unsigned int item = ListCardLength;
		while (1)
		{
			fread(tail, sizeof(*tail), 1, pf);
			List a = realloc(tail, sizeof(Node) + (sizeof(char) * tail->data.namelength));
			if (a != NULL)
				tail = a;
			fread(tail->data.cardUserName, sizeof(char) * (tail->data.namelength + 1), 1, pf);
			if (item)
			{
				item--;
				tail = tail->next;
				tail = (Node*)malloc(sizeof(Node));
			}
			else
				break;
		}
		item = BalanceCardLength;
		while (1)
		{
			fread(tailBalanceCard, sizeof(*tailBalanceCard), 1, pf);
			tailBalanceCard = tailBalanceCard->next;
			if (item)
			{
				tailBalanceCard = (struct BalanceCard*)malloc(sizeof(struct BalanceCard));
			}
			else break;
		}
		item = DestoryCardLength;
		while (1)
		{
			fread(tailDestoryCard, sizeof(*tailDestoryCard), 1, pf);
			tailDestoryCard = tailDestoryCard->next;
			if (item)
			{
				tailDestoryCard = (struct DestoryCard*)malloc(sizeof(struct DestoryCard));
			}
			else break;
		}

		fclose(pf);
	}
	
	printf("-------------------------------\n");
	printf("\t欢迎使用本系统~\n");
	printf("\t请先登录\n");
	char opt[2] = { 0 };
	while (1)
	{
		WelcomeMenu();
		scanf("%1s", opt);
		switch (opt[0])
		{
		case '1':
		{
			UserMenu(0);
			char cardid[cardId_side] = { 0 };
			scanf("%s", cardid);

			if (cardid[0] == '0') { opt[0] = '0'; continue; }

			List user = FindId(cardid, cardList);
			if (user == NULL)
			{
				printf("该用户不存在，请联系管理员\n");
				break;
			}
			else
			{
				if (user->data.status == '1')
				{
					printf("该卡已冻结，请联系管理员\n");
					break;
				}
			}
			// 用户功能菜单
			while (1) {
			printf("姓名：%s\n",user->data.cardUserName);
			UserMenu(1);
				scanf("%1s", opt);
				switch (opt[0])
				{
				case '1':
					printf("您的余额：%.2lf\n", user->data.balance);
					break;
				case '2':
				{
					printf("请输入充值金额（精确到小数点后两位）：\n");
					double add_balance = 0;
					while (scanf("%lf", &add_balance) != 1 || add_balance <= 0)
					{
						printf("输入非法！请输入正数：\n");
						// 清空缓冲区
						while (getchar() != '\n');
					}
					user->data.add_balance += add_balance;
					struct BalanceCard* item = (struct BalanceCard*)malloc(sizeof(struct BalanceCard));
					strcpy(item->cardUserId, user->data.cardId);
					item->next = BalanceHEAD->next;
					BalanceHEAD->next = item;
					BalanceCardLength++;
					printf("充值申请提交成功！等待管理员处理\n");
					break;
				}
				case '3':
				{
					printf("请输入消费金额：\n");
					double consume = 0;
					while (scanf("%lf", &consume) != 1 || consume <= 0)
					{
						printf("输入非法！请输入正数：\n");
						while (getchar() != '\n');
					}
					if (user->data.balance < consume)
					{
						printf("余额不足，消费失败\n");
					}
					else
					{
						user->data.balance -= consume;
						printf("消费成功！当前余额：%.2lf\n", user->data.balance);
					}
					break;
				}
				case '4':
				{
					printf("确定注销？1确认/0退出：\n");
					scanf("%1s", opt);
					if (opt[0] == '1')
					{
						if (user->data.balance != 0)
						{
							printf("注销失败：余额不为0\n");
						}
						else if (user->data.add_balance != 0)
						{
							printf("注销失败：有待充值金额\n");
						}
						else if (FindDestoryCardName(user->data.cardUserName, DestoryHEAD))
						{
							printf("请勿重复提交注销申请\n");
						}
						else
						{
							struct DestoryCard* item = (struct DestoryCard*)malloc(sizeof(struct DestoryCard));
							strcpy(item->cardUserId, user->data.cardId);
							item->next = DestoryHEAD->next;
							DestoryHEAD->next = item;
							DestoryCardLength++;
							printf("注销申请提交成功！\n");
						}
					}
					break;
				}
				case '0':
					printf("退出用户登录\n");
					break;
				default:
					printf("选择不合法！\n");
					char a;
					while (a=getchar() != '\n');
					break;
				}
				if (opt[0] == '0')
					break;
			}
			opt[0] = '1';
			break;
		}

		case '2':
		{
			AdminMenu(0);
			char adminPassward[Max_AdminPassward_side] = { 0 };
			scanf("%s", adminPassward);

			if (adminPassward[0] == '0')
			{
				break;
			}
			if (strcmp(adminPassward, AdminPassward) != 0)
			{
				printf("密码错误！\n");
				break;
			}

			// 管理员功能菜单
			while (1)
			{
				AdminMenu(1);
				scanf("%1s", opt);
				switch (opt[0])
				{
				case '1':
					ChargBalanceCard(&BalanceHEAD, cardList);
					BalanceCardLength=0;
					break;
				case '2':
					HandleDestory(&DestoryHEAD, cardList,&ListCardLength);
					DestoryCardLength=0;
					break;
				case '3':
					View(cardList);
					break;
				case '4':
					//  Add(tail, cardList,&ListCardLength); 
						printf("请输入姓名（最长为7）。一经建立无法修改。\n");
						char name[2 * Max_cardUserName_side];
						int namelength;
						while (1)
						{
							scanf("%20s", name);
							char ch;
							while ((ch = getchar()) != '\n');//清空缓冲区
							if ((namelength = strlen(name)) < (Max_cardUserName_side + 1))
								break;
							else
							{
								printf("该姓名过于长，请重新输入吧\n");
							}
						}
						Node* user = (Node*)malloc(sizeof(Node) + strlen(name));
						if (user == NULL)
						{
							printf("创建失败\n");
							return 1;
						}
						strcpy(user->data.cardUserName, name);
						user->data.balance = 0.0;
						user->data.add_balance = 0.0;
						user->data.status = 0;
						user->data.namelength = namelength;
						char strId[cardId_side];
						do {
							int intId = rand() % (int)(cardId_Max - cardId_Min) + cardId_Min;
							sprintf(strId, "%d", intId);//随机数转字符串
						} while (FindId(strId, cardList) != NULL);
						strcpy(user->data.cardId, strId);

						ListCardLength++;

						user->next = cardList->next;
						user->last = cardList;
						if (cardList->next != NULL)
							cardList->next->last = user;
						cardList->next = user;

						user->next = NULL;
						printf("创建成功。您的饭卡ID为>%s\n", user->data.cardId);
					break;

				case '5':
					printf("-------------------------------\n");
					printf("\t请选择您要进行的操作\n");
					printf("-------------------------------\n");
					printf("\t1冻结/2解冻\n");
					printf("\t如需退出请按 0\n");
					printf("-------------------------------\n");
					scanf("%1s", &opt[0]);
					switch (opt[0])
					{
					case '0':
						break;
					case '1':
					{
						printf("请输入需要冻结的饭卡号\n");
						char cardName[Max_cardUserName_side] = { 0 };
						scanf("%s", cardName);
						List user = FindName(cardName, cardList);
						if (user == NULL)
						{
							printf("该用户不存在\n");
							break;
						}
						else if (user->data.status == '1')
						{
							printf("该用户已经处于冻结状态\n");
						}
						else
						{
							user->data.status = '1';
							printf("冻结成功\n");
						}
						printf("冻结进程退出\n");
						break;
					}
					case '2':
					{
						printf("请输入需要解冻的饭卡号\n");
						char cardName[Max_cardUserName_side] = { 0 };
						scanf("%s", cardName);
						List user = FindName(cardName, cardList);
						if (user == NULL)
						{
							printf("该用户不存在\n");
						}
						else if (user->data.status == '0')
						{
							printf("该用户已经处于正常状态\n");
						}
						else
						{
							user->data.status = '0';
							printf("解冻成功\n");
						}
						printf("解冻进程退出\n");
						break;
					}
					default:
						printf("选择不合法，请重新输入\n");
						char a;
						while (a = getchar() != '\n');
						break;
					}
					break;

				case '6':
					printf("请输入新密码\n");
					while (1)
					{
						scanf("%s", adminPassward);
						int length = strlen(adminPassward);
						if (length <= Max_AdminPassward_side && length >= Min_AdminPassward_side)
						{
							strcpy(AdminPassward, adminPassward);
							adminLength = length;
							break;
						}
						else 
						{
							printf("长度不符合，请重新输入\n");
						}
					}
					break;
				case '0':
					printf("退出管理员登录\n");
					break;
				default:
					printf("选择不合法！\n");
					char a;
					while (a = getchar() != '\n');
					break;
				}
				if (opt[0] == '0')
				{
					break;
				}
			}
			opt[0] = '1';
		}

		case '0':
			printf("感谢使用本软件~\n");
			break;

		default:
			printf("选择不合法，请重新输入\n");
			char a;
			while (a = getchar() != '\n');
			break;
		}

		printf("\n");

		if (opt[0] == '0')
		{
			break;
		}
	}

	pf = fopen("card.dat", "wb+");

	rewind(pf);
	fwrite(&adminLength, sizeof(unsigned int), 1, pf);
	fwrite(AdminPassward, sizeof(char)* (adminLength + 1), 1, pf);
	fwrite(&ListCardLength, sizeof(unsigned int), 1, pf);
	fwrite(&BalanceCardLength, sizeof(unsigned int), 1, pf);
	fwrite(&DestoryCardLength, sizeof(unsigned int), 1, pf);
	fwrite(&HEAD->next, sizeof(HEAD->next), 1, pf);
	fwrite(&BalanceHEAD->next, sizeof(BalanceHEAD->next), 1, pf);
	fwrite(&DestoryHEAD->next, sizeof(DestoryHEAD->next), 1, pf);

	HEAD = HEAD->next;
	while (HEAD != NULL)
	{
		fwrite(HEAD, sizeof(Node), 1, pf);
		fwrite(HEAD->data.cardUserName, sizeof(char) * (HEAD->data.namelength), 1, pf);
		if (HEAD != NULL)
		{
			Node* item = HEAD->next;
			free(HEAD);
			HEAD = item;
		}
	}
	BalanceHEAD = BalanceHEAD->next;
	while (BalanceHEAD != NULL)
	{
		struct BalanceCard* item = BalanceHEAD->next;
		fwrite(BalanceHEAD, sizeof(*BalanceHEAD), 1, pf);
		free(BalanceHEAD);
		BalanceHEAD = item;
	}
	DestoryHEAD = DestoryHEAD->next;
	while (DestoryHEAD != NULL)
	{
		struct DestoryCard* item = DestoryHEAD->next;
		fwrite(DestoryHEAD, sizeof(*DestoryHEAD), 1, pf);
		free(DestoryHEAD);
		DestoryHEAD = item;
	}

	fclose(pf);
return 0;
}


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
		printf("\t6 更改密码\n");
		printf("\t0 退出\n");
		printf("-------------------------------\n");
		break;
	}
}

List FindId(char* findcardId, List cardList)
{
	if (cardList == NULL||cardList->next == NULL|| findcardId == NULL )
		return NULL;

	List current = cardList->next;

	while (current)
	{
		if (strcmp(findcardId, current->data.cardId) != 0)
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
		if (strcmp(findcardname, current->data.cardUserName) != 0)
		{
			current = current->next;
		}
		else
			return current;
	}
	return NULL;
}

int Add(List tail,List cardList, unsigned int *length)
{
	char name[2 * Max_cardUserName_side];
	printf("请输入姓名（最长为7）。一经建立无法修改。\n");
	int namelength;
	while (1)
	{
		scanf("%20s", name);
		char ch;
		while ((ch = getchar() )!= '\n');//清空缓冲区
		if ((namelength = strlen(name) ) < (Max_cardUserName_side + 1))
			break;
		else
		{
			printf("该姓名过于长，请重新输入吧\n");
		}
	}
	Node* user = (Node*)malloc(sizeof(Node) + strlen(name));
	if (user == NULL)
	{
		printf("创建失败\n");
		return 1;
	}
	strcpy(user->data.cardUserName, name);
	user->data.balance = 0.0;
	user->data.add_balance = 0.0;
	user->data.status = 0;
	user->data.namelength = namelength;
	char strId[cardId_side];
	do {
		int intId = rand() % (int)(cardId_Max - cardId_Min) + cardId_Min;
		sprintf(strId, "%d", intId);//随机数转字符串
	} while (FindId(strId,cardList) != NULL);
	strcpy(user->data.cardId, strId);
	
	(*length)++;
	user->last = tail;
	tail->next = user;
	tail = user;

	user->next = NULL;
	printf("创建成功。您的饭卡ID为>%s\n",user->data.cardId);
	return 0;
}

int DestoryName(char* cardName, List cardList)
{
	List item0 = FindName(cardName, cardList);
	List item1 = item0;
	List item2 = item1;
	item1 = item0->last;
	item2 = item0->next;
	item1->next = item0->next;
	item2->last = item0->last;
	free(item0);
	return 0;
}

struct DestoryCard* FindDestoryCardName(char* findcardname, struct DestoryCard* DestoryCardList)
{
	struct DestoryCard* current = DestoryCardList->next;

	while (current)
	{
		if (strcmp(findcardname, current->cardUserId))
		{
			current = current->next;
		}
		else
			return current;
	}
	return NULL;
}

void ChargBalanceCard(struct BalanceCard** BalanceCardList, List cardList)
{
	{
		struct BalanceCard* p = (*BalanceCardList)->next;
		struct BalanceCard* temp = NULL;
		if (!p)
		{
			printf("暂无充值申请！\n");
			return;
		}

		while (p)
		{
			temp = p->next;
			List user = FindId(p->cardUserId, cardList);
			if (user)
			{
				user->data.balance += user->data.add_balance;
				user->data.add_balance = 0;
				printf("用户【%s】充值成功！余额：%.2lf\n", user->data.cardUserName, user->data.balance);
			}
			free(p);
			p = temp;
		}
		(*BalanceCardList)->next = NULL;
		printf("所有充值处理完毕！\n");
	}
}

void HandleDestory(struct DestoryCard** DesHead, List cardList, unsigned int* length)
{
	struct DestoryCard* p = (*DesHead)->next;
	struct DestoryCard* temp = NULL;
	if (!p)
	{
		printf("暂无注销申请！\n");
		return;
	}

	while (p)
	{
		temp = p->next;
		List user = FindId(p->cardUserId, cardList);
		if (user)
		{
			DestoryName(user->data.cardUserName,cardList);
			(*length)--;
			printf("用户【%s】注销成功！\n", user->data.cardUserName);
		}
		free(p);
		p = temp;
	}
	(*DesHead)->next = NULL;
	printf("所有注销处理完毕！\n");
}

void View(List cardList)
{
	List user = cardList;
	if (user->next == NULL|| user == NULL)
	{
		printf("暂无饭卡信息\n");
		return;
	}
	printf("姓名\t饭卡号\t\t实际金额\t未到帐金额\t状态\n");
	while (user = user->next)
	{
		printf("%s\t%s\t\t%.2lf\t\t%.2lf\t\t%s\n",user->data.cardUserName,user->data.cardId,user->data.balance,user->data.add_balance,STATUS(user->data.status));
	}
}
