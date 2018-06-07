#include "ccAssistant.h"


//#include <Sql/sch_schema.h>
//#include <Sql/sch_source.h>

#define IMAGECLASS Icons
#define IMAGEFILE <ccAssistant/Icons.iml>
#include <Draw/iml_source.h>

//#include <XmlSql_Test_App/DefaultLayout.brc>

ccAssistant::ccAssistant()
{
	
	Title("ccAssistant").Sizeable(true).Zoomable(true).Maximize(true);
	Icon(Icons::main());
	LargeIcon(Icons::main());
	
//	CtrlLayout(connectionwindow, "Connection");
	CtrlLayout(sqlquery, "SQL Query");
	CtrlLayout(eventsview, "Online events");
	
	

	Button* btn;// = &connectionwindow.connect_btn;
//	*btn << [=] {Connect();};
	
	
	//Sql query window
	btn = &sqlquery.query_btn;
//	btn->Disable();
	*btn << [=]{ Query();};	
	
	EditString& edit= sqlquery.sql_edit;
//	edit.Disable();
	edit.WhenEnter << [=] {Query();};
	
	//Main menu
	AddFrame(menu); 
	menu.Set(THISBACK(MainMenu));
	Init();
}

ccAssistant::~ccAssistant()
{
	ShutdownThreads();
	Sleep(100);
	Close();
}


void ccAssistant::Close()
{
	// Save our current layout & settings
	StringStream s;
	s.Create();
	SerializeWindow(s);
	data.s = Base64Encode(s.GetResult());
			
	FileOut out;
	if (out.Open(ConfigFile("settings.xml"))){
		out << StoreAsXML(data);
		out.Close();
	}
	TopWindow::Close();
}

void ccAssistant::MainMenu(Bar& bar)
{
	bar.Add("Windows", Image(), THISBACK(DockWindowMenu));
	bar.Separator();
	Size sz = bar.GetStdSize();

	bar.Add(connGrid.LeftPosZ(2,450).TopPosZ(2,sz.cy),450+sz.cx+10,sz.cy+10);
//	bar.Separator();	
	bar.Add(connButton.LeftPosZ(0,80).TopPosZ(2,sz.cy),80+sz.cx,sz.cy);
	
}
void ccAssistant::DockInit()
{
LOG("Dockinit()");
//	Register(connectionwindow).AllowDockNone();
	Register(sqlquery);
	Register(eventsview).AllowDockAll();
//	Register(Dockable(sqlquerywindow, "SQL Query").SizeHint(Size(800, 200)));


	//Load config data
	String s;
    s = LoadFile(ConfigFile("settings.xml"));
    LoadFromXML(data, s);
//    LOG( "Settings layout -->: " + data.s);
	UpdateConnGrid();
		StringStream in (Base64Decode(data.s));
		SerializeWindow(in);

}

void  ccAssistant::Connect()
{
	if (connGrid.IsEmpty()){
	 ErrorOK(t_("No connections defined!!!")); 
	 return;
	}
	
	int v = connGrid.GetIndex();

	String ip = conn[v].serverIP;// = ~connectionwindow.serverIP_edit;
	String psw = conn[v].password; // = ~connectionwindow.psw_edit;
	String login = conn[v].login;// = ~connectionwindow.login_edit;
	uint32 port = conn[v].port;
	
	//	bool result = false;
	Progress& p = Progress(this,t_("Connectiong to iProtect..."),0);
	p.Create();

	if(xmlsql.GetSessionID().IsEmpty()){
		if (!ip.IsEmpty()&&!psw.IsEmpty()&&!login.IsEmpty())
		{
	
			xmlsql.SetServerIP(ip).Port(port).SetLogin(login).SetPassword(psw);
			
			bool loging, result = false;
			loging = true;
			Thread().Run ([&]{  result = xmlsql.Login();
							loging = false;
							});
		while(loging){
			p.Step();
			Sleep(40);
		}
				if (result) 
				{
					//Do zmiany 
					eventsview.GetEventSetup(xmlsql);
					
					if(!xmlsql.StartListen())ErrorOK(t_("Transaction listner didn't start correctly!"));
				}
				else {
					ErrorOK(t_("Connection to iProtect db could not be established")); 
					return;
					}
					connGrid.Disable();
					connButton.Label(t_("Disconect")).Image(GridImg::Cancel());

		}
	}

	else
	{
	//		listnerer.Stop();
	//		listnerer.Logout();
			xmlsql.Logout();
			connGrid.Enable();
			connButton.Label("Connect").Image(GridImg::Commit());
		//	ShutdownThreads();
	//		connectionwindow.connect_btn.SetLabel(t_("Connect"));
	}
	
}

bool ccAssistant::Query()
{
	GridCtrl& grid = sqlquery.grid;
	grid.Clear(true);
	String sql = ~sqlquery.sql_edit;
	VectorMap <String, Vector<String>> result;

	if(xmlsql.Query(sql,result))
	{
	//	DDUMP(result);
		if (result.GetCount() <= 0) {
			PromptOK(t_("Empty result"));
			return true;
		}
		for(int i=0; i < result.GetCount(); i++)
			grid.AddColumn(result.GetKey(i),100);
		
		int c = result[0].GetCount();
		for(int i=0; i < c; i++){
			Vector <Value> tmp;
			for(int r=0; r < result.GetCount(); r++)
			tmp.Add(result[r][i]);
			grid.Add(tmp);
		}
	}
	else return false;
	
	return true;
}

void ccAssistant::Init()
{

//	LOG("init()");
	//Connection drop grid
	
	String s = t_("Server IP");
	connGrid.AddColumn("SERVER_IP",s,GetStdFontSize().cx*s.GetCount()).Width(30).Max(GetStdFontSize().cx*16);
	s = t_("Login");
	connGrid.AddColumn("LOGIN",s,GetStdFontSize().cx*5).Width(GetStdFontSize().cx*s.GetCount());
	s = t_("Name");
	connGrid.AddColumn("NAME",s);

	connGrid.AddPlus(THISBACK(AddConnection)).Left();
	connGrid.AddSelect(THISBACK(EditConnection)).Left();
	connGrid.AddButton(DropGrid::BTN_CLEAN,THISBACK(RemoveConnection)).Left();
	
	//connGrid.AddText("Connect",THISBACK(Connect)).SetImage(GridImg::Commit());
	connGrid.SetValueColumn(0);
	connGrid.AddValueColumn(0).AddValueColumn(1).AddValueColumn(2);
	
	//Connection button

	connButton.Label(t_("Connect")).Image(GridImg::Commit());
	connButton << THISFN(Connect);
	
	//GridCtrl& grid = sqlquery.grid;
	//grid.SetToolBar();	
	
	
	
	xmlsql.WhenError << [] {ErrorOK("Disconection from iProtect server");};
	xmlsql.WhenNewTransaction << [=](Transaction t){eventsview.NewTransaction(t);};

	
}

void ccAssistant::AddConnection()
{

	AddEditConnection(conn.Add());
	
}

void ccAssistant::EditConnection()
{
	AddEditConnection(conn[connGrid.GetCurrentRow()]);
	
}

void ccAssistant::AddEditConnection(Connection& c)
{
	bool add = false;
	ConnectionWnd wnd;

	if (c.serverIP.IsVoid()) CtrlLayout(wnd, t_("Add connection"));
	else {
		CtrlLayout(wnd, t_("Edit connection"));
		wnd.Edit(c);
	}
	switch(wnd.Run())
	{
		case IDOK:
		c.description = ~wnd.nameEdit;
		c.login = ~wnd.loginEdit;
		c.password = ~wnd.passwordEdit;
		c.serverIP = ~wnd.serveripEdit;
		c.port = ~wnd.portSwitch;	
		break;
		case IDCANCEL:
			if (c.serverIP.IsVoid()) conn.Drop();
	}
	UpdateConnGrid();
}

void ccAssistant::UpdateConnGrid()
{
	int idx = connGrid.GetIndex(); 
	int count = connGrid.GetCount();
	connGrid.Clear();
	for( int i = 0; i<conn.GetCount(); i++)
	{
		connGrid.Add(conn[i].serverIP,conn[i].login,conn[i].description);
	}
	if (count = conn.GetCount()&& idx >=0 && idx < conn.GetCount()) connGrid.SetIndex(idx);
}


void ccAssistant::RemoveConnection()
{

	if(PromptOKCancel(t_("Remove selected connection?"))){
		conn.Remove(connGrid.GetCurrentRow());
		UpdateConnGrid();
	}
}












/////////////////////////////////////////


/////////////////////////////////////////

SqlQuery::SqlQuery()
{
	CtrlLayout(*this);
	grid.Searching();
	grid.Navigating();
	grid.SetToolBar();
	grid.WhenToolBar = THISFN(ToolBar);

	sql_edit.SetFocus();
	query_btn.SetFocus();
	
}

void SqlQuery::ToolBar(Bar& bar)
{
	
	grid.Navigating(false);
	grid.NavigatingBar(bar);
	bar.Separator();
	grid.StdToolBar(bar);	
		
	
	bar.Separator();
	Bar::Item& btn = counterBtn.Label(t_("Number of records: ")+ grid.GetVisibleCount());
	
	bar.Add(counterBtn);//counterLabel,200,20);
	//grid.NavigatingBar(bar);
	//grid.StdToolBar(bar);
	

	
}
/////////////////////////////////////////


/////////////////////////////////////////

GUI_APP_MAIN
{
	ccAssistant().Run();
}
