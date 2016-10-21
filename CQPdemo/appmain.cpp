
/*
* CoolQ Demo for VC++ 
* Api Version 9
* Written by Coxxs & Thanks for the help of orzFly
*/

#include "stdafx.h"
#include "string"
#include "cqp.h"
#include "appmain.h" //应用AppID等信息，请正确填写，否则酷Q可能无法加载
#include "time.h"
using namespace std;


int ac = -1; //AuthCode 调用酷Q的方法时需要用到
bool enabled = false;




/*
* 狼人游戏的相关变量参数
*/

//start 表示狼人环节的状态机，0表示没有游戏，1表示刚开始，等待其他人报名参加，2表示所有人员就位，开始游戏，3表示狼人杀人，4表示女巫救人，5表示预言家验人，6表示守卫守人。
int start = 0;
int playernum = 0; //报名参加狼人的玩家数量
int dying = 0; //表示被狼人选中的即将死亡的玩家
int ansofwitch = 0;	//表示女巫选择
int ansofseer = 0;	//表示预言家选择
int64_t playerqq[15];	//报名参加狼人的玩家QQ
char playername[15][50];	//报名参加狼人玩家的名字
int64_t start_playerqq;		//发起狼人游戏者QQ


int64_t uniqueQQgroup;
int isGroup = 0;	//表示需要向讨论组发送信息还是群发送，0表示向讨论组发送，1表示向群发送


//昵称相关结构体
struct CQ_Type_GroupMember
{
	int64_t				GroupID;			// 群号
	int64_t				QQID;				// QQ号
	std::string			nick;				// QQ昵称
	std::string			card;				// 群名片
	int					sex;				// 性别 0/男 1/女
	int					age;				// 年龄
	std::string			area;				// 地区
	int					jointime;			// 入群时间
	int					lastsent;			// 上次发言时间
	std::string			level_name;			// 头衔名字
	int					permission;			// 权限等级 1/成员 2/管理员 3/群主
	bool				unfriendly;			// 不良成员记录
	std::string			title;				// 自定义头衔
	int					titleExpiretime;	// 头衔过期时间
	bool				cardcanchange;		// 管理员是否能协助改名
};

struct CQ_TYPE_QQ
{
	int64_t				QQID;				//QQ号
	std::string			nick;				//昵称
	int					sex;				//性别
	int					age;				//年龄
};



//解析用户昵称的

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
void analysis_nickname(unsigned char* in, char** out, bool group)	//group = 1表示从QQ群过来的信息；0表示从讨论组
{
	size_t _len = 0;
	unsigned char *outtemp = 0;
	base64_decode(in, strlen((const char*)in), &outtemp, &_len);

	if (group)
	{
		//根据机器人的编码规则，进行读取中文字符
		unsigned char *infomation = outtemp;
		infomation += 16;	//得到昵称的长度位置
		unsigned short nickname_len = (infomation[0] << 8) | infomation[1];	//位运算，将16进制转化为10进制，得到昵称长度
		infomation += 2;	//得到昵称位置
		unsigned char *nickname;
		nickname = (unsigned char *)malloc(sizeof(unsigned char)* nickname_len);

		for (unsigned int i = 0; i < nickname_len; i++)
		{
			nickname[i] = infomation[i];

		}
		nickname[nickname_len] = '\0';	//得到昵称名字



		infomation = infomation + nickname_len;	//得到群名片的长度位置

		unsigned short groupname_len = (infomation[0] << 8) | infomation[1];	//位运算，将16进制转化为10进制，得到群名片长度

		if (groupname_len != 0)
		{
			infomation += 2;	//得到群名片位置
			unsigned char *groupname;
			groupname = (unsigned char *)malloc(sizeof(unsigned char)* nickname_len);
			for (unsigned int i = 0; i < groupname_len; i++)
			{
				groupname[i] = infomation[i];
			}

			groupname[groupname_len] = '\0';	//得到群名片名字


			*out = (char*)groupname;

		}
		else//表示没有设置群名片
		{
			*out = (char*)nickname;
		}
	}

	else  //从讨论组
	{
		//根据机器人的编码规则，进行读取中文字符
		unsigned char *infomation = outtemp;
		infomation += 8;	//得到昵称的长度位置
		unsigned short nickname_len = (infomation[0] << 8) | infomation[1];	//位运算，将16进制转化为10进制，得到昵称长度
		infomation += 2;	//得到昵称位置
		unsigned char *nickname;
		nickname = (unsigned char *)malloc(sizeof(unsigned char)* nickname_len);

		for (unsigned int i = 0; i < nickname_len; i++)
		{
			nickname[i] = infomation[i];

		}
		nickname[nickname_len] = '\0';	//得到昵称名字


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

char *inf[8] = { "狼人", "女巫", "预言家", "猎人", "白痴", "守卫", "村民", "替罪羊" };

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
		sendmessage(ac, fromGroup, "欢迎开启狼人游戏，我是这次游戏的上帝，小小萌神，请多指教。（感谢我的爸爸FrankShaw让我当上帝）\n请输入“#加入狼人”,来参加这次狼人游戏");
	
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

	strcpy(msg, "游戏开始！\n");

	sprintf(buf, "游戏一共有%d个人", number);
	strcat(msg, buf);

	strcat(msg, "角色分配如下：\n");

	sprintf(buf, "狼人一共有%d个\n", wolf);
	strcat(msg, buf);

	sprintf(buf, "%d名女巫\n", witch);
	strcat(msg, buf);

	sprintf(buf, "%d名预言家\n", seer);
	strcat(msg, buf);

	if (hunter)
	{
		sprintf(buf, "%d名猎人\n", hunter);
		strcat(msg, buf);
	}
	if (diot)
	{
		sprintf(buf, "%d名白痴\n", diot);
		strcat(msg, buf);
	}
	if (guard)
	{
		sprintf(buf, "%d名守卫\n", guard);
		strcat(msg, buf);
	}
	if (scapegoat)
	{
		sprintf(buf, "%d名替罪羊（注意，替罪羊为非神非民的好人）\n", scapegoat);
		strcat(msg, buf);
	}


	sprintf(buf, "%d名普通村民\n", villager);
	strcat(msg, buf);

	if (mode)
		strcat(msg, "本局游戏模式为屠边局，只要杀掉所有神职或者所有平民，狼人即为胜利。\n");
	else
		strcat(msg, "本局游戏模式为屠城局，狼人必须杀死所有好人方阵营方可胜利。\n");
	if (self == 0)
		strcat(msg, "本局游戏，女巫不可自救。\n");
	else if (self == 1)
		strcat(msg, "本局游戏，女巫只有第一天晚上可以自救。\n");
	else if (self == 2)
		strcat(msg, "本局游戏，女巫可以自救。\n");

	sendmessage(ac, fromGroup, msg);

	free(msg);
	free(buf);
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




void print_player(int64_t fromGroup, int num)
{
	
	sendmessage(ac, fromGroup, "正在分配角色，请等待...\n");

	for (int i = 0; i < playernum; i++)
	{
		char msg[80];
		sprintf(msg, "你是%d号玩家，你的身份是%s", i+1, inf[player[i] - 1]);
		CQ_sendPrivateMsg(ac, playerqq[i], msg);
		Sleep(1000);
	}
		
	sendmessage(ac, fromGroup, "角色分配完毕，游戏开始。\n祝玩的愉快！");


}

void wolftimetoselect()
{

	char boddy[120]="你的狼同伴是：\n";
	for (int i = 0; i < playernum; i++)
	{
		if (player[i] == 1)
		{
			char cnum[3];
			itoa(i + 1, cnum, 10);
			strcat(boddy, cnum);
			strcat(boddy, "号狼人\n");
		}
		
	}
	strcat(boddy, "请狼人讨论好之后，其中任意一位狼人回复一个数字，表示今晚要杀玩家的号码\n");

	for (int i = 0; i < playernum; i++)
	{
		if (player[i] == 1)
		{
			char msg[50];
			sprintf(msg, "你作为%d号狼人，", i+1);
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
	if (player[i] == 2)	//表示这个玩家是女巫
	{
		fromqq = playerqq[i];
		break;
	}
	if (self == 0 && player[dying - 1] == 2)
		CQ_sendPrivateMsg(ac, fromqq, "女巫，今晚你被狼人杀死了，由于不能自救，你只能选择是否使用毒药。\n如果使用，请直接回复需要毒杀的玩家的序号，如果不用，请回复0。\n");

	else if (self == 1 && day > 1 && player[dying - 1] == 2)
		CQ_sendPrivateMsg(ac, fromqq, "女巫，今晚你被狼人杀死了，由于已经不是第一天晚上，你不能自救，你只能选择是否使用毒药。\n如果使用，请直接回复需要毒杀的玩家的序号，如果不用，请回复0。\n");

	else
	{
		char msg[120] = "女巫，今晚死的人是";
		char cnum[3];
		itoa(dying, cnum, 10);
		strcat(msg, cnum);
		strcat(msg, "号，如果你救这位玩家，请回复#；\n如果不救回复0；\n如果不救人，并且要毒人，请直接回复玩家序号\n");
		CQ_sendPrivateMsg(ac, fromqq, msg);
	}

}


void dealwithdeadpeople(int ans, int dead)
{
	if (ans == -1)
		dying = 0;
	else if (ans == 0)
		player[dying] = player[dying] * -1;	//死人身份为负数，并且绝对值就是原身份
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
	if (player[i] == 3)	//表示这个玩家是预言家
	{
		fromqq = playerqq[i];
		break;
	}

	CQ_sendPrivateMsg(ac, fromqq, "你是预言家，请回复一个玩家编号，验明身份");
}


void nightover()
{
	sendmessage(ac, uniqueQQgroup, "好啦，昨晚上所有身份都已经行动完毕，现在开始竞选警长，如果没有警长，请回复#天亮了，查看昨晚情况");
}


void sunrise(int64_t fromGroup)
{
	if (ansofwitch == -1)
		sendmessage(ac, fromGroup, "昨天晚上是平安夜喔~");
	else if (ansofwitch == 0)
	{
		char msg[60] = "昨天晚上死者是";
		char cnum[3];
		itoa(dying, cnum, 10);
		strcat(msg, cnum);
		strcat(msg, "号，好可怜喔");
		sendmessage(ac, fromGroup, msg);

	}
	else
	{
		char msg[60] = "昨天晚上有两位玩家死亡，分别是";
		char cnum[3];
		itoa(dying, cnum, 10);
		strcat(msg, cnum);
		strcat(msg, "号和");
		itoa(ansofwitch, cnum, 10);
		strcat(msg, cnum);
		strcat(msg, "号，好可怕的夜晚喔");
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
* 返回应用的ApiVer、Appid，打包后将不会调用
*/
CQEVENT(const char*, AppInfo, 0)() {
	return CQAPPINFO;
}


/* 
* 接收应用AuthCode，酷Q读取应用信息后，如果接受该应用，将会调用这个函数并传递AuthCode。
* 不要在本函数处理其他任何代码，以免发生异常情况。如需执行初始化代码请在Startup事件中执行（Type=1001）。
*/
CQEVENT(int32_t, Initialize, 4)(int32_t AuthCode) {
	ac = AuthCode;
	return 0;
}


/*
* Type=1001 酷Q启动
* 无论本应用是否被启用，本函数都会在酷Q启动后执行一次，请在这里执行应用初始化代码。
* 如非必要，不建议在这里加载窗口。（可以添加菜单，让用户手动打开窗口）
*/
CQEVENT(int32_t, __eventStartup, 0)() {

	return 0;
}


/*
* Type=1002 酷Q退出
* 无论本应用是否被启用，本函数都会在酷Q退出前执行一次，请在这里执行插件关闭代码。
* 本函数调用完毕后，酷Q将很快关闭，请不要再通过线程等方式执行其他代码。
*/
CQEVENT(int32_t, __eventExit, 0)() {

	return 0;
}

/*
* Type=1003 应用已被启用
* 当应用被启用后，将收到此事件。
* 如果酷Q载入时应用已被启用，则在_eventStartup(Type=1001,酷Q启动)被调用后，本函数也将被调用一次。
* 如非必要，不建议在这里加载窗口。（可以添加菜单，让用户手动打开窗口）
*/
CQEVENT(int32_t, __eventEnable, 0)() {
	enabled = true;
	return 0;
}


/*
* Type=1004 应用将被停用
* 当应用被停用前，将收到此事件。
* 如果酷Q载入时应用已被停用，则本函数*不会*被调用。
* 无论本应用是否被启用，酷Q关闭前本函数都*不会*被调用。
*/
CQEVENT(int32_t, __eventDisable, 0)() {
	enabled = false;
	return 0;
}


/*
* Type=21 私聊消息
* subType 子类型，11/来自好友 1/来自在线状态 2/来自群 3/来自讨论组
*/
CQEVENT(int32_t, __eventPrivateMsg, 24)(int32_t subType, int32_t sendTime, int64_t fromQQ, const char *msg, int32_t font) {

	//如果要回复消息，请调用酷Q方法发送，并且这里 return EVENT_BLOCK - 截断本条消息，不再继续处理  注意：应用优先级设置为"最高"(10000)时，不得使用本返回值
	//如果不回复消息，交由之后的应用/过滤器处理，这里 return EVENT_IGNORE - 忽略本条消息

	if (start == 3)
	{
		for (int i = 0; i < playernum; i++)
		{
			if (playerqq[i] == fromQQ && player[i] == 1)	//表示这个玩家是狼人
			{
				dying = atoi(msg);
				if (dying > 0 && dying <= playernum)
				{

					char m[80];
					strcpy(m, "你选择了");
					strcat(m, msg);
					strcat(m, "号玩家，现在请闭眼");
					CQ_sendPrivateMsg(ac, fromQQ, m);
					start = 4;
					witchtoselect();	//进入女巫选择环节
				}

				else
					CQ_sendPrivateMsg(ac, fromQQ, "输入玩家的编号有误，请重新输入");
			}
		}
		
	}
	
	else if (start == 4)
	{
		for (int i = 0; i < playernum; i++)
		{
			if (playerqq[i] == fromQQ && player[i] == 2)	//表示这个玩家是女巫
			{
				if (strcmp(msg, "#") == 0)
					ansofwitch = -1;
				else
					ansofwitch = atoi(msg);

				//msg的回复是字符，直接用字符来处理。如果你救这位玩家，请回复#，如果不救回复0，如果不救人，并且要毒人，请直接回复玩家序号"
				if (ansofwitch >= -1 && ansofwitch <= playernum)
				{
					if (ansofwitch == 0)
					{
						CQ_sendPrivateMsg(ac, fromQQ, "女巫，今晚你选择了不救人，太残忍了");
						
					}
					else if (ansofwitch > 0 && ansofwitch <= playernum)
					{
						char m[120];
						strcpy(m, "女巫你选择了不救人，并且毒杀");
						strcat(m, msg);
						strcat(m, "号玩家，太可怕了。现在请闭眼");
						CQ_sendPrivateMsg(ac, fromQQ, m);
						
					}

					else if (ansofwitch == -1)
					{
						CQ_sendPrivateMsg(ac, fromQQ, "女巫，今晚你选择了拯救这个可怜人。现在请闭眼");
						
					}
					
					start = 5;	//进入预言家回合
					seertoselect();	//进入预言家选择
				}
				else
					CQ_sendPrivateMsg(ac, fromQQ, "女巫你输入的选择有误，请重新输入");
			}
		}

	}


	else if (start == 5)	//预言家回合
	{
		for (int i = 0; i < playernum; i++)
		{
			if (playerqq[i] == fromQQ && player[i] == 3)	//表示这个玩家是预言家
			{
				ansofseer = atoi(msg);
				if (dying > 0 && dying <= playernum)
				{

					char m[80];
					strcpy(m, "你选择了");
					strcat(m, msg);
					strcat(m, "号玩家，他的身份是...");
					CQ_sendPrivateMsg(ac, fromQQ, m);
					if (player[ansofseer - 1] > 1)
					{
						CQ_sendPrivateMsg(ac, fromQQ, "好人");
						start = 6;
						dealwithdeadpeople(ansofwitch, dying);	//处理死人
						nightover();
					}
					else if (player[ansofseer - 1] == 1)
					{
						CQ_sendPrivateMsg(ac, fromQQ, "狼人");
						start = 6;
						dealwithdeadpeople(ansofwitch, dying);	//处理死人
						nightover();
					}
					else
						CQ_sendPrivateMsg(ac, fromQQ, "死人，请重新输入一个活着的玩家");

					
				}

				else
					CQ_sendPrivateMsg(ac, fromQQ, "输入玩家的编号有误，请重新输入");
			}
		}
	}

	else if (start == 6)	//死人可以回复#查看身份来查看所有人的身份
	{
		for (int i = 0; i < playernum; i++)
		{
			if (playerqq[i] == fromQQ && player[i] < 0 && strcmp(msg, "#查看身份") == 0)	//检查这个人是死人，并且这个人要求查看身份
			{
				char buf[1500] = "现在给你看所有玩家身份：\n";
				for (int j = 0; j < playernum; j++)
				{
					char buff[60];
					sprintf(buff, "%d号玩家：%s，身份：%s\n", j + 1, playername[j], inf[abs(player[j]) - 1]);
					strcat(buf, buff);
				}
				CQ_sendPrivateMsg(ac, fromQQ, buf);
			}
		}
	}

	
	return EVENT_BLOCK;
}


/*
* Type=2 群消息
*/
CQEVENT(int32_t, __eventGroupMsg, 36)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, const char *fromAnonymous, const char *msg, int32_t font) {

	if (strcmp(msg, "#开始狼人") == 0)
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
				sendmessage(ac, fromGroup, "狼人游戏已经开始！");

		}
		else
			sendmessage(ac, fromGroup, "你不是我爸爸，不给你开狼人游戏");

	}


	else if (strcmp(msg, "#结束狼人") == 0)
	{
		if (start != 0)
		{
			if (start_playerqq == fromQQ)
			{
				sendmessage(ac, fromGroup, "结束狼人游戏！");
				start = 0;
			}
			else
				sendmessage(ac, fromGroup, "你不是狼人游戏的发起者，不能结束狼人游戏！");
		}
		else
			sendmessage(ac, fromGroup, "并没有开始狼人游戏！");
	}



	else if (strcmp(msg, "#加入狼人") == 0)
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
				if (playerqq[i] == fromQQ)	//检测重复加入的函数，前期开发先去掉
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
				sprintf(buf, "%s已经加入了狼人，不能重复加入", playername[i]);
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
				sprintf(buf, "%s加入成功\n当前人数为%d人:\n", nickname, playernum);
				for (int i = 0; i < 15; i++)
				if (playerqq[i] != 0)
				{
					char buff[15];
					sprintf(buff, "%d号玩家：", i + 1);
					strcat(buf, buff);
					strcat(buf, playername[i]);
					strcat(buf, "\n");
				}
				sendmessage(ac, fromGroup, buf);
			}
			
			
		}
		else
			sendmessage(ac, fromGroup, "没有正在进行的狼人游戏，请输入指令“开始狼人”，来开启一场狼人游戏");
	}

	else if (strcmp(msg, "#开启狼人游戏") == 0)
	{
		if (start == 1)
			if(playernum >= 8 && playernum <= 15)
			{
				initgame(playernum, fromGroup);
				start = 2;
				deal_character(playernum);
				print_player(fromGroup, playernum);
				start = 3;	//进入狼人杀人阶段
				wolftimetoselect();
			}
			else
				sendmessage(ac, fromGroup, "人数不足，无法开启游戏");
		else
			sendmessage(ac, fromGroup, "无法开启，游戏不在正确状态");
		
	}


	else if (start == 6 && strcmp(msg, "#天亮了") == 0)
	{
		sunrise(fromGroup);
	}


	if (strcmp(msg, "#查看状态") == 0)
	{
		char m[3];
		itoa(start, m, 10);
		sendmessage(ac, fromGroup, m);
	}


	if (strcmp(msg, "#查看玩家") == 0)
	{
		char buf[300] = "现在的玩家为：\n";
		for (int i = 0; i < 15; i++)
		if (playerqq[i] != 0)
		{
			char buff[15];
			sprintf(buff, "%d号玩家：", i + 1);
			strcat(buf, buff);
			strcat(buf, playername[i]);
			strcat(buf, "\n");
		}
		

		sendmessage(ac, fromGroup, buf);
	}


	return EVENT_BLOCK; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=4 讨论组消息
*/
CQEVENT(int32_t, __eventDiscussMsg, 32)(int32_t subType, int32_t sendTime, int64_t fromDiscuss, int64_t fromQQ, const char *msg, int32_t font) {

	if (strcmp(msg, "#开始狼人") == 0)
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
				sendmessage(ac, fromDiscuss, "狼人游戏已经开始！");

		}
		else
			sendmessage(ac, fromDiscuss, "你不是我爸爸，不给你开狼人游戏");

	}


	else if (strcmp(msg, "#结束狼人") == 0)
	{
		if (start != 0)
		{
			if (start_playerqq == fromQQ)
			{
				sendmessage(ac, fromDiscuss, "结束狼人游戏！");
				start = 0;
			}
			else
				sendmessage(ac, fromDiscuss, "你不是狼人游戏的发起者，不能结束狼人游戏！");
		}
		else
			sendmessage(ac, fromDiscuss, "并没有开始狼人游戏！");
	}



	else if (strcmp(msg, "#加入狼人") == 0)
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
				if (playerqq[i] == fromQQ)	//检测重复加入的函数，前期开发先去掉
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
				sprintf(buf, "%s已经加入了狼人，不能重复加入", playername[i]);
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
				sprintf(buf, "%s加入成功\n当前人数为%d人:\n", nickname, playernum);
				for (int i = 0; i < 15; i++)
				if (playerqq[i] != 0)
				{
					char buff[15];
					sprintf(buff, "%d号玩家：", i + 1);
					strcat(buf, buff);
					strcat(buf, playername[i]);
					strcat(buf, "\n");
				}
				sendmessage(ac, fromDiscuss, buf);
			}


		}
		else
			sendmessage(ac, fromDiscuss, "没有正在进行的狼人游戏，请输入指令“开始狼人”，来开启一场狼人游戏");
	}

	else if (strcmp(msg, "#开启狼人游戏") == 0)
	{
		if (start == 1)
		if (playernum >= 8 && playernum <= 15)
		{
			initgame(playernum, fromDiscuss);
			start = 2;
			deal_character(playernum);
			print_player(fromDiscuss, playernum);
			start = 3;	//进入狼人杀人阶段
			wolftimetoselect();
		}
		else
			sendmessage(ac, fromDiscuss, "人数不足，无法开启游戏");
		else
			sendmessage(ac, fromDiscuss, "无法开启，游戏不在正确状态");

	}


	else if (start == 6 && strcmp(msg, "#天亮了") == 0)
	{
		sunrise(fromDiscuss);
	}


	if (strcmp(msg, "#查看状态") == 0)
	{
		char m[3];
		itoa(start, m, 10);
		sendmessage(ac, fromDiscuss, m);
	}


	if (strcmp(msg, "#查看玩家") == 0)
	{
		char buf[300] = "现在的玩家为：\n";
		for (int i = 0; i < 15; i++)
		if (playerqq[i] != 0)
		{
			char buff[15];
			sprintf(buff, "%d号玩家：", i+1);
			strcat(buf, buff);
			strcat(buf, playername[i]);
			strcat(buf, "\n");
		}


		sendmessage(ac, fromDiscuss, buf);
	}
	return EVENT_BLOCK; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=101 群事件-管理员变动
* subType 子类型，1/被取消管理员 2/被设置管理员
*/
CQEVENT(int32_t, __eventSystem_GroupAdmin, 24)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t beingOperateQQ) {

	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=102 群事件-群成员减少
* subType 子类型，1/群员离开 2/群员被踢 3/自己(即登录号)被踢
* fromQQ 操作者QQ(仅subType为2、3时存在)
* beingOperateQQ 被操作QQ
*/
CQEVENT(int32_t, __eventSystem_GroupMemberDecrease, 32)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, int64_t beingOperateQQ) {

	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=103 群事件-群成员增加
* subType 子类型，1/管理员已同意 2/管理员邀请
* fromQQ 操作者QQ(即管理员QQ)
* beingOperateQQ 被操作QQ(即加群的QQ)
*/
CQEVENT(int32_t, __eventSystem_GroupMemberIncrease, 32)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, int64_t beingOperateQQ) {

	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=201 好友事件-好友已添加
*/
CQEVENT(int32_t, __eventFriend_Add, 16)(int32_t subType, int32_t sendTime, int64_t fromQQ) {

	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=301 请求-好友添加
* msg 附言
* responseFlag 反馈标识(处理请求用)
*/
CQEVENT(int32_t, __eventRequest_AddFriend, 24)(int32_t subType, int32_t sendTime, int64_t fromQQ, const char *msg, const char *responseFlag) {

	//CQ_setFriendAddRequest(ac, responseFlag, REQUEST_ALLOW, "");

	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=302 请求-群添加
* subType 子类型，1/他人申请入群 2/自己(即登录号)受邀入群
* msg 附言
* responseFlag 反馈标识(处理请求用)
*/
CQEVENT(int32_t, __eventRequest_AddGroup, 32)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, const char *msg, const char *responseFlag) {

	//if (subType == 1) {
	//	CQ_setGroupAddRequestV2(ac, responseFlag, REQUEST_GROUPADD, REQUEST_ALLOW, "");
	//} else if (subType == 2) {
	//	CQ_setGroupAddRequestV2(ac, responseFlag, REQUEST_GROUPINVITE, REQUEST_ALLOW, "");
	//}

	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}

/*
* 菜单，可在 .json 文件中设置菜单数目、函数名
* 如果不使用菜单，请在 .json 及此处删除无用菜单
*/
CQEVENT(int32_t, __menuA, 0)() {
	MessageBoxA(NULL, "这是menuA，在这里载入窗口，或者进行其他工作。", "" ,0);
	return 0;
}

CQEVENT(int32_t, __menuB, 0)() {
	MessageBoxA(NULL, "这是menuB，在这里载入窗口，或者进行其他工作。", "" ,0);
	return 0;
}
