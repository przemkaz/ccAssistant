#include "ccAssistant.h"



EventsView::EventsView()
{
	CtrlLayout(*this);
	max_rows = 10000;
	GridInit();
//	tgrid.WhenNewRow << [=] {PromptOK("When Search");};
}

void EventsView::NewTransaction(Transaction t)
{
	//GridCtrl& tgrid = eventsview.tgrid;
	//tgrid.Search()
	GuiLock __;
	if(tgrid.GetCount()>10000)
		tgrid.RemoveLast();

		Vector<Value> v;
		v << t.transTIME << t.transTEXT << AsString(t.transTYPE)+": " + eventSetup.Get(t.transTYPE).name[lang-1] <<t.transID;
		
		tgrid.Insert(0);
		tgrid.Set(0,v);
		
		GridCtrl::ItemRect& row = tgrid.GetRow(0);
		
		EventSetup& ts = eventSetup.Get(t.transTYPE);
		
		if (ts.bg_color != 0 || ts.fg_color !=0)
		{
			
			row.Bg(Color(ts.bg_color,0)).Fg(Color(ts.fg_color,0));
			 
		}
		
		if (!String(~tgrid.find).IsEmpty())
		{
			//row.Hidden();
			tgrid.DoFind();
			info <<= AsString("Search_string: " + AsString(tgrid.search_string.GetCount()));	
		}
		
}
void EventsView::GridInit()
{

	tgrid.SearchMoveCursor();
	tgrid.SetToolBar();
	tgrid.WhenToolBar << THISFN(ToolBar);	

	tgrid.AddColumn("TIME",t_(" Transaction time ")).Max(GetStdFontSize().cx * AsString(GetSysTime()).GetCount());
	tgrid.AddColumn("TEXT",t_("Transaction text"));
	tgrid.AddColumn("TRANSTYPE",t_("Transaction type")).Max(tgrid.GetSize().cx);
	tgrid.AddColumn("TRANSID",t_("Transaction ID")).Hidden().Index();
}

bool EventsView::GetEventSetup(XmlSql& xmlsql)
{
	VectorMap <String, Vector <String>> result;
	eventSetup.Clear();

	
//	SqlStatement select = Select(TRANSETUPID,BACKGROUNDCOLOR,FOREGROUNDCOLOR).From(TRANSETUP).OrderBy(TRANSETUPID);
//	LOG(select.GetText());
	String select = "select TRANSETUPID,BACKGROUNDCOLOR,FOREGROUNDCOLOR from TRANSETUP";
	if(xmlsql.Query(select,result))
	{
	DUMPM(result);
	int c = result[0].GetCount();
		for(int i=0; i < c; i++){
			EventSetup &es = eventSetup.GetAdd(atoi(~result[0][i]));
			es.eventID = atoi(~result[0][i]);
			es.bg_color = atoi(~result[1][i]);
			es.fg_color = atoi(~result[2][i]);
		
		}
	}
	select = "select LANGUAGE from SYSTEMUSER where LOGINNAME ='"+xmlsql.GetLoginName()+"'";
	if(xmlsql.Query(select,result))
		lang = atoi(~result[0][0]);
	else return false;
	
	select = "select USERTEXTNR, LAN1TEXT, LAN2TEXT, LAN3TEXT from USERTEXT where NAME ='TRANSTYPE'";
	if(xmlsql.Query(select,result)){
		//DUMPM(result);
		int c = result[0].GetCount();
		for(int i=0; i < c; i++){
			LOG(~result[1][i]+String(" -->") +AsString(i) +String(" --> atoi : ") + AsString (~result[0][i]));
		//	if(transSetup.Get Get(atoi(~result[0][i]))
			EventSetup &es = eventSetup.GetAdd(atoi(~result[0][i]));
			for (int j=1; j<result.GetCount(); ++j)
			es.name.Add(~result[j][i]);
			DUMPC(es.name);
		}
		
	}
	else return false;
	
	
	return true;
}

void EventsView::ToolBar(Bar& bar)
{
	
	tgrid.NavigatingBar(bar);
	bar.Separator();
	tgrid.Searching();
	tgrid.StdToolBar(bar);	
			
//	bar.Separator();
	counterBtn.Label(t_("Number of records: ")+ tgrid.GetVisibleCount());
	
//	bar.Add(counterBtn);//counterLabel,200,20);
	info.SetQTF("[/ Number of records: ]");
//	Size tsz = tgrid.Find();
	Size sz = bar.GetStdSize();
	bar.Add(info.LeftPosZ(1,400).TopPosZ(1,sz.cy),400+sz.cx+10,sz.cy+5);
}
