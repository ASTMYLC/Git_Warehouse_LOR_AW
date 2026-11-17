#include"Tool.h"
#include"GUI.h"
#pragma execution_character_set("utf-8")
using namespace std;
extern unordered_map<int, PageTemplate> G_Page_Lib;
PageEditState PES;

bool Gameloop = 0;


int main()
{
	SetConsoleOutputCP(CP_UTF8);
	wcout.imbue(std::locale(".UTF-8"));
	// ===== 载入书页库：CSV 是唯一数据源 =====
	std::wstring loadReport;
	if (!Load_PageLib_From_CSV(loadReport))
	{
		// 首次启动（还没有 CSV）：把 WareHouse 里硬编码的出厂预设写出去
		Save_PageLib_To_CSV();
	}
	wcout << loadReport << endl;
	//初始化
	ofstream CorePage_ptr;
	CorePage_ptr.open("Core_Page_temp.txt", ios_base::out);
	CorePage_ptr << "名称:Ayin" << "\n";
	CorePage_ptr << "当前Hp:120" << "\n" << "当前玛那:4(Max:9)" << "\n" << "玛那回复效率:2" << "\n" << "以太:0" << "\n" << "流系等级:0" << "\n" << "流系硬币:0(Max:3)" << "\n";
	CorePage_ptr.close();
	cout << "预设书页文件初始化完成" << endl;
	Core_Page Ayin(L"Ayin", 120, 3, 2, 0, 0, 0);
	Core_Page Carmen(L"Carmen", 120, 3, 2, 0, 0, 0);
	Ayin.ODeck.push_back(Combat_Page::Get(1001));
	Ayin.ODeck.push_back(Combat_Page::Get(1002));
	Ayin.ODeck.push_back(Combat_Page::Get(1001));
	Ayin.Deck.push_back(Combat_Page::Get(1002));
	Ayin.Deck.push_back(Combat_Page::Get(1003));
	Ayin.Deck.push_back(Combat_Page::Get(1003));
	Carmen.ODeck.push_back(Combat_Page::Get(1001));
	//队伍初始化
	vector<Core_Page> T1;
	vector<Core_Page> T2;
	T1.push_back(Ayin);
	T2.push_back(Carmen);

	//控制代码
	int GPS_1 = 0;

	while (!Gameloop)
	{
		GPS_1 = 0;
		cout << "尝试载入" << endl;
		cout << "展示预设角色:Ayin\n" << endl;
		Ayin.Display();
		Ayin.Flux_Coin_add(1);
		Ayin.Flux_Coin_add(1);
		Ayin.Flux_Coin_add(0);
		Ayin.Display();
		Ayin.End_of_the_Round();
		Ayin.Display();
		cout << "输入数字来选择功能" << endl;
		cout << "1.更改核心书页配置" << endl;
		cout << "2.选择战役" << endl;
		cout << "3.调整作战参数" << endl;
		cout << "4.退出程序" << endl;
		cin >> GPS_1;
		if (GPS_1 == 2)
		{
			cout << "拼点测试开始" << endl;
			bool endofline = true;
			for (size_t i=1,j=1; endofline; i++)
			{
				if (i > Ayin.ODeck[0].dice.size() && j > Carmen.ODeck[0].dice.size())
				{
					break;
				}
				else if (i <= Ayin.ODeck[0].dice.size() && j <= Carmen.ODeck[0].dice.size())
				{
					Main_Fight_Against(Ayin, Ayin.ODeck[0].dice[i-1], 7, Carmen, Carmen.ODeck[0].dice[j-1], 8);
				}
				else if (i>Ayin.ODeck[0].dice.size()&&j<=Carmen.ODeck[0].dice.size())
				{
					//等待后续接入
				}
				else if(i<=Ayin.ODeck[0].dice.size()&&j>Carmen.ODeck[0].dice.size())
				{
					//等待后续接入
				}
				if (Ayin.ODeck[0].dice[i-1].consume == true)
				{
					i++;
				}
				if (Carmen.ODeck[0].dice[j-1].consume == true)
				{
					j++;
				}
			}
			
		}
		else if (GPS_1 == 4)
		{
			Gameloop = 1;
			cout << "进程即将结束" << endl;
			system("pause");
			break;
		}

	}
	//GUI代码

	// 初始窗口给个 1080p 屏放得下的大小，内容会自动等比缩放；用户可以随意拉伸
	// 如果 sf::Style::Default 报错，换成 sf::Style::Titlebar | sf::Style::Resize | sf::Style::Close
	sf::RenderWindow window(sf::VideoMode({ 1280, 800 }), L"主菜单", sf::Style::Default);
	Sync_View(window);
	
	sf::Font font;
	bool fontOK = font.openFromFile("C:/Windows/Fonts/msyh.ttc");
	if (!fontOK) fontOK = font.openFromFile("C:/Windows/Fonts/msyh.ttf");
	if (!fontOK) fontOK = font.openFromFile("C:/Windows/Fonts/simhei.ttf");
	
	const sf::Time fixedStep = sf::seconds(1.f / 60.f);
	sf::Time accumulator = sf::Time::Zero;
	sf::Clock clock;
	float transitionAlpha = 0.f;

	MenuState MS_State_Temp = MS_Main;
	while (window.isOpen())
	{
		Sync_View(window);
		while (const std::optional event = window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
			{
				window.close();
			}//ESC快捷键(暂定，后续会优化)
			if (event->is<sf::Event::KeyPressed>())
			{
				if (event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Escape)
				{
					if (PES.editingName || PES.editingId)
					{
						PES.editingName = false;
						PES.editingId = false;
					}
					else
					{
						MS_State_Temp = MS_Main;
					}
				}
			}

			//文字输入部分
			if (event->is<sf::Event::TextEntered>())
			{
				if (MS_State_Temp == MS_CombatEdit && (PES.editingName || PES.editingId))
				{
					auto te = *event->getIf<sf::Event::TextEntered>();
					ProcessEditorText(te.unicode);
				}
			}

			//鼠标部分
			if (event->is<sf::Event::MouseButtonPressed>())
			{
				auto m = *event->getIf<sf::Event::MouseButtonPressed>();
				if (m.button != sf::Mouse::Button::Left)
				{
					continue;
				}
				//sf::Vector2f pos = { (float)m.position.x,(float)m.position.y };
				// m.position 是窗口像素坐标，必须换算成逻辑坐标才能和按钮矩形比较
				sf::Vector2f pos = window.mapPixelToCoords(m.position);
				if (MS_State_Temp == MS_Main)
				{
					for (size_t i = 0; i < G_Items.size(); i++)
					{
						if (ItemRect(i).contains(pos))
						{
							MS_State_Temp = G_Items[i].target;
							Push_Feedback(L"进入：" + G_Items[i].text, pos);
							break;
						}
					}
				}
				else if (MS_State_Temp == MS_CombatEdit)
				{
					int action = Editor_Hit(pos);
					if (action == EA_PICK_PAGE)
					{
						int row = 0;
						for (auto& kv : G_Page_Lib)
						{
							if (PageListItemRect(row).contains(pos))
							{
								PES.curPageID = kv.first;
								PES.curDiceIdx = -1;
								break;
							}
							row++;
						}
					}
					else if (action == EA_PICK_DICE)
					{
						// 如果已经在编辑某颗骰子，点击列表中的骰子 → 取消选中，回到列表
						if (PES.curDiceIdx >= 0)
						{
							PES.curDiceIdx = -1;
						}
						else
						{
							// 列表模式：遍历找到点击的是哪颗
							auto& pg = G_Page_Lib[PES.curPageID];
							for (size_t di = 0; di < pg.dice.size(); di++)
							{
								if (DiceListItemRect(di).contains(pos))
								{
									PES.curDiceIdx = (int)di;
									break;
								}
							}
						}
					}

					else
					{
						ProcessEditorClick(action);
					}
					// 处理完再取快照：显示的是"点击生效后"的数据
					Push_Feedback(Describe_Editor_Action(action), pos);
				}

			}
		}
		sf::Time frameTime = clock.restart();
		frameTime = std::min(frameTime, sf::seconds(0.25f));
		accumulator += frameTime;
		while (accumulator >= fixedStep)
		{
			Update(fixedStep, MS_State_Temp, transitionAlpha);
			accumulator -= fixedStep;
		}
		window.clear(sf::Color(25, 25, 30));
		switch (MS_State_Temp)
		{
		case MS_Main:
			Draw_Main(window, font,fontOK);
			break;
		case MS_CoreEdit:
			Draw_Placeholder(window, font, L"战斗书页编辑", fontOK);
			break;
		case MS_CombatEdit:
			Draw_PageEditor(window, font, fontOK);
			break;
		case MS_FieldSetup:
			Draw_Placeholder(window, font, L"场地设置",fontOK);
			break;
		case MS_TeamSetup:
			Draw_Placeholder(window, font, L"参战人员安排",fontOK);
			break;
		case MS_Todo:
			Draw_Placeholder(window, font, L"敬请期待",fontOK);
			break;
		case MS_Quit:
			window.close();
			break;
		}
		// 点击反馈画在所有界面之上（主菜单、编辑器、占位页都生效）
		Draw_Feedback(window, font, fontOK);
		window.display();
	}
}