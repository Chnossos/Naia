#include "_Common.hpp"

Database::Database (int dbconn_count)
: conn_count(dbconn_count)
{
	connections = new Connection[conn_count];
	for (int i = 0 ; i < conn_count ; ++i)
		connections[i].connection = NULL;
}

Database::~Database ()
{
	for (int i = 0 ; i < conn_count ; ++i)
	{
		if (connections[i].connection == NULL)
			continue;
		mysql_close (connections[i].connection);
		connections[i].connection = NULL;
	}
	delete[] connections;
}

bool Database::connect (string db_host, int db_port, string db_name, string db_user, string db_pass)
{
	host = db_host;
	port = db_port;
	name = db_name;
	user = db_user;
	pass = db_pass;

	MYSQL * temp, * temp2;
	/*my_bool my_true = true;*/

	for (int i = 0 ; i < conn_count ; ++i)
	{
		temp = mysql_init (NULL);
		if (mysql_options (temp, MYSQL_SET_CHARSET_NAME, "utf8"))
		{
			cerr << endl;
			kernel.log.print_error ("MySQLDatabase: Could not set utf8 character set.\n");
		}

		/*if (mysql_options (temp, MYSQL_OPT_RECONNECT, &my_true))
			kernel.log.print_error ("MySQLDatabase: MYSQL_OPT_RECONNECT could not be set, connection drops may occur but will be counteracted.");*/

		temp2 = mysql_real_connect (temp, host.c_str(), user.c_str(), pass.c_str(), name.c_str(), port, NULL, 0);
		if (temp2 == NULL)
		{
			cerr << endl;
			kernel.log.print_error ("Connection failed : %s\n", mysql_error (temp));
			return false;
		}
		connections[i].connection = temp2;
	}
	return true;
}

bool Database::send_query (Connection * con, const char * query, bool self)
{
	int result = mysql_query (con->connection, query);
	if (result > 0)
	{
		if (self == false && handle_error (con, mysql_errno (con->connection)))
			result = send_query (con, query, true);
		else
			kernel.log.print_error ("Database: query failed : [%s]\nQuery: [%s]\n", mysql_error (con->connection), query);
	}

	return (result == 0 ? true : false);
}

bool Database::reconnect (Connection * conn)
{
	MYSQL * temp, * temp2;

	temp = mysql_init (NULL);
	temp2 = mysql_real_connect (temp, host.c_str(), user.c_str(), pass.c_str(), name.c_str(), port, NULL , 0);
	if (temp2 == NULL)
	{
		kernel.log.print_error ("Database: Could not reconnect to database :\n%s\n", mysql_error (temp));
		mysql_close (temp);
		return false;
	}

	if (conn->connection != NULL)
		mysql_close (conn->connection);

	conn->connection = temp;
	return true;
}

bool Database::handle_error (Connection * connection, int error_no)
{
	// Handle errors that should cause a reconnect to the Database.
	switch (error_no)
	{
		case 2006:  // Mysql server has gone away
		case 2008:  // Client ran out of memory
		case 2013:  // Lost connection to sql server during query
		case 2055:  // Lost connection to sql server - system error
		{
			// Let's instruct a reconnect to the db when we encounter these errors.
			return reconnect (connection);
		}
		break;
	}
	return false;
}

Connection * Database::get_free_connection ()
{
	int i = 0;
	for(;;)
	{
		Connection * con = &connections[ ((i++) % conn_count) ];
		if (con->conn_mutex.try_lock())
			return con;

		if( !(i % 20) )
			boost::this_thread::sleep (boost::posix_time::milliseconds(10));
	}
	return NULL;
}

bool Database::execute (const char * query, ...)
{
	char buffer[16384];

	va_list vlist;
	va_start (vlist, query);
	vsnprintf (buffer, 16384, query, vlist);
	va_end (vlist);

	Connection * con = get_free_connection();
	bool result = send_query (con, buffer, false);
	con->conn_mutex.unlock();

	return result;
}

bool Database::execute (const string & query)
{
	Connection * con = get_free_connection();
	bool result = send_query (con, query.c_str(), false);
	con->conn_mutex.unlock();

	return result;
}

QueryResult * Database::store_result (Connection * con)
{
	QueryResult * qResult;

	MYSQL_RES * result 	= mysql_store_result (con->connection);
	int rows 			= mysql_affected_rows (con->connection);
	int fields 			= mysql_field_count (con->connection);

	if (rows == 0 || fields == 0)
	{
		if (result != NULL)
			mysql_free_result (result);

		return NULL;
	}

	qResult = new QueryResult (result, fields, rows);
	qResult->next_row();
	return qResult;
}

QueryResult * Database::query (const char * query, ...)
{
	char sql[16384];
	va_list vlist;
	va_start (vlist, query);
	vsnprintf (sql, 16384, query, vlist);
	va_end (vlist);

	// Send the query
	QueryResult * qResult = NULL;
	Connection * con = get_free_connection();
	if (send_query (con, sql, false))
		qResult = store_result (con);
	con->conn_mutex.unlock();

	return qResult;
}

QueryResult * Database::query (const string & query)
{
	QueryResult * QR = NULL;
	Connection * con = get_free_connection();
	if (send_query (con, query.c_str(), false))
		QR = store_result (con);
	con->conn_mutex.unlock();

	return QR;
}

//////////////////////////////////////////////////////

QueryResult::QueryResult (MYSQL_RES * res, int fields, int rows) : result(res), field_count(fields), row_count(rows)
{
	current_row = new Field[fields];
}

QueryResult::~QueryResult ()
{
	mysql_free_result (result);
	delete [] current_row;
}

bool QueryResult::next_row ()
{
	MYSQL_ROW row = mysql_fetch_row (result);
	if (row == NULL)
		return false;

	for (int i = 0 ; i < field_count ; ++i)
		current_row[i].set_value (row[i]);

	return true;
}

