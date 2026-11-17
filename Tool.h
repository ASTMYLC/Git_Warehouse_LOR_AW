#pragma once
#pragma execution_character_set("utf-8")
#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<random>
#include<algorithm>
#include <utility>
#include <unordered_map>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <windows.h>
using namespace std;
enum EditAction {
	EA_NONE = 0,
	EA_PICK_PAGE, EA_NEW_PAGE,
	EA_PICK_DICE,
	EA_COST_DEC, EA_COST_INC,
	EA_NAME_BOX, EA_ID_BOX, EA_COUNTER_TOGGLE,
	EA_DICE_TYPE, EA_DMIN_DEC, EA_DMIN_INC, EA_DMAX_DEC, EA_DMAX_INC, EA_DMG_TYPE,
	EA_ADD_DICE, EA_DEL_DICE,
	EA_DICE_BUFF, EA_DICE_BUFF_TARGET, EA_DICE_BUFF_INTENSITY_DEC, EA_DICE_BUFF_INTENSITY_INC,
	EA_DICE_BUFF_STACKS_DEC, EA_DICE_BUFF_STACKS_INC,
	EA_DICE_ADD_BUFF, EA_DICE_DEL_BUFF, EA_DICE_COUNTER,
	EA_EFFECT_PICK, EA_EFFECT_ADD, EA_EFFECT_DEL,
	EA_EFFECT_TRIGGER, EA_EFFECT_TYPE, EA_EFFECT_VALUE_DEC, EA_EFFECT_VALUE_INC,
	EA_EFFECT_TARGET, EA_EFFECT_BUFF, EA_EFFECT_BUFF_TARGET,
	EA_EFFECT_BUFF_INTENSITY_DEC, EA_EFFECT_BUFF_INTENSITY_INC,
	EA_EFFECT_BUFF_STACKS_DEC, EA_EFFECT_BUFF_STACKS_INC,
	EA_EFFECT_ADD_BUFF, EA_EFFECT_DEL_BUFF,

	EA_SAVE, EA_DEL_PAGE
};
enum dice_type {
	Attack = 1,
	Block = 2,
	Evade = 3,
};
enum buff {
	None_pos = 0,
	Strength = 1,
	Endurance = 2,
	Haste = 3,
	Protection = 4,
	Guard = 5,
	Charge = 6,
	Gas_pos = 7
};
enum debuff {
	None_pas = 0,
	Burn = 1,
	Fragile = 2,
	Sinking = 3,
	Termor = 4,
	Paralysis = 5,
	Stagger = 6,
	Gas_pas = 7,
	bleed = 8,
	wear = 9,
};
// Buff 施加目标（位掩码，可组合：SELF|ALLY = 自己+队友）这个思路很好啊，但是可能只能对自己和队友两个人，我们可能需要自由选择人数，如果这样的话，这里存在的意义应该是“我可以选择的对象类型”
enum BuffTarget {
	TARGET_SELF = 1,   // 0b001
	TARGET_ENEMY = 2,   // 0b010
	TARGET_ALLY = 4,   // 0b100
};
enum Damage_Type {
	None_Damage = 0,
	Slash_Damage = 1,
	Pierce_Damage = 2,
	Blunt_Damage = 3,
};
// 效果触发时机
enum EffectTrigger {
	TRIGGER_ON_USE = 0,        // 使用时
	TRIGGER_AFTER_USE = 1,     // 使用后
	TRIGGER_ON_CLASH_WIN = 2,  // 拼点胜利
	TRIGGER_ON_CLASH_LOSE = 3, // 拼点失败
	TRIGGER_ON_FIGHT = 4,	   // 战斗开始
};
// 效果类型
enum EffectType {
	EFFECT_APPLY_BUFF = 0,    // 施加buff
	EFFECT_HEAL_HP = 1,       // 回复HP
	EFFECT_DICE_POWER = 2,    // 骰子威力增减
	//待定
};
// 效果作用的骰子目标
enum DiceTarget {
	DICE_SELF = 0,     // 当前骰子自身
	DICE_NEXT = 1,     // 下一颗骰子
	DICE_LAST = 2,     // 最后一颗骰子
	DICE_ALL = 3,      // 所有剩余骰子
};
// 每种 Buff 的全局属性定义
struct BuffTypeInfo {
	wstring name;
	bool has_intensity;    // 是否有"强度"（如烟气）
	int max_intensity;
	bool has_stacks;       // 是否有"层数"（如力量、流血）
	int max_stacks;
};
extern const unordered_map<int, BuffTypeInfo> G_BuffTypes;
typedef struct SpeedEntry {
	wstring owner = L"";
	int value = 0;
	int team = 0;
	int Index = 0;
}SpeedEntry;
// 单条 buff 施加实例
struct BuffInstance {
	int buff_id = 0;       // buff 类型 ID
	int intensity = 0;     // 强度值（仅 has_intensity 的 buff 有效）
	int stacks = 0;        // 层数值（仅 has_stacks 的 buff 有效）
	int target = 1;        // BuffTarget 位掩码，默认 TARGET_SELF
};
typedef struct Basic_Dice {
	int type = 0;
	int min = 1, max = 6;
	int damage_type = 0;
	vector<BuffInstance> on_hit_buffs;
	bool Counter = 0;
	bool consume = true;
}Basic_Dice;
// 书页触发效果
struct PageEffect {
	int trigger = 0;                   // EffectTrigger
	int effect_type = 0;               // EffectType
	int value = 0;                     // HP回复量 或 威力增减值
	int buff_target = TARGET_SELF;     // BuffTarget buff 施加给谁
	int dice_target = DICE_SELF;       // DiceTarget buff 作用于哪颗骰子
	vector<BuffInstance> buffs;        // 若 effect_type == APPLY_BUFF
};

struct PageTemplate {
	int id=0;
	wstring name;
	int cost=1;
	vector<Basic_Dice> dice;
	bool Counter = 0;
	vector<PageEffect> effects;
};
struct Decision {
	int TargetTeam = 0;
	int TargetIdx=-1; // 指定
	int ChosenPageIdx=-1;         // 从手牌里选了哪张书页
	bool Pass=false;                 // 是否不行动
};
class Combat_Page {
public:

	int id = 0;
	wstring name = L"Error";
	int cost = 0;
	vector<Basic_Dice> dice;

	bool Counter = 0;

	Basic_Dice DiceMake(int type, int min, int max, int Dmg_type, bool Counter);
	void AddDice(int type, int min, int max, int Dmg_type, bool Counter = 0);
	static Combat_Page Get(int id);

};

class Core_Page {
public:
	wstring name;
	int Max_Hp = 40;
	int Hp = 40;
	float Slash_Resistance = 1.0f;
	float Pierce_Resistance = 1.0f;
	float Blunt_Resistance = 1.0f;

	int Max_Stagger = 40;
	int Stagger = 40;
	float Res_Slash = 1.0f;
	float Res_Pierce = 1.0f;
	float Res_Blunt = 1.0f;
	bool Is_Staggered = 0;
	float Staggered_Slash_Resistance = 2.0;
	float Staggered_Pierce_Resistance = 2.0;
	float Staggered_Blunt_Resistance = 2.0;

	int Basic_Mana_tab[7] = { 3,4,5,6,7,8,0 };
	int Mana_tab_Flexible = 0;
	int Mana = 3;
	int mana_recover = 1;

	int Nihulity = 12;

	int Flux_Max_Coin[6] = { 3,3,5,7,9,0 };

	int Flux_coin_total = 0;
	int Flux_coin_pos = 0;
	int Flux_coin_pas = 0;

	bool Flux_Level_Ready = 0;
	float Flux_tendence = 0;
	int Flux_Level_N = 0;

	int Speed_Dice = 1;
	int Speed_Dice_Max = 5;
	int Speed_Dice_Min = 2;
	int Speed_Dice_Flexible = 1;
	vector<int> Speed_Dice_Value;//数量等待填入，数值等待填入,class内函数已写完，等待gameloop调用
	vector<Decision>Speed_Decision;

	int Combat_Page_Flexible = 0;
	vector<Combat_Page> Combat_Page_Carried;//后续在游戏循环的初始化会接入基础书页上限为6，总携带书页上限为6+Combat_Page_Flexible，等待push_back
	vector<Combat_Page> ODeck;//手牌堆
	vector<Combat_Page> Deck;//抽牌堆
	vector<Combat_Page> Discard;//弃牌堆
	vector<Basic_Dice> Single_Defense_Zone;//单方面攻击区

	int Team_ID = 0;
	void Display();
	void Speed_Dice_Create();
	void Speed_Dice_Initialize();
	void Speed_Dice_Erase();
	void Speed_Dice_Erase_Delebrately();
	void Mana_recover();
	void Damage(int damage);
	Core_Page(wstring nam, int hp, int Man, int Man_co, int Nihu, int Flu_Le, int Flu_co);
	void Flux_Level_Check();
	void Flux_Coin_add(int state);
	void Flux_Level_Up();
	void End_of_the_Round();
	void Draw_a_Page(int index);
	void Use_Up_a_Page(int index);
	void Decision_Init();
};
class GameLoop {
public:
	int round = 0;

	vector<SpeedEntry> SpeedList_total;
	vector<SpeedEntry> SpeedList_1;
	vector<SpeedEntry> SpeedList_2;

	void Round_Begin(vector<Core_Page>& T1, vector<Core_Page>& T2);
	void Draw_Combat_Page();
	void Basic_Mana_Recover(vector<Core_Page>& T);
	void combat();
	int Count_Alive(const vector<Core_Page>& T);
	void Speed_Dice_Roll(vector<Core_Page>& T, vector<SpeedEntry>& SpeedList_Dice);
	void Add_To_Team(vector<Core_Page>& T1, vector<Core_Page>& T2, const Core_Page& src, int team_id);
	//拼点工具


	void Set_Decision_Target(Core_Page& owner, int diceIdx, int targetTeam, int targetIdx);
	void Set_Decision_Page(Core_Page& owner, int diceIdx, int pageIdx);
	void Set_Decision_Pass(Core_Page& owner, int diceIdx);
};
float GetRes(Core_Page& t, int dmgType);
float GetChaosRes(Core_Page& t, int dmgType);
void DealAttrDmg(Core_Page& t, int dmgType, int power);
void DealRealStag(Core_Page& t, int power);
void RecoverStag(Core_Page& t, int power);
void Main_Fight_Against(Core_Page& A, Basic_Dice& da, int va, Core_Page& B, Basic_Dice& db, int vb);
void One_Side_Attack(Core_Page& A, Basic_Dice& da, int va, Core_Page& B, Basic_Dice& db, int vb, bool& consumeA, bool& consumeB);




// 存档：写 PageLib.csv（先备份 .bak，再原子替换），并顺带导出 PageLib.txt 快照
std::wstring Save_PageLib_To_CSV();
// 读档：读 PageLib.csv。成功返回 true；失败时 report 写明原因
bool Load_PageLib_From_CSV(std::wstring& report);
// 只导出 txt 快照（只写不读，供 diff / 记事本查看）
std::wstring Export_PageLib_To_Txt();
