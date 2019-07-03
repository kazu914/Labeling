
# include <Siv3D.hpp>
# include <HamFramework.hpp>
# include <thread>
# include <Windows.h>
# include <tchar.h>
# include <string>
# include <fstream> 
# include <stdio.h>
# include <iostream>
# include <direct.h>
# include <time.h>
# include <iomanip> //時間を取得するため
# include <sstream> //値を簡単に文字列にするため
# include <chrono>
# include <future>
# include <conio.h>

std::string zeropadding(int i, int n) {
	std::ostringstream sout;
	sout << std::setfill('0') << std::setw(n) << i;
	return sout.str();
}

std::string get_time() {
	auto time = DateTime::Now();
	std::string str = std::to_string(time.year) + "_" + std::to_string(time.month) + "_" + zeropadding(time.day, 2) + "_" + zeropadding(time.hour, 2) + "_" + zeropadding(time.minute, 2) + "_" + zeropadding(time.second, 2) + "_" + zeropadding(time.milliseconds, 3);
	return str;
}

//各画面で共有するデータ
//m_data->IDなどでアクセス
struct CommonData
{
	int ID = 0;
	int name = 0;
	String sub;
	int sub_ID = -1;
	int chap = 0;
	int video_num = 0;
	int32 camera_id = 0;
	String message = L"";
	Array<FilePath> filelist;
};
using MyApp = SceneManager<String, CommonData>;


class FolderSelection : public MyApp::Scene
{
public:
	Array<FilePath> filelist;

	void init() override {
		Window::SetTitle(L"FolderSelection");
	}

	void update() override
	{
		const Rect directory_select_bottun(40, 40, 250, 30);
		font(L"ディレクトリ選択").draw(directory_select_bottun.pos.movedBy(24, 8), Palette::Black);
		if (directory_select_bottun.leftClicked) {
			if (const auto folder = Dialog::GetFolder())
			{
				m_data->filelist = FileSystem::DirectoryContents(folder.value());
			}
			/*for (const auto& path : filelist)
			{
				if (FileSystem::Extension(path) == L"jpg")
				{
					const Image image(path);
					const Texture texture(image);
					texture.draw();
					WaitKey();
				}
			}*/
		}

		const Rect Login_button(300, 400, 300, 60);
		Login_button.draw(Color(28, 200, 186));
		font(L"Start Labeling").draw(Login_button.pos.movedBy(24, 8), Palette::Black);
		font(m_data->message).draw(Login_button.pos.movedBy(24, 30), Palette::Black);



		if (Login_button.leftClicked | Input::KeyEnter.clicked) {
				changeScene(L"Labeling");
		}

	}
	void draw() const override {
	}

	Font font{ 10 };
};

class Labeling : public MyApp::Scene
{
public:
	void init() override {
		Window::Resize(640, 640);
		Window::SetStyle(WindowStyle::Sizeable);
		
	}

	void chengeImg() {
		//画像を変える処理
	}

	void update() override
	{
		const Rect Engaged_Button(10, 600, 200, 60);
		Engaged_Button.draw(Color(28, 200, 186));
		font(L"Engaged").draw(Engaged_Button.pos.movedBy(24, 8), Palette::Black);

		const Rect Not_Engaged_Button(300, 600, 200, 60);
		Not_Engaged_Button.draw(Color(28, 200, 186));
		font(L"Not Engaged").draw(Not_Engaged_Button.pos.movedBy(24, 8), Palette::Black);

		if (Engaged_Button.leftClicked | Input::Key1.clicked) {
			Println(L"Engaged");
		}
		if (Not_Engaged_Button.leftClicked | Input::Key0.clicked) {
			Println(L"Not Engaged");
		}
		
		for (const auto& path : m_data->filelist)
		{
			if (FileSystem::Extension(path) == L"jpg")
			{
				const Image image(path);
				const Texture texture(image);
				texture.resize(640*0.8, 480*0.8).draw(30, 30);
				font(Window::Width(), L'×', Window::Height()).draw();

				
				WaitKey();

			}
		}
	}
	Font font{ 10 };

};


void Main() {
	MyApp manager;

	manager.add<FolderSelection>(L"FolderSelection");
	manager.add<Labeling>(L"Labeling");

	while (System::Update()) {
		manager.updateAndDraw();
	}
}
