// CDlgFoundry.cpp: 实现文件
//

#include "stdafx.h"
#include "Diablo Edit2.h"
#include "DlgFoundry.h"
#include "afxdialogex.h"

using namespace std;

static CString stringOf(const CEdit & edit) {
	CString s;
	edit.GetWindowText(s);
	return s;
}

static int intOf(const CString & str) { return _tstoi(str); }

static int intOf(const CEdit & edit) { return intOf(stringOf(edit)); }

// CDlgFoundry 对话框

IMPLEMENT_DYNAMIC(CDlgFoundry, CDialogEx)

CDlgFoundry::CDlgFoundry(CD2Item & item, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_Foundry, pParent)
	, m_stItem(item)
	, m_bUnidentified(FALSE), m_bEthereal(FALSE), m_sExtSockets(_T(""))
{}

CDlgFoundry::~CDlgFoundry() {}

void CDlgFoundry::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_LEVEL, m_sText[0]);
	DDX_Text(pDX, IDC_STATIC_OWNER, m_sText[1]);
	DDX_Text(pDX, IDC_STATIC_CLASS, m_sText[2]);
	DDX_Text(pDX, IDC_CHECK_UNIDENTIFIED, m_sText[3]);
	DDX_Text(pDX, IDC_CHECK_ETHEREAL, m_sText[4]);
	DDX_Text(pDX, IDC_STATIC_DURABILITY, m_sText[5]);
	DDX_Text(pDX, IDC_CHECK_INDESTRUCTIBLE, m_sText[6]);
	DDX_Text(pDX, IDC_STATIC_QUANTITY, m_sText[7]);
	DDX_Text(pDX, IDC_STATIC_QUALITY, m_sText[8]);
	DDX_Text(pDX, IDC_STATIC_Sockets, m_sText[9]);
	DDX_Text(pDX, IDC_STATIC_DEFENCE, m_sText[10]);
	DDX_Text(pDX, IDC_STATIC_MONSTER, m_sText[11]);
	DDX_Text(pDX, IDC_STATIC_FIRST_NAME, m_sText[12]);
	DDX_Text(pDX, IDC_STATIC_SECOND_NAME, m_sText[13]);
	DDX_Text(pDX, IDC_STATIC_SUBTYPE, m_sText[14]);
	DDX_Text(pDX, IDC_STATIC_PREFIX1, m_sText[15]);
	DDX_Text(pDX, IDC_STATIC_SUFFIX1, m_sText[16]);
	DDX_Text(pDX, IDC_STATIC_SET_NAME, m_sText[17]);
	DDX_Text(pDX, IDC_STATIC_UNIQUE_NAME, m_sText[18]);
	DDX_Control(pDX, IDC_COMBO_CLASS, m_cbClass);
	DDX_Control(pDX, IDC_COMBO_QUALITY, m_cbQuality);
	DDX_Control(pDX, IDC_COMBO_MONSTER, m_cbMonster);
	DDX_Control(pDX, IDC_COMBO_FIRST_NAME, m_cbRareCraftedName[0]);
	DDX_Control(pDX, IDC_COMBO_SECOND_NAME, m_cbRareCraftedName[1]);
	DDX_Control(pDX, IDC_COMBO_SUBTYPE, m_cbSubType);
	DDX_Control(pDX, IDC_COMBO_PREFX1, m_cbPrefix[0]);
	DDX_Control(pDX, IDC_COMBO_PREFIX2, m_cbPrefix[1]);
	DDX_Control(pDX, IDC_COMBO_PREFIX3, m_cbPrefix[2]);
	DDX_Control(pDX, IDC_COMBO_SUFFIX1, m_cbSuffix[0]);
	DDX_Control(pDX, IDC_COMBO_SUFFIX2, m_cbSuffix[1]);
	DDX_Control(pDX, IDC_COMBO_SUFFIX3, m_cbSuffix[2]);
	DDX_Control(pDX, IDC_COMBO_SET_NAME, m_cbSetName);
	DDX_Control(pDX, IDC_COMBO_UNIQUE_NAME, m_cbUniqueName);
	DDX_Control(pDX, IDC_EDIT_LEVEL, m_edLevel);
	DDX_Check(pDX, IDC_CHECK_UNIDENTIFIED, m_bUnidentified);
	DDX_Check(pDX, IDC_CHECK_ETHEREAL, m_bEthereal);
	DDX_Control(pDX, IDC_CHECK_INDESTRUCTIBLE, m_chIndestructible);
	DDX_Control(pDX, IDC_EDIT_DURABILITY_MAX, m_edMaxDurability);
	DDX_Control(pDX, IDC_EDIT_DURABILITY, m_edDurability);
	DDX_Control(pDX, IDC_EDIT_QUANTITY, m_edQuantity);
	DDX_Control(pDX, IDC_EDIT_OWNER, m_edOwner);
	DDX_Control(pDX, IDC_EDIT_BASE_SOCKETS, m_edSockets);
	DDX_Text(pDX, IDC_STATIC_SOCKETS_EXT, m_sExtSockets);
	DDX_Control(pDX, IDC_EDIT_DEFENCE, m_edDefence);
	DDX_Control(pDX, IDC_TAB_PROPERTY, m_tabPropList);
	DDX_Control(pDX, IDC_LIST1, m_lstProperty);
	DDX_Control(pDX, IDC_EDIT1, m_edParam[0]);
	DDX_Control(pDX, IDC_EDIT2, m_edParam[1]);
	DDX_Control(pDX, IDC_EDIT3, m_edParam[2]);
	DDX_Control(pDX, IDC_EDIT4, m_edParam[3]);
	DDX_Text(pDX, IDC_STATIC_TIP1, m_sTip[0]);
	DDX_Text(pDX, IDC_STATIC_TIP2, m_sTip[1]);
	DDX_Text(pDX, IDC_STATIC_TIP3, m_sTip[2]);
	DDX_Text(pDX, IDC_STATIC_TIP4, m_sTip[3]);
}


BEGIN_MESSAGE_MAP(CDlgFoundry, CDialogEx)
	ON_BN_CLICKED(IDC_CHECK_INDESTRUCTIBLE, &CDlgFoundry::OnBnClickedCheckIndestructible)
	ON_CBN_SELCHANGE(IDC_COMBO_QUALITY, &CDlgFoundry::OnCbnSelchangeComboQuality)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_PROPERTY, &CDlgFoundry::OnTcnSelchangeTabProperty)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CDlgFoundry::OnLvnItemchangedList1)
	ON_EN_CHANGE(IDC_EDIT1, &CDlgFoundry::OnEnChangeParam)
	ON_EN_CHANGE(IDC_EDIT2, &CDlgFoundry::OnEnChangeParam)
	ON_EN_CHANGE(IDC_EDIT3, &CDlgFoundry::OnEnChangeParam)
	ON_EN_CHANGE(IDC_EDIT4, &CDlgFoundry::OnEnChangeParam)
END_MESSAGE_MAP()


// CDlgFoundry 消息处理程序

void CDlgFoundry::InitUI() {
	//UI text
	int idx = 0;
	for (int i = 0; i < int(size(m_sText)); ++i)
		m_sText[i] = ::theApp.FoundryUI(idx++);
	for (int i = 0; i < int(size(m_aProps)); ++i)
		m_tabPropList.InsertItem(i, ::theApp.FoundryUI(idx++));
		//UI combox
	for (UINT i = 0; i < ::theApp.CLASS_NAME_SIZE; ++i)
		m_cbClass.AddString(::theApp.ClassName(i));
	for (UINT i = 0; i < ::theApp.ItemQualityNameSize(); ++i)
		m_cbQuality.AddString(::theApp.ItemQualityName(i));
	for (UINT i = 0; i < ::theApp.MonsterNameSize(); ++i)
		m_cbMonster.AddString(::theApp.MonsterName(i));
	for (UINT i = 0; i < ::theApp.RareCraftedNameSize(); ++i) {
		const auto name = ::theApp.RareCraftedName(i);
		m_cbRareCraftedName[0].InsertString(-1, name);
		m_cbRareCraftedName[1].InsertString(-1, name);
	}
	WCHAR tmp[] = _T("0");
	for (; tmp[0] < _T('8'); ++tmp[0])
		m_cbSubType.InsertString(-1, tmp);
	for (UINT i = 0; i < ::theApp.MagicPrefixSize(); ++i) {
		const auto str = ::theApp.MagicPrefix(i);
		for (auto & cb : m_cbPrefix)
			cb.InsertString(-1, str);
	}
	for (UINT i = 0; i < ::theApp.MagicSuffixSize(); ++i) {
		const auto str = ::theApp.MagicSuffix(i);
		for (auto & cb : m_cbSuffix)
			cb.InsertString(-1, str);
	}
	for (UINT i = 0; i < ::theApp.SetItemNameSize(); ++i)
		m_cbSetName.InsertString(-1, ::theApp.SetItemName(i));
	for (UINT i = 0; i < ::theApp.UniqueNameSize(); ++i)
		m_cbUniqueName.InsertString(-1, ::theApp.UniqueName(i));
	//Property list
	m_lstProperty.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_CHECKBOXES);
	m_lstProperty.InsertColumn(0, ::theApp.FoundryUI(idx++), LVCFMT_LEFT, 60);
	m_lstProperty.InsertColumn(1, ::theApp.FoundryUI(idx++), LVCFMT_LEFT, 300);
	for (UINT i = 0; i < ::theApp.PropertyNameSize(); ++i) {
		m_lstProperty.InsertItem(i, CSFormat(_T("%3d"), i));				//属性ID
		m_lstProperty.SetItemText(i, 1, ::theApp.PropertyDescription(i));	//属性描述
	}
}

void CDlgFoundry::ReadItemEar(const CEar & ear) {
	m_edLevel.EnableWindow(TRUE);
	m_edLevel.SetWindowText(CSFormat(_T("%d"), ear.iEarLevel));
	m_edOwner.EnableWindow(TRUE);
	m_edOwner.SetWindowText(CString(ear.sEarName));
	m_cbClass.EnableWindow(TRUE);
	m_cbClass.SetCurSel(ear.iEarClass);
}

void CDlgFoundry::ReadItemGold(const CGoldQuantity & gold) {
	m_edQuantity.EnableWindow(TRUE);
	m_edQuantity.SetWindowText(CSFormat(_T("%d"), int(gold.wQuantity)));
}

void CDlgFoundry::ReadItemLongName(const CLongName & longName) {
	m_cbRareCraftedName[0].SetCurSel(longName.iName1);
	m_cbRareCraftedName[1].SetCurSel(longName.iName2);
	m_cbPrefix[0].SetCurSel(longName.wPref1.exist() ? longName.wPref1 : 0);
	m_cbPrefix[1].SetCurSel(longName.wPref2.exist() ? longName.wPref2 : 0);
	m_cbPrefix[2].SetCurSel(longName.wPref3.exist() ? longName.wPref3 : 0);
	m_cbSuffix[0].SetCurSel(longName.wSuff1.exist() ? longName.wSuff1 : 0);
	m_cbSuffix[1].SetCurSel(longName.wSuff2.exist() ? longName.wSuff2 : 0);
	m_cbSuffix[2].SetCurSel(longName.wSuff3.exist() ? longName.wSuff3 : 0);
}

void CDlgFoundry::ReadExtItemInfo(const CExtItemInfo & extInfo) {
	m_edLevel.EnableWindow(TRUE);
	m_edLevel.SetWindowText(CSFormat(_T("%d"), int(extInfo.iDropLevel)));
	ASSERT(0 < extInfo.iQuality);
	m_cbQuality.EnableWindow(TRUE);
	m_cbQuality.SetCurSel(extInfo.iQuality - 1);
	OnCbnSelchangeComboQuality();
	switch (extInfo.iQuality) {
		case 1:		//low
			ASSERT(extInfo.loQual.exist());
			m_cbSubType.SetCurSel(extInfo.loQual);
			break;
		case 3:		//high
			ASSERT(extInfo.hiQual.exist());
			m_cbSubType.SetCurSel(extInfo.hiQual);
			break;
		case 4:		//magical
			ASSERT(extInfo.wPrefix.exist());
			ASSERT(extInfo.wSuffix.exist());
			m_cbPrefix[0].SetCurSel(extInfo.wPrefix);
			m_cbSuffix[0].SetCurSel(extInfo.wSuffix);
			break;
		case 5:		//set
			ASSERT(extInfo.wSetID.exist());
			m_cbSetName.SetCurSel(extInfo.wSetID);
			break;
		case 6:		//rare
			ASSERT(extInfo.pRareName.exist());
			ReadItemLongName(*extInfo.pRareName);
			break;
		case 7:		//unique
			ASSERT(extInfo.wUniID.exist());
			m_cbUniqueName.SetCurSel(extInfo.wUniID);
			break;
		case 8:		//crafted
			ASSERT(extInfo.pCraftName.exist());
			ReadItemLongName(*extInfo.pCraftName);
			break;
		default:;	//normal
	}
	m_edOwner.EnableWindow(TRUE);
	if (extInfo.sPersonName.exist())
		m_edOwner.SetWindowText(CString(extInfo.sPersonName));
	if (extInfo.wMonsterID.exist()) {
		m_cbMonster.EnableWindow(TRUE);
		m_cbMonster.SetCurSel(extInfo.wMonsterID);
	}
}

void CDlgFoundry::ReadTypeSpecificInfo(CTypeSpecificInfo & spec) {
	if (spec.iDefence.exist()) {
		m_edDefence.EnableWindow(TRUE);
		m_edDefence.SetWindowText(CSFormat(_T("%d"), spec.GetDefence()));
	}
	if (spec.iMaxDurability.exist()) {
		m_edMaxDurability.SetWindowText(CSFormat(_T("%d"), int(spec.iMaxDurability)));
		m_edDurability.SetWindowText(CSFormat(_T("%d"), spec.iCurDur.exist() ? int(spec.iCurDur) : 0));
		m_chIndestructible.EnableWindow(TRUE);
		m_chIndestructible.SetCheck(spec.IsIndestructible());
		if (!m_chIndestructible.GetCheck()) {
			m_edMaxDurability.EnableWindow(TRUE);
			m_edDurability.EnableWindow(TRUE);
		}
	}
	m_edSockets.EnableWindow(TRUE);
	const auto s = spec.Sockets();
	m_edSockets.SetWindowText(CSFormat(_T("%d"), s.first));
	m_sExtSockets = CSFormat(_T("%d"), s.second);
	if (spec.iQuantity.exist()) {
		m_edQuantity.EnableWindow(TRUE);
		m_edQuantity.SetWindowText(CSFormat(_T("%d"), int(spec.iQuantity)));
	}
	m_aProps[0] = { spec.stPropertyList, TRUE };
	for (int i = 1; i <= 5; ++i)
		if (spec.apSetProperty[i - 1].exist())
			m_aProps[i].first = *spec.apSetProperty[i - 1];
	if (spec.stRuneWordPropertyList.exist())
		m_aProps[6] = { *spec.stRuneWordPropertyList, TRUE };
}

void CDlgFoundry::ReadItemInfo(CItemInfo & info) {
	if (info.pGold.exist())
		return ReadItemGold(*info.pGold);
	if (info.pExtItemInfo.exist())
		ReadExtItemInfo(*info.pExtItemInfo);
	if (info.pTpSpInfo.exist())
		ReadTypeSpecificInfo(*info.pTpSpInfo);
}

void CDlgFoundry::ReadItem() {
	SetWindowText(m_stItem.ItemName());
	m_bUnidentified = !m_stItem.bIdentified;
	m_bEthereal = m_stItem.bEthereal;
	if (m_stItem.pEar.exist())			//耳朵
		return ReadItemEar(*m_stItem.pEar);
	//普通物品
	ASSERT(m_stItem.pItemInfo.exist());
	ReadItemInfo(*m_stItem.pItemInfo);
}

static void validate(bool expr, int index) {
	if (expr)
		return;
	::MessageBox(0, ::theApp.MsgBoxInfo(index), ::theApp.MsgError(), MB_ICONERROR);
	throw 0;
}

void CDlgFoundry::WriteItemEar(CEar & ear) const {
	ear.iEarLevel = intOf(m_edLevel);
	validate((0 < ear.iEarLevel && ear.iEarLevel <= 99), 22);
	validate(::SetCharName(ear.sEarName, stringOf(m_edOwner)), 0);
	ear.iEarClass = m_cbClass.GetCurSel();
	validate((0 <= ear.iEarClass && ear.iEarClass < 7), 23);
}

void CDlgFoundry::WriteItemGold(CGoldQuantity & gold) const {
	gold.wQuantity = intOf(m_edQuantity);
}

static void setPreSuffix(BOOL & exist, MayExist<WORD> & index, const CComboBox & cb, int max, int msg) {
	exist = (cb.GetCurSel() > 0);
	if (exist) {
		index.ensure() = cb.GetCurSel();
		validate(index < max, msg);
	} else
		index.reset();
}

void CDlgFoundry::WriteItemLongName(CLongName & longName) const
{
	longName.iName1 = m_cbRareCraftedName[0].GetCurSel();
	validate(longName.iName1 < ::theApp.RareCraftedNameSize(), 34);
	longName.iName2 = m_cbRareCraftedName[1].GetCurSel();
	validate(longName.iName2 < ::theApp.RareCraftedNameSize(), 35);
	setPreSuffix(longName.bPref1, longName.wPref1, m_cbPrefix[0], ::theApp.MagicPrefixSize(), 36);
	setPreSuffix(longName.bPref2, longName.wPref2, m_cbPrefix[1], ::theApp.MagicPrefixSize(), 37);
	setPreSuffix(longName.bPref3, longName.wPref3, m_cbPrefix[2], ::theApp.MagicPrefixSize(), 38);
	setPreSuffix(longName.bSuff1, longName.wSuff1, m_cbSuffix[0], ::theApp.MagicSuffixSize(), 39);
	setPreSuffix(longName.bSuff2, longName.wSuff2, m_cbSuffix[1], ::theApp.MagicSuffixSize(), 40);
	setPreSuffix(longName.bSuff3, longName.wSuff3, m_cbSuffix[2], ::theApp.MagicSuffixSize(), 41);
}

static BOOL checkPersonalizedName(const CString & name) {
	if (name.IsEmpty() || name.GetLength() > 15)
		return FALSE;
	for (int i = 0; i < name.GetLength(); ++i)
		if (name[i] < 32 || 126 < name[i])
			return FALSE;
	return TRUE;
}

void CDlgFoundry::WriteExtItemInfo(CExtItemInfo & extInfo, BOOL & bPersonalized, const CItemMetaData & meta) const {
	extInfo.iDropLevel = intOf(m_edLevel);
	validate((0 < extInfo.iDropLevel && extInfo.iDropLevel <= 99), 24);
	extInfo.iQuality = m_cbQuality.GetCurSel() + 1;
	validate((0 < extInfo.iQuality && extInfo.iQuality < 9), 25);
	extInfo.loQual.reset();
	extInfo.hiQual.reset();
	extInfo.wPrefix.reset();
	extInfo.wSuffix.reset();
	extInfo.wSetID.reset();
	extInfo.pRareName.reset();
	extInfo.wUniID.reset();
	extInfo.pCraftName.reset();
	switch (extInfo.iQuality) {
		case 1:		//low
			extInfo.loQual.ensure() = m_cbSubType.GetCurSel();
			validate((extInfo.loQual < 8), 26);
			break;
		case 3:		//high
			extInfo.hiQual.ensure() = m_cbSubType.GetCurSel();
			validate((extInfo.hiQual < 8), 26);
			break;
		case 4:		//magical
			extInfo.wPrefix.ensure() = m_cbPrefix[0].GetCurSel();
			validate(extInfo.wPrefix < ::theApp.MagicPrefixSize(), 44);
			extInfo.wSuffix.ensure() = m_cbSuffix[0].GetCurSel();
			validate(extInfo.wSuffix < ::theApp.MagicSuffixSize(), 45);
			break;
		case 5:		//set
			extInfo.wSetID.ensure() = m_cbSetName.GetCurSel();
			validate(extInfo.wSetID < ::theApp.SetItemNameSize(), 42);
			break;
		case 6:		//rare
			WriteItemLongName(extInfo.pRareName.ensure());
			break;
		case 7:		//unique
			extInfo.wUniID.ensure() = m_cbUniqueName.GetCurSel();
			validate(extInfo.wUniID < ::theApp.UniqueNameSize(), 43);
			break;
		case 8:		//crafted
			WriteItemLongName(extInfo.pCraftName.ensure());
			break;
		default:;	//normal
	}
	CString name = stringOf(m_edOwner);
	bPersonalized = !name.IsEmpty();
	if (bPersonalized){
		validate(checkPersonalizedName(name), 27);
		auto & v = extInfo.sPersonName.ensure();
		fill(v.begin(), v.end(), 0);
		for (int i = 0; i < name.GetLength(); ++i)
			v[i] = char(name[i]);
	}else
		extInfo.sPersonName.reset();
	if (meta.HasMonsterID) {
		extInfo.wMonsterID.ensure() = m_cbMonster.GetCurSel();
		validate((extInfo.wMonsterID < ::theApp.MonsterNameSize()), 33);
	} else
		extInfo.wMonsterID.reset();
}

void CDlgFoundry::WriteTypeSpecificInfo(CTypeSpecificInfo & spec, const CItemMetaData & meta, BOOL & bSocketed, BOOL bIsSet, BOOL bRuneWord) const {
	if (meta.HasDef) {
		int d = intOf(m_edDefence);
		validate((10 <= d && d < 2058), 28);
		spec.SetDefence(d);
	} else
		spec.iDefence.reset();
	if (meta.HasDur) {
		spec.iMaxDurability.ensure() = (m_chIndestructible.GetCheck() ? 0 : intOf(m_edMaxDurability));
		validate((spec.iMaxDurability < 256), 29);
	} else
		spec.iMaxDurability.reset();
	if (spec.iMaxDurability.exist() && spec.iMaxDurability > 0) {
		spec.iCurDur.ensure() = intOf(m_edDurability);
		validate((spec.iCurDur <= spec.iMaxDurability), 30);
	}else
		spec.iCurDur.reset();
	const int s1 = intOf(m_edSockets), s2 = intOf(m_sExtSockets);
	validate((0 <= s1 && 0 <= s2 && s1 + s2 <= 6), 31);
	bSocketed = (s1 + s2 > 0);
	if (bSocketed) {
		spec.iSocket.ensure() = s1;
	} else
		spec.iSocket.reset();
	if (meta.IsStacked) {
		spec.iQuantity.ensure() = intOf(m_edQuantity);
		validate((spec.iQuantity < 512), 32);
	} else
		spec.iQuantity.reset();
	ASSERT(m_aProps[0].second);
	spec.stPropertyList = m_aProps[0].first;
	if (bIsSet) {
		spec.aHasSetPropList.ensure();
		int i = 0;
		for (BOOL & b : spec.aHasSetPropList) {
			auto & p = m_aProps[++i];
			b = (p.second && !p.first.mProperty.empty());
		}
	} else
		spec.aHasSetPropList.reset();
	for (int i = 0; i < int(size(spec.apSetProperty)); ++i)
		if (spec.aHasSetPropList.exist() && spec.aHasSetPropList[i])
			spec.apSetProperty[i].ensure() = m_aProps[i + 1].first;
		else
			spec.apSetProperty[i].reset();
	if (bRuneWord) {
		ASSERT(m_aProps[6].second);
		spec.stRuneWordPropertyList.ensure() = m_aProps[6].first;
	} else
		spec.stRuneWordPropertyList.reset();
}

void CDlgFoundry::WriteItemInfo(CItemInfo & info, BOOL bSimple, BOOL & bPersonalized, const CItemMetaData & meta, BOOL & bSocketed, BOOL bRuneWord) const {
	if (info.IsGold()) {
		WriteItemGold(info.pGold.ensure());
	} else
		info.pGold.reset();
	if (bSimple) {
		info.pExtItemInfo.reset();
		info.pTpSpInfo.reset();
	} else {
		WriteExtItemInfo(info.pExtItemInfo.ensure(), bPersonalized, meta);
		WriteTypeSpecificInfo(info.pTpSpInfo.ensure(), meta, bSocketed, info.pExtItemInfo->IsSet(), bRuneWord);
	}
}

void CDlgFoundry::WriteItem(CD2Item & item) const {
	item.bIdentified = !m_bUnidentified;
	item.bEthereal = m_bEthereal;
	if (item.bEar) {
		WriteItemEar(item.pEar.ensure());
		item.pItemInfo.reset();
	} else {
		WriteItemInfo(item.pItemInfo.ensure(), item.bSimple, item.bPersonalized, item.MetaData(), item.bSocketed, item.bRuneWord);
		item.pEar.reset();
	}
}


BOOL CDlgFoundry::GatherData() {
	try {
		UpdateData(TRUE);
		CD2Item item(m_stItem);
		WriteItem(item);
		m_stItem = item;
	} catch (...) {
		return FALSE;
	}
	return TRUE;
}

BOOL CDlgFoundry::OnInitDialog() {
	CDialogEx::OnInitDialog();
	InitUI();
	ReadItem();
	UpdatePropList();
	UpdateData(FALSE);
	return TRUE;
}

void CDlgFoundry::OnOK() {
	if (GatherData())
		CDialogEx::OnOK();
}

void CDlgFoundry::OnBnClickedCheckIndestructible() {
	const BOOL ch = m_chIndestructible.GetCheck();
	m_edDurability.EnableWindow(!ch);
	m_edMaxDurability.EnableWindow(!ch);
}

void CDlgFoundry::OnCbnSelchangeComboQuality() {
	//Disable all combox
	for (auto & cb : m_cbRareCraftedName)
		cb.EnableWindow(FALSE);
	for (auto & cb : m_cbPrefix)
		cb.EnableWindow(FALSE);
	for (auto & cb : m_cbSuffix)
		cb.EnableWindow(FALSE);
	m_cbSubType.EnableWindow(FALSE);
	m_cbSetName.EnableWindow(FALSE);
	m_cbUniqueName.EnableWindow(FALSE);
	//Set props
	for (int i = 0; i < 5; ++i)
		m_aProps[i + 1].second = (m_cbQuality.GetCurSel() == 4);
	switch (m_cbQuality.GetCurSel()) {
		case 0:case 2:	//low & high
			m_cbSubType.EnableWindow(TRUE);
			break;
		case 3:			//magical
			m_cbPrefix[0].EnableWindow(TRUE);
			m_cbSuffix[0].EnableWindow(TRUE);
			break;
		case 4:			//set
			m_cbSetName.EnableWindow(TRUE);
			break;
		case 5:case 7:	//rare
			for (auto & cb : m_cbRareCraftedName)
				cb.EnableWindow(TRUE);
			for (auto & cb : m_cbPrefix)
				cb.EnableWindow(TRUE);
			for (auto & cb : m_cbSuffix)
				cb.EnableWindow(TRUE);
			break;
		case 6:			//unique
			m_cbUniqueName.EnableWindow(TRUE);
			break;
		default:;		//normal
	}
}

void CDlgFoundry::OnTcnSelchangeTabProperty(NMHDR *pNMHDR, LRESULT *pResult) {
	UpdatePropList();
	UpdateData(FALSE);
	*pResult = 0;
}

static int CALLBACK PropItemCompare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort) {
	auto & list = *reinterpret_cast<const CListCtrl *>(lParamSort);
	const auto i = static_cast<int>(lParam1), j = static_cast<int>(lParam2);
	const auto ch1 = list.GetCheck(i), ch2 = list.GetCheck(j);
	if (ch1 != ch2)
		return (ch1 ? -1 : 1);
	const auto s1 = list.GetItemText(i, 0), s2 = list.GetItemText(j, 0);
	return (s1 < s2 ? -1 : 1);
}

void CDlgFoundry::UpdatePropList() {
	//reset params
	m_lstProperty.SetItemState(m_iSelectedPropItem, 0, LVIS_SELECTED);
	m_iSelectedPropItem = -1;
	SetupPropParameters(-1, 0);
	//uncheck all items
	for (int i = 0; i < m_lstProperty.GetItemCount(); ++i)
		if (m_lstProperty.GetCheck(i)) {
			m_lstProperty.SetCheck(i, FALSE);
			const auto code = _tstoi(m_lstProperty.GetItemText(i, 0).GetBuffer());
			m_lstProperty.SetItemText(i, 1, ::theApp.PropertyDescription(code));
		}
	m_lstProperty.SortItemsEx(PropItemCompare, reinterpret_cast<LPARAM>(&m_lstProperty));
	//check items
	const int i = m_tabPropList.GetCurSel();
	ASSERT(0 <= i && i < int(size(m_aProps)));
	const auto & p = m_aProps[i];
	m_lstProperty.EnableWindow(p.second);
	//update desc and sort
	if (p.second) {
		for (auto & p : p.first.mProperty) {
			m_lstProperty.SetCheck(p.first, TRUE);
			m_lstProperty.SetItemText(p.first, 1, ::theApp.PropertyDescription(p.first, p.second));
		}
		m_lstProperty.SortItemsEx(PropItemCompare, reinterpret_cast<LPARAM>(&m_lstProperty));
	}
}

void CDlgFoundry::SetupPropParameters(int id, int value) {
	if (0 <= id) {
		ASSERT(0 <= value);
		const auto p = ::theApp.PropertyParameters(id, value);
		ASSERT(p.size() <= size(m_edParam));
		for (int i = 0; i < int(size(m_edParam)); ++i) {
			if (i < int(p.size())) {
				auto & a = p[i];
				m_edParam[i].SetWindowText(CSFormat(_T("%d"), a.iValue));
				m_sTip[i] = (CSFormat(_T("%d - %d"), a.iMin, a.iMax));
			} else {
				m_edParam[i].SetWindowText(_T(""));
				m_sTip[i] = (_T(""));
			}
			m_edParam[i].EnableWindow(i < int(p.size()));
		}
	} else
		for (int i = 0; i < int(size(m_edParam)); ++i) {
			m_edParam[i].SetWindowText(_T(""));
			m_edParam[i].EnableWindow(FALSE);
			m_sTip[i] = (_T(""));
		}
}

BOOL CDlgFoundry::ChangePropItemSelection(LPNMLISTVIEW pNMLV) {
	if ((pNMLV->uChanged & LVIF_STATE) && (pNMLV->uNewState & LVIS_SELECTED)) {
		//find out property list
		const int sel = m_tabPropList.GetCurSel();
		ASSERT(0 <= sel && sel < int(size(m_aProps)));
		const auto & p = m_aProps[sel].first.mProperty;
		//setup parameters
		m_iSelectedPropItem = -1;
		const auto code = _tstoi(m_lstProperty.GetItemText(pNMLV->iItem, 0).GetBuffer());
		const auto wh = p.find(code);
		if (wh == p.end())
			SetupPropParameters(-1, 0);
		else
			SetupPropParameters(wh->first, wh->second);
		m_iSelectedPropItem = pNMLV->iItem;
		return TRUE;
	}
	return FALSE;
}

BOOL CDlgFoundry::ChangePropItemChecked(LPNMLISTVIEW pNMLV) {
	BOOL bPrevState = (BOOL)(((pNMLV->uOldState & LVIS_STATEIMAGEMASK) >> 12) - 1);
	if (bPrevState < 0)
		bPrevState = 0;
	BOOL bChecked = (BOOL)(((pNMLV->uNewState & LVIS_STATEIMAGEMASK) >> 12) - 1);
	if (bChecked < 0)
		bChecked = 0;
	if (bPrevState == bChecked)
		return FALSE;
	//find out property list
	const int sel = m_tabPropList.GetCurSel();
	ASSERT(0 <= sel && sel < int(size(m_aProps)));
	auto & p = m_aProps[sel].first.mProperty;
	//handle item
	const int idx = pNMLV->iItem;
	const BOOL ret = (idx == m_iSelectedPropItem);
	const auto code = _tstoi(m_lstProperty.GetItemText(idx, 0).GetBuffer());
	if (bChecked) {	//check a new item
		auto wh = p.find(code);
		if (wh == p.end())
			p[code] = ::theApp.PropertyMetaData(code).DefaultValue();
		if (ret)
			SetupPropParameters(code, p[code]);
	} else {			//uncheck an item
		p.erase(code);
		m_lstProperty.SetItemText(idx, 1, ::theApp.PropertyDescription(code));
		if (ret)
			SetupPropParameters(-1, 0);
	}
	return ret;
}

void CDlgFoundry::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult) {
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	BOOL update = ChangePropItemSelection(pNMLV);
	update = (ChangePropItemChecked(pNMLV) || update);
	if(update)
		UpdateData(FALSE);
	*pResult = 0;
}

void CDlgFoundry::OnEnChangeParam() {
	//prop item & meta
	if (m_iSelectedPropItem < 0)
		return;
	const auto code = _tstoi(m_lstProperty.GetItemText(m_iSelectedPropItem, 0).GetBuffer());
	auto & meta = ::theApp.PropertyMetaData(code);
	//params
	vector<int> p;
	for (auto & ed : m_edParam)
		p.push_back(intOf(ed));
	const int v = meta.GetValue(p);
	if (v < 0)
		return;
	//update desc
	m_lstProperty.SetItemText(m_iSelectedPropItem, 1, ::theApp.PropertyDescription(code, v));
	//update prop list
	const int sel = m_tabPropList.GetCurSel();
	ASSERT(0 <= sel && sel < int(size(m_aProps)));
	auto & pp = m_aProps[sel].first.mProperty;
	pp[code] = v;
}
