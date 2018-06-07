#include "ccAssistant.h"

void Connection::Xmlize(XmlIO& xml)
{
	xml
		("ServerIP", serverIP)
		("Port", port)
		("Login", login)
		("Password", password)
		("Description", description)
	;
}



ConnectionWnd::ConnectionWnd()
{
	
	//CtrlLayout(*this,t_("Connection Settings"));
	
	passwordEdit.Password();
	okButton.Ok() <<= Acceptor(IDOK);
	cancelButton.Cancel() <<=Rejector(IDCANCEL);
	
	portSwitch.Add(443,"HTTPS port: 443");
	portSwitch.Add(8443,"HTTPS port: 8443");
	portSwitch.Add(6060,"HTTP port: 6060");
	portSwitch <<= 443;
}

void ConnectionWnd::Add(Connection& conn)
{
	

}

void ConnectionWnd::Edit(Connection& conn)
{
	serveripEdit <<= conn.serverIP;
	passwordEdit <<= conn.password;
	loginEdit <<= conn.login;
	nameEdit <<= conn.description;
	portSwitch <<= conn.port;
}
