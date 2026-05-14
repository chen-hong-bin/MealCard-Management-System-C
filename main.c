#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

void FixGbk(void) {
	SetConsoleOutputCP(936);
	SetConsoleCP(936);
}

#define cardId_side 8//7位数字，一位'\0'
#define cardId_Min (pow(10,cardId_side-2)+1) 
#define cardId_Max (pow(10,cardId_side-1)-1)

#define Max_cardUserName_side 11
#define Max_AdminPassward_side 7
#define Min_AdminPassward_side 3

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
}Node, * List;

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

void WelcomeMenu(void);// 展示欢迎页菜单。
void UserMenu(int process);// 根据普通用户的登录状态呈现出对应菜单。
void AdminMenu(int process);// 根据管理员的登录状态呈现出对应菜单。
List FindId(char* findcardId, List cardList);//根据用户Id在主饭卡链表中查找饭卡信息是否存在：NULL，不存在；返回地址值，存在。
List FindName(char* findcardName, List cardList);// 根据用户名在主饭卡链表中查找饭卡信息是否存在：NULL，不存在；返回地址值，存在。
int DestoryName(char* cardName, List cardList);// 根据用户名在主饭卡链表中删除某饭卡信息。0，删除成功；1，因不存在而删除失败。
struct DestoryCard* FindDestoryCardId(char* findcardid, struct DestoryCard* DestoryCardList);// 根据Id在注销链表中查找饭卡信息。NULL，不存在；返回地址值，存在。
void ChargBalanceCard(struct BalanceCard* BalanceCardList, List cardList);// 处理充值链表，把充值链表对应用户的待到账金额加入到余额中。
void Charge(struct BalanceCard* BalanceCardList, unsigned int* length, Node* user);// 在充值链表中加入用户Id, 并且修改用户的待到账金额。
void HandleDestory(struct DestoryCard* DesHead, List cardList, unsigned int* length);// 处理注销链表，把注销链表对应用户在主饭卡链表中删去。
void Pay(Node* user);// 消费支付。
void View(List cardList);// 遍历主饭卡链表
int Add(List cardList, unsigned int* ListCardLength, Node* user);// 添加新的饭卡信息，并接入主饭卡链表。
void AppDestory(Node* user, struct DestoryCard* DestoryHEAD, unsigned int* DestoryCardLength, char* opt);// 申请注销函数。
void Alter(char* AdminPassward, unsigned int* adminLength, char* adminPassward);// 更改管理员登陆密码。
void Freeze(Node* user, List cardList);// 冻结用户饭卡。
void Thaw(Node* user, List cardList);// 解冻用户饭卡。

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
	strcpy(HEAD->data.cardId, "0000000");

	const List cardList = HEAD;
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
		for (unsigned int i = 0; i < ListCardLength; i++)
		{
			// 先读基础节点
			List node = (List)malloc(sizeof(Node));
			fread(node, sizeof(Node), 1, pf);

			// 再根据 namelength 重新分配柔性数组空间
			List newNode = (List)realloc(node, sizeof(Node) + node->data.namelength + 1);
			if (newNode!= NULL) node = newNode;

			// 读取用户名
			fread(node->data.cardUserName, sizeof(char) * (node->data.namelength + 1), 1, pf);

			// 正确插入双向链表
			node->next = NULL;
			node->last = tail;
			tail->next = node;
			tail = node;
		}
		for (unsigned int i = 0; i < BalanceCardLength; i++)
		{
			struct BalanceCard* node = (struct BalanceCard*)malloc(sizeof(struct BalanceCard));
			fread(node, sizeof(*node), 1, pf);
			node->next = NULL;

			tailBalanceCard->next = node;
			tailBalanceCard = node;
		}

		// 读取注销链表
		for (unsigned int i = 0; i < DestoryCardLength; i++)
		{
			struct DestoryCard* node = (struct DestoryCard*)malloc(sizeof(struct DestoryCard));
			fread(node, sizeof(*node), 1, pf);
			node->next = NULL;

			tailDestoryCard->next = node;
			tailDestoryCard = node;
		}

		fclose(pf);
	}

	Node* user = NULL;
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
		case '1': {
			UserMenu(0);
			char cardid[cardId_side] = { 0 };
			scanf("%s", cardid);

			if (cardid[0] == '0') { opt[0] = '0'; continue; }

			user = FindId(cardid, cardList);
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
				printf("姓名：%s\n", user->data.cardUserName);
				UserMenu(1);
				scanf("%1s", opt);
				switch (opt[0])
				{
				case '1': {
					printf("您的余额：%.2lf\n", user->data.balance);
					break;
				}
				case '2': {
					Charge(BalanceCardList, &BalanceCardLength, user);
					break;
				}
				case '3': {
					Pay(user);
					break;
				}
				case '4': {

					AppDestory(user, DestoryHEAD, &DestoryCardLength, opt);

					break;
				}
				case '0': {
					printf("退出用户登录\n");
					break;
				}
				default: {
					printf("选择不合法！\n");
					char a;
					while (a = getchar() != '\n');
					break;
				}
				}
				if (opt[0] == '0')
					break;
			}
			opt[0] = '1';
			break;
		}

		case '2': {
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
				case '1': {
					ChargBalanceCard(BalanceHEAD, cardList);
					BalanceCardLength = 0;
					break;
				}
				case '2': {
					HandleDestory(DestoryHEAD, cardList, &ListCardLength);
					DestoryCardLength = 0;
					break;
				}
				case '3': {
					View(cardList);
					break;
				}
				case '4': {
					Add(cardList, &ListCardLength, user);
					break;
				}
				case '5': {
					printf("-------------------------------\n");
					printf("\t请选择您要进行的操作\n");
					printf("-------------------------------\n");
					printf("\t1冻结/2解冻\n");
					printf("\t如需退出请按 0\n");
					printf("-------------------------------\n");
					scanf("%1s", &opt[0]);
					switch (opt[0])
					{
					case '0': {
						break;
					}
					case '1': {
						Freeze(user, cardList);
						break;
					}
					case '2': {
						Thaw(user, cardList);
						break;
					}
					default: {
						printf("选择不合法，请重新输入\n");
						char a;
						while (a = getchar() != '\n');
						break;
					}
					}
					break;
				}
				case '6': {
					Alter(AdminPassward, &adminLength, adminPassward);
					break;
				}
				case '0': {
					printf("退出管理员登录\n");
					break;
				}
				default: {
					printf("选择不合法！\n");
					char a;
					while (a = getchar() != '\n');
					break;
				}
				}
				if (opt[0] == '0')
				{
					break;
				}
			}
			opt[0] = '1';
		}

		case '0': {
			printf("感谢使用本软件~\n");
			break;
		}
		default: {
			printf("选择不合法，请重新输入\n");
			char a;
			while (a = getchar() != '\n');
			break;
		}
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
	fwrite(AdminPassward, sizeof(char) * (adminLength + 1), 1, pf);
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
		fwrite(HEAD->data.cardUserName, sizeof(char) * (HEAD->data.namelength + 1), 1, pf);
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
	case 0: {
		printf("-------------------------------\n");
		printf("\t请输入您的饭卡号\n");
		printf("-------------------------------\n");
		printf("\t如需退出请按 0\n");
		printf("-------------------------------\n");
		break;
	}
	case 1: {
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
}

void AdminMenu(int process)
{
	switch (process)
	{
	case 0: {
		printf("-------------------------------\n");
		printf("\t请输入您的管理员密钥\n");
		printf("\t默认为admin\n");
		printf("-------------------------------\n");
		printf("\t如需退出请按 0\n");
		printf("-------------------------------\n");
		break;
	}
	case 1: {
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
}

List FindId(char* findcardId, List cardList)
{
	if (cardList == NULL || cardList->next == NULL || findcardId == NULL)
		return NULL;

	List current = cardList;

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
	if (cardList == NULL || cardList->next == NULL || findcardname == NULL)
		return NULL;

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

int DestoryName(char* cardName, List cardList)
{
	List item0 = FindName(cardName, cardList);
	if (item0 == NULL)
		return 1;
	List Last = item0->last;
	List Next = item0->next;

	Last->next = Next;
	if (Next != NULL)
		Next->last = Last;

	free(item0);
	return 0;
}

struct DestoryCard* FindDestoryCardId(char* findcardid, struct DestoryCard* DestoryCardList)
{
	if (DestoryCardList == NULL || DestoryCardList->next == NULL ||findcardid == NULL)
		return NULL;

	struct DestoryCard* current = DestoryCardList->next;

	while (current)
	{
		if (strcmp(findcardid, current->cardUserId) == 0)
		{
			return current;
		}
		current = current->next;
	}
	return NULL;
}

void ChargBalanceCard(struct BalanceCard* BalanceCardList, List cardList)
{
	{
		struct BalanceCard* p = BalanceCardList->next;
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
		BalanceCardList->next = NULL;
		printf("所有充值处理完毕！\n");
	}
}

void HandleDestory(struct DestoryCard* DesHead, List cardList, unsigned int* length)
{
	struct DestoryCard* p = DesHead->next;
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
			DestoryName(user->data.cardUserName, cardList);
			(*length)--;
			printf("用户【%s】注销成功！\n", user->data.cardUserName);
		}
		free(p);
		p = temp;
	}
	DesHead->next = NULL;
	printf("所有注销处理完毕！\n");
}

void View(List cardList)
{
	List user = cardList;
	if (user == NULL || user->next == NULL)
	{
		printf("暂无饭卡信息\n");
		return;
	}
	printf("姓名\t饭卡号\t\t实际金额\t未到帐金额\t状态\n");
	user = user->next;
	while (user != NULL)
	{
		printf("%s\t%s\t\t%.2lf\t\t%.2lf\t\t%s\n", user->data.cardUserName, user->data.cardId, user->data.balance, user->data.add_balance, STATUS(user->data.status));
		user = user->next;
	}
}
void Charge(struct BalanceCard* BalanceCardList, unsigned int* length, Node* user)
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
	item->next = BalanceCardList->next;
	BalanceCardList->next = item;
	(*length)++;
	printf("充值申请提交成功！等待管理员处理\n");

}
void Pay(Node* user)
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
}

void AppDestory(Node* user, struct DestoryCard* DestoryHEAD, unsigned int* DestoryCardLength, char* opt)
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
		else if (FindDestoryCardId(user->data.cardUserName, DestoryHEAD) == (struct DestoryCard*)user)
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
}
void Alter(char* AdminPassward, unsigned int* adminLength, char* adminPassward)
{
	printf("请输入新密码\n");
	while (1)
	{
		scanf("%s", adminPassward);
		unsigned int length = strlen(adminPassward);
		if (length <= Max_AdminPassward_side && length >= Min_AdminPassward_side)
		{
			strcpy(AdminPassward, adminPassward);
			*adminLength = length;
			break;
		}
		else
		{
			printf("长度不符合，请重新输入\n");
		}
	}
}
void Freeze(Node* user, List cardList)
{
	printf("请输入需要冻结的饭卡号\n");
	char cardName[Max_cardUserName_side] = { 0 };
	scanf("%s", cardName);
	user = FindId(cardName, cardList);
	if (user == NULL)
	{
		printf("该用户不存在\n");
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
	return;
}
void Thaw(Node* user, List cardList)
{
	printf("请输入需要解冻的饭卡号\n");
	char cardName[Max_cardUserName_side] = { 0 };
	scanf("%s", cardName);
	user = FindId(cardName, cardList);
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
	return;
}
int Add(List cardList, unsigned int* ListCardLength, Node* user)
{
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
	user = (Node*)malloc(sizeof(Node) + strlen(name));
	if (user == NULL)
	{
		printf("空间不足,创建失败\n");
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

	(*ListCardLength)++;

	user->next = cardList->next;
	user->last = cardList;
	if (cardList->next != NULL)
		cardList->next->last = user;
	cardList->next = user;

	printf("创建成功。您的饭卡ID为>%s\n", user->data.cardId);
	return 0;
}