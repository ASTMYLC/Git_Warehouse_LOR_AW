#pragma once
#pragma execution_character_set("utf-8")
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <string>
#include <vector>
#include"Tool.h"
struct PageEditState{
	int curPageID = 1001;
	int curDiceIdx = -1;
	bool editingName = false;
	bool editingId = false;
	std::wstring idEditStr;
	float cursorTimer = 0.f;
	bool cursorVisible = true;
};
extern PageEditState PES;
enum MenuState {
	MS_Main = 0,
	MS_CoreEdit = 1,
	MS_CombatEdit = 2,
	MS_FieldSetup = 3,
	MS_TeamSetup = 4,
	MS_Quit = 5,
	MS_Todo = 6,
};
enum BattlePhase{
	BP_DECISION,
	BP_RESOLVE
};
struct MenuItem {
	std::wstring text;
	MenuState target;
};
void Draw_Main(sf::RenderWindow& w, const sf::Font& font,bool fontok);
void Draw_Placeholder(sf::RenderWindow& w, const sf::Font& font, const wchar_t* label,bool fontok);
void Update(sf::Time dt, MenuState& state, float& alpha);
constexpr unsigned WIN_W = 1920, WIN_H = 1200;
constexpr float BTN_W = 760, BTN_H = 116.f, GAP = 40.f;
sf::View Make_Letterbox_View(sf::Vector2u windowSize);//等比分辨率运算.待写入
void Sync_View(sf::RenderWindow& w);//视口转窗口
sf::Vector2f Get_Mouse_Pos(sf::RenderWindow& w);//换算鼠标位置,鼠标像素->逻辑坐标
// ===== 点击反馈：点击特效 + 左下角数据提示 =====
struct ClickFeedback
{
	std::wstring text;                    // 左下角显示的文字
	sf::Vector2f pos{ 0.f, 0.f };         // 点击位置（画扩散方框用）
	float flashTimer = 0.f;               // 点击特效剩余时间
	float textTimer = 0.f;                // 文字剩余时间
};
extern ClickFeedback G_Feedback;

void Push_Feedback(const std::wstring& text);                     // 只换文字（不画特效）
void Push_Feedback(const std::wstring& text, sf::Vector2f pos);   // 文字 + 点击特效
void Update_Feedback(float dt);                                   // 推进计时
void Draw_Feedback(sf::RenderWindow& w, const sf::Font& font, bool fontok);
std::wstring Describe_Editor_Action(int action);                  // 动作名 + 当前数据快照




extern std::vector<MenuItem> G_Items;//菜单与一级控件参数
sf::FloatRect ItemRect(size_t i);
void Draw_Battle(sf::RenderWindow& w, const sf::Font& font, bool fontok, const std::vector<Core_Page>& T1, const std::vector<Core_Page>& T2, int curOwnerIdx, int curDiceIdx, BattlePhase phase);
sf::FloatRect EnemyPortrait(size_t idx);
sf::FloatRect AllyPortraitRect(size_t idx);
sf::FloatRect CardRect(size_t idx);
sf::FloatRect SpeedDiceRect(size_t idx);
sf::FloatRect SkipBtnRect();
sf::FloatRect PageListItemRect(size_t i);
sf::FloatRect NewPageBtnRect();
sf::FloatRect DiceListItemRect(size_t);
sf::FloatRect CostDecRect();
sf::FloatRect CostUbcRect();
sf::FloatRect NameBoxRect();
sf::FloatRect IdBoxRect();
sf::FloatRect CounterToggleRect();
sf::FloatRect DiceTypeRect();
sf::FloatRect DiceMinDecRect();
sf::FloatRect DiceMinIncRect();
sf::FloatRect DiceMaxDecRect();
sf::FloatRect DiceMaxIncRect();
sf::FloatRect DmgTypeRect();
sf::FloatRect BuffTypeRect();
sf::FloatRect DiceCounterRect();
sf::FloatRect BuffTargetRect();
sf::FloatRect BuffIntensityDecRect();
sf::FloatRect BuffIntensityIncRect();
sf::FloatRect BuffStacksDecRect();
sf::FloatRect BuffStacksIncRect();
sf::FloatRect AddBuffToDiceRect();
sf::FloatRect DelBuffFromDiceRect();
sf::FloatRect AddDiceRect();
sf::FloatRect DelDiceRect();
sf::FloatRect SaveBtnRect();
sf::FloatRect DelPageRect();
void Draw_PageEditor(sf::RenderWindow& w, const sf::Font& font, bool fontok);
int Editor_Hit(sf::Vector2f mouse);
void ProcessEditorClick(int action);
void ProcessEditorText(uint32_t unicode);