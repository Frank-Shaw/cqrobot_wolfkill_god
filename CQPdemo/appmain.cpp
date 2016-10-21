
/*
* CoolQ Demo for VC++ 
* Api Version 9
* Written by Coxxs & Thanks for the help of orzFly
*/

#include "stdafx.h"
#include "string"
#include "cqp.h"
#include "appmain.h" //Ӧ��AppID����Ϣ������ȷ��д�������Q�����޷�����
#include "time.h"
using namespace std;


int ac = -1; //AuthCode ���ÿ�Q�ķ���ʱ��Ҫ�õ�
bool enabled = false;




/*
* ������Ϸ����ر�������
*/

//start ��ʾ���˻��ڵ�״̬����0��ʾû����Ϸ��1��ʾ�տ�ʼ���ȴ������˱����μӣ�2��ʾ������Ա��λ����ʼ��Ϸ��3��ʾ����ɱ�ˣ�4��ʾŮ�׾��ˣ�5��ʾԤ�Լ����ˣ�6��ʾ�������ˡ�
int start = 0;
int playernum = 0; //�����μ����˵��������
int dying = 0; //��ʾ������ѡ�еļ������������
int ansofwitch = 0;	//��ʾŮ��ѡ��
int ansofseer = 0;	//��ʾԤ�Լ�ѡ��
int64_t playerqq[15];	//�����μ����˵����QQ
char playername[15][50];	//�����μ�������ҵ�����
int64_t start_playerqq;		//����������Ϸ��QQ


int64_t uniqueQQgroup;
int isGroup = 0;	//��ʾ��Ҫ�������鷢����Ϣ����Ⱥ���ͣ�0��ʾ�������鷢�ͣ�1��ʾ��Ⱥ����


//�ǳ���ؽṹ��
struct CQ_Type_GroupMember
{
	int64_t				GroupID;			// Ⱥ��
	int64_t				QQID;				// QQ��
	std::string			nick;				// QQ�ǳ�
	std::string			card;				// Ⱥ��Ƭ
	int					sex;				// �Ա� 0/�� 1/Ů
	int					age;				// ����
	std::string			area;				// ����
	int					jointime;			// ��Ⱥʱ��
	int					lastsent;			// �ϴη���ʱ��
	std::string			level_name;			// ͷ������
	int					permission;			// Ȩ�޵ȼ� 1/��Ա 2/����Ա 3/Ⱥ��
	bool				unfriendly;			// ������Ա��¼
	std::string			title;				// �Զ���ͷ��
	int					titleExpiretime;	// ͷ�ι���ʱ��
	bool				cardcanchange;		// ����Ա�Ƿ���Э������
};

struct CQ_TYPE_QQ
{
	int64_t				QQID;				//QQ��
	std::string			nick;				//�ǳ�
	int					sex;				//�Ա�
	int					age;				//����
};



//�����û��ǳƵ�

static const char reverse_table[128] =
{
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63,
	52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64,
	64, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
	15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64,
	64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
	41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64
};

unsigned char *base64_decode(unsigned char *bindata, size_t inlen, unsigned char **out, size_t *outlen)
{
	size_t _outlen = *outlen;
	unsigned char *_out = NULL;
	int bits_collected = 0;
	unsigned int accumulator = 0;
	size_t out_pos = 0;

	if (NULL == *out)
	{
		_outlen = inlen;
		_out = (unsigned char *)malloc(_outlen);
	}
	else
	{
		_outlen = *outlen;
		_out = *out;
	}

	int c = 0;
	for (int i = 0; i < inlen; i++)
	{
		c = bindata[i];
		if (isspace(c) || c == '=')
		{
			// Skip whitespace and padding. Be liberal in what you accept.
			continue;
		}
		if ((c > 127) || (c < 0) || (reverse_table[c] > 63))
		{
			return NULL;
		}
		accumulator = (accumulator << 6) | reverse_table[c];
		bits_collected += 6;
		if (bits_collected >= 8)
		{
			bits_collected -= 8;
			_out[out_pos++] = (char)((accumulator >> bits_collected) & 0xffu);
		}
	}

	*outlen = _outlen;
	*out = _out;
	return _out;
}


//analysis_nickname
void analysis_nickname(unsigned char* in, char** out, bool group)	//group = 1��ʾ��QQȺ��������Ϣ��0��ʾ��������
{
	size_t _len = 0;
	unsigned char *outtemp = 0;
	base64_decode(in, strlen((const char*)in), &outtemp, &_len);

	if (group)
	{
		//���ݻ����˵ı�����򣬽��ж�ȡ�����ַ�
		unsigned char *infomation = outtemp;
		infomation += 16;	//�õ��ǳƵĳ���λ��
		unsigned short nickname_len = (infomation[0] << 8) | infomation[1];	//λ���㣬��16����ת��Ϊ10���ƣ��õ��ǳƳ���
		infomation += 2;	//�õ��ǳ�λ��
		unsigned char *nickname;
		nickname = (unsigned char *)malloc(sizeof(unsigned char)* nickname_len);

		for (unsigned int i = 0; i < nickname_len; i++)
		{
			nickname[i] = infomation[i];

		}
		nickname[nickname_len] = '\0';	//�õ��ǳ�����



		infomation = infomation + nickname_len;	//�õ�Ⱥ��Ƭ�ĳ���λ��

		unsigned short groupname_len = (infomation[0] << 8) | infomation[1];	//λ���㣬��16����ת��Ϊ10���ƣ��õ�Ⱥ��Ƭ����

		if (groupname_len != 0)
		{
			infomation += 2;	//�õ�Ⱥ��Ƭλ��
			unsigned char *groupname;
			groupname = (unsigned char *)malloc(sizeof(unsigned char)* nickname_len);
			for (unsigned int i = 0; i < groupname_len; i++)
			{
				groupname[i] = infomation[i];
			}

			groupname[groupname_len] = '\0';	//�õ�Ⱥ��Ƭ����


			*out = (char*)groupname;

		}
		else//��ʾû������Ⱥ��Ƭ
		{
			*out = (char*)nickname;
		}
	}

	else  //��������
	{
		//���ݻ����˵ı�����򣬽��ж�ȡ�����ַ�
		unsigned char *infomation = outtemp;
		infomation += 8;	//�õ��ǳƵĳ���λ��
		unsigned short nickname_len = (infomation[0] << 8) | infomation[1];	//λ���㣬��16����ת��Ϊ10���ƣ��õ��ǳƳ���
		infomation += 2;	//�õ��ǳ�λ��
		unsigned char *nickname;
		nickname = (unsigned char *)malloc(sizeof(unsigned char)* nickname_len);

		for (unsigned int i = 0; i < nickname_len; i++)
		{
			nickname[i] = infomation[i];

		}
		nickname[nickname_len] = '\0';	//�õ��ǳ�����


		*out = (char*)nickname;

	}
}



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
//0 deadpeople
//1 werewolf
//2 witch
//3 seer
//4 hunter
//5 diot
//6 guard
//7 villager
//8 scapregoat
int player[15] = { 0 };

char *inf[8] = { "����", "Ů��", "Ԥ�Լ�", "����", "�׳�", "����", "����", "������" };

int mode;
int self;
int day = 1;



void init()
{
	playernum = 0;
	dying = 0;
	day = 1;
	ansofwitch = 0;
	ansofseer = 0;
	start_playerqq = 0;
	for (int i = 0; i < 15; i++)
	{
		player[i] = 0;
		playerqq[i] = 0;
	}
	

}

void sendmessage(int ac, int64_t from, char* msg)
{
	if (isGroup)
		CQ_sendGroupMsg(ac, from, msg);
	else
		CQ_sendDiscussMsg(ac, from, msg);
}



void welcome_word(int64_t fromGroup)
{
		sendmessage(ac, fromGroup, "��ӭ����������Ϸ�����������Ϸ���ϵۣ�СС�������ָ�̡�����л�ҵİְ�FrankShaw���ҵ��ϵۣ�\n�����롰#�������ˡ�,���μ����������Ϸ");
	
}



int initgame(int number, int64_t fromGroup)
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

	char *msg;
	msg = (char *)malloc(sizeof(char)* 300);
	memset(msg, '\0', 300);

	char buf[30];

	strcpy(msg, "��Ϸ��ʼ��\n");

	sprintf(buf, "��Ϸһ����%d����", number);
	strcat(msg, buf);

	strcat(msg, "��ɫ�������£�\n");

	sprintf(buf, "����һ����%d��\n", wolf);
	strcat(msg, buf);

	sprintf(buf, "%d��Ů��\n", witch);
	strcat(msg, buf);

	sprintf(buf, "%d��Ԥ�Լ�\n", seer);
	strcat(msg, buf);

	if (hunter)
	{
		sprintf(buf, "%d������\n", hunter);
		strcat(msg, buf);
	}
	if (diot)
	{
		sprintf(buf, "%d���׳�\n", diot);
		strcat(msg, buf);
	}
	if (guard)
	{
		sprintf(buf, "%d������\n", guard);
		strcat(msg, buf);
	}
	if (scapegoat)
	{
		sprintf(buf, "%d��������ע�⣬������Ϊ�������ĺ��ˣ�\n", scapegoat);
		strcat(msg, buf);
	}


	sprintf(buf, "%d����ͨ����\n", villager);
	strcat(msg, buf);

	if (mode)
		strcat(msg, "������ϷģʽΪ���߾֣�ֻҪɱ��������ְ��������ƽ�����˼�Ϊʤ����\n");
	else
		strcat(msg, "������ϷģʽΪ���Ǿ֣����˱���ɱ�����к��˷���Ӫ����ʤ����\n");
	if (self == 0)
		strcat(msg, "������Ϸ��Ů�ײ����Ծȡ�\n");
	else if (self == 1)
		strcat(msg, "������Ϸ��Ů��ֻ�е�һ�����Ͽ����Ծȡ�\n");
	else if (self == 2)
		strcat(msg, "������Ϸ��Ů�׿����Ծȡ�\n");

	sendmessage(ac, fromGroup, msg);

	free(msg);
	free(buf);
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




void print_player(int64_t fromGroup, int num)
{
	
	sendmessage(ac, fromGroup, "���ڷ����ɫ����ȴ�...\n");

	for (int i = 0; i < playernum; i++)
	{
		char msg[80];
		sprintf(msg, "����%d����ң���������%s", i+1, inf[player[i] - 1]);
		CQ_sendPrivateMsg(ac, playerqq[i], msg);
		Sleep(1000);
	}
		
	sendmessage(ac, fromGroup, "��ɫ������ϣ���Ϸ��ʼ��\nף�����죡");


}

void wolftimetoselect()
{

	char boddy[120]="�����ͬ���ǣ�\n";
	for (int i = 0; i < playernum; i++)
	{
		if (player[i] == 1)
		{
			char cnum[3];
			itoa(i + 1, cnum, 10);
			strcat(boddy, cnum);
			strcat(boddy, "������\n");
		}
		
	}
	strcat(boddy, "���������ۺ�֮����������һλ���˻ظ�һ�����֣���ʾ����Ҫɱ��ҵĺ���\n");

	for (int i = 0; i < playernum; i++)
	{
		if (player[i] == 1)
		{
			char msg[50];
			sprintf(msg, "����Ϊ%d�����ˣ�", i+1);
			CQ_sendPrivateMsg(ac, playerqq[i], msg);
			Sleep(500);
			CQ_sendPrivateMsg(ac, playerqq[i], boddy);
		}
		
				
	}
	
}


void witchtoselect()
{
	int64_t fromqq;
	for (int i = 0; i < playernum; i++)
	if (player[i] == 2)	//��ʾ��������Ů��
	{
		fromqq = playerqq[i];
		break;
	}
	if (self == 0 && player[dying - 1] == 2)
		CQ_sendPrivateMsg(ac, fromqq, "Ů�ף������㱻����ɱ���ˣ����ڲ����Ծȣ���ֻ��ѡ���Ƿ�ʹ�ö�ҩ��\n���ʹ�ã���ֱ�ӻظ���Ҫ��ɱ����ҵ���ţ�������ã���ظ�0��\n");

	else if (self == 1 && day > 1 && player[dying - 1] == 2)
		CQ_sendPrivateMsg(ac, fromqq, "Ů�ף������㱻����ɱ���ˣ������Ѿ����ǵ�һ�����ϣ��㲻���Ծȣ���ֻ��ѡ���Ƿ�ʹ�ö�ҩ��\n���ʹ�ã���ֱ�ӻظ���Ҫ��ɱ����ҵ���ţ�������ã���ظ�0��\n");

	else
	{
		char msg[120] = "Ů�ף�������������";
		char cnum[3];
		itoa(dying, cnum, 10);
		strcat(msg, cnum);
		strcat(msg, "�ţ���������λ��ң���ظ�#��\n������Ȼظ�0��\n��������ˣ�����Ҫ���ˣ���ֱ�ӻظ�������\n");
		CQ_sendPrivateMsg(ac, fromqq, msg);
	}

}


void dealwithdeadpeople(int ans, int dead)
{
	if (ans == -1)
		dying = 0;
	else if (ans == 0)
		player[dying] = player[dying] * -1;	//�������Ϊ���������Ҿ���ֵ����ԭ���
	else
	{
		player[dying] = player[dying] * -1;
		player[ans] = player[ans] * -1;
	}

}


void seertoselect()
{
	int64_t fromqq;
	for (int i = 0; i < playernum; i++)
	if (player[i] == 3)	//��ʾ��������Ԥ�Լ�
	{
		fromqq = playerqq[i];
		break;
	}

	CQ_sendPrivateMsg(ac, fromqq, "����Ԥ�Լң���ظ�һ����ұ�ţ��������");
}


void nightover()
{
	sendmessage(ac, uniqueQQgroup, "������������������ݶ��Ѿ��ж���ϣ����ڿ�ʼ��ѡ���������û�о�������ظ�#�����ˣ��鿴�������");
}


void sunrise(int64_t fromGroup)
{
	if (ansofwitch == -1)
		sendmessage(ac, fromGroup, "����������ƽ��ҹ�~");
	else if (ansofwitch == 0)
	{
		char msg[60] = "��������������";
		char cnum[3];
		itoa(dying, cnum, 10);
		strcat(msg, cnum);
		strcat(msg, "�ţ��ÿ����");
		sendmessage(ac, fromGroup, msg);

	}
	else
	{
		char msg[60] = "������������λ����������ֱ���";
		char cnum[3];
		itoa(dying, cnum, 10);
		strcat(msg, cnum);
		strcat(msg, "�ź�");
		itoa(ansofwitch, cnum, 10);
		strcat(msg, cnum);
		strcat(msg, "�ţ��ÿ��µ�ҹ���");
		sendmessage(ac, fromGroup, msg);
	}
		

	day++;
}



void wolf_startgame(int64_t fromGroup, int64_t fromQQ)
{

	srand((unsigned int)time(NULL));
	init();
	welcome_word(fromGroup);
		
}



/* 
* ����Ӧ�õ�ApiVer��Appid������󽫲������
*/
CQEVENT(const char*, AppInfo, 0)() {
	return CQAPPINFO;
}


/* 
* ����Ӧ��AuthCode����Q��ȡӦ����Ϣ��������ܸ�Ӧ�ã���������������������AuthCode��
* ��Ҫ�ڱ��������������κδ��룬���ⷢ���쳣���������ִ�г�ʼ����������Startup�¼���ִ�У�Type=1001����
*/
CQEVENT(int32_t, Initialize, 4)(int32_t AuthCode) {
	ac = AuthCode;
	return 0;
}


/*
* Type=1001 ��Q����
* ���۱�Ӧ���Ƿ����ã������������ڿ�Q������ִ��һ�Σ���������ִ��Ӧ�ó�ʼ�����롣
* ��Ǳ�Ҫ����������������ش��ڡ���������Ӳ˵������û��ֶ��򿪴��ڣ�
*/
CQEVENT(int32_t, __eventStartup, 0)() {

	return 0;
}


/*
* Type=1002 ��Q�˳�
* ���۱�Ӧ���Ƿ����ã������������ڿ�Q�˳�ǰִ��һ�Σ���������ִ�в���رմ��롣
* ������������Ϻ󣬿�Q���ܿ�رգ��벻Ҫ��ͨ���̵߳ȷ�ʽִ���������롣
*/
CQEVENT(int32_t, __eventExit, 0)() {

	return 0;
}

/*
* Type=1003 Ӧ���ѱ�����
* ��Ӧ�ñ����ú󣬽��յ����¼���
* �����Q����ʱӦ���ѱ����ã�����_eventStartup(Type=1001,��Q����)�����ú󣬱�����Ҳ��������һ�Ρ�
* ��Ǳ�Ҫ����������������ش��ڡ���������Ӳ˵������û��ֶ��򿪴��ڣ�
*/
CQEVENT(int32_t, __eventEnable, 0)() {
	enabled = true;
	return 0;
}


/*
* Type=1004 Ӧ�ý���ͣ��
* ��Ӧ�ñ�ͣ��ǰ�����յ����¼���
* �����Q����ʱӦ���ѱ�ͣ�ã��򱾺���*����*�����á�
* ���۱�Ӧ���Ƿ����ã���Q�ر�ǰ��������*����*�����á�
*/
CQEVENT(int32_t, __eventDisable, 0)() {
	enabled = false;
	return 0;
}


/*
* Type=21 ˽����Ϣ
* subType �����ͣ�11/���Ժ��� 1/��������״̬ 2/����Ⱥ 3/����������
*/
CQEVENT(int32_t, __eventPrivateMsg, 24)(int32_t subType, int32_t sendTime, int64_t fromQQ, const char *msg, int32_t font) {

	//���Ҫ�ظ���Ϣ������ÿ�Q�������ͣ��������� return EVENT_BLOCK - �ضϱ�����Ϣ�����ټ�������  ע�⣺Ӧ�����ȼ�����Ϊ"���"(10000)ʱ������ʹ�ñ�����ֵ
	//������ظ���Ϣ������֮���Ӧ��/�������������� return EVENT_IGNORE - ���Ա�����Ϣ

	if (start == 3)
	{
		for (int i = 0; i < playernum; i++)
		{
			if (playerqq[i] == fromQQ && player[i] == 1)	//��ʾ������������
			{
				dying = atoi(msg);
				if (dying > 0 && dying <= playernum)
				{

					char m[80];
					strcpy(m, "��ѡ����");
					strcat(m, msg);
					strcat(m, "����ң����������");
					CQ_sendPrivateMsg(ac, fromQQ, m);
					start = 4;
					witchtoselect();	//����Ů��ѡ�񻷽�
				}

				else
					CQ_sendPrivateMsg(ac, fromQQ, "������ҵı����������������");
			}
		}
		
	}
	
	else if (start == 4)
	{
		for (int i = 0; i < playernum; i++)
		{
			if (playerqq[i] == fromQQ && player[i] == 2)	//��ʾ��������Ů��
			{
				if (strcmp(msg, "#") == 0)
					ansofwitch = -1;
				else
					ansofwitch = atoi(msg);

				//msg�Ļظ����ַ���ֱ�����ַ���������������λ��ң���ظ�#��������Ȼظ�0����������ˣ�����Ҫ���ˣ���ֱ�ӻظ�������"
				if (ansofwitch >= -1 && ansofwitch <= playernum)
				{
					if (ansofwitch == 0)
					{
						CQ_sendPrivateMsg(ac, fromQQ, "Ů�ף�������ѡ���˲����ˣ�̫������");
						
					}
					else if (ansofwitch > 0 && ansofwitch <= playernum)
					{
						char m[120];
						strcpy(m, "Ů����ѡ���˲����ˣ����Ҷ�ɱ");
						strcat(m, msg);
						strcat(m, "����ң�̫�����ˡ����������");
						CQ_sendPrivateMsg(ac, fromQQ, m);
						
					}

					else if (ansofwitch == -1)
					{
						CQ_sendPrivateMsg(ac, fromQQ, "Ů�ף�������ѡ����������������ˡ����������");
						
					}
					
					start = 5;	//����Ԥ�Լһغ�
					seertoselect();	//����Ԥ�Լ�ѡ��
				}
				else
					CQ_sendPrivateMsg(ac, fromQQ, "Ů���������ѡ����������������");
			}
		}

	}


	else if (start == 5)	//Ԥ�Լһغ�
	{
		for (int i = 0; i < playernum; i++)
		{
			if (playerqq[i] == fromQQ && player[i] == 3)	//��ʾ��������Ԥ�Լ�
			{
				ansofseer = atoi(msg);
				if (dying > 0 && dying <= playernum)
				{

					char m[80];
					strcpy(m, "��ѡ����");
					strcat(m, msg);
					strcat(m, "����ң����������...");
					CQ_sendPrivateMsg(ac, fromQQ, m);
					if (player[ansofseer - 1] > 1)
					{
						CQ_sendPrivateMsg(ac, fromQQ, "����");
						start = 6;
						dealwithdeadpeople(ansofwitch, dying);	//��������
						nightover();
					}
					else if (player[ansofseer - 1] == 1)
					{
						CQ_sendPrivateMsg(ac, fromQQ, "����");
						start = 6;
						dealwithdeadpeople(ansofwitch, dying);	//��������
						nightover();
					}
					else
						CQ_sendPrivateMsg(ac, fromQQ, "���ˣ�����������һ�����ŵ����");

					
				}

				else
					CQ_sendPrivateMsg(ac, fromQQ, "������ҵı����������������");
			}
		}
	}

	else if (start == 6)	//���˿��Իظ�#�鿴������鿴�����˵����
	{
		for (int i = 0; i < playernum; i++)
		{
			if (playerqq[i] == fromQQ && player[i] < 0 && strcmp(msg, "#�鿴���") == 0)	//�������������ˣ����������Ҫ��鿴���
			{
				char buf[1500] = "���ڸ��㿴���������ݣ�\n";
				for (int j = 0; j < playernum; j++)
				{
					char buff[60];
					sprintf(buff, "%d����ң�%s����ݣ�%s\n", j + 1, playername[j], inf[abs(player[j]) - 1]);
					strcat(buf, buff);
				}
				CQ_sendPrivateMsg(ac, fromQQ, buf);
			}
		}
	}

	
	return EVENT_BLOCK;
}


/*
* Type=2 Ⱥ��Ϣ
*/
CQEVENT(int32_t, __eventGroupMsg, 36)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, const char *fromAnonymous, const char *msg, int32_t font) {

	if (strcmp(msg, "#��ʼ����") == 0)
	{
		if (fromQQ == 452434701)
		{
			if (start == 0)
			{
				start_playerqq = fromQQ;
				isGroup = 1;
				uniqueQQgroup = fromGroup;
				wolf_startgame(fromGroup, fromQQ);
				start = 1;
			}
			else
				sendmessage(ac, fromGroup, "������Ϸ�Ѿ���ʼ��");

		}
		else
			sendmessage(ac, fromGroup, "�㲻���Ұְ֣������㿪������Ϸ");

	}


	else if (strcmp(msg, "#��������") == 0)
	{
		if (start != 0)
		{
			if (start_playerqq == fromQQ)
			{
				sendmessage(ac, fromGroup, "����������Ϸ��");
				start = 0;
			}
			else
				sendmessage(ac, fromGroup, "�㲻��������Ϸ�ķ����ߣ����ܽ���������Ϸ��");
		}
		else
			sendmessage(ac, fromGroup, "��û�п�ʼ������Ϸ��");
	}



	else if (strcmp(msg, "#��������") == 0)
	{
		if (start == 1)
		{
			int joined = 0;
			int set = 0;
			int i;
			for (i = 0; i < 15; i++)
			{
				if (playerqq[i] == 0)
					set = i;
				if (playerqq[i] == fromQQ)	//����ظ�����ĺ�����ǰ�ڿ�����ȥ��
				{
					joined = 1;
					break;
				}
				if (playerqq[i] == 0)
					break;
			}
			if (joined)
			{
				char buf[50];
				sprintf(buf, "%s�Ѿ����������ˣ������ظ�����", playername[i]);
				sendmessage(ac, fromGroup, buf);
			}
			else
			{

				unsigned char* member = (unsigned char *)CQ_getGroupMemberInfoV2(ac, fromGroup, fromQQ, 1);
				

				char *nickname;

				nickname = (char *)malloc(sizeof(char)* 50);

				analysis_nickname(member, &nickname, 1);

				strcpy(playername[set], nickname);
				playerqq[set] = fromQQ;
				playernum++;
				char buf[300];
				sprintf(buf, "%s����ɹ�\n��ǰ����Ϊ%d��:\n", nickname, playernum);
				for (int i = 0; i < 15; i++)
				if (playerqq[i] != 0)
				{
					char buff[15];
					sprintf(buff, "%d����ң�", i + 1);
					strcat(buf, buff);
					strcat(buf, playername[i]);
					strcat(buf, "\n");
				}
				sendmessage(ac, fromGroup, buf);
			}
			
			
		}
		else
			sendmessage(ac, fromGroup, "û�����ڽ��е�������Ϸ��������ָ���ʼ���ˡ���������һ��������Ϸ");
	}

	else if (strcmp(msg, "#����������Ϸ") == 0)
	{
		if (start == 1)
			if(playernum >= 8 && playernum <= 15)
			{
				initgame(playernum, fromGroup);
				start = 2;
				deal_character(playernum);
				print_player(fromGroup, playernum);
				start = 3;	//��������ɱ�˽׶�
				wolftimetoselect();
			}
			else
				sendmessage(ac, fromGroup, "�������㣬�޷�������Ϸ");
		else
			sendmessage(ac, fromGroup, "�޷���������Ϸ������ȷ״̬");
		
	}


	else if (start == 6 && strcmp(msg, "#������") == 0)
	{
		sunrise(fromGroup);
	}


	if (strcmp(msg, "#�鿴״̬") == 0)
	{
		char m[3];
		itoa(start, m, 10);
		sendmessage(ac, fromGroup, m);
	}


	if (strcmp(msg, "#�鿴���") == 0)
	{
		char buf[300] = "���ڵ����Ϊ��\n";
		for (int i = 0; i < 15; i++)
		if (playerqq[i] != 0)
		{
			char buff[15];
			sprintf(buff, "%d����ң�", i + 1);
			strcat(buf, buff);
			strcat(buf, playername[i]);
			strcat(buf, "\n");
		}
		

		sendmessage(ac, fromGroup, buf);
	}


	return EVENT_BLOCK; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}


/*
* Type=4 ��������Ϣ
*/
CQEVENT(int32_t, __eventDiscussMsg, 32)(int32_t subType, int32_t sendTime, int64_t fromDiscuss, int64_t fromQQ, const char *msg, int32_t font) {

	if (strcmp(msg, "#��ʼ����") == 0)
	{
		if (fromQQ == 452434701)
		{
			if (start == 0)
			{
				start_playerqq = fromQQ;
				isGroup = 0;
				uniqueQQgroup = fromDiscuss;
				wolf_startgame(fromDiscuss, fromQQ);
				start = 1;
			}
			else
				sendmessage(ac, fromDiscuss, "������Ϸ�Ѿ���ʼ��");

		}
		else
			sendmessage(ac, fromDiscuss, "�㲻���Ұְ֣������㿪������Ϸ");

	}


	else if (strcmp(msg, "#��������") == 0)
	{
		if (start != 0)
		{
			if (start_playerqq == fromQQ)
			{
				sendmessage(ac, fromDiscuss, "����������Ϸ��");
				start = 0;
			}
			else
				sendmessage(ac, fromDiscuss, "�㲻��������Ϸ�ķ����ߣ����ܽ���������Ϸ��");
		}
		else
			sendmessage(ac, fromDiscuss, "��û�п�ʼ������Ϸ��");
	}



	else if (strcmp(msg, "#��������") == 0)
	{
		if (start == 1)
		{
			int joined = 0;
			int set = 0;
			int i;
			for (i = 0; i < 15; i++)
			{
				if (playerqq[i] == 0)
					set = i;
				if (playerqq[i] == fromQQ)	//����ظ�����ĺ�����ǰ�ڿ�����ȥ��
				{
					joined = 1;
					break;
				}
				if (playerqq[i] == 0)
					break;
			}
			if (joined)
			{
				char buf[50];
				sprintf(buf, "%s�Ѿ����������ˣ������ظ�����", playername[i]);
				sendmessage(ac, fromDiscuss, buf);
			}
			else
			{
				
				unsigned char* member = (unsigned char *)CQ_getStrangerInfo(ac, fromQQ, 1);


				char *nickname;

				nickname = (char *)malloc(sizeof(char)* 50);

				analysis_nickname(member, &nickname, 0);

				strcpy(playername[set], nickname);
				playerqq[set] = fromQQ;
				playernum++;
				char buf[300];
				sprintf(buf, "%s����ɹ�\n��ǰ����Ϊ%d��:\n", nickname, playernum);
				for (int i = 0; i < 15; i++)
				if (playerqq[i] != 0)
				{
					char buff[15];
					sprintf(buff, "%d����ң�", i + 1);
					strcat(buf, buff);
					strcat(buf, playername[i]);
					strcat(buf, "\n");
				}
				sendmessage(ac, fromDiscuss, buf);
			}


		}
		else
			sendmessage(ac, fromDiscuss, "û�����ڽ��е�������Ϸ��������ָ���ʼ���ˡ���������һ��������Ϸ");
	}

	else if (strcmp(msg, "#����������Ϸ") == 0)
	{
		if (start == 1)
		if (playernum >= 8 && playernum <= 15)
		{
			initgame(playernum, fromDiscuss);
			start = 2;
			deal_character(playernum);
			print_player(fromDiscuss, playernum);
			start = 3;	//��������ɱ�˽׶�
			wolftimetoselect();
		}
		else
			sendmessage(ac, fromDiscuss, "�������㣬�޷�������Ϸ");
		else
			sendmessage(ac, fromDiscuss, "�޷���������Ϸ������ȷ״̬");

	}


	else if (start == 6 && strcmp(msg, "#������") == 0)
	{
		sunrise(fromDiscuss);
	}


	if (strcmp(msg, "#�鿴״̬") == 0)
	{
		char m[3];
		itoa(start, m, 10);
		sendmessage(ac, fromDiscuss, m);
	}


	if (strcmp(msg, "#�鿴���") == 0)
	{
		char buf[300] = "���ڵ����Ϊ��\n";
		for (int i = 0; i < 15; i++)
		if (playerqq[i] != 0)
		{
			char buff[15];
			sprintf(buff, "%d����ң�", i+1);
			strcat(buf, buff);
			strcat(buf, playername[i]);
			strcat(buf, "\n");
		}


		sendmessage(ac, fromDiscuss, buf);
	}
	return EVENT_BLOCK; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}


/*
* Type=101 Ⱥ�¼�-����Ա�䶯
* subType �����ͣ�1/��ȡ������Ա 2/�����ù���Ա
*/
CQEVENT(int32_t, __eventSystem_GroupAdmin, 24)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t beingOperateQQ) {

	return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}


/*
* Type=102 Ⱥ�¼�-Ⱥ��Ա����
* subType �����ͣ�1/ȺԱ�뿪 2/ȺԱ���� 3/�Լ�(����¼��)����
* fromQQ ������QQ(��subTypeΪ2��3ʱ����)
* beingOperateQQ ������QQ
*/
CQEVENT(int32_t, __eventSystem_GroupMemberDecrease, 32)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, int64_t beingOperateQQ) {

	return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}


/*
* Type=103 Ⱥ�¼�-Ⱥ��Ա����
* subType �����ͣ�1/����Ա��ͬ�� 2/����Ա����
* fromQQ ������QQ(������ԱQQ)
* beingOperateQQ ������QQ(����Ⱥ��QQ)
*/
CQEVENT(int32_t, __eventSystem_GroupMemberIncrease, 32)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, int64_t beingOperateQQ) {

	return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}


/*
* Type=201 �����¼�-���������
*/
CQEVENT(int32_t, __eventFriend_Add, 16)(int32_t subType, int32_t sendTime, int64_t fromQQ) {

	return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}


/*
* Type=301 ����-�������
* msg ����
* responseFlag ������ʶ(����������)
*/
CQEVENT(int32_t, __eventRequest_AddFriend, 24)(int32_t subType, int32_t sendTime, int64_t fromQQ, const char *msg, const char *responseFlag) {

	//CQ_setFriendAddRequest(ac, responseFlag, REQUEST_ALLOW, "");

	return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}


/*
* Type=302 ����-Ⱥ���
* subType �����ͣ�1/����������Ⱥ 2/�Լ�(����¼��)������Ⱥ
* msg ����
* responseFlag ������ʶ(����������)
*/
CQEVENT(int32_t, __eventRequest_AddGroup, 32)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, const char *msg, const char *responseFlag) {

	//if (subType == 1) {
	//	CQ_setGroupAddRequestV2(ac, responseFlag, REQUEST_GROUPADD, REQUEST_ALLOW, "");
	//} else if (subType == 2) {
	//	CQ_setGroupAddRequestV2(ac, responseFlag, REQUEST_GROUPINVITE, REQUEST_ALLOW, "");
	//}

	return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}

/*
* �˵������� .json �ļ������ò˵���Ŀ��������
* �����ʹ�ò˵������� .json ���˴�ɾ�����ò˵�
*/
CQEVENT(int32_t, __menuA, 0)() {
	MessageBoxA(NULL, "����menuA�����������봰�ڣ����߽�������������", "" ,0);
	return 0;
}

CQEVENT(int32_t, __menuB, 0)() {
	MessageBoxA(NULL, "����menuB�����������봰�ڣ����߽�������������", "" ,0);
	return 0;
}
