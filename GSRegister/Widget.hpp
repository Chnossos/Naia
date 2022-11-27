#ifndef WIDGET_HPP
#define WIDGET_HPP

#include <QWidget>
#include <map>

namespace Ui {
	class Widget;
}

class Widget : public QWidget
{
	Q_OBJECT

public:
	explicit Widget(QWidget *parent = 0);
	~Widget();

public slots:
	void loading_done(int);
	void current_loading_done(int);
	void server_selected(int);
	void add_server();

private:
	Ui::Widget *ui;
	int server_count;
	int current_ID;
	std::map<int, std::pair<int, QString> > servers;
};

#endif // WIDGET_HPP
