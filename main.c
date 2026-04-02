#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define cardId_side 8//7位数字，一位'\0'
#define cardId_Min (pow(10,cardId_side-2)+1) 
#define cardId_Max (pow(10,cardId_side-1)-1)

#define Max_cardUserName_side 7
#define Max_AdminPassward_side 7
#define Min_AdminPassward_side 3

#define _STR(N) #N // 转字符串
#define STR(N) _STR(N) //宏展开遇到#或者##会停止展开
#define STATUS(N) N?"冻结":"正常" 

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
int Add(List* tail, List cardList);//添加饭卡信息 返回值：0，正常添加；1，因账号已存在而退出。
int DestoryId(char* cardName, List cardList);// 删除某饭卡信息。0，删除成功；1，因不存在而删除失败。
int DestoryName(char* cardName,List cardList);
double Balance(char* cardName, List cardList);
struct DestoryCard* FindDestoryCardName(char* findcardname, struct DestoryCard* DestoryCardList);
void ChargBalanceCard(struct BalanceCard** BalanceCardList, List cardList);
void HandleDestory(struct DestoryCard** DesHead, List cardList);
void View(List cardList);
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
	char opt[2] = { 0 };
	/*do
	{
		WelcomeMenu();
		scanf("%1s", &opt[0]);
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
						struct BalanceCard* item = (struct BalanceCard*)malloc(sizeof(struct BalanceCard) + strlen(user->data.cardUserName));
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
							struct DestoryCard* item = (struct DestoryCard*)malloc(sizeof(struct DestoryCard) + strlen(user->data.cardUserName));
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
						Charge();
						break;
					case '2':
						break;
					case '3':
						break;
					case '4':
						Add(tail, cardList);
						break;
					case '5':
						break;
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
		if (opt[0] == '0')
		{
			printf("感谢使用本软件~\n");
			break;
		}
	} while (opt[0] != '0');
	// Print(pf);
	return 0;
}
	*/
while(1)
{
	WelcomeMenu();
	scanf("%1s", opt);
	switch (opt[0])
	{
	case '1':
	{
		UserMenu(0);
		char cardName[Max_cardUserName_side] = { 0 };
		scanf("%s", cardName);

		// 退出判断
		if (cardName[0] == '0') { opt[0] = '0'; break; }

		List user = FindName(cardName, cardList);
		if (!user)
		{
			printf("该用户不存在，请联系管理员\n");
			break;
		}
		// 修复：==判断，不是=赋值
		if (user->data.status == '1')
		{
			printf("该卡已冻结，请联系管理员\n");
			break;
		}

		// 用户功能菜单
		UserMenu(1);
		while(1){
			scanf("%1s", opt);
			switch (opt[0])
			{
			case '1':
				printf("您的余额：%.2lf\n", Balance(cardName, cardList));
				break;
			case '2':
			{
				printf("请输入充值金额（精确到小数点后两位）：\n");
				double add_balance = 0;
				// 修复：scanf格式+取地址
				while (scanf("%lf", &add_balance) != 1 || add_balance <= 0)
				{
					printf("输入非法！请输入正数：\n");
					// 清空缓冲区
					while (getchar() != '\n');
				}
				user->data.add_balance = add_balance;
				// 修复：柔性数组分配+1存储结束符
				struct BalanceCard* item = (struct BalanceCard*)malloc(
					sizeof(struct BalanceCard) + strlen(user->data.cardUserName) + 1);
				strcpy(item->cardUserName, user->data.cardUserName);
				item->next = BalanceHEAD->next;
				BalanceHEAD->next = item;
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
						struct DestoryCard* item = (struct DestoryCard*)malloc(
							sizeof(struct DestoryCard) + strlen(user->data.cardUserName) + 1);
						strcpy(item->cardUserName, user->data.cardUserName);
						item->next = DestoryHEAD->next;
						DestoryHEAD->next = item;
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
				break;
			}
		}
		break;
	}

	case '2':
	{
		AdminMenu(0);
		char adminPassward[Max_AdminPassward_side] = { 0 };
		scanf("%s", adminPassward);

		if (adminPassward[0] == '0') { opt[0] = '0'; break; }
		if (strcmp(adminPassward, AdminPassward) != 0)
		{
			printf("密码错误！\n");
			break;
		}

		// 管理员功能菜单
		do {
			AdminMenu(1);
			scanf("%1s", opt);
			switch (opt[0])
			{
			case '1':
				ChargBalanceCard(&BalanceHEAD, cardList);
				break;
			case '2':
				HandleDestory(&DestoryHEAD, cardList);
				break;
			case '3':
				printf("功能开发中...\n");
				break;
			case '4':
				Add(&tail, cardList); // 传址修复尾指针
				break;
			case '5':
				printf("功能开发中...\n");
				break;
			case '0':
				printf("退出管理员登录\n");
				break;
			default:
				printf("选择不合法！\n");
				break;
			}
			break;
		} while (opt[0] != '0');
	}

	case '0':
		printf("感谢使用本软件~\n");
		break;

	default:
		printf("选择不合法，请重新输入\n");
		break;
	}
	printf("\n");

}

free(HEAD);
free(BalanceHEAD);
free(DestoryHEAD);
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

int Add(List* tail,List cardList)
{
	char name[2 * Max_cardUserName_side];
	printf("请输入姓名（最长为7）。一经建立无法修改。\n");
	while (1)
	{
		scanf("%49s", name);
		char ch;
		while (ch = getchar() != '\n');//清空缓冲区
		if (strlen(name) < Max_cardUserName_side + 1)
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
		char strId[cardId_side];
	do {
		int intId = rand() % (int)(cardId_Max - cardId_Min) + cardId_Min;
		sprintf(strId, "%d", intId);//随机数转字符串
	} while (FindId(strId,cardList));
	strcpy(user->data.cardId, strId);
	
	user->last = *tail;
	(*tail)->next = user;
	(*tail) = user;
	user->next = NULL;
	printf("创建成功。您的饭卡ID为>%s\n",user->data.cardId);
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

struct DestoryCard* FindDestoryCardName(char* findcardname, struct DestoryCard* DestoryCardList)
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
			List user = FindName(p->cardUserName, cardList);
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

void HandleDestory(struct DestoryCard** DesHead, List cardList)
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
		List user = FindName(p->cardUserName, cardList);
		if (user)
		{
			DestoryName(user->data.cardUserName,cardList);
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
	List user = cardList->next;
	if (user == NULL)
	{
		printf("暂无饭卡信息\n");
		return;
	}
	printf("姓名\t饭卡号\t实际金额\t未到帐金额\t状态\n");
	while (1)
	{
		printf("%s\t%s\t%.2lf\t%.2lf\t%s\n",user->data.cardUserName,user->data.cardId,user->data.balance,user->data.add_balance,STATUS(user->data.status));
		if (user->next == NULL)
			break;
		user = user->next;
	}
}

//void Print(FILE* pf)
//{
//
//	fclose(pf);
//	pf = NULL;
//}