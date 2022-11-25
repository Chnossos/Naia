#ifndef DATABASE_H_INCLUDED
#define DATABASE_H_INCLUDED

struct Connection
{
	Connection () {}

	MYSQL * connection;
	boost::mutex conn_mutex;
};

class Field
{
	public:
		void set_value (char * val) { value = val; }

		template <typename T> T get_value ()
		{
			return Naia::from_string <T> (value);
		}

	private:
		char * value;
};

template <> inline bool Field::get_value <bool> ()
{
	return Naia::lower_case (value) == "true" ? true : false;
}

template <> inline string Field::get_value <string> ()
{
	return string (value);
}

class QueryResult
{
	public:
		QueryResult (MYSQL_RES * res, int fields, int rows);
		~QueryResult ();

		Field * fetch () const { return current_row; }
		int get_field_count () const { return field_count; }
		int get_row_count () const { return row_count; }
		bool next_row ();

	protected:
		MYSQL_RES * result;
		Field * current_row;
		int field_count;
		int row_count;
};

class Database
{
	public:
		Database (int dbconn_count);
		~Database ();
		bool connect (string db_host, int db_port, string db_name, string db_user, string db_pass);

		QueryResult * query (const  char * query, ...);
		QueryResult * query (const string & query);
		bool execute (const char * query, ...);
		bool execute (const string & query);

	private:
	// Var
		Connection * connections;
		string host, name, user, pass;
		int port, conn_count;

	// Functions
		Connection * get_free_connection ();
		QueryResult * store_result (Connection *);
		bool reconnect (Connection *);
		bool handle_error (Connection *, int error_no);
		bool send_query (Connection *, const char * query, bool self);
};

#endif // DATABASE_H_INCLUDED
