
// Diablo Edit2.cpp: 定义应用程序的类行为。
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "Diablo Edit2.h"
#include "MainFrm.h"

#include "Diablo Edit2Doc.h"
#include "Diablo Edit2View.h"

#include "../Generate Data/compress_quicklz.h"

#include <string>
#include <fstream>
#include <sstream>
#include <streambuf>
#include <cstdarg>
#include <cctype>
#include <numeric>

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CString CSFormat(LPCTSTR lpszFormat, ...) {
	CString ret;
	va_list argList;
	va_start(argList, lpszFormat);
	ret.FormatV(lpszFormat, argList);
	va_end(argList);
	return ret;
}


//class CProperty

CProperty::CProperty(vector<pair<int, int>> fields)
	: fields_(fields)
{
	bitsSum_ = accumulate(fields_.begin(), fields_.end(), 0, [](auto s, auto f) { return s + f.first; });
}

vector<int> CProperty::Parse(DWORD value) const {
	vector<int> ret;
	for (auto & f : fields_) {
		ret.push_back((value & ((DWORD(1) << f.first) - 1)) + f.second);
		value >>= f.first;
	}
	return ret;
}

// CDiabloEdit2App

BEGIN_MESSAGE_MAP(CDiabloEdit2App, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CDiabloEdit2App::OnAppAbout)
	// 基于文件的标准文档命令
//	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
//	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
END_MESSAGE_MAP()

const int CDiabloEdit2App::CLASS_SKILL_INDEX[CLASS_NAME_SIZE][CLASS_SKILL_NAME_SIZE] = {
	{0,1,10,11,20,2,3,12,21,22,4,13,14,23,24,5,6,15,25,26,7,8,16,17,27,9,18,19,28,29},	//Amazon
	{0,1,10,20,21,2,11,12,22,23,3,4,13,14,24,5,6,15,16,25,7,17,18,26,27,8,9,19,28,29},	//Sorceress
	{0,10,11,20,21,1,2,12,13,22,3,4,14,23,24,5,6,15,16,25,7,8,17,26,27,9,18,19,28,29},	//Necromancer
	{0,1,10,20,21,2,11,12,22,23,3,4,13,24,25,5,6,14,15,26,7,8,16,17,27,9,18,19,28,29},	//Paladin
	{0,10,11,12,20,21,1,2,13,14,15,22,23,3,4,16,24,5,6,17,25,7,18,26,27,8,9,19,28,29},	//Barbarian
	{0,1,10,11,20,2,3,12,21,22,4,13,14,23,24,5,6,15,16,25,7,17,18,26,27,8,9,19,28,29},	//Druid
	{0,10,11,20,21,1,2,12,22,23,3,4,13,14,24,5,15,16,25,26,6,7,17,27,28,8,9,18,19,29}	//Assassin
};


// CDiabloEdit2App 构造

CDiabloEdit2App::CDiabloEdit2App() noexcept
{
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// 如果应用程序是利用公共语言运行时支持(/clr)构建的，则: 
	//     1) 必须有此附加设置，“重新启动管理器”支持才能正常工作。
	//     2) 在您的项目中，您必须按照生成顺序向 System.Windows.Forms 添加引用。
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: 将以下应用程序 ID 字符串替换为唯一的 ID 字符串；建议的字符串格式
	//为 CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("DiabloEdit2.AppID.NoVersion"));

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}

// 唯一的 CDiabloEdit2App 对象

CDiabloEdit2App theApp;

static PTCHAR ExtractFilePath(PTCHAR pszFilePathName){
	PTCHAR cur = pszFilePathName,last = 0;
	for(;*cur != 0;++cur)
		if(*cur == _T('\\') || *cur == _T('/') || *cur == _T(':'))
			last = cur;
	if(last)
		*++last = 0;
	return pszFilePathName;
}

// CDiabloEdit2App 初始化

BOOL CDiabloEdit2App::InitInstance()
{
	if (!ReadLangRes()) {
		::MessageBox(0, _T("Load language resource file failed."), _T("Error"), MB_OK | MB_ICONERROR);
		return FALSE;
	}
	if (!ReadItemRes()) {
		::MessageBox(0, _T("Load item resource file failed."), _T("Error"), MB_OK | MB_ICONERROR);
		return FALSE;
	}
	if (!ReadPropRes()) {
		::MessageBox(0, _T("Load property resource file failed."), _T("Error"), MB_OK | MB_ICONERROR);
		return FALSE;
	}
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。  否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	// 初始化 OLE 库
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// 使用 RichEdit 控件需要 AfxInitRichEdit2()
	// AfxInitRichEdit2();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));
	LoadStdProfileSettings(4);  // 加载标准 INI 文件选项(包括 MRU)


	// 注册应用程序的文档模板。  文档模板
	// 将用作文档、框架窗口和视图之间的连接
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CDiabloEdit2Doc),
		RUNTIME_CLASS(CMainFrame),       // 主 SDI 框架窗口
		RUNTIME_CLASS(CDiabloEdit2View));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);


	// 分析标准 shell 命令、DDE、打开文件操作的命令行
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);



	// 调度在命令行中指定的命令。  如果
	// 用 /RegServer、/Register、/Unregserver 或 /Unregister 启动应用程序，则返回 FALSE。
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// 唯一的一个窗口已初始化，因此显示它并对其进行更新
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// 仅当具有后缀时才调用 DragAcceptFiles
	//  在 SDI 应用程序中，这应在 ProcessShellCommand  之后发生

	//得到程序的运行目录路径
	TCHAR path[1024];
	::GetModuleFileName(0,path,1024);
	m_sAppPath = ExtractFilePath(path);

	return TRUE;
}

int CDiabloEdit2App::ExitInstance() {
	//TODO: 处理可能已添加的附加资源
	AfxOleTerm(FALSE);

	return CWinApp::ExitInstance();
}

template<typename T,class Array>
static inline T & Interpret(Array & p,int i){
	return *reinterpret_cast<T *>(&p[i]);
}

static string readDataFile(const char * input, const char magic[4]) {
	ASSERT(input && magic);
	// Read input file
	ifstream inf(input, ios_base::binary);
	if (!inf.is_open())
		return FALSE;
	string in_buf;
	in_buf.assign(istreambuf_iterator<char>(inf), istreambuf_iterator<char>());
	inf.close();
	// Decompress data
	string out_buf;
	if (!CCompressorQuickLZ().decompress(in_buf, out_buf))
		return FALSE;
	// Check data
	if (out_buf.size() < 5)
		return FALSE;
	if (out_buf.substr(1, 4) != magic)
		return FALSE;
	return out_buf;
}

template<typename T>
static bool parse(istream & is, T & value) {
	if (!is.good())
		return false;
	string s;
	getline(is, s, '\t');
	value = (s.empty() ? 0 : stoi(s));
	return !is.bad();
}

static bool parse(istream & is, string & str) {
	if (is.good())
		getline(is, str, '\t');
	return !is.bad();
}

static bool parse(istream & is, vector<string> & strs) {
	if (is.good())
		for (string s; getline(is, s, '\t'); strs.push_back(s));
	return !is.bad();
}

//Escape "\\" -> '\' and "\n" -> NewLine.
static string escape(string str) {
	string ret;
	size_t pp = 0;
	for (size_t p = 0;;) {
		auto i = str.find_first_of('\\', p);
		if (i == string::npos)
			break;
		p = i + 1;
		if (p < str.size()) {
			const char c = str[p], n = (c == 'n' ? '\n' : (c == '\\' ? '\\' : 0));
			if (n) {
				ret.append(str.substr(pp, i - pp));
				ret.push_back(n);
				pp = ++p;
			}
		}
	}
	ret.append(str.substr(pp));
	return ret;
}

static void compare(const vector<CString> & expected, const vector<CString> & lang) {
	for (size_t i = 0, j = 0; i < lang.size() || j < expected.size();) {
		if (expected[j].IsEmpty())
			++j;
		else if (lang[i] != expected[j] && i != 371)
			throw 0;
		else
			++i, ++j;
	}
}

static string trimLeft(const string & str) {
	size_t i = 0;
	for (; i < str.length() && isspace(str[i]); ++i);
	return (i ? str.substr(i) : str);
}

BOOL CDiabloEdit2App::ReadLangRes(void) {
	// Read input file
	ifstream inf("lang.txt");
	if (!inf.is_open())
		return FALSE;
	// Deserialize data
	auto & iss = inf;
	decltype(m_saLanguage) langs;
	decltype(m_aLangBases) bases;
	int base = 0;
	for (string line; getline(iss, line);) {
		line = trimLeft(line);
		if (line.empty())	//空行
			continue;
		else if (line[0] == '*') {	// New section
			base = (langs.empty() ? 0 : langs[0].size());
			if(base)
				bases.push_back(base);
			continue;
		}
 		istringstream ls(line);
		unsigned int idx;
		vector<string> msgs;
		parse(ls, idx) && parse(ls, msgs);
		idx += base;
		if (msgs.size() > langs.size())
			langs.resize(msgs.size());
		for (size_t i = 0; i < msgs.size(); ++i) {
			auto & lang = langs[i];
			auto & msg = msgs[i];
			// Microsoft Excel adds "" to msg sometimes.
			if (msg.size() > 2 && msg[0] == '"' && *msg.rbegin() == '"')
				msg = msg.substr(1, msg.size() - 2);
			msg = escape(msg);
			if (idx < lang.size()) {
				if (!lang[idx].IsEmpty())
					throw 0;
				lang[idx] = msg.c_str();
			} else {
				if (idx > lang.size())
					lang.resize(idx);
				lang.emplace_back(msg.c_str());
			}
		}
	}
	if(!langs.empty())
		bases.push_back(langs[0].size());
	//Decorate
	for (size_t i = 1; i < langs.size(); ++i) {
		auto & lang = langs[i];
		const auto & prev = langs[i - 1];
		if (lang.size() < prev.size())
			lang.resize(prev.size());
		ASSERT(lang.size() == prev.size());
		for (size_t j = 0; j < lang.size(); ++j)
			if (lang[j].IsEmpty())
				lang[j] = prev[j];
	}
	reverse(bases.begin(), bases.end());
	bases.push_back(0);		//OTHER_MSG
	langs.swap(m_saLanguage);
	bases.swap(m_aLangBases);
	return TRUE;
}

BOOL CDiabloEdit2App::ReadItemRes(void) {
	// Read input file
	string out_buf = readDataFile("itemdata.dat", "ITEM");
	if (out_buf.empty())
		return FALSE;
	// Deserialize data
	istringstream iss(out_buf);
	int picOff = 0;
	decltype(m_mapItemData) itemMap;
	for (string line; getline(iss, line);) {
		line = trimLeft(line);
		if (line.empty() || line[0] == '*')	//空行或注释
			continue;
		istringstream ls(line);
		string id;
		int pic = 0, range = 0;
		CItemDataStruct item;
		parse(ls, id)
			&& parse(ls, pic)
			&& parse(ls, item.NameIndex)
			&& parse(ls, range)
			&& parse(ls, item.Equip)
			&& parse(ls, item.HasDef)
			&& parse(ls, item.HasDur)
			&& parse(ls, item.IsStacked)
			&& parse(ls, item.IsTome)
			&& parse(ls, item.HasMonsterID)
			&& parse(ls, item.IsCharm)
			&& parse(ls, item.HasSpellID)
			&& parse(ls, item.IsUnique)
			&& parse(ls, item.Damage1Min)
			&& parse(ls, item.Damage1Max)
			&& parse(ls, item.Damage2Min)
			&& parse(ls, item.Damage2Max);
		// ID
		if (id.size() != 3)
			return FALSE;
		id.resize(4, ' ');
		const DWORD key = *reinterpret_cast<const DWORD *>(&id[0]);

		item.name = id;
		// Pic
		if (pic < 0)
			picOff += pic;
		item.PicIndex = (pic > 0 ? pic : WORD(itemMap.size() + picOff));
		// Name
		if (!item.NameIndex)
			item.NameIndex = WORD(itemMap.size());
		// Range
		item.Range = (range ? ((range / 10) << 4) + range % 10 : 0x11);
		itemMap[key] = item;
	}
	itemMap.swap(m_mapItemData);
	return TRUE;
}

BOOL CDiabloEdit2App::ReadPropRes() {
	// Read input file
	string out_buf = readDataFile("property.dat", "PROP");
	if (out_buf.empty())
		return FALSE;
	// Deserialize data
	istringstream iss(out_buf);
	decltype(m_vProperty) props;
	for (string line; getline(iss, line);) {
		line = trimLeft(line);
		if (line.empty() || line[0] == '*')	//空行或注释
			continue;
		istringstream ls(line);
		int id = -1;
		vector<pair<int, int>> fields;
		parse(ls, id);
		if (id < 0)
			return FALSE;
		for (int b1 = 0, b2 = 0; parse(ls, b1) && parse(ls, b2); fields.emplace_back(b1, b2));
		if (id >= int(props.size()))
			props.resize(id + 1);
		props[id] = CProperty(fields);
	}
	props.swap(m_vProperty);
	return TRUE;
}

CString CDiabloEdit2App::PorpertyDescription(WORD id, DWORD value) const {
	const auto & prop = PropertyData(id);
	const auto desc = PropertyName(id);
	auto & a = prop.Parse(value);
	ASSERT(3 < a.size());
	switch (id) {
		case 54:a[2] /= 25; break;
		case 56:case 59:a[0] /= 25; break;
		case 57:a[0] = (a[0] * a[2]) >> 8; a[1] = (a[1] * a[2]) >> 8; a[2] /= 25; break;
		case 83:return CSFormat(desc, a[1], ClassName(a[0]));
		case 97:case 151:return CSFormat(desc, a[1], ClassSkillName(a[0]).first);
		case 98:return CSFormat(desc, (1 <= a[0] && a[0] <= 5 ? Msg(4 + a[0]) : Msg(10)));
		case 107:{
			const auto p = ClassSkillName(a[0]);
			return CSFormat(desc, a[1], p.first, ClassName(p.second));
		}
		case 112:a[0] = a[0] * 100 / 127; break;
		case 126:swap(a[0], a[1]); break;
		case 155:case 179:case 180:return CSFormat(desc, a[1], MonsterName(a[0]));
		case 188:return CSFormat(desc, a[2], ClassSkillTabName(a[0], a[1]), ClassName(a[1]));
		case 195:case 196:case 197:case 198:case 199:case 201:
			return CSFormat(desc, a[2], a[0], ClassSkillName(a[1]).first);
		case 204:return CSFormat(desc, a[0], ClassSkillName(a[1]).first, a[2], a[3]);
		case 214:case 215:case 216:case 217:case 218:case 219:case 220:case 221:case 222:case 223:
		case 226:case 227:case 228:case 229:case 230:case 231:case 232:case 233:case 234:case 235:
		case 236:case 237:case 238:case 239:case 240:case 241:case 242:case 243:case 244:case 245:
		case 246:case 247:case 248:case 249:case 250:
			a[0] >>= 3; break;
		case 224:case 225:a[0] >>= 1; break;
		case 252:case 253:a[0] = 100 / a[0]; break;
		case 268:case 269:case 270:case 271:case 272:case 273:case 274:case 275:case 276:case 277:
		case 278:case 279:case 280:case 281:case 282:case 283:case 284:case 285:case 286:case 287:
		case 288:case 289:case 290:case 291:case 292:case 293:case 294:case 295:case 296:case 297:
		case 298:case 299:case 300:case 301:case 302:case 303:
			return CSFormat(desc, a[1], a[2], TimeName(a[1]));
		case 305:case 306:case 307:case 308:case 333:case 334:case 335:case 336:
			a[0] = -a[0]; break;
	}
	return CSFormat(desc, a[0], a[1], a[2], a[3]);
}

pair<CString, int> CDiabloEdit2App::ClassSkillName(UINT skill_id) const {
	if (skill_id < 6)
		return make_pair(NormalSkillName(skill_id), -1);
	else if (skill_id < 156) {
		skill_id -= 6;
		const int cid = skill_id / 30, index = CLASS_SKILL_INDEX[cid][skill_id % 30];
		return make_pair(ClassSkillName(index, cid), cid);
	} else if (217 <= skill_id && skill_id < 221)
		return make_pair(NormalSkillName(skill_id - 211), -1);
	else if (221 <= skill_id && skill_id < 281) {
		skill_id -= 221;
		const int cid = skill_id / 30 + 5, index = CLASS_SKILL_INDEX[cid][skill_id % 30];
		return make_pair(ClassSkillName(index, cid), cid);
	}
	return make_pair(NormalSkillName(10), -1);
}

// CDiabloEdit2App 消息处理程序


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg() noexcept;

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() noexcept : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// 用于运行对话框的应用程序命令
void CDiabloEdit2App::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CDiabloEdit2App 消息处理程序



