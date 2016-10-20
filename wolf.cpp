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

char *inf[8] = { "����", "Ů��", "Ԥ�Լ�", "����", "�׳�", "����", "����", "������" };

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

	printf("��Ϸ��ʼ��\n");
	printf("��Ϸһ����%d����", number);
	printf("��ɫ�������£�\n");
	printf("����һ����%d��\n", wolf);
	printf("%d��Ů��\n", witch);
	printf("%d��Ԥ�Լ�\n", seer);
	if (hunter)
		printf("%d������\n", hunter);
	if (diot)
		printf("%d���׳�\n", diot);
	if (guard)
		printf("%d������\n", guard);
	if (scapegoat)
		printf("%d��������ע�⣬������Ϊ�������ĺ��ˣ�\n", scapegoat);
	printf("%d����ͨ����\n", villager);
	if (mode)
		printf("������ϷģʽΪ���߾֣�ֻҪɱ��������ְ��������ƽ�����˼�Ϊʤ����\n");
	else
		printf("������ϷģʽΪ���Ǿ֣����˱���ɱ�����к��˷���Ӫ����ʤ����\n");
	if (self == 0)
		printf("������Ϸ��Ů�ײ����Ծȡ�\n");
	else if (self == 1)
		printf("������Ϸ��Ů��ֻ�е�һ�����Ͽ����Ծȡ�\n");
	else if (self == 2)
		printf("������Ϸ��Ů�׿����Ծȡ�\n");
	printf("���ڷ����ɫ����ȴ�...\n");
	printf("��ɫ������ϣ���Ϸ��ʼ��\n");
	printf("ף�����죡\n");
	return 0;
}

void deal_character(int num)
{
	//����ʹ�÷���ֵ��Ҳ�����ȸ��ݽ�ɫ����������һ�����н�ɫ��������Ϣ�����У��ٽ��˵����а�˳����������е���
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
		printf("%d����ң����Ϊ%s\n", i + 1, inf[player[i] - 1]);
	}

}

int wolftimetoselect()
{
	int num;
	printf("���ˣ���ѡ��һλ��ң���Ϊ����ɱ����\n");
	printf("���ѡ���ǣ�");
	scanf("%d", &num);
	return num;
}

int witchtoselect(int die)
{
	char ans;
	int poison;


	if (self == 0 && player[die-1] == 2)
	{
		printf("Ů�ף������㱻����ɱ���ˣ����ڲ����Ծȣ���ֻ��ѡ���Ƿ�ʹ�ö�ҩ��\n");
		printf("���ʹ�ã���ֱ�ӻظ���Ҫ��ɱ����ҵ���ţ�������ã���ظ�0��\n");
		printf("���ѡ���ǣ�");
		scanf("%d", &poison);
		return poison;
	}

	if (self == 1 && day > 1 && player[die-1] == 2)
	{
		printf("Ů�ף������㱻����ɱ���ˣ������Ѿ����ǵ�һ�����ϣ��㲻���Ծȣ���ֻ��ѡ���Ƿ�ʹ�ö�ҩ��\n");
		printf("���ʹ�ã���ֱ�ӻظ���Ҫ��ɱ����ҵ���ţ�������ã���ظ�0��\n");
		printf("���ѡ���ǣ�");
		scanf("%d", &poison);
		return poison;
	}

	printf("Ů�ף�������������%d�ţ���ѡ��Ȼ��ǲ��ȣ���y or n��\n", die);
	printf("���ѡ���ǣ�");
	getchar();
	scanf("%c", &ans);
	if (ans == 'y')
		return -1;
	else if (ans == 'n')
	{
		printf("������Ĳ��̣���������ѡ��û�о�����ˡ�\n");
		printf("�㻹��һƿ��ҩ���Ƿ�ʹ�ã�\n");
		printf("���ʹ�ã���ֱ�ӻظ���Ҫ��ɱ����ҵ���ţ�������ã���ظ�0��\n");
		printf("���ѡ���ǣ�");
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
		printf("����������ƽ��ҹ��\n");
	else if (ansofwitch == 0)
		printf("��������������%d�ţ�\n", dying);
	else
		printf("������������λ������ˣ��ֱ���%d�ź�%d�ţ�\n", (dying < ansofwitch ? dying : ansofwitch), (ansofwitch > dying ? ansofwitch : dying));	//this method is to print the number orderly

	if (ansofwitch != -1)
		dealwithdeadpeople(ansofwitch, dying);
	

	day++;	//the second day


	system("pause\n");

	

	
}