# include <Siv3D.hpp>
# include <HamFramework.hpp>
# include <map>
# include <algorithm>

//各画面で共有するデータ
//m_data->IDなどでアクセス
struct CommonData
{
	String parentfolder = L"";
	Array<FilePath> filelist;
	String labeler = L"";
	int imgLength;
};
using MyApp = SceneManager<String, CommonData>;


class FolderSelection : public MyApp::Scene
{
public:
	Array<FilePath> filelist;
	bool isSelected = false;

	void init() override {
		Window::SetTitle(L"FolderSelection");
		System::SetExitEvent(WindowEvent::CloseButton);
	}

	void update() override
	{
		const Rect directory_select_bottun(40, 40, 250, 30); 
		directory_select_bottun.draw(Color(28, 200, 186));
		font(L"フォルダー選択(Click Here)").draw(directory_select_bottun.pos.movedBy(24, 8), Palette::Black);
		if (directory_select_bottun.leftClicked) {
			if (const auto folder = Dialog::GetFolder())
			{
				m_data->filelist = {};
				m_data->parentfolder = folder.value();
				filelist = FileSystem::DirectoryContents(folder.value());
			}
			for (const auto& path : filelist)
			{
				if (FileSystem::Extension(path) == L"jpg")
				{
					m_data->filelist.push_back(path);
				}
				
			}
			std::sort(m_data->filelist.begin(), m_data->filelist.end());
			m_data->imgLength = m_data->filelist.size();
		}
		if (m_data->imgLength == 0)
		{
			font(L"No images in the folder").draw();
		}
		const Rect directory_parant(0, 80, 700, 30);
		directory_parant.draw(Color(28, 200, 186));
		font(m_data->parentfolder).draw(directory_parant.pos.movedBy(24, 8), Palette::Black);

		const Rect labeler_bottun(0, 150, 700, 30);
		labeler_bottun.draw(Color(28, 200, 186));
		Input::GetCharsHelper(m_data->labeler);
		font(L"labelar:" + m_data->labeler).draw(labeler_bottun.pos.movedBy(24, 8), Palette::Black);


		const Rect Login_button(300, 400, 300, 60);
		Login_button.draw(Color(28, 200, 186));
		font(L"Start Labeling").draw(Login_button.pos.movedBy(24, 8), Palette::Black);

		if (Login_button.leftClicked | Input::KeyEnter.clicked) {
			if (m_data->imgLength != 0 && m_data->labeler!=L"") {
				changeScene(L"Labeling");
			}
		}
	}
	void draw() const override {
	}

	Font font{ 10 };
};

class Labeling : public MyApp::Scene
{
public:
	DynamicTexture texture;
	int index = 0;
	String filename = L"";
	std::map<FilePath,int> flag;
	FilePath flagpath;

	void init() override {
		Window::Resize(580, 700);
		Window::SetStyle(WindowStyle::Sizeable);
		flagpath = m_data->parentfolder + L"/continuouslabel_by_" + m_data->labeler.replace(L"\n",L"") + L".csv";
		CSVReader csvreader(flagpath);

		
		for (int i = 0; i < m_data->imgLength; i++) {
			String tmp_path = m_data->filelist[i].replace(m_data->parentfolder, L"").replace(L".jpg",L"");
			flag[tmp_path] = -1;
		}

		for (int i = 0; i < csvreader.rows; i++) {
			flag[csvreader.get<String>(i, 0)] = csvreader.get<int32>(i, 1);
		}
		csvreader.close();

		chengeImg(m_data->filelist[index]);
	}

	void chengeImg(FilePath imgPath) {
		Image image(imgPath); 
		texture.fill(image);
		filename = imgPath.replace(m_data->parentfolder, L"").replace(L".jpg",L"");
	}

	void update() override
	{
		font(index+1,L"/",m_data->imgLength,L"\n", filename).draw();

		texture.resize(640 * 0.8, 480 * 0.8).draw(30, 40);

		const Rect Engaged_Button(10, 450, 200, 60);
		Engaged_Button.draw(Color(255, 255, 127));
		font(L"Engaged:Key1").draw(Engaged_Button.pos.movedBy(24, 8), Palette::Black);
		if (Engaged_Button.leftClicked | Input::Key1.pressed) {
			flag[filename] = 1;
			if (index < m_data->imgLength - 1) {
				index++;
				chengeImg(m_data->filelist[index]);
			}
		}

		const Rect Not_Engaged_Button(300, 450, 200, 60);
		Not_Engaged_Button.draw(Color(127, 255, 255));
		font(L"Not Engaged:Key0").draw(Not_Engaged_Button.pos.movedBy(24, 8), Palette::Black);
		if (Not_Engaged_Button.leftClicked | Input::Key0.pressed) {
			flag[filename] = 0;
			if (index < m_data->imgLength -1 ) {
				index++;
				chengeImg(m_data->filelist[index]);
			}
		}

		const Rect Previous_Button(10, 530, 200, 60);
		Previous_Button.draw(Color(250, 240, 230));
		font(L"<- Previous frame").draw(Previous_Button.pos.movedBy(24, 8), Palette::Black);

		const Rect Next_Button(300, 530, 200, 60);
		Next_Button.draw(Color(250, 240, 230));
		font(L"Next frame ->").draw(Next_Button.pos.movedBy(24, 8), Palette::Black);


		const Rect State_Button(10, 610, 200, 60);
		if (flag[filename] == 0) {
			State_Button.draw(Color(127, 255, 255));
			font(L"Label:Not Engaged").draw(State_Button.pos.movedBy(24, 8), Palette::Black);
		}
		else if (flag[filename] == 1) {
			State_Button.draw(Color(255, 255, 127));
			font(L"Label:Engaged").draw(State_Button.pos.movedBy(24, 8), Palette::Black);
		}
		else {
			State_Button.draw(Color(246, 54, 127));
			font(L"No Label").draw(State_Button.pos.movedBy(24, 8), Palette::Black);
		}

		const Rect Back_Button(300, 610, 200, 60);
		Back_Button.draw(Color(246, 54, 127));
		font(L"Exit").draw(Back_Button.pos.movedBy(24, 8), Palette::Black);
		if (Back_Button.leftClicked | Input::KeyEscape.pressed) {
			FileSystem::Remove(flagpath);
			CSVWriter writer(flagpath);
			for (int i = 0; i < m_data->imgLength; i++) {
				String tmp_path = m_data->filelist[i].replace(m_data->parentfolder, L"").replace(L".jpg", L"");
				writer.write(tmp_path);
				writer.write(flag[tmp_path]);
				writer.nextLine();
			}
			writer.close();
			changeScene(L"FolderSelection");
		}

		if (Input::KeyRight.pressed | Next_Button.leftClicked) {
			if (index < m_data->imgLength - 1) {
				index++;
				chengeImg(m_data->filelist[index]);
			}
		}
		if (Input::KeyLeft.pressed | Previous_Button.leftClicked) {
			if (index > 0) {
				index--;
				chengeImg(m_data->filelist[index]);
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
