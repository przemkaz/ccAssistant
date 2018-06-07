#ifndef _ccAssistant_ccAssistant_h
#define _ccAssistant_ccAssistant_h

#include <CtrlLib/CtrlLib.h>
#include <Docking/Docking.h>
#include <GridCtrl/GridCtrl.h>
#include <DropGrid/DropGrid.h>

#include <XmlSql/XmlSql.h>
using namespace Upp;

//#define SCHEMADIALECT <plugin/sqlite3/Sqlite3Schema.h>
//#define MODEL <XmlSql/iProtect.sch>
//#include "Sql/sch_header.h"

#define LAYOUTFILE <ccAssistant/ccAssistant.lay>
#include <CtrlCore/lay.h>

#define IMAGECLASS Icons
#define IMAGEFILE <ccAssistant/Icons.iml>
#include <Draw/iml_header.h>

struct Connection;

class ConnectionWnd : public WithConnectionLayout<TopWindow>
{
	typedef ConnectionWnd CLASSNAME;
	public:
	ConnectionWnd();
	void Edit(Connection& conn);
	void Add(Connection& conn);
};

class EventsView : public WithEventsLayout<DockableCtrl>
{
	typedef EventsView CLASSNAME;
	uint32 lang;
	uint32 max_rows; 
	
	VectorMap<int,EventSetup> eventSetup;
	
	ToolButton counterBtn;
	RichTextCtrl info;
	
	void GridInit();
	void ToolBar(Bar& bar);
//	void 
	public:
	EventsView();
	
	void NewTransaction(Transaction t);
	
	void SetMaxRows (const uint32 max=10000) {max_rows= max;};
	uint32 GetMaxRows() {return max_rows;};
	
	bool GetEventSetup( XmlSql& xmlsql);
};

class SqlQuery : public WithSqlQueryLayout<DockableCtrl>
{
	ToolButton counterBtn;
	void ToolBar(Bar& bar);
	
	typedef SqlQuery CLASSNAME;
	public:
	SqlQuery();
	
};

class ccAssistant :public DockWindow {

public:
	typedef ccAssistant CLASSNAME;
	ccAssistant();
	~ccAssistant();
	virtual void Close();	
private:

	EventsView eventsview;
//	ConnectionWnd connectionwindow;
    SqlQuery     sqlquery;

/// Connection section.
	
	ToolButton connButton;
	DropGrid connGrid;

	void AddEditConnection (Connection& c);
	void AddConnection();
	void EditConnection ();
	void RemoveConnection();
	void UpdateConnGrid ();

///	
	
	
	struct AppData
	{
		Vector <Connection> connections;
		String s;
		void Xmlize(XmlIO& xml)
			{
			xml
			("Connections", connections)
			("Layout",s)
			;
			}
	
	} data;
	
	Vector<Connection>& conn = data.connections;
////	
	
	XmlSql xmlsql;

	int lang;
	virtual void DockInit();
	
	MenuBar menu;
	void MainMenu(Bar& bar);
	


	void Init();
	void Connect();
	bool Query();
	
	//test
	ToolButton tb;
	Label lab;

	
	//Transaction setup
	
	
	// Transaction window
	Label eventLab;
	void NewTransaction(Transaction t);
	
	//DO Usunięcia !!!
};


struct Connection: Moveable<Connection>
{

	String serverIP;
	String login;
	String password;	
	String description;
	
	int  port;
	
	void Xmlize(XmlIO& xml);
	
	Connection (const Connection& conn)
	{
		serverIP = conn.serverIP;
		login = conn.login;
		password = conn.password;
		description = conn.description;
		port = conn.port;		
	}
	
	Connection()
	{
		port=0;
		serverIP=login=password=description = String().GetVoid();
	}
};

// test 3
#endif
