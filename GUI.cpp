#include"GUI.h"
#include"Tool.h"
#pragma execution_character_set("utf-8")
float topMargin = 80.f;
extern unordered_map<int, PageTemplate> G_Page_Lib;

float battleTopMargin = 160.f;// 顶部标题区高度

constexpr float UNIT_W = 160.f, UNIT_H = 90.f, UNIT_GAP_X = 40.f;// 单位图标尺寸

constexpr float CARD_W = 150.f, CARD_H = 210.f, CARD_GAP = 30.f;// 手牌卡片尺寸

constexpr float DICE_W = 60.f, DICE_H = 60.f, DICE_GAP = 20.f;// 速度骰子图标

constexpr float LIST_W = 260, LIST_X = 40, LIST_ROW_H = 60;

constexpr float ATTR_X = 340, ATTR_W = 600, PROP_H = 64, STEP_W = 46, PROP_LABEL_W = 120;

constexpr float DICE_X = 340, DICE_Y = 470;

constexpr float PROP_X = 1000;

sf::View Make_Letterbox_View(sf::Vector2u windowSize)
{
	sf::View view;
	//逻辑的画布永远是WIN_W*WIN_H

	view.setSize({ (float)WIN_W,(float)WIN_H });
	view.setCenter({ (float)WIN_W * 0.5f, (float)WIN_H * 0.5f});
	float winW = (float)windowSize.x;
	float winH = (float)windowSize.y;
	if (winW <= 0.f || winH <= 0.f)
	{
		return view;
	}
	float windowRatio = winW / winH;
	float viewRatio = (float)WIN_W / (float)WIN_H;
	float sizeX = 1.f, sizeY = 1.f;
	float posX = 0.f, posY = 0.f;

	if (windowRatio > viewRatio)
	{
		//可视窗口比逻辑窗口更宽
		sizeX = viewRatio / windowRatio;
		posX = (1.f - sizeX) * 0.5f;
	}
	else
	{
		sizeY = windowRatio / viewRatio;
		posY = (1.f - sizeY) * 0.5f;
	}

	view.setViewport(sf::FloatRect({ posX,posY }, { sizeX,sizeY }));
	return view;

}

void Sync_View(sf::RenderWindow& w)
{
	w.setView(Make_Letterbox_View(w.getSize()));
}

sf::Vector2f Get_Mouse_Pos(sf::RenderWindow& w)
{
	// mapPixelToCoords 会自动套用当前 View（含 viewport 偏移），
	// 所以点在黑边上会得到负数或超出 1920/1200 的坐标，天然不会命中任何按钮
	return w.mapPixelToCoords(sf::Mouse::getPosition(w));
}

// ===== 点击反馈 =====
constexpr float CLICK_FLASH_TIME = 0.25f;	// 点击扩散方框持续多久
constexpr float CLICK_TEXT_TIME = 3.0f;		// 左下角文字停留多久
constexpr float CLICK_TEXT_FADE = 0.5f;		// 最后多久开始淡出

ClickFeedback G_Feedback;

void Push_Feedback(const std::wstring& text)
{
	G_Feedback.text = text;
	G_Feedback.textTimer = CLICK_TEXT_TIME;
}

void Push_Feedback(const std::wstring& text, sf::Vector2f pos)
{
	Push_Feedback(text);
	G_Feedback.pos = pos;
	G_Feedback.flashTimer = CLICK_FLASH_TIME;
}

void Update_Feedback(float dt)
{
	if (G_Feedback.flashTimer > 0.f)
	{
		G_Feedback.flashTimer -= dt;
		if (G_Feedback.flashTimer < 0.f) G_Feedback.flashTimer = 0.f;
	}
	if (G_Feedback.textTimer > 0.f)
	{
		G_Feedback.textTimer -= dt;
		if (G_Feedback.textTimer < 0.f) G_Feedback.textTimer = 0.f;
	}
}
void Draw_Feedback(sf::RenderWindow& w, const sf::Font& font, bool fontok)
{
	// 1) 点击瞬间从鼠标点扩散出去的方框
	if (G_Feedback.flashTimer > 0.f)
	{
		float t = 1.f - G_Feedback.flashTimer / CLICK_FLASH_TIME;	// 0 → 1
		float size = 26.f + 34.f * t;
		sf::RectangleShape ring({ size, size });
		ring.setPosition({ G_Feedback.pos.x - size * 0.5f, G_Feedback.pos.y - size * 0.5f });
		ring.setFillColor(sf::Color(0, 0, 0, 0));
		ring.setOutlineThickness(3.f);
		ring.setOutlineColor(sf::Color(255, 220, 120,
			static_cast<unsigned char>(255.f * (1.f - t))));	// 边扩散边变淡
		w.draw(ring);
	}

	// 2) 左下角的数据提示
	if (!fontok || G_Feedback.textTimer <= 0.f || G_Feedback.text.empty())
	{
		return;
	}
	unsigned char alpha = 255;
	if (G_Feedback.textTimer < CLICK_TEXT_FADE)
	{
		alpha = static_cast<unsigned char>(255.f * G_Feedback.textTimer / CLICK_TEXT_FADE);
	}
	sf::Text t(font, G_Feedback.text, 20);
	t.setFillColor(sf::Color(230, 230, 210, alpha));
	t.setPosition({ 40.f, WIN_H - 46.f });	// 左下角，避开右下角的保存/删除按钮
	w.draw(t);
}
std::vector<MenuItem> G_Items = {
		{L"核心书页",MS_CoreEdit},
		{L"战斗书页",MS_CombatEdit},
		{L"战斗场地设置",MS_FieldSetup},
		{L"战斗人员安排",MS_TeamSetup},
		{L"退出",MS_Quit},
		{L"其他",MS_Todo}
};
sf::FloatRect ItemRect(size_t i) 
{
	float total = (float)G_Items.size() * (BTN_H + GAP) - GAP;
	float StartY = (WIN_H - total) / 2;
	float x = (WIN_W - BTN_W) / 2;
	float y = StartY + (float)i * (BTN_H+GAP);
	return { {x,y},{BTN_W,BTN_H} };
}
void Draw_Main(sf::RenderWindow& w, const sf::Font& font,bool fontok)//这一步为什么要用const&font?既不修改又不拷贝副本，这导致我在FontOK这里出现问题
{
	sf::Vector2f mouse = Get_Mouse_Pos(w);	//逻辑坐标
	for (size_t i = 0; i < G_Items.size(); i++)
	{
		sf::FloatRect r = ItemRect(i);
		sf::RectangleShape btn({ BTN_W,BTN_H });
		btn.setPosition({ r.position.x,r.position.y });
		btn.setFillColor(r.contains(mouse) ? sf::Color(95, 95, 100) : sf::Color(55, 55, 60));
		btn.setOutlineThickness(4.f);
		btn.setOutlineColor(sf::Color(140, 140, 150));
		w.draw(btn);
		if (fontok)
		{
			sf::Text t(font, G_Items[i].text, 26);
			t.setFillColor(sf::Color::White);
			auto b = t.getLocalBounds();
			t.setPosition({ r.position.x + (BTN_W - b.size.x) / 2.f - b.position.x,r.position.y + (BTN_H - b.size.y) / 2.f - b.position.y });
			w.draw(t);
		}
	}
	
}
void Draw_Placeholder(sf::RenderWindow& w, const sf::Font& font, const wchar_t* label,bool fontok)
{
	if (fontok)
	{
		sf::Text t(font, label, 48);
		t.setFillColor(sf::Color::White);
		t.setOutlineThickness(3.f);
		t.setOutlineColor(sf::Color::Black);
		auto b = t.getLocalBounds();
		t.setPosition({ (WIN_W - b.size.x) / 2.f - b.position.x,topMargin-b.position.y});
		float pad = 20.f;//文字与框之间的留白像素
		sf::Vector2f frameSize = b.size + sf::Vector2f(2.f * pad, 2.f * pad);
		sf::Vector2f framePos = t.getPosition() + b.position - sf::Vector2f(pad, pad);
		sf::RectangleShape frame(frameSize);
		frame.setPosition(framePos);
		frame.setFillColor(sf::Color(0, 0, 0, 0));
		frame.setOutlineThickness(2.f);
		frame.setOutlineColor(sf::Color::White);
		
		w.draw(frame);
		w.draw(t);
	}
}
void Update(sf::Time dt, MenuState & state, float & alpha)
{
	Update_Feedback(dt.asSeconds());	// 点击反馈计时
	// 光标闪烁仅在编辑状态下运行
	if (PES.editingName || PES.editingId) {
		PES.cursorTimer += dt.asSeconds();
		// 每 0.5 秒翻转一次光标可见性
		if (PES.cursorTimer >= 0.5f) {
			PES.cursorTimer -= 0.5f;
			PES.cursorVisible = !PES.cursorVisible;
		}
	}
}
void Draw_Battle(sf::RenderWindow& w, const sf::Font& font, bool fontok, const std::vector<Core_Page>& T1, const std::vector<Core_Page>& T2, int curOwnerIdx, int curDiceIdx, BattlePhase phase)
{

}
sf::FloatRect EnemyPortrait(size_t idx)
{

}
sf::FloatRect AllyPortraitRect(size_t idx)
{
	float x = 80.f;
	float y = battleTopMargin + (float)idx * (UNIT_H + UNIT_GAP_X);
	return { {x, y}, {UNIT_W, UNIT_H} };
}
sf::FloatRect CardRect(size_t idx)
{

}
sf::FloatRect SpeedDiceRect(size_t idx)
{

}
sf::FloatRect SkipBtnRect()
{

}

sf::FloatRect PageListItemRect(size_t i)
{
	return{ {LIST_X,120.f + (float)i * (LIST_ROW_H + 8.f) },{LIST_W,LIST_ROW_H} };
}
sf::FloatRect NewPageBtnRect()
{
	return { {LIST_X,120.f + (float)G_Page_Lib.size() * (LIST_ROW_H + 8.f)},{LIST_W,50.f} };
}
sf::FloatRect DiceListItemRect(size_t i)
{
	return { {ATTR_X,DICE_Y + (float)i * 52.f},{ATTR_W,46.f} };
}
sf::FloatRect CostDecRect()
{
	return { {ATTR_X + PROP_LABEL_W,190.f},{STEP_W,40.f} };
}
sf::FloatRect CostUbcRect()
{
	return { {ATTR_X + PROP_LABEL_W + STEP_W + 8.f,190.f},{STEP_W,40.f} };
}
sf::FloatRect NameBoxRect()
{
	return { {ATTR_X + PROP_LABEL_W,130.f},{ATTR_W - PROP_LABEL_W,40.f} };
}
sf::FloatRect IdBoxRect()
{
	return { {ATTR_X + PROP_LABEL_W, 80.f}, {100.f, 40.f} };
}
sf::FloatRect CounterToggleRect()
{
	return{ {ATTR_X + PROP_LABEL_W,250.f},{STEP_W * 3,40.f} };
}
sf::FloatRect DiceTypeRect()
{
	return { {PROP_X + PROP_LABEL_W, 480.f}, {STEP_W * 4, 40.f} };
}
sf::FloatRect DiceMinDecRect()
{
	return { {PROP_X + PROP_LABEL_W, 540},{STEP_W,40} };
}
sf::FloatRect DiceMinIncRect()
{
	return { {PROP_X + PROP_LABEL_W + STEP_W + 8, 540},{STEP_W,40} };
}
sf::FloatRect DiceMaxDecRect()
{
	return { {PROP_X + PROP_LABEL_W, 600},{STEP_W,40} };
}
sf::FloatRect DiceMaxIncRect()
{
	return { {PROP_X + PROP_LABEL_W + STEP_W + 8, 600},{STEP_W,40} };
}
sf::FloatRect DmgTypeRect()
{
	return { {PROP_X + PROP_LABEL_W, 660},{STEP_W * 4,40} };
}
sf::FloatRect BuffTypeRect()
{
	return { {PROP_X + PROP_LABEL_W, 820},{STEP_W * 4,40} };
}
sf::FloatRect DiceCounterRect()
{
	return { {PROP_X + PROP_LABEL_W, 720},{STEP_W * 3,40} };
}
sf::FloatRect BuffTargetRect()
{
	return { {PROP_X + PROP_LABEL_W, 880},{STEP_W * 4,40} };
}
sf::FloatRect BuffIntensityDecRect()
{
	return { {PROP_X + PROP_LABEL_W, 940},{STEP_W,40} };
}
sf::FloatRect BuffIntensityIncRect()
{
	return { {PROP_X + PROP_LABEL_W + STEP_W + 8, 940},{STEP_W,40} };
}
sf::FloatRect BuffStacksDecRect()
{
	return { {PROP_X + PROP_LABEL_W, 1000},{STEP_W,40} };
}
sf::FloatRect BuffStacksIncRect()
{
	return { {PROP_X + PROP_LABEL_W + STEP_W + 8, 1000},{STEP_W,40} };
}
sf::FloatRect AddBuffToDiceRect()
{
	return { {PROP_X, 1060},{150,44} };
}
sf::FloatRect DelBuffFromDiceRect()
{
	return { {PROP_X + 170, 1060},{150,44} };
}

sf::FloatRect AddDiceRect()
{
	return { {ATTR_X, 820},{150,44} };
}
sf::FloatRect DelDiceRect()
{
	return { {ATTR_X + 170, 820},{150,44} };
}
sf::FloatRect SaveBtnRect()
{
	return { {WIN_W - 360.f, WIN_H - 90.f}, {160.f, 56.f} };
}
sf::FloatRect DelPageRect()
{
	return { {WIN_W - 180.f, WIN_H - 90.f}, {150.f, 56.f} };
}



static const wchar_t* DiceTypeName(int t)
{
	if(t == Attack)
	{
		return L"进攻";
	}
	if(t == Block)
	{
		return L"招架";
	}
	return L"闪避";
}
static const wchar_t* BuffTargetName(int mask)
{
	switch (mask) {
	case 1: return L"自己";
	case 2: return L"敌人";
	case 3: return L"自己+敌人";
	case 4: return L"队友";
	case 5: return L"自己+队友";
	case 6: return L"敌人+队友";
	case 7: return L"全部";
	default: return L"???";
	}
}

static const wchar_t* BuffName(int id)
{
	switch (id) {
	case 0: return L"无";
	case 1: return L"力量";
	case 2: return L"忍耐";
	case 3: return L"迅捷";
	case 4: return L"守护";
	case 5: return L"格挡";
	case 6: return L"充能";
	case 7: return L"烟气";
	case 8: return L"流血";
	case 9: return L"磨损";
	default: return L"???";
	}
}

static const wchar_t* DmgName(int t)
{
	if (t == Slash_Damage)
	{
		return L"斩击";
	}
	if (t == Pierce_Damage)
	{
		return L"突刺";
	}
	if (t == Blunt_Damage)
	{
		return L"打击";
	}
	return L"无";
}

void Draw_PageEditor(sf::RenderWindow& w, const sf::Font& font, bool fontok)
{
	if (!fontok) return;
	// 标题
	sf::Text title(font, L"核心书页编辑器", 34);
	title.setFillColor(sf::Color::White);
	title.setPosition({ 60.f, 40.f });
	w.draw(title);

	// 左侧书页列表
	int row = 0;
	for (auto& kv : G_Page_Lib) 
	{
		sf::FloatRect r = PageListItemRect(row);
		bool sel = (kv.first == PES.curPageID);
		sf::RectangleShape box({ r.size.x, r.size.y });
		box.setPosition({ r.position.x, r.position.y });
		box.setFillColor(sel ? sf::Color(60, 100, 60) : sf::Color(45, 45, 50));
		box.setOutlineThickness(2.f);
		box.setOutlineColor(sf::Color(120, 120, 130));
		w.draw(box);
		sf::Text t(font, std::to_wstring(kv.first) + L" " + kv.second.name, 20);
		t.setFillColor(sf::Color::White);
		t.setPosition({ r.position.x + 10.f, r.position.y + (r.size.y - 24.f) / 2.f });
		w.draw(t);
		row++;
	}
	// 新建按钮
	sf::FloatRect nb = NewPageBtnRect();
	sf::RectangleShape nbb({ nb.size.x, nb.size.y });
	nbb.setPosition({ nb.position.x, nb.position.y });
	nbb.setFillColor(sf::Color(50, 80, 110));
	w.draw(nbb);
	sf::Text nt(font, L"[+] 新建书页", 20);
	nt.setFillColor(sf::Color::White);
	nt.setPosition({ nb.position.x + 10.f, nb.position.y + 10.f });
	w.draw(nt);

	// 判空不够：map 非空但 curPageID 已失效时，operator[] 同样会插入一张空书页
	auto itpg = G_Page_Lib.find(PES.curPageID);
	if (itpg == G_Page_Lib.end()) return;
	auto& pg = itpg->second;

	// 属性区
	sf::Text idL(font, L"编号", 22);
	idL.setPosition({ ATTR_X,80.f });
	idL.setFillColor(sf::Color::White);
	w.draw(idL);
	sf::RectangleShape idBox(IdBoxRect().size);
	idBox.setPosition(IdBoxRect().position);
	idBox.setFillColor(sf::Color(50, 50, 60));
	idBox.setOutlineThickness(PES.editingId ? 2.f : 1.f);
	idBox.setOutlineColor(PES.editingId ? sf::Color(255, 200, 50) : sf::Color(150, 150, 150));
	w.draw(idBox);
	std::wstring idText;
	if (PES.editingId)
	{
		idText = PES.idEditStr;
		if (PES.cursorVisible)
		{
			idText += L"|";
		}
	}
	else 
	{
		idText = std::to_wstring(PES.curPageID);
	}
	sf::Text idV(font, idText, 20);
	idV.setFillColor(sf::Color::Yellow);
	idV.setPosition({ IdBoxRect().position.x + 8.f,IdBoxRect().position.y + 8.f });
	w.draw(idV);
	sf::Text nmL(font, L"名称:", 22); nmL.setPosition({ ATTR_X, 130.f }); nmL.setFillColor(sf::Color::White); w.draw(nmL);

	sf::RectangleShape nmBox2(NameBoxRect().size);
	nmBox2.setPosition(NameBoxRect().position);
	nmBox2.setFillColor(sf::Color(50, 50, 60));
	nmBox2.setOutlineThickness(PES.editingName ? 2.f : 1.f);
	nmBox2.setOutlineColor(PES.editingName ? sf::Color(255, 200, 50) : sf::Color(150, 150, 150));
	w.draw(nmBox2);

	std::wstring nameText = pg.name;
	if (PES.editingName && PES.cursorVisible)
	{
		nameText += L"|";
	}
	sf::Text nm(font, nameText, 20);
	nm.setFillColor(sf::Color::White);
	nm.setPosition({ NameBoxRect().position.x + 8.f,NameBoxRect().position.y + 8.f });
	w.draw(nm);

	sf::Text cL(font, L"费用:", 22); cL.setPosition({ ATTR_X, 190.f }); cL.setFillColor(sf::Color::White); w.draw(cL);
	// 费用 - + 按钮
	auto drawStep = [&](sf::FloatRect rr, const wchar_t* s) {
		sf::RectangleShape b({ rr.size.x, rr.size.y }); b.setPosition({ rr.position.x, rr.position.y });
		b.setFillColor(sf::Color(70, 70, 80)); b.setOutlineThickness(1.f); b.setOutlineColor(sf::Color(180, 180, 180)); w.draw(b);
		sf::Text tt(font, s, 22); tt.setFillColor(sf::Color::White);
		auto bb = tt.getLocalBounds(); tt.setPosition({ rr.position.x + (rr.size.x - bb.size.x) / 2.f - bb.position.x, rr.position.y + (rr.size.y - bb.size.y) / 2.f - bb.position.y }); w.draw(tt);
		};
	drawStep(CostDecRect(), L"-"); drawStep(CostUbcRect(), L"+");
	sf::Text cv(font, std::to_wstring(pg.cost), 22); cv.setFillColor(sf::Color::Yellow);
	cv.setPosition({ CostUbcRect().position.x + CostUbcRect().size.x + 14.f, 192.f }); w.draw(cv);

	// 反击开关
	sf::Text ckL(font, L"反击:", 22); ckL.setPosition({ ATTR_X, 250.f }); ckL.setFillColor(sf::Color::White); w.draw(ckL);
	sf::FloatRect ckr = CounterToggleRect();
	sf::RectangleShape ckb({ ckr.size.x, ckr.size.y }); ckb.setPosition({ ckr.position.x, ckr.position.y });
	ckb.setFillColor(pg.Counter ? sf::Color(80, 130, 60) : sf::Color(70, 70, 80)); w.draw(ckb);
	sf::Text ckv(font, pg.Counter ? L"开" : L"关", 20); ckv.setFillColor(sf::Color::White);
	ckv.setPosition({ ckr.position.x + 10.f, ckr.position.y + 8.f }); w.draw(ckv);

	// 骰子列表
	if (PES.curDiceIdx >= 0 && PES.curDiceIdx < (int)pg.dice.size())
	{
		// 已选中某颗骰子 → 标题改为"正在编辑"，只显示这一颗在顶端
		sf::Text dlL(font, L"正在编辑骰子 #" + std::to_wstring(PES.curDiceIdx + 1) + L"（点击返回列表）", 22);
		dlL.setPosition({ ATTR_X, DICE_Y - 30.f }); dlL.setFillColor(sf::Color::White); w.draw(dlL);


		sf::FloatRect r = DiceListItemRect(0);  // 固定在位置 0（顶端）
		sf::RectangleShape b({ r.size.x, r.size.y }); b.setPosition({ r.position.x, r.position.y });
		b.setFillColor(sf::Color(80, 90, 60)); b.setOutlineThickness(1.f); b.setOutlineColor(sf::Color(130, 130, 140)); w.draw(b);
		auto& d = pg.dice[PES.curDiceIdx];
		sf::Text tt(font, std::to_wstring(PES.curDiceIdx + 1) + L" " + DiceTypeName(d.type) +
			L" " + std::to_wstring(d.min) + L"-" + std::to_wstring(d.max) +
			L" " + DmgName(d.damage_type), 18);
		tt.setFillColor(sf::Color::White); tt.setPosition({ r.position.x + 10.f, r.position.y + 12.f }); w.draw(tt);
	}
	else
	{
		// 未选中 → 显示全部骰子（原逻辑）
		sf::Text dlL(font, L"骰子列表:", 22); dlL.setPosition({ ATTR_X, DICE_Y - 30.f }); dlL.setFillColor(sf::Color::White); w.draw(dlL);
		for (size_t di = 0; di < pg.dice.size(); di++) {
			sf::FloatRect r = DiceListItemRect(di);
			bool sel = ((int)di == PES.curDiceIdx);
			sf::RectangleShape b({ r.size.x, r.size.y }); b.setPosition({ r.position.x, r.position.y });
			b.setFillColor(sel ? sf::Color(80, 90, 60) : sf::Color(40, 40, 45)); b.setOutlineThickness(1.f); b.setOutlineColor(sf::Color(130, 130, 140)); w.draw(b);
			auto& d = pg.dice[di];
			sf::Text tt(font, std::to_wstring(di + 1) + L" " + DiceTypeName(d.type) +
				L" " + std::to_wstring(d.min) + L"-" + std::to_wstring(d.max) +
				L" " + DmgName(d.damage_type), 18);
			tt.setFillColor(sf::Color::White); tt.setPosition({ r.position.x + 10.f, r.position.y + 12.f }); w.draw(tt);
		}
	}

	// 添加/删除骰子
	sf::Text ad(font, L"添加骰子", 20); sf::FloatRect ar = AddDiceRect();
	sf::RectangleShape ab({ ar.size.x, ar.size.y }); ab.setPosition(ar.position); ab.setFillColor(sf::Color(60, 90, 60)); w.draw(ab);
	ad.setPosition({ ar.position.x + 10.f, ar.position.y + 10.f }); ad.setFillColor(sf::Color::White); w.draw(ad);
	sf::Text dt(font, L"删除骰子", 20); sf::FloatRect dr = DelDiceRect();
	sf::RectangleShape db({ dr.size.x, dr.size.y }); db.setPosition(dr.position); db.setFillColor(sf::Color(110, 60, 60)); w.draw(db);
	dt.setPosition({ dr.position.x + 10.f, dr.position.y + 10.f }); dt.setFillColor(sf::Color::White); w.draw(dt);

	// 当前选中骰子的属性编辑区
	if (PES.curDiceIdx >= 0 && PES.curDiceIdx < (int)pg.dice.size()) {
		auto& d = pg.dice[PES.curDiceIdx];
		auto drawProp = [&](const wchar_t* label, sf::FloatRect typeRect, const wchar_t* val) {
			sf::Text l(font, label, 20); l.setPosition({ typeRect.position.x - PROP_LABEL_W, typeRect.position.y }); l.setFillColor(sf::Color::White); w.draw(l);
			sf::FloatRect tr = typeRect;
			sf::RectangleShape b({ tr.size.x, tr.size.y }); b.setPosition(tr.position); b.setFillColor(sf::Color(60, 70, 80)); b.setOutlineThickness(1.f); b.setOutlineColor(sf::Color(160, 160, 170)); w.draw(b);
			sf::Text v(font, val, 20); v.setFillColor(sf::Color::Yellow); v.setPosition({ tr.position.x + 8.f, tr.position.y + 8.f }); w.draw(v);
			};
		drawProp(L"类型:", DiceTypeRect(), DiceTypeName(d.type));
		drawProp(L"最小值:", DiceMinDecRect(), std::to_wstring(d.min).c_str()); // 简化，实际 - 值按钮另画
		drawProp(L"最大值:", DiceMaxDecRect(), std::to_wstring(d.max).c_str());
		// 伤害类型：仅进攻型骰子显示
		if (d.type == Attack) {
			drawProp(L"伤害:", DmgTypeRect(), DmgName(d.damage_type));
		}
		// === 骰子命中 Buff 编辑 ===
		if (!d.on_hit_buffs.empty()) {
			auto& bf = d.on_hit_buffs[0];  // 当前编辑第一个 buff（后续可扩展列表）
			auto& info = G_BuffTypes.at(bf.buff_id);

			// Buff 类型
			drawProp(L"Buff:", BuffTypeRect(), BuffName(bf.buff_id));
			// Buff 目标
			drawProp(L"目标:", BuffTargetRect(), BuffTargetName(bf.target));

			// 强度（仅 has_intensity 的 buff 显示）
			if (info.has_intensity) {
				drawProp(L"强度:", BuffIntensityDecRect(), std::to_wstring(bf.intensity).c_str());
				drawStep(BuffIntensityDecRect(), L"-"); drawStep(BuffIntensityIncRect(), L"+");
				sf::Text intv(font, std::to_wstring(bf.intensity), 20); intv.setFillColor(sf::Color::Yellow);
				intv.setPosition({ BuffIntensityIncRect().position.x + BuffIntensityIncRect().size.x + 14.f, BuffIntensityDecRect().position.y + 8.f }); w.draw(intv);
			}
			// 层数（仅 has_stacks 的 buff 显示）
			if (info.has_stacks) {
				float sy = info.has_intensity ? 920.f : 860.f;  // 无强度时上移
				// 动态位置略复杂，简化：固定用 BuffStacksDecRect
				drawProp(L"层数:", BuffStacksDecRect(), std::to_wstring(bf.stacks).c_str());
				drawStep(BuffStacksDecRect(), L"-"); drawStep(BuffStacksIncRect(), L"+");
				sf::Text stv(font, std::to_wstring(bf.stacks), 20); stv.setFillColor(sf::Color::Yellow);
				stv.setPosition({ BuffStacksIncRect().position.x + BuffStacksIncRect().size.x + 14.f, BuffStacksDecRect().position.y + 8.f }); w.draw(stv);
			}
		}
		else {
			// 无 buff 时显示提示
			sf::Text nobuff(font, L"（无命中Buff，点击添加）", 18);
			nobuff.setPosition({ BuffTypeRect().position.x - PROP_LABEL_W, BuffTypeRect().position.y }); nobuff.setFillColor(sf::Color(150, 150, 150)); w.draw(nobuff);
		}
		// 添加/删除骰子Buff按钮
		sf::Text abf(font, L"+Buff", 18); sf::FloatRect abfr = AddBuffToDiceRect();
		sf::RectangleShape abfb({ abfr.size.x, abfr.size.y }); abfb.setPosition(abfr.position); abfb.setFillColor(sf::Color(60, 90, 60)); w.draw(abfb);
		abf.setPosition({ abfr.position.x + 10.f, abfr.position.y + 10.f }); abf.setFillColor(sf::Color::White); w.draw(abf);
		sf::Text dbf(font, L"-Buff", 18); sf::FloatRect dbfr = DelBuffFromDiceRect();
		sf::RectangleShape dbfb({ dbfr.size.x, dbfr.size.y }); dbfb.setPosition(dbfr.position); dbfb.setFillColor(sf::Color(110, 60, 60)); w.draw(dbfb);
		dbf.setPosition({ dbfr.position.x + 10.f, dbfr.position.y + 10.f }); dbf.setFillColor(sf::Color::White); w.draw(dbf);

		// 骰子反击开关
		sf::Text dcL(font, L"骰子反击:", 20); dcL.setPosition({ DiceCounterRect().position.x - PROP_LABEL_W, DiceCounterRect().position.y }); dcL.setFillColor(sf::Color::White); w.draw(dcL);
		sf::RectangleShape dcb({ DiceCounterRect().size.x, DiceCounterRect().size.y });
		dcb.setPosition(DiceCounterRect().position);
		dcb.setFillColor(d.Counter ? sf::Color(80, 130, 60) : sf::Color(70, 70, 80));
		dcb.setOutlineThickness(1.f); dcb.setOutlineColor(sf::Color(160, 160, 170)); w.draw(dcb);
		sf::Text dcv(font, d.Counter ? L"开" : L"关", 20); dcv.setFillColor(sf::Color::White);
		dcv.setPosition({ DiceCounterRect().position.x + 10.f, DiceCounterRect().position.y + 8.f }); w.draw(dcv);
		// min/max 的 - 和 + 按钮
		drawStep(DiceMinDecRect(), L"-"); drawStep(DiceMinIncRect(), L"+");
		sf::Text minv(font, std::to_wstring(d.min), 20); minv.setFillColor(sf::Color::Yellow);
		minv.setPosition({ DiceMinIncRect().position.x + DiceMinIncRect().size.x + 14.f, DiceMinDecRect().position.y + 8.f }); w.draw(minv);
		drawStep(DiceMaxDecRect(), L"-"); drawStep(DiceMaxIncRect(), L"+");
		sf::Text maxv(font, std::to_wstring(d.max), 20); maxv.setFillColor(sf::Color::Yellow);
		maxv.setPosition({ DiceMaxIncRect().position.x + DiceMaxIncRect().size.x + 14.f, DiceMaxDecRect().position.y + 8.f }); w.draw(maxv);
	}

	// 底部 保存/删除
	sf::FloatRect sv = SaveBtnRect(); sf::RectangleShape svb({ sv.size.x, sv.size.y }); svb.setPosition(sv.position); svb.setFillColor(sf::Color(50, 100, 60)); w.draw(svb);
	sf::Text svt(font, L"保存", 22); svt.setFillColor(sf::Color::White); svt.setPosition({ sv.position.x + 40.f, sv.position.y + 12.f }); w.draw(svt);
	sf::FloatRect dp = DelPageRect(); sf::RectangleShape dpb({ dp.size.x, dp.size.y }); dpb.setPosition(dp.position); dpb.setFillColor(sf::Color(120, 50, 50)); w.draw(dpb);
	sf::Text dpt(font, L"删除书页", 20); dpt.setFillColor(sf::Color::White); dpt.setPosition({ dp.position.x + 25.f, dp.position.y + 14.f }); w.draw(dpt);
}
int Editor_Hit(sf::Vector2f mouse)
{
	int row = 0;
	for (auto& kv : G_Page_Lib) { if (PageListItemRect(row).contains(mouse)) return EA_PICK_PAGE; row++; }
	if (NewPageBtnRect().contains(mouse)) return EA_NEW_PAGE;
	auto curPg = G_Page_Lib.find(PES.curPageID);
	if (curPg != G_Page_Lib.end())
	{
		for (size_t di = 0; di < curPg->second.dice.size(); di++)
		{
			if (DiceListItemRect(di).contains(mouse)) return EA_PICK_DICE;
		}
	}
	if (CostDecRect().contains(mouse)) return EA_COST_DEC;
	if (CostUbcRect().contains(mouse)) return EA_COST_INC;
	if (IdBoxRect().contains(mouse)) return EA_ID_BOX;
	if (NameBoxRect().contains(mouse)) return EA_NAME_BOX;
	if (CounterToggleRect().contains(mouse)) return EA_COUNTER_TOGGLE;
	if (DiceTypeRect().contains(mouse)) return EA_DICE_TYPE;
	if (DiceMinDecRect().contains(mouse)) return EA_DMIN_DEC;
	if (DiceMinIncRect().contains(mouse)) return EA_DMIN_INC;
	if (DiceMaxDecRect().contains(mouse)) return EA_DMAX_DEC;
	if (DiceMaxIncRect().contains(mouse)) return EA_DMAX_INC;
	if (DmgTypeRect().contains(mouse)) return EA_DMG_TYPE;
	if (BuffTypeRect().contains(mouse)) return EA_DICE_BUFF;
	if (DiceCounterRect().contains(mouse)) return EA_DICE_COUNTER;
	if (BuffTargetRect().contains(mouse)) return EA_DICE_BUFF_TARGET;
	if (BuffIntensityDecRect().contains(mouse)) return EA_DICE_BUFF_INTENSITY_DEC;
	if (BuffIntensityIncRect().contains(mouse)) return EA_DICE_BUFF_INTENSITY_INC;
	if (BuffStacksDecRect().contains(mouse)) return EA_DICE_BUFF_STACKS_DEC;
	if (BuffStacksIncRect().contains(mouse)) return EA_DICE_BUFF_STACKS_INC;
	if (AddBuffToDiceRect().contains(mouse)) return EA_DICE_ADD_BUFF;
	if (DelBuffFromDiceRect().contains(mouse)) return EA_DICE_DEL_BUFF;

	if (AddDiceRect().contains(mouse)) return EA_ADD_DICE;
	if (DelDiceRect().contains(mouse)) return EA_DEL_DICE;
	if (SaveBtnRect().contains(mouse)) return EA_SAVE;
	if (DelPageRect().contains(mouse)) return EA_DEL_PAGE;
	return EA_NONE;
}
void ProcessEditorClick(int action)
{
	auto itpg = G_Page_Lib.find(PES.curPageID);
	if (itpg == G_Page_Lib.end())
	{
		return;
	}
	auto& pg = itpg->second;
	switch (action)
	{
	case EA_NAME_BOX:
		PES.editingName = true;
		PES.editingId = false;
		PES.cursorVisible = true;
		PES.cursorTimer = 0.f;
		break;
	case EA_ID_BOX:
		PES.editingId = true;
		PES.editingName = false;
		PES.idEditStr = std::to_wstring(PES.curPageID);
		PES.cursorVisible = true;
		PES.cursorTimer = 0.f;
		break;
	case EA_COST_DEC:
		if (PES.editingId)
		{
			try {
				int newId = std::stoi(PES.idEditStr);
				if(newId>0&&G_Page_Lib.find(newId)==G_Page_Lib.end())
				{ 
					auto node = G_Page_Lib.extract(PES.curPageID);
					if (!node.empty())
					{
						node.key() = newId;
						node.mapped().id = newId;
						G_Page_Lib.insert(std::move(node));
						PES.curPageID = newId;
					}
				}
			}catch(...){/*非法输入,忽略*/ }
			PES.editingId = false;
		}
		if (PES.editingName)
		{
			PES.editingName = false;
		}
		if (pg.cost > 1)
		{
			pg.cost--;
		}
		break;
	case EA_COST_INC:
		if (PES.editingId)
		{
			try {
				int newId = std::stoi(PES.idEditStr);
				if (newId > 0 && G_Page_Lib.find(newId) == G_Page_Lib.end())
				{
					auto node = G_Page_Lib.extract(PES.curPageID);
					if (!node.empty())
					{
						node.key() = newId;
						node.mapped().id = newId;
						G_Page_Lib.insert(std::move(node));
						PES.curPageID = newId;
					}
				}
			}catch(...){}
			PES.editingId = false;
		}
		if (PES.editingName)PES.editingName = false;
		if (pg.cost < 9)pg.cost++;//卡九费
		break;
	case EA_NEW_PAGE:
	{
		// 先确认可能正在进行的编辑
		if (PES.editingId) {
			try {
				int newId = std::stoi(PES.idEditStr);
				if (newId > 0 && G_Page_Lib.find(newId) == G_Page_Lib.end()) {
					auto node = G_Page_Lib.extract(PES.curPageID);
					if (!node.empty()) {
						node.key() = newId;
						node.mapped().id = newId;
						G_Page_Lib.insert(std::move(node));
						PES.curPageID = newId;
					}
				}
			}
			catch (...) {}
			PES.editingId = false;
		}
		if (PES.editingName) PES.editingName = false;
		// 创建新书页
		int newId = 2001;
		while (G_Page_Lib.count(newId)) newId++;
		G_Page_Lib[newId] = { newId, L"新建书页", 1, {}, false };
		PES.curPageID = newId;
		PES.curDiceIdx = -1;
		break;
	}
	case EA_COUNTER_TOGGLE:
		if (PES.editingId) {
			try {
				int newId = std::stoi(PES.idEditStr);
				if (newId > 0 && G_Page_Lib.find(newId) == G_Page_Lib.end()) {
					auto node = G_Page_Lib.extract(PES.curPageID);
					if (!node.empty()) {
						node.key() = newId;
						node.mapped().id = newId;
						G_Page_Lib.insert(std::move(node));
						PES.curPageID = newId;
					}
				}
			}
			catch (...) {}
			PES.editingId = false;
		}
		if (PES.editingName) PES.editingName = false;
		pg.Counter = !pg.Counter;
		break;
	case EA_DICE_TYPE:
		if (PES.editingId) { /* 确认编辑 */ try {
			int newId = std::stoi(PES.idEditStr);
			if (newId > 0 && G_Page_Lib.find(newId) == G_Page_Lib.end()) {
				auto node = G_Page_Lib.extract(PES.curPageID);
				if (!node.empty()) {
					node.key() = newId; node.mapped().id = newId;
					G_Page_Lib.insert(std::move(node)); PES.curPageID = newId;
				}
			}
		}
		catch (...) {} PES.editingId = false;
		}
		if (PES.editingName) PES.editingName = false;
		if (PES.curDiceIdx >= 0 && PES.curDiceIdx < (int)pg.dice.size()) {
			auto& d = pg.dice[PES.curDiceIdx];
			d.type = (d.type == Evade) ? Attack : d.type + 1;
		}
		break;
	case EA_DMIN_DEC:
		if (PES.editingId) {
			try {
				int newId = std::stoi(PES.idEditStr);
				if (newId > 0 && G_Page_Lib.find(newId) == G_Page_Lib.end()) {
					auto node = G_Page_Lib.extract(PES.curPageID);
					if (!node.empty()) {
						node.key() = newId; node.mapped().id = newId;
						G_Page_Lib.insert(std::move(node)); PES.curPageID = newId;
					}
				}
			}
			catch (...) {} PES.editingId = false;
		}
		if (PES.editingName) PES.editingName = false;
		if (PES.curDiceIdx >= 0 && PES.curDiceIdx < (int)pg.dice.size()) {
			auto& d = pg.dice[PES.curDiceIdx];
			if (d.min > 0) d.min--;
		}
		break;
	case EA_DMIN_INC:
		if (PES.editingId) {
			try {
				int newId = std::stoi(PES.idEditStr);
				if (newId > 0 && G_Page_Lib.find(newId) == G_Page_Lib.end()) {
					auto node = G_Page_Lib.extract(PES.curPageID);
					if (!node.empty()) {
						node.key() = newId; node.mapped().id = newId;
						G_Page_Lib.insert(std::move(node)); PES.curPageID = newId;
					}
				}
			}
			catch (...) {} PES.editingId = false;
		}
		if (PES.editingName) PES.editingName = false;
		if (PES.curDiceIdx >= 0 && PES.curDiceIdx < (int)pg.dice.size()) {
			auto& d = pg.dice[PES.curDiceIdx];
			if (d.min < d.max) d.min++;
		}
		break;
	case EA_DMAX_DEC:
		if (PES.editingId) {
			try {
				int newId = std::stoi(PES.idEditStr);
				if (newId > 0 && G_Page_Lib.find(newId) == G_Page_Lib.end()) {
					auto node = G_Page_Lib.extract(PES.curPageID);
					if (!node.empty()) {
						node.key() = newId; node.mapped().id = newId;
						G_Page_Lib.insert(std::move(node)); PES.curPageID = newId;
					}
				}
			}
			catch (...) {} PES.editingId = false;
		}
		if (PES.editingName) PES.editingName = false;
		if (PES.curDiceIdx >= 0 && PES.curDiceIdx < (int)pg.dice.size()) {
			auto& d = pg.dice[PES.curDiceIdx];
			if (d.max > d.min) d.max--;  // 修复：原来是 d.max++
		}
		break;
	case EA_DMAX_INC:
		if (PES.editingId) {
			try {
				int newId = std::stoi(PES.idEditStr);
				if (newId > 0 && G_Page_Lib.find(newId) == G_Page_Lib.end()) {
					auto node = G_Page_Lib.extract(PES.curPageID);
					if (!node.empty()) {
						node.key() = newId; node.mapped().id = newId;
						G_Page_Lib.insert(std::move(node)); PES.curPageID = newId;
					}
				}
			}
			catch (...) {} PES.editingId = false;
		}
		if (PES.editingName) PES.editingName = false;
		if (PES.curDiceIdx >= 0 && PES.curDiceIdx < (int)pg.dice.size()) {
			auto& d = pg.dice[PES.curDiceIdx];
			d.max++;
		}
		break;
	case EA_DMG_TYPE:
		if (PES.editingId) {
			try {
				int newId = std::stoi(PES.idEditStr);
				if (newId > 0 && G_Page_Lib.find(newId) == G_Page_Lib.end()) {
					auto node = G_Page_Lib.extract(PES.curPageID);
					if (!node.empty()) {
						node.key() = newId; node.mapped().id = newId;
						G_Page_Lib.insert(std::move(node)); PES.curPageID = newId;
					}
				}
			}
			catch (...) {} PES.editingId = false;
		}
		if (PES.editingName) PES.editingName = false;
		if (PES.curDiceIdx >= 0 && PES.curDiceIdx < (int)pg.dice.size()) {
			auto& d = pg.dice[PES.curDiceIdx];
			d.damage_type = (d.damage_type == Blunt_Damage) ? Slash_Damage : d.damage_type + 1;
		}
		break;
	case EA_DICE_BUFF:
		// 循环切换 buff 类型
		if (PES.editingId) {
			try {
				int newId = std::stoi(PES.idEditStr);
				if (newId > 0 && G_Page_Lib.find(newId) == G_Page_Lib.end()) {
					auto node = G_Page_Lib.extract(PES.curPageID);
					if (!node.empty()) {
						node.key() = newId; node.mapped().id = newId;
						G_Page_Lib.insert(std::move(node)); PES.curPageID = newId;
					}
				}
			}
			catch (...) {} PES.editingId = false;
		}
		if (PES.editingName) PES.editingName = false;
		if (PES.curDiceIdx >= 0 && PES.curDiceIdx < (int)pg.dice.size()) {
			auto& bf = pg.dice[PES.curDiceIdx].on_hit_buffs;
			if (!bf.empty()) {
				bf[0].buff_id = (bf[0].buff_id >= 9) ? 0 : bf[0].buff_id + 1;
				// 切换 buff 类型时重置强度/层数为 0
				bf[0].intensity = 0;
				bf[0].stacks = 0;
			}
		}
		break;
	case EA_DICE_BUFF_TARGET:
		// 循环切换目标：1→2→4→3→5→6→7→1
		if (PES.editingId) {
			try {
				int newId = std::stoi(PES.idEditStr);
				if (newId > 0 && G_Page_Lib.find(newId) == G_Page_Lib.end()) {
					auto node = G_Page_Lib.extract(PES.curPageID);
					if (!node.empty()) {
						node.key() = newId; node.mapped().id = newId;
						G_Page_Lib.insert(std::move(node)); PES.curPageID = newId;
					}
				}
			}
			catch (...) {} PES.editingId = false;
		}
		if (PES.editingName) PES.editingName = false;
		if (PES.curDiceIdx >= 0 && PES.curDiceIdx < (int)pg.dice.size()) {
			auto& bf = pg.dice[PES.curDiceIdx].on_hit_buffs;
			if (!bf.empty()) {
				static const int targets[] = { 1,2,4,3,5,6,7 };
				int idx = 0;
				for (int i = 0; i < 7; i++) if (bf[0].target == targets[i]) { idx = i; break; }
				bf[0].target = targets[(idx + 1) % 7];
			}
		}
		break;
	case EA_DICE_BUFF_INTENSITY_DEC:
		if (PES.editingId) {
			try {
				int newId = std::stoi(PES.idEditStr);
				if (newId > 0 && G_Page_Lib.find(newId) == G_Page_Lib.end()) {
					auto node = G_Page_Lib.extract(PES.curPageID);
					if (!node.empty()) {
						node.key() = newId; node.mapped().id = newId;
						G_Page_Lib.insert(std::move(node)); PES.curPageID = newId;
					}
				}
			}
			catch (...) {} PES.editingId = false;
		}
		if (PES.editingName) PES.editingName = false;
		if (PES.curDiceIdx >= 0 && PES.curDiceIdx < (int)pg.dice.size()) {
			auto& bf = pg.dice[PES.curDiceIdx].on_hit_buffs;
			if (!bf.empty() && bf[0].intensity > 0) bf[0].intensity--;
		}
		break;
	case EA_DICE_BUFF_INTENSITY_INC:
		if (PES.editingId) {
			try {
				int newId = std::stoi(PES.idEditStr);
				if (newId > 0 && G_Page_Lib.find(newId) == G_Page_Lib.end()) {
					auto node = G_Page_Lib.extract(PES.curPageID);
					if (!node.empty()) {
						node.key() = newId; node.mapped().id = newId;
						G_Page_Lib.insert(std::move(node)); PES.curPageID = newId;
					}
				}
			}
			catch (...) {} PES.editingId = false;
		}
		if (PES.editingName) PES.editingName = false;
		if (PES.curDiceIdx >= 0 && PES.curDiceIdx < (int)pg.dice.size()) {
			auto& bf = pg.dice[PES.curDiceIdx].on_hit_buffs;
			if (!bf.empty()) {
				int maxv = G_BuffTypes.at(bf[0].buff_id).max_intensity;
				if (bf[0].intensity < maxv) bf[0].intensity++;
			}
		}
		break;
	case EA_DICE_BUFF_STACKS_DEC:
		if (PES.editingId) {
			try {
				int newId = std::stoi(PES.idEditStr);
				if (newId > 0 && G_Page_Lib.find(newId) == G_Page_Lib.end()) {
					auto node = G_Page_Lib.extract(PES.curPageID);
					if (!node.empty()) {
						node.key() = newId; node.mapped().id = newId;
						G_Page_Lib.insert(std::move(node)); PES.curPageID = newId;
					}
				}
			}
			catch (...) {} PES.editingId = false;
		}
		if (PES.editingName) PES.editingName = false;
		if (PES.curDiceIdx >= 0 && PES.curDiceIdx < (int)pg.dice.size()) {
			auto& bf = pg.dice[PES.curDiceIdx].on_hit_buffs;
			if (!bf.empty() && bf[0].stacks > 0) bf[0].stacks--;
		}
		break;
	case EA_DICE_BUFF_STACKS_INC:
		if (PES.editingId) {
			try {
				int newId = std::stoi(PES.idEditStr);
				if (newId > 0 && G_Page_Lib.find(newId) == G_Page_Lib.end()) {
					auto node = G_Page_Lib.extract(PES.curPageID);
					if (!node.empty()) {
						node.key() = newId; node.mapped().id = newId;
						G_Page_Lib.insert(std::move(node)); PES.curPageID = newId;
					}
				}
			}
			catch (...) {} PES.editingId = false;
		}
		if (PES.editingName) PES.editingName = false;
		if (PES.curDiceIdx >= 0 && PES.curDiceIdx < (int)pg.dice.size()) {
			auto& bf = pg.dice[PES.curDiceIdx].on_hit_buffs;
			if (!bf.empty()) {
				int maxv = G_BuffTypes.at(bf[0].buff_id).max_stacks;
				if (bf[0].stacks < maxv) bf[0].stacks++;
			}
		}
		break;
	case EA_DICE_ADD_BUFF:
		if (PES.editingId) {
			try {
				int newId = std::stoi(PES.idEditStr);
				if (newId > 0 && G_Page_Lib.find(newId) == G_Page_Lib.end()) {
					auto node = G_Page_Lib.extract(PES.curPageID);
					if (!node.empty()) {
						node.key() = newId; node.mapped().id = newId;
						G_Page_Lib.insert(std::move(node)); PES.curPageID = newId;
					}
				}
			}
			catch (...) {} PES.editingId = false;
		}
		if (PES.editingName) PES.editingName = false;
		if (PES.curDiceIdx >= 0 && PES.curDiceIdx < (int)pg.dice.size()) {
			auto& bf = pg.dice[PES.curDiceIdx].on_hit_buffs;
			if (bf.empty()) bf.push_back({ 1, 0, 0, TARGET_SELF });  // 默认力量，目标自己
		}
		break;
	case EA_DICE_DEL_BUFF:
		if (PES.editingId) {
			try {
				int newId = std::stoi(PES.idEditStr);
				if (newId > 0 && G_Page_Lib.find(newId) == G_Page_Lib.end()) {
					auto node = G_Page_Lib.extract(PES.curPageID);
					if (!node.empty()) {
						node.key() = newId; node.mapped().id = newId;
						G_Page_Lib.insert(std::move(node)); PES.curPageID = newId;
					}
				}
			}
			catch (...) {} PES.editingId = false;
		}
		if (PES.editingName) PES.editingName = false;
		if (PES.curDiceIdx >= 0 && PES.curDiceIdx < (int)pg.dice.size()) {
			auto& bf = pg.dice[PES.curDiceIdx].on_hit_buffs;
			if (!bf.empty()) bf.clear();
		}
		break;
	case EA_DICE_COUNTER:
		if (PES.editingId) {
			try {
				int newId = std::stoi(PES.idEditStr);
				if (newId > 0 && G_Page_Lib.find(newId) == G_Page_Lib.end()) {
					auto node = G_Page_Lib.extract(PES.curPageID);
					if (!node.empty()) {
						node.key() = newId; node.mapped().id = newId;
						G_Page_Lib.insert(std::move(node)); PES.curPageID = newId;
					}
				}
			}
			catch (...) {} PES.editingId = false;
		}
		if (PES.editingName) PES.editingName = false;
		if (PES.curDiceIdx >= 0 && PES.curDiceIdx < (int)pg.dice.size()) {
			pg.dice[PES.curDiceIdx].Counter = !pg.dice[PES.curDiceIdx].Counter;
		}
		break;
	case EA_ADD_DICE:
		if (PES.editingId) {
			try {
				int newId = std::stoi(PES.idEditStr);
				if (newId > 0 && G_Page_Lib.find(newId) == G_Page_Lib.end()) {
					auto node = G_Page_Lib.extract(PES.curPageID);
					if (!node.empty()) {
						node.key() = newId; node.mapped().id = newId;
						G_Page_Lib.insert(std::move(node)); PES.curPageID = newId;
					}
				}
			}
			catch (...) {} PES.editingId = false;
		}
		if (PES.editingName) PES.editingName = false;
		pg.dice.push_back({ Attack, 1, 6, Slash_Damage, {}, false, true });
		break;
	case EA_DEL_DICE:
		if (PES.editingId) {
			try {
				int newId = std::stoi(PES.idEditStr);
				if (newId > 0 && G_Page_Lib.find(newId) == G_Page_Lib.end()) {
					auto node = G_Page_Lib.extract(PES.curPageID);
					if (!node.empty()) {
						node.key() = newId; node.mapped().id = newId;
						G_Page_Lib.insert(std::move(node)); PES.curPageID = newId;
					}
				}
			}
			catch (...) {} PES.editingId = false;
		}
		if (PES.editingName) PES.editingName = false;
		if (PES.curDiceIdx >= 0 && PES.curDiceIdx < (int)pg.dice.size()) {
			pg.dice.erase(pg.dice.begin() + PES.curDiceIdx);
			PES.curDiceIdx = -1;
		}
		break;
	case EA_SAVE:
		if (PES.editingId) {
			try {
				int newId = std::stoi(PES.idEditStr);
				if (newId > 0 && G_Page_Lib.find(newId) == G_Page_Lib.end()) {
					auto node = G_Page_Lib.extract(PES.curPageID);
					if (!node.empty()) {
						node.key() = newId; node.mapped().id = newId;
						G_Page_Lib.insert(std::move(node)); PES.curPageID = newId;
					}
				}
			}
			catch (...) {} PES.editingId = false;
		}
		if (PES.editingName) PES.editingName = false;
		Push_Feedback(Save_PageLib_To_CSV());
		break;
	case EA_DEL_PAGE:
		if (PES.editingId) PES.editingId = false;
		if (PES.editingName) PES.editingName = false;
		G_Page_Lib.erase(PES.curPageID);
		if (!G_Page_Lib.empty()) {
			PES.curPageID = G_Page_Lib.begin()->first;
			PES.curDiceIdx = -1;
		}
		break;
	case EA_NONE:
		// 点击空白区域：仅确认编辑
		if (PES.editingId) {
			try {
				int newId = std::stoi(PES.idEditStr);
				if (newId > 0 && G_Page_Lib.find(newId) == G_Page_Lib.end()) {
					auto node = G_Page_Lib.extract(PES.curPageID);
					if (!node.empty()) {
						node.key() = newId;
						node.mapped().id = newId;
						G_Page_Lib.insert(std::move(node));
						PES.curPageID = newId;
					}
				}
			}
			catch (...) {}
			PES.editingId = false;
		}
		if (PES.editingName) PES.editingName = false;
		break;
	default:
		if (PES.editingId)
		{
			try {
				int newId = std::stoi(PES.idEditStr);
				if (newId > 0 && G_Page_Lib.find(newId) == G_Page_Lib.end())
				{
					auto node = G_Page_Lib.extract(PES.curPageID);
					if (!node.empty())
					{
						node.key() = newId;
						node.mapped().id = newId;
						G_Page_Lib.insert(std::move(node));
						PES.curPageID = newId;
					}
				}
			}
			catch (...) {
				wcerr << L"MAN" << endl;
			}
			PES.editingId = false;
		}
		if (PES.editingName)PES.editingName = false;
		break;
	}

}
void ProcessEditorText(uint32_t unicode)
{
	if (unicode == 8)//退格
	{
		if (PES.editingName && !G_Page_Lib[PES.curPageID].name.empty())
		{
			G_Page_Lib[PES.curPageID].name.pop_back();
		}
		if (PES.editingId && !PES.idEditStr.empty())
		{
			PES.idEditStr.pop_back();
		}
		PES.cursorVisible = true;
		PES.cursorTimer = 0.f;
		return;
	}
	if (unicode == 13)//回车
	{
		if (PES.editingId)
		{
			try {
				int newId = std::stoi(PES.idEditStr);
				if (newId > 0 && G_Page_Lib.find(newId) == G_Page_Lib.end())
				{
					auto node = G_Page_Lib.extract(PES.curPageID);
					if (!node.empty())
					{
						node.key() = newId;
						node.mapped().id = newId;
						G_Page_Lib.insert(std::move(node));
						PES.curPageID = newId;
					}
				}
			}catch(...){}
			PES.editingId = false;
		}
		if (PES.editingName)PES.editingName = false;
		return;
	}
	// Escape ASCII 27
	if (unicode == 27) {
		if (PES.editingId) {
			PES.editingId = false;
			// idEditStr 丢弃，不做任何修改
		}
		if (PES.editingName) {
			PES.editingName = false;
			// pg.name 保持编辑前的值（因为我们直接修改 pg.name，
			// 如果要支持取消恢复，需要额外保存旧值。简化处理：不恢复）
		}
		return;
	}

	// 普通字符输入
	// 过滤控制字符（只接受可打印字符）
	if (unicode < 32) return;  // 跳过其他控制字符

	if (PES.editingName) {
		// 名称：限制长度 20 字符
		auto& name = G_Page_Lib[PES.curPageID].name;
		if (name.size() < 20) {
			name.push_back((wchar_t)unicode);
		}
	}
	if (PES.editingId) {
		// 编号：只接受数字 0-9，限制长度 5 位
		if (unicode >= L'0' && unicode <= L'9') {
			if (PES.idEditStr.size() < 5) {
				PES.idEditStr.push_back((wchar_t)unicode);
			}
		}
	}

	// 每次输入后重置光标为可见
	PES.cursorVisible = true;
	PES.cursorTimer = 0.f;
}

// ===== 点击反馈：动作名 =====
static const wchar_t* Action_Name(int action)
{
	switch (action)
	{
	case EA_NONE:                    return L"空白";
	case EA_PICK_PAGE:               return L"选择书页";
	case EA_NEW_PAGE:                return L"新建书页";
	case EA_PICK_DICE:               return L"选择骰子";
	case EA_COST_DEC:                return L"费用 -";
	case EA_COST_INC:                return L"费用 +";
	case EA_NAME_BOX:                return L"编辑名称";
	case EA_ID_BOX:                  return L"编辑编号";
	case EA_COUNTER_TOGGLE:          return L"书页反击开关";
	case EA_DICE_TYPE:               return L"骰子类型";
	case EA_DMIN_DEC:                return L"最小值 -";
	case EA_DMIN_INC:                return L"最小值 +";
	case EA_DMAX_DEC:                return L"最大值 -";
	case EA_DMAX_INC:                return L"最大值 +";
	case EA_DMG_TYPE:                return L"伤害类型";
	case EA_ADD_DICE:                return L"添加骰子";
	case EA_DEL_DICE:                return L"删除骰子";
	case EA_DICE_BUFF:               return L"Buff 类型";
	case EA_DICE_BUFF_TARGET:        return L"Buff 目标";
	case EA_DICE_BUFF_INTENSITY_DEC: return L"Buff 强度 -";
	case EA_DICE_BUFF_INTENSITY_INC: return L"Buff 强度 +";
	case EA_DICE_BUFF_STACKS_DEC:    return L"Buff 层数 -";
	case EA_DICE_BUFF_STACKS_INC:    return L"Buff 层数 +";
	case EA_DICE_ADD_BUFF:           return L"添加 Buff";
	case EA_DICE_DEL_BUFF:           return L"删除 Buff";
	case EA_DICE_COUNTER:            return L"骰子反击开关";
	case EA_SAVE:                    return L"保存";
	case EA_DEL_PAGE:                return L"删除书页";
	default:                         return L"未定义动作";
	}
}

std::wstring Describe_Editor_Action(int action)
{
	std::wstring s = Action_Name(action);

	// 用 find 而不是 operator[]：找不到时不能凭空插一张空书页进去
	auto it = G_Page_Lib.find(PES.curPageID);
	if (it == G_Page_Lib.end())
	{
		s += L"    （当前没有选中任何书页）";
		return s;
	}
	const PageTemplate& pg = it->second;

	s += L"    书页 ";
	s += std::to_wstring(pg.id);
	s += L"「";
	s += pg.name;
	s += L"」  费用 ";
	s += std::to_wstring(pg.cost);
	s += L"  骰子 ";
	s += std::to_wstring(pg.dice.size());
	s += L" 颗  反击 ";
	s += (pg.Counter ? L"开" : L"关");

	if (PES.curDiceIdx >= 0 && PES.curDiceIdx < (int)pg.dice.size())
	{
		const Basic_Dice& d = pg.dice[PES.curDiceIdx];
		s += L"  |  骰子 #";
		s += std::to_wstring(PES.curDiceIdx + 1);
		s += L" ";
		s += DiceTypeName(d.type);
		s += L" ";
		s += std::to_wstring(d.min);
		s += L"-";
		s += std::to_wstring(d.max);
		if (d.type == Attack)
		{
			s += L" ";
			s += DmgName(d.damage_type);
		}
		s += L"  Buff ";
		s += std::to_wstring(d.on_hit_buffs.size());
		s += L" 条";
	}
	return s;
}