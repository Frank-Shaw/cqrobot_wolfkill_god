#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
//number of people from 8 to 15
//meaning of number is "mode", "self", "werewolf", "witch", "seer", "hunter", "diot", "guard", "villager", "scapregoat"
//mode = 0 express killall && 1 express killgod
//self = 0 express witch could not save herself && 1 express just time 0 could save herself && 2 express could save herself
const int character[8][10] =
{
	{ 0, 2, 3, 1, 1, 0, 0, 0, 3, 0 },
	{ 1, 2, 3, 1, 1, 1, 0, 0, 3, 0 },
	{ 1, 1, 3, 1, 1, 1, 0, 0, 4, 0 },
	{ 1, 0, 3, 1, 1, 0, 1, 0, 4, 1 },
	{ 1, 1, 4, 1, 1, 1, 1, 0, 4, 0 },
	{ 1, 0, 4, 1, 1, 1, 1, 0, 4, 1 },
	{ 0, 1, 5, 1, 1, 1, 1, 0, 5, 0 },
	{ 1, 1, 5, 1, 1, 1, 1, 1, 5, 0 },
};

//use number to express character
//1 werewolf
//2 witch
//3 seer
//4 hunter
//5 diot
//6 guard
//7 villager
//8 scapregoat
int player[15] = {0};

char *inf[8] = { "狼人", "女巫", "预言家", "猎人", "白痴", "守卫", "村民", "替罪羊" };

int mode;
int self;
int day = 1;


void welcome_word()
{
	printf("Welcome to WolfKill Game, This game is produced by FrankShaw\n");
	
}

int init()
{
	printf("Input the amount of people (8 - 15): ");
	int num = -1;
	scanf("%d", &num);
	return num;
}
/*
int startgame(int number)
{
	FILE *fd;
	char filename[80] = "Data\\";
	char filenum[3];
	sprintf(filenum, "%d", number);
	strcat(filename, filenum);
	strcat(filename, ".txt");
	if ((fd = fopen(filename, "r")) == NULL)
	{
		printf("Open Rule File Error!\n");
		return 1;
	}
	char c;
	int count = 0; //count the position of character number
	int character[6];//werewolf, witch, seer, hunter, idiot, villager
	while ((c = fgetc(fd)) != '#')
	{		
		if (c == ':')
			;
	}


	fclose(fd);
}
text read version
*/



int initgame(int number)
{
	mode = character[number - 8][0];
	self = character[number - 8][1];

	int wolf = character[number - 8][2];
	int witch = character[number - 8][3];
	int seer = character[number - 8][4];
	int hunter = character[number - 8][5];
	int diot = character[number - 8][6];
	int guard = character[number - 8][7];
	int villager = character[number - 8][8];
	int scapegoat = character[number - 8][9];

	printf("游戏开始！\n");
	printf("游戏一共有%d个人", number);
	printf("角色分配如下：\n");
	printf("狼人一共有%d个\n", wolf);
	printf("%d名女巫\n", witch);
	printf("%d名预言家\n", seer);
	if (hunter)
		printf("%d名猎人\n", hunter);
	if (diot)
		printf("%d名白痴\n", diot);
	if (guard)
		printf("%d名守卫\n", guard);
	if (scapegoat)
		printf("%d名替罪羊（注意，替罪羊为非神非民的好人）\n", scapegoat);
	printf("%d名普通村民\n", villager);
	if (mode)
		printf("本局游戏模式为屠边局，只要杀掉所有神职或者所有平民，狼人即为胜利。\n");
	else
		printf("本局游戏模式为屠城局，狼人必须杀死所有好人方阵营方可胜利。\n");
	if (self == 0)
		printf("本局游戏，女巫不可自救。\n");
	else if (self == 1)
		printf("本局游戏，女巫只有第一天晚上可以自救。\n");
	else if (self == 2)
		printf("本局游戏，女巫可以自救。\n");
	printf("正在分配角色，请等待...\n");
	printf("角色分配完毕，游戏开始。\n");
	printf("祝玩的愉快！\n");
	return 0;
}

void deal_character(int num)
{
	//这里使用反向赋值，也就首先根据角色数量，生成一个带有角色数量和信息的序列，再将人的序列按顺序放置在序列当中
	int pre_character[15] = { 0 };
	int i;
	int pos = 0;
	//1 werewolf
	for (i = 0; i < character[num - 8][2]; i++)
		pre_character[pos++] = 1;
	//2 witch
	for (i = 0; i < character[num - 8][3]; i++)
		pre_character[pos++] = 2;
	//3 seer
	for (i = 0; i < character[num - 8][4]; i++)
		pre_character[pos++] = 3;
	//4 hunter
	for (i = 0; i < character[num - 8][5]; i++)
		pre_character[pos++] = 4;
	//5 diot
	for (i = 0; i < character[num - 8][6]; i++)
		pre_character[pos++] = 5;
	//6 guard
	for (i = 0; i < character[num - 8][7]; i++)
		pre_character[pos++] = 6;
	//7 villager
	for (i = 0; i < character[num - 8][8]; i++)
		pre_character[pos++] = 7;
	//8 scapregoat
	for (i = 0; i < character[num - 8][9]; i++)
		pre_character[pos++] = 8;

	pos = 0;
	for (i = 0; i < num; i++)
	{
		int count = rand() % num + 1;
		while (count)
		{
			pos = (pos + 1) % num;
			if (!player[pos])
				count--;
		}
		player[pos] = pre_character[i];
	}


}


void print_player(int num)
{
	int i;
	for (i = 0; i < num; i++)
	{
		printf("%d号玩家，身份为%s\n", i + 1, inf[player[i] - 1]);
	}

}

int wolftimetoselect()
{
	int num;
	printf("狼人，请选择一位玩家，成为今晚被杀对象\n");
	printf("你的选择是：");
	scanf("%d", &num);
	return num;
}

int witchtoselect(int die)
{
	char ans;
	int poison;


	if (self == 0 && player[die-1] == 2)
	{
		printf("女巫，今晚你被狼人杀死了，由于不能自救，你只能选择是否使用毒药。\n");
		printf("如果使用，请直接回复需要毒杀的玩家的序号，如果不用，请回复0。\n");
		printf("你的选择是：");
		scanf("%d", &poison);
		return poison;
	}

	if (self == 1 && day > 1 && player[die-1] == 2)
	{
		printf("女巫，今晚你被狼人杀死了，由于已经不是第一天晚上，你不能自救，你只能选择是否使用毒药。\n");
		printf("如果使用，请直接回复需要毒杀的玩家的序号，如果不用，请回复0。\n");
		printf("你的选择是：");
		scanf("%d", &poison);
		return poison;
	}

	printf("女巫，今晚死的人是%d号，你选择救还是不救？（y or n）\n", die);
	printf("你的选择是：");
	getchar();
	scanf("%c", &ans);
	if (ans == 'y')
		return -1;
	else if (ans == 'n')
	{
		printf("由于你的残忍，今天晚上选择没有救这个人。\n");
		printf("你还有一瓶毒药，是否使用？\n");
		printf("如果使用，请直接回复需要毒杀的玩家的序号，如果不用，请回复0。\n");
		printf("你的选择是：");
		scanf("%d", &poison);
		return poison;
	}
	else
	{
		return 0;
	}
}

void dealwithdeadpeople(int ans, int dead)
{
	player[dead] = 0;
	if (ans != 0)
		player[ans] = 0;
}


int main()
{

	srand(time(NULL));

	int numberOfPeople;

	welcome_word();

	numberOfPeople = init();

	if (initgame(numberOfPeople))
		return -1;

	deal_character(numberOfPeople);

	print_player(numberOfPeople);

	int dying = wolftimetoselect();

	int ansofwitch = witchtoselect(dying); //if ansofwitch -1 express no people die, 0 express number of dying people is dead, if it lager than 0, express number of ansofwitch and dying both are dead.

	if (ansofwitch == -1)
		printf("昨天晚上是平安夜！\n");
	else if (ansofwitch == 0)
		printf("昨天晚上死者是%d号！\n", dying);
	else
		printf("昨天晚上有两位玩家死了，分别是%d号和%d号！\n", (dying < ansofwitch ? dying : ansofwitch), (ansofwitch > dying ? ansofwitch : dying));	//this method is to print the number orderly

	if (ansofwitch != -1)
		dealwithdeadpeople(ansofwitch, dying);
	

	day++;	//the second day


	system("pause\n");

	

	
}