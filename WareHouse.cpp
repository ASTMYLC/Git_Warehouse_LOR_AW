#include"Tool.h"
#include <map>
#pragma execution_character_set("utf-8")
random_device rd;
mt19937 gen(rd());
const unordered_map<int, BuffTypeInfo> G_BuffTypes = {
	{0, {L"无",    false, 0, false, 0}},
	{1, {L"力量",  false, 0, true,  10}},
	{2, {L"忍耐",  false, 0, true,  10}},
	{3, {L"迅捷",  false, 0, true,  10}},
	{4, {L"守护",  false, 0, true,  10}},
	{5, {L"格挡",  false, 0, true,  10}},
	{6, {L"充能",  false, 0, true,  20}},
	{7, {L"烟气",  true,  10, false, 0}},
	{8, {L"流血",  false, 0, true,  20}},
	{9, {L"磨损",  false, 0, true,  20}},
};
unordered_map<int, PageTemplate> G_Page_Lib = {
	{ 1001, { 1001, L"一刀毙命", 2, {
		{Attack,6,12,Slash_Damage,{},false,true}}, false, {} } },

	{ 1002, { 1002, L"狴犴争讼", 3, {
		{Block,3,8,None_Damage,{},false,true},
		{Block,3,8,None_Damage,{},false,true}}, false, {} } },

	{ 1003, { 1003, L"百战一夜", 3, {
		{Attack,7,11,Pierce_Damage,{},false,true},
		{Attack,8,12,Slash_Damage,{},false,true},
		{Evade,7,11,None_Damage,{},false,true}}, false, {} } },

	{ 1837, { 1837, L"Victoria", 8, {
		{Attack,25,50,Slash_Damage,{},false,true},
		{Attack,18,37,Slash_Damage,{},false,true},
		{Block,18,90,None_Damage,{},false,true}}, false, {} } },
};

void Core_Page:: Display()
{
	wcout << L"角色:" << name << endl;
		cout << "当前Hp:" << Hp << endl;
	cout << "斩击抗性:" << Slash_Resistance << "  斩击混乱抗性:" << Res_Slash << endl;
	cout << "突刺抗性:" << Pierce_Resistance << "  突刺混乱抗性:" << Res_Pierce << endl;
	cout << "打击抗性:" << Blunt_Resistance << "  打击混乱抗性:" << Res_Blunt << endl;
	cout << "当前玛那:" << Mana << endl;
	cout << "当前玛那回复效率:" << mana_recover << endl;
	cout << "以太:" << Nihulity << endl;
	int Speed_Dice_Count = 1;
	int Speed_Dice_Count_temp_2 = 0;
	for (int i = 0; i < Speed_Dice_Value.size(); i++)
	{
		if (Speed_Dice_Value[i])
		{
			Speed_Dice_Count_temp_2++;
		}
	}

	for (int i = 0; i < Speed_Dice_Count_temp_2; i++)
	{

		if (Speed_Dice_Value[i])
		{
			cout << "速度骰子:" << "(" << Speed_Dice_Count << "\\" << Speed_Dice_Count_temp_2 << ")" << endl;
			cout << "速度值:" << Speed_Dice_Value[i];
			cout << "选择书页:" << endl;//等待后续接入
			cout << "指定对象:" << endl;//等待后续接入
			Speed_Dice_Count++;
		}
	}
	Flux_Level_Check();
	cout << "\n";

}
void Core_Page::Speed_Dice_Create()
{
	Speed_Dice_Value.push_back(0);
}
void Core_Page::Speed_Dice_Initialize()
{
	Speed_Dice_Value.clear();
	for (int i = 0; i < (Speed_Dice_Flexible + Speed_Dice); i++)
	{
		Speed_Dice_Value.push_back(0);
	}
}
void Core_Page::Speed_Dice_Erase()
{
	if ((int)Speed_Dice_Value.size() > Speed_Dice)
	{
		for (int i = 0; i < (int)Speed_Dice_Value.size(); i++)
		{
			if ((int)Speed_Dice_Value.size() > Speed_Dice)
			{
				Speed_Dice_Value.pop_back();
			}
			else
			{
				break;
			}
		}
	}
}
void Core_Page::Speed_Dice_Erase_Delebrately()
{
	if ((int)Speed_Dice_Value.size() > 0)
	{
		for (int i = 0; i < (int)Speed_Dice_Value.size(); i++)
		{
			if ((int)Speed_Dice_Value.size() > 0)
			{
				Speed_Dice_Value.pop_back();
			}
			else
			{
				break;
			}
		}
	}
}
void Core_Page::Mana_recover()
{

	for (int i = 0; i < mana_recover; i++)
	{
		if (Mana < (Basic_Mana_tab[Flux_Level_N] + Mana_tab_Flexible))
		{
			Mana++;
		}
	}

}
void Core_Page::Damage(int damage)
{
	Hp -= damage;
	if (!Hp)
	{
		//等待书写死亡函数
	}
}
Core_Page::Core_Page(wstring nam, int hp, int Man, int Man_co, int Nihu, int Flu_Le, int Flu_co)
{
	name = nam;
	Hp = hp;
	Mana = Man;
	mana_recover = Man_co;
	Nihulity = Nihu;
	Flux_Level_N = Flu_Le;
	Flux_coin_total = Flu_co;
}

void Core_Page::Flux_Level_Check()
{

	if (Flux_Level_N >= 0 && Flux_Level_N < 5)
	{
		if (Flux_coin_total == Flux_Max_Coin[Flux_Level_N])
		{
			cout << "流系硬币已满,回合结束时流系等级提升" << endl;
		}
		cout << "当前流系等级:" << Flux_Level_N << endl;
		cout << "流系硬币:" << Flux_coin_total << "(" << Flux_coin_total << "\\" << Flux_Max_Coin[Flux_Level_N] << ")" << endl;
	}
	else if (Flux_Level_N == 5)
	{
		cout << "当前流系等级:5" << endl;
		cout << "流系硬币:" << Flux_coin_total << "(" << Flux_coin_total << "\\Max)" << endl;
	}
	else
	{
		cout << "流系硬币计算错误,错误编号325" << endl;
	}
}
void Core_Page::Flux_Coin_add(int state)
{
	if (Flux_Level_N >= 0 && Flux_Level_N < 5)
	{
		if (Flux_coin_total < Flux_Max_Coin[Flux_Level_N])
		{
			Flux_coin_total++;
			if (state)
			{
				Flux_coin_pos++;
			}
			else
			{
				Flux_coin_pas++;
			}
		}
		if (Flux_coin_total == Flux_Max_Coin[Flux_Level_N])
		{
			Flux_Level_Ready = 1;
		}
	}
	else if (Flux_Level_N == 5)
	{
		return;
	}
	else
	{
		wcout << L"MAN!流系硬币增加错误，错误对象:" << name << endl;
	}
}
void Core_Page::Flux_Level_Up()
{
	if (Flux_Level_N > 5 || Flux_Level_N < 0)
	{
		wcout << L"流系等级提升工具函数出现未知错误,错误对象:" << name << endl;
		return;
	}
	if (Flux_Level_N >= 5)
	{
		return;
	}
	Flux_Level_N = 1 + Flux_Level_N;
	float Total_Flux_tendence = (float)(Flux_coin_pos + Flux_coin_pas);
	Flux_tendence = (Total_Flux_tendence > 0) ? (float)Flux_coin_pos / Total_Flux_tendence : 0.5f;
	cout << "流系等级提升！" << endl;
	if (Flux_tendence < 0.5)
	{
		cout << "以太偏流..." << endl;
	}
	else if (Flux_tendence > 0.5)
	{
		cout << "玛那汇聚！" << endl;
	}
	Mana = Basic_Mana_tab[Flux_Level_N] + Mana_tab_Flexible;
	//抽取一张书页,等待后续接入
	//触发其他被动效果
	Flux_coin_total = 0;
	Flux_coin_pos = 0;
	Flux_coin_pas = 0;
	Flux_Level_Ready = 0;
}
void Core_Page::End_of_the_Round()
{
	if (Flux_Level_Ready)
	{
		Flux_Level_Up();
	}
}
void Core_Page::Draw_a_Page(int index)//这部分的index需要外部调用时使用随机值，但该随机值的上下限需要变动来适应变动的书页总数，这一步较难实现
{
	ODeck.push_back(Deck[index]);
	Deck.push_back(Deck[index]);
}
void Core_Page::Use_Up_a_Page(int index)
{
	//需要一个标记来判断该书页是否可回收，如果不含该标记，则不会再将该书页返回Deck抽牌堆
	Deck.push_back(ODeck[index]);
	ODeck.erase(ODeck.begin() + index);
}

Basic_Dice Combat_Page::DiceMake(int type, int min, int max, int Dmg_type, bool Counter)
{
	Basic_Dice d;
	d.type = type;
	d.min = min;
	d.max = max;
	d.damage_type = Dmg_type;
	d.Counter = Counter;
	return d;
}
void Combat_Page::AddDice(int type, int min, int max, int Dmg_type, bool Counter)
{
	dice.push_back(DiceMake(type, min, max, Dmg_type, Counter));
}
Combat_Page Combat_Page::Get(int id)
{
	auto it = G_Page_Lib.find(id);
	if (it == G_Page_Lib.end())
	{
		cout << "错误: 未找到书页id " << id << endl;
		return Combat_Page{};
	}

	const PageTemplate& t = it->second;
	Combat_Page p;
	p.id = t.id;
	p.name = t.name;
	p.cost = t.cost;
	p.Counter = t.Counter;
	p.dice = t.dice;
	return p;
}
void Core_Page::Decision_Init()
{
	Speed_Decision.clear();
	for (size_t i = 0; i < Speed_Dice_Value.size(); i++)
	{
		Speed_Decision.emplace_back();
	}
}





void GameLoop::Round_Begin(vector<Core_Page>& T1, vector<Core_Page>& T2)
{
	SpeedList_total.clear();
	int Alive_T1 = 0;
	int Alive_T2 = 0;
	Alive_T1 = Count_Alive(T1);
	Alive_T2 = Count_Alive(T2);
	Speed_Dice_Roll(T1, SpeedList_1);
	Speed_Dice_Roll(T2, SpeedList_2);

	for (int i = 0; i < (int)SpeedList_1.size(); i++)
	{
		SpeedList_total.push_back(SpeedList_1[i]);
	}
	for (int i = 0; i < (int)SpeedList_2.size(); i++)
	{
		SpeedList_total.push_back(SpeedList_2[i]);
	}
	for (int i = 0; i < ((int)SpeedList_total.size()) - 1; i++)
	{
		for (int j = 0; j < ((int)SpeedList_total.size()) - 1 - i; j++)
		{
			if (SpeedList_total[j].value < SpeedList_total[j + 1].value)
			{
				swap(SpeedList_total[j], SpeedList_total[j + 1]);
			}
		}
	}
	Basic_Mana_Recover(T1);
	Basic_Mana_Recover(T2);
	
}
void GameLoop::Set_Decision_Target(Core_Page& owner, int diceIdx, int targetTeam, int targetIdx)
{
	if (diceIdx < 0 || diceIdx >= (int)owner.Speed_Decision.size())
	{
		cout << "速度骰子下标越界" << endl;
		return;
	}
	owner.Speed_Decision[diceIdx].TargetTeam = targetTeam;
	owner.Speed_Decision[diceIdx].TargetIdx = targetIdx;
	owner.Speed_Decision[diceIdx].Pass = false;
}
void GameLoop::Set_Decision_Page(Core_Page& owner, int diceIdx, int pageIdx)
{
	if (diceIdx < 0 || diceIdx >= (int)owner.Speed_Decision.size())
	{
		return;
	}
	owner.Speed_Decision[diceIdx].ChosenPageIdx = pageIdx;
}
void GameLoop::Set_Decision_Pass(Core_Page& owner, int diceIdx)
{
	if (diceIdx < 0 || diceIdx >= (int)owner.Speed_Decision.size())
	{
		return;
	}
	owner.Speed_Decision[diceIdx].Pass = true;
}
void GameLoop::Draw_Combat_Page()
{

}
void GameLoop::Basic_Mana_Recover(vector<Core_Page>& T)
{
	for (int i = 0; i < T.size(); i++)
	{
		for (int j = 0; j < T[i].mana_recover; j++)
		{
			T[i].Mana_recover();
		}
	}
}
void GameLoop::combat()
{
	int GPS_Speed = 0;
	bool Round_fight = true;
	while (Round_fight)
	{
		
	}
}
int GameLoop::Count_Alive(const vector<Core_Page>& T)
{
	int num_of_alive = 0;
	for (int i = 0; i < (int)T.size(); i++)
	{
		if (T[i].Hp > 0)
		{
			num_of_alive++;
		}
	}
	return num_of_alive;
}
void GameLoop::Speed_Dice_Roll(vector<Core_Page>& T, vector<SpeedEntry>& SpeedList_Dice)
{
	int k = 0;
	for (int i = 0; i < (int)T.size(); i++)
	{
		k = 0;
		for (int j = 0; j < T[i].Speed_Dice; j++)
		{

			uniform_int_distribution<int>dist(T[i].Speed_Dice_Min, T[i].Speed_Dice_Max);
			int Speed_Dice_Temp = dist(gen);
			SpeedEntry s;
			s.owner = T[i].name;
			s.team = T[i].Team_ID;
			s.value = Speed_Dice_Temp;
			T[i].Speed_Dice_Value[k] = Speed_Dice_Temp;
			SpeedList_Dice.push_back(s);
			k++;
		}
	}
}
void GameLoop::Add_To_Team(vector<Core_Page>&T1,vector<Core_Page>&T2,const Core_Page&src,int team_id)
{
	Core_Page cp_tp = src;
	if (team_id == 1)
	{
		cp_tp.Team_ID = 1;
		T1.push_back(cp_tp);
		wcout << L"「" << cp_tp.name << L"」已加入 友方队伍(当前" << T1.size() << L"人)" << endl;
	}
	else if (team_id == 2)
	{
		cp_tp.Team_ID = 2;
		T2.push_back(cp_tp);
		wcout << L"「" << cp_tp.name << L"」已加入 敌方队伍(当前" << T2.size() << L"人)" << endl;
	}
	else
	{
		cout<<"MAN,What can I say? Unsigned Team OUT!"<<endl;
	}

}
float GetRes(Core_Page& t, int dmgType)
{
	if (dmgType == Slash_Damage)
	{
		return t.Slash_Resistance;
	}
	if (dmgType == Pierce_Damage)
	{
		return t.Pierce_Resistance;
	}
		return t.Blunt_Resistance;
}
float GetChaosRes(Core_Page & t, int dmgType)
{
	if (dmgType == Slash_Damage)
	{
		return t.Res_Slash;
	}
	if (dmgType == Pierce_Damage)
	{
		return t.Res_Pierce;
	}
		return t.Res_Blunt;
}
void DealAttrDmg(Core_Page& t, int dmgType, int power)
{
	t.Hp -= (int)(power * GetRes(t, dmgType));
	t.Stagger -= (int)(power * GetChaosRes(t, dmgType));
	if (t.Hp < 0)
		t.Hp = 0;
	if (t.Stagger < 0)
		t.Stagger = 0;
}
void DealRealStag(Core_Page& t, int power)
{
	t.Stagger -= power;
	if (t.Stagger < 0)
		t.Stagger = 0;
}
void RecoverStag(Core_Page& t, int power)
{
	t.Stagger += power;
	if (t.Stagger > t.Max_Stagger)
		t.Stagger = t.Max_Stagger;
}
void Main_Fight_Against(Core_Page&A,Basic_Dice&da,int va,Core_Page&B,Basic_Dice&db,int vb)
{
	da.consume = true;
	db.consume = true;
	if (va == vb||(da.type==Evade&&db.type==Evade))
	{
		return;
	}
	if (va > vb)
	{
		A.Flux_Coin_add(1);
		B.Flux_Coin_add(0);
		if (da.type == Attack && db.type == Attack)
		{
			DealAttrDmg(B, da.damage_type, va);
		}
		else if (da.type == Attack && db.type == Block)
		{
			DealAttrDmg(B,da.damage_type,va-vb);
		}
		else if (da.type == Attack && db.type == Evade)
		{
			DealAttrDmg(B, da.damage_type, va);
		}
		else if (da.type == Block && db.type == Attack)
		{
			DealRealStag(B, va - vb);
		}
		else if (da.type == Block && db.type == Block)
		{
			DealRealStag(B, va);
		}
		else if (da.type == Block && db.type == Evade)
		{
			DealRealStag(B, va);
		}
		else if (da.type == Evade && db.type == Attack)
		{
			RecoverStag(A, va);
			da.consume = false;
		}
		else if (da.type == Evade && db.type == Block)
		{
			RecoverStag(A, va);
		}

		
	}
	else
	{
		A.Flux_Coin_add(0);
		B.Flux_Coin_add(1);
		if (db.type == Attack && da.type == Attack)
		{
			DealAttrDmg(A, db.damage_type, vb);
		}
		else if (db.type == Attack && da.type == Block)
		{
			DealAttrDmg(A,db.damage_type, vb - va);
		}
		else if (db.type == Attack && da.type == Evade)
		{
			DealAttrDmg(A, db.damage_type, vb);
		}
		else if (db.type == Block && da.type == Attack)
		{
			DealRealStag(A, vb-va);
		}
		else if (db.type == Block && da.type == Block)
		{
			DealRealStag(A, vb);
		}
		else if (db.type == Block && da.type == Evade)
		{
			DealRealStag(A, vb);
		}
		else if (db.type == Evade && da.type == Attack)
		{
			RecoverStag(B, vb);
			db.consume = false;
		}
		else if (db.type == Evade && da.type == Block)
		{
			RecoverStag(B, vb);
		}
	}
}
void One_Side_Attack(Core_Page& A, Basic_Dice& da, int va, Core_Page& B, Basic_Dice& db, int vb, bool& consumeA, bool& consumeB)
{

}
// ===================== 书页库存档（CSV，按列名读取） =====================
// PageLib.csv     —— 唯一可读写的数据源（Excel 用「CSV UTF-8」打开/保存）
// PageLib.csv.bak —— 每次保存前的自动备份
// PageLib.txt     —— 每次保存时导出的只读快照
static std::wstring Exe_Dir()
{
	wchar_t path[MAX_PATH] = {};
	DWORD n = GetModuleFileNameW(nullptr, path, MAX_PATH);
	if (n == 0) return std::wstring();
	std::wstring s(path, n);
	size_t p = s.find_last_of(L"\\/");
	if (p == std::wstring::npos) return std::wstring();
	return s.substr(0, p + 1);
}
static const std::wstring PAGE_LIB_CSV = Exe_Dir() + L"PageLib.csv";
static const std::wstring PAGE_LIB_TXT = Exe_Dir() + L"PageLib.txt";
static const std::wstring PAGE_LIB_BAK = Exe_Dir() + L"PageLib.csv.bak";
static const std::wstring PAGE_LIB_TMP = Exe_Dir() + L"PageLib.csv.tmp";

// ---------- 列名表：唯一的"列定义" ----------
// 输出表头用它；读取时按名字找列，所以：
//   1) 以后加字段：这里加一个名字 + Build_Page_Rows 加一格 + Load 加一行，插哪都行；
//   2) 旧文件缺这一列 → 自动取默认值；
//   3) 文件里多余的列 → 直接忽略。
static const char* const CSV_COLUMNS[] = {
	"kind", "page", "idx", "name", "cost", "counter",
	"type", "min", "max", "damage", "dice_counter", "consume",
	"buff", "intensity", "stacks", "buff_target"
};
constexpr size_t CSV_COL_COUNT = sizeof(CSV_COLUMNS) / sizeof(CSV_COLUMNS[0]);

// 写文件时用的列号（顺序必须和 CSV_COLUMNS 一致）
enum CsvCol {
	COL_KIND = 0, COL_PAGE, COL_IDX, COL_NAME, COL_COST, COL_COUNTER,
	COL_TYPE, COL_MIN, COL_MAX, COL_DAMAGE, COL_DICECOUNTER, COL_CONSUME,
	COL_BUFF, COL_INTENSITY, COL_STACKS, COL_BUFFTARGET
};

// ---------- 编码转换 ----------
static std::string W2U8(const std::wstring& w)
{
	if (w.empty()) return std::string();
	int len = WideCharToMultiByte(CP_UTF8, 0, w.c_str(), (int)w.size(), nullptr, 0, nullptr, nullptr);
	if (len <= 0) return std::string();
	std::string s((size_t)len, '\0');
	WideCharToMultiByte(CP_UTF8, 0, w.c_str(), (int)w.size(), &s[0], len, nullptr, nullptr);
	return s;
}
static std::wstring U82W(const std::string& s)
{
	if (s.empty()) return std::wstring();
	int len = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), nullptr, 0);
	if (len <= 0) return std::wstring();
	std::wstring w((size_t)len, L'\0');
	MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), &w[0], len);
	return w;
}

// ---------- CSV 基础 ----------
static std::string To_Lower_Ascii(const std::string& s)
{
	std::string o = s;
	for (char& c : o) if (c >= 'A' && c <= 'Z') c = (char)(c - 'A' + 'a');
	return o;
}
static std::string Trim(const std::string& s)
{
	size_t a = s.find_first_not_of(" \t");
	if (a == std::string::npos) return std::string();
	size_t b = s.find_last_not_of(" \t");
	return s.substr(a, b - a + 1);
}
static std::string Csv_Escape(const std::string& s)
{
	bool need = false;
	for (char c : s)
	{
		if (c == ',' || c == '"' || c == '\n' || c == '\r') { need = true; break; }
	}
	if (!need) return s;
	std::string out = "\"";
	for (char c : s)
	{
		if (c == '"') out += "\"\"";
		else out += c;
	}
	out += "\"";
	return out;
}
// 按逗号切分一行，支持双引号包裹（内部的 "" 表示一个引号）
static std::vector<std::string> Csv_Split(const std::string& line)
{
	std::vector<std::string> out;
	std::string cur;
	bool inQuote = false;
	for (size_t i = 0; i < line.size(); i++)
	{
		char c = line[i];
		if (inQuote)
		{
			if (c == '"')
			{
				if (i + 1 < line.size() && line[i + 1] == '"') { cur += '"'; i++; }
				else inQuote = false;
			}
			else cur += c;
		}
		else
		{
			if (c == '"') inQuote = true;
			else if (c == ',') { out.push_back(cur); cur.clear(); }
			else cur += c;
		}
	}
	out.push_back(cur);
	return out;
}
static std::string Join_Row(const std::vector<std::string>& row)
{
	std::string s;
	for (size_t i = 0; i < row.size(); i++)
	{
		if (i) s += ',';
		s += Csv_Escape(row[i]);
	}
	return s;
}
static int Clamp_Int(int v, int lo, int hi)
{
	if (v < lo) return lo;
	if (v > hi) return hi;
	return v;
}

// ---------- 按列号取值（底层） ----------
static std::string Col_Str(const std::vector<std::string>& f, size_t i)
{
	return (i < f.size()) ? Trim(f[i]) : std::string();
}
static int Col_Int(const std::vector<std::string>& f, size_t i, int def)
{
	if (i >= f.size()) return def;
	std::string t = Trim(f[i]);
	if (t.empty()) return def;
	try { return std::stoi(t); }
	catch (...) { return def; }		// 手改出 "abc" 也不会崩
}

// ---------- 按列名取值（上层） ----------
static std::unordered_map<std::string, size_t> Default_Header_Map()
{
	std::unordered_map<std::string, size_t> m;
	for (size_t i = 0; i < CSV_COL_COUNT; i++) m[CSV_COLUMNS[i]] = i;
	return m;
}
static std::unordered_map<std::string, size_t> Header_Map_From(const std::vector<std::string>& f)
{
	std::unordered_map<std::string, size_t> m;
	for (size_t i = 0; i < f.size(); i++)
	{
		std::string name = To_Lower_Ascii(Trim(f[i]));
		if (!name.empty()) m[name] = i;
	}
	return m;
}
// 找不到这一列时返回 (size_t)-1
static size_t Col_Index(const std::unordered_map<std::string, size_t>& hdr, const char* name)
{
	auto it = hdr.find(name);
	return (it == hdr.end()) ? (size_t)-1 : it->second;
}
static std::string Field_Str(const std::vector<std::string>& f,
	const std::unordered_map<std::string, size_t>& hdr, const char* name)
{
	size_t i = Col_Index(hdr, name);
	if (i == (size_t)-1) return std::string();
	return Col_Str(f, i);
}
static int Field_Int(const std::vector<std::string>& f,
	const std::unordered_map<std::string, size_t>& hdr, const char* name, int def)
{
	size_t i = Col_Index(hdr, name);
	if (i == (size_t)-1) return def;
	return Col_Int(f, i, def);
}
// 表头行判定：第一格不是 kind/page/dice/buff，但某格正好是 "kind"
static bool Is_Header_Row(const std::vector<std::string>& f)
{
	if (f.empty()) return false;
	std::string first = To_Lower_Ascii(Trim(f[0]));
	if (first == "page" || first == "dice" || first == "buff") return false;
	for (const std::string& c : f)
	{
		if (To_Lower_Ascii(Trim(c)) == "kind") return true;
	}
	return false;
}

// ---------- 内存 → 行 ----------
static bool By_Page_Id(const PageTemplate* a, const PageTemplate* b)
{
	return a->id < b->id;		// 输出按 id 排序，文件内容才稳定
}
static std::vector<std::vector<std::string>> Build_Page_Rows()
{
	std::vector<std::vector<std::string>> rows;

	std::vector<const PageTemplate*> list;
	list.reserve(G_Page_Lib.size());
	for (const auto& kv : G_Page_Lib) list.push_back(&kv.second);
	std::sort(list.begin(), list.end(), By_Page_Id);

	for (const PageTemplate* p : list)
	{
		std::vector<std::string> r(CSV_COL_COUNT, "");
		r[COL_KIND] = "PAGE";
		r[COL_PAGE] = std::to_string(p->id);
		r[COL_NAME] = W2U8(p->name);
		r[COL_COST] = std::to_string(p->cost);
		r[COL_COUNTER] = (p->Counter ? "1" : "0");
		rows.push_back(r);

		for (size_t di = 0; di < p->dice.size(); di++)
		{
			const Basic_Dice& d = p->dice[di];

			std::vector<std::string> dr(CSV_COL_COUNT, "");
			dr[COL_KIND] = "DICE";
			dr[COL_PAGE] = std::to_string(p->id);
			dr[COL_IDX] = std::to_string(di);
			dr[COL_TYPE] = std::to_string(d.type);
			dr[COL_MIN] = std::to_string(d.min);
			dr[COL_MAX] = std::to_string(d.max);
			dr[COL_DAMAGE] = std::to_string(d.damage_type);
			dr[COL_DICECOUNTER] = (d.Counter ? "1" : "0");
			dr[COL_CONSUME] = (d.consume ? "1" : "0");
			rows.push_back(dr);

			for (const BuffInstance& bf : d.on_hit_buffs)
			{
				std::vector<std::string> br(CSV_COL_COUNT, "");
				br[COL_KIND] = "BUFF";
				br[COL_PAGE] = std::to_string(p->id);
				br[COL_IDX] = std::to_string(di);
				br[COL_BUFF] = std::to_string(bf.buff_id);
				br[COL_INTENSITY] = std::to_string(bf.intensity);
				br[COL_STACKS] = std::to_string(bf.stacks);
				br[COL_BUFFTARGET] = std::to_string(bf.target);
				rows.push_back(br);
			}
		}
	}
	return rows;
}

// ---------- 行 → 内存 ----------
// 把一张书页放进临时表；顺带修掉明显不合法的数据
static void Commit_Page(std::unordered_map<int, PageTemplate>& dst, PageTemplate pg, int& dupCount, int& fixCount)
{
	if (pg.name.empty()) pg.name = L"未命名";

	if (pg.id <= 0)
	{
		int nid = 2001;
		while (dst.count(nid)) nid++;
		pg.id = nid;
		fixCount++;
		dst[nid] = pg;
		return;
	}
	if (dst.count(pg.id))
	{
		dupCount++;
		return;
	}
	dst[pg.id] = pg;
}

// 读文件时，每张书页先攒在草稿里：骰子和 buff 用各自的 idx 当键，
// 所以行的先后顺序完全不影响结果。
struct PageDraft
{
	PageTemplate page;
	std::map<int, Basic_Dice> dice;
	std::map<int, std::vector<BuffInstance>> buffs;
};

// ---------- 导出 txt 快照（只写不读） ----------
std::wstring Export_PageLib_To_Txt()
{
	std::ofstream ofs(PAGE_LIB_TXT.c_str());
	if (!ofs) return L"txt 快照写出失败";

	ofs << "\xEF\xBB\xBF";
	ofs << "# PageLib.txt —— 自动生成的只读快照，数据源是 PageLib.csv，请不要改这里\n";
	ofs << "# 列：";
	for (size_t i = 0; i < CSV_COL_COUNT; i++)
	{
		if (i) ofs << ' ';
		ofs << CSV_COLUMNS[i];
	}
	ofs << "\n";

	std::vector<std::vector<std::string>> rows = Build_Page_Rows();
	for (const auto& r : rows)
	{
		std::string s;
		for (size_t i = 0; i < r.size(); i++)
		{
			if (i) s += '\t';
			s += r[i];
		}
		ofs << s << "\n";
	}
	ofs.close();
	return L"";
}

// ---------- 保存 ----------
std::wstring Save_PageLib_To_CSV()
{
	// 1) 备份旧文件
	if (GetFileAttributesW(PAGE_LIB_CSV.c_str()) != INVALID_FILE_ATTRIBUTES)
	{
		CopyFileW(PAGE_LIB_CSV.c_str(), PAGE_LIB_BAK.c_str(), FALSE);
	}

	// 2) 先写临时文件，写完再替换（写到一半崩了也不会毁掉原文件）
	std::ofstream ofs(PAGE_LIB_TMP.c_str());
	if (!ofs)
	{
		return L"保存失败：无法创建 PageLib.csv.tmp（检查目录权限）";
	}

	ofs << "\xEF\xBB\xBF";
	for (size_t i = 0; i < CSV_COL_COUNT; i++)
	{
		if (i) ofs << ',';
		ofs << CSV_COLUMNS[i];
	}
	ofs << "\n";

	std::vector<std::vector<std::string>> rows = Build_Page_Rows();
	for (const auto& r : rows) ofs << Join_Row(r) << "\n";

	ofs.close();
	if (!ofs)
	{
		DeleteFileW(PAGE_LIB_TMP.c_str());
		return L"保存失败：写入过程中出错（磁盘满？）";
	}
	if (!MoveFileExW(PAGE_LIB_TMP.c_str(), PAGE_LIB_CSV.c_str(), MOVEFILE_REPLACE_EXISTING))
	{
		DeleteFileW(PAGE_LIB_TMP.c_str());
		return L"保存失败：无法替换 PageLib.csv（文件是不是正被 Excel 打开？）";
	}

	// 3) 顺带导出 txt 快照
	Export_PageLib_To_Txt();

	// 4) 统计还没落盘的东西（目前只有页面效果）
	int effectCount = 0;
	for (const auto& kv : G_Page_Lib) effectCount += (int)kv.second.effects.size();

	// 直接给完整路径，省得以后再问"文件存哪了"
	std::wstring rep = L"已保存 " + std::to_wstring(G_Page_Lib.size()) + L" 张书页 → " + PAGE_LIB_CSV;

	if (effectCount > 0)
	{
		rep += L"（" + std::to_wstring(effectCount) + L" 个页面效果暂未写入 CSV）";
	}
	return rep;
}

// ---------- 读取 ----------
bool Load_PageLib_From_CSV(std::wstring& report)
{
	std::ifstream ifs(PAGE_LIB_CSV.c_str());
	if (!ifs)
	{
		report = L"未找到 " + PAGE_LIB_CSV + L"（首次启动），使用出厂预设";
		return false;
	}

	std::unordered_map<std::string, size_t> hdr = Default_Header_Map();	// 先按默认列序，读到表头再覆盖
	std::unordered_map<int, PageDraft> drafts;
	std::unordered_map<int, bool> pageSeen;		// 只有真的写了 PAGE 行才算见过
	int lastPageId = 0;
	int lineNo = 0, dupCount = 0, fixCount = 0, badCount = 0;

	std::string raw;
	while (std::getline(ifs, raw))
	{
		lineNo++;
		// 去掉第一行的 UTF-8 BOM
		if (lineNo == 1 && raw.size() >= 3 &&
			(unsigned char)raw[0] == 0xEF && (unsigned char)raw[1] == 0xBB && (unsigned char)raw[2] == 0xBF)
		{
			raw = raw.substr(3);
		}
		if (!raw.empty() && raw.back() == '\r') raw.pop_back();

		std::string line = Trim(raw);
		if (line.empty() || line[0] == '#') continue;

		std::vector<std::string> f = Csv_Split(line);

		if (Is_Header_Row(f))
		{
			hdr = Header_Map_From(f);		// 以文件里的表头为准：列可以任意排序
			continue;
		}

		std::string kind = To_Lower_Ascii(Field_Str(f, hdr, "kind"));
		int pageId = Field_Int(f, hdr, "page", lastPageId);		// 没填 page 就沿用上一页
		if (pageId <= 0)
		{
			badCount++;			// page 为空，而且前面也没有 PAGE 行
			continue;
		}

		if (kind == "page")
		{
			PageDraft& d = drafts[pageId];
			if (pageSeen[pageId]) dupCount++;	// 同一页出现两次 PAGE 行，后一条覆盖
			pageSeen[pageId] = true;
			d.page.id = pageId;
			d.page.name = U82W(Field_Str(f, hdr, "name"));
			d.page.cost = Clamp_Int(Field_Int(f, hdr, "cost", 1), 1, 9);
			d.page.Counter = (Field_Int(f, hdr, "counter", 0) != 0);
			lastPageId = pageId;
		}
		else if (kind == "dice")
		{
			Basic_Dice bd;
			bd.type = Clamp_Int(Field_Int(f, hdr, "type", Attack), Attack, Evade);
			bd.min = Clamp_Int(Field_Int(f, hdr, "min", 1), 0, 99);
			bd.max = Clamp_Int(Field_Int(f, hdr, "max", 6), bd.min, 99);	// max 不许小于 min
			bd.damage_type = Clamp_Int(Field_Int(f, hdr, "damage", None_Damage), None_Damage, Blunt_Damage);
			bd.Counter = (Field_Int(f, hdr, "dice_counter", 0) != 0);
			bd.consume = (Field_Int(f, hdr, "consume", 1) != 0);

			PageDraft& d = drafts[pageId];
			int idx = Field_Int(f, hdr, "idx", (int)d.dice.size());
			d.dice[idx] = bd;		// 以 idx 为键：行顺序随便
			if (d.page.id == 0) d.page.id = pageId;
			lastPageId = pageId;
		}
		else if (kind == "buff")
		{
			BuffInstance bf;
			// buff_id 必须落在 0~9，否则编辑器里 G_BuffTypes.at 会直接抛异常
			bf.buff_id = Clamp_Int(Field_Int(f, hdr, "buff", 0), 0, 9);
			bf.intensity = Clamp_Int(Field_Int(f, hdr, "intensity", 0), 0, 99);
			bf.stacks = Clamp_Int(Field_Int(f, hdr, "stacks", 0), 0, 99);
			bf.target = Clamp_Int(Field_Int(f, hdr, "buff_target", TARGET_SELF), TARGET_SELF, 7);

			PageDraft& d = drafts[pageId];
			int idx = Field_Int(f, hdr, "idx", 0);
			d.buffs[idx].push_back(bf);
			if (d.page.id == 0) d.page.id = pageId;
			lastPageId = pageId;
		}
		else
		{
			badCount++;			// 未知 kind
		}
	}

	if (drafts.empty())
	{
		report = L"没读到可用书页：" + PAGE_LIB_CSV + L"（保留出厂预设）";
		return false;
	}

	// 组装：std::map 已按 idx 升序，顺序塞回书页并挂上 buff
	std::unordered_map<int, PageTemplate> fresh;
	for (auto& kv : drafts)
	{
		PageDraft& d = kv.second;
		if (d.page.id <= 0) d.page.id = kv.first;

		for (auto& dk : d.dice)
		{
			Basic_Dice bd = dk.second;
			auto it = d.buffs.find(dk.first);
			if (it != d.buffs.end()) bd.on_hit_buffs = it->second;
			d.page.dice.push_back(bd);
		}
		Commit_Page(fresh, d.page, dupCount, fixCount);
	}

	if (fresh.empty())
	{
		report = L"没读到可用书页：" + PAGE_LIB_CSV + L"（保留出厂预设）";
		return false;
	}

	G_Page_Lib = fresh;			// 文件是唯一数据源：整体替换

	report = L"已载入 " + std::to_wstring(G_Page_Lib.size()) + L" 张书页 ← " + PAGE_LIB_CSV;
	if (dupCount > 0) report += L"，重复 PAGE 行 " + std::to_wstring(dupCount) + L" 条";
	if (fixCount > 0) report += L"，补全缺失 id " + std::to_wstring(fixCount) + L" 张";
	if (badCount > 0) report += L"，忽略无法识别行 " + std::to_wstring(badCount) + L" 条";
	return true;
}
