#include "Includes.hpp"

Widget::Widget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::Widget)
{
	ui->setupUi(this);

	// Affichage et immobilisation

	this->setFixedSize(this->width(), this->height());
	ui->comboBox->setEnabled(false);
	ui->pushButton->setEnabled(false);

	// Connexion des signaux
	QObject::connect (ui->_total_progress_bar, SIGNAL(valueChanged(int)), this, SLOT(loading_done(int)));
	QObject::connect (ui->_current_progress_bar, SIGNAL(valueChanged(int)), this, SLOT(current_loading_done(int)));
	QObject::connect (ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(server_selected(int)));
	QObject::connect (ui->pushButton, SIGNAL(clicked()), this, SLOT(add_server()));

	this->show();

	// Chargement

	ui->textEdit->append("<font color=#00CED1>GSRegister version: 1.0.0</font>");
	ui->textEdit->append("Loading ...");
	ui->textEdit->append("========================");

	QDomDocument doc;
	QFile file ("./data/servernames.xml");
	if (!file.open(QIODevice::ReadOnly))
		return;
	if (!doc.setContent(&file))
	{
		file.close();
		return;
	}
	file.close();

	QDomElement docElem = doc.documentElement();
	server_count = docElem.lastChild().lineNumber() - docElem.firstChild().lineNumber() + 1; // Nombre d'étapes
	int first = 1 - docElem.firstChild().lineNumber();
	QDomNode n = docElem.firstChild();
	while(!n.isNull())
	{
		QDomElement e = n.toElement(); // try to convert the node to an element.
		if(!e.isNull())
		{
			ui->_current_progress_bar->setValue(0);
			current_ID = n.lineNumber() + first;

			QString name = e.attribute("name");
			int ID = e.attribute("id").toInt();

			servers[current_ID] = std::make_pair(ID, name);

			ui->textEdit->append(QString ("Loaded [" + e.attribute("id") + "]\t" + name));

			if (name.length() > 11
				|| ID == 23 || ID == 6002 || ID == 7001)
				ui->comboBox->addItem(QString("[" + e.attribute("id") + "] " + name + "\ttype: " + e.attribute("type")));
			else
				ui->comboBox->addItem(QString("[" + e.attribute("id") + "] " + name + "\t\ttype: " + e.attribute("type")));

			ui->_current_progress_bar->setValue(100);
		}
		n = n.nextSibling();
	}
}

Widget::~Widget()
{
	delete ui;
}

void Widget::loading_done(int value)
{
	if (value != 100) return;
	ui->textEdit->append("========================");
	ui->textEdit->append("<font color=#32CD32>Loading Done.</font>");
	ui->comboBox->setItemText(0, "Select a server to add.");
	ui->comboBox->setEnabled(true);
}

void Widget::current_loading_done(int value)
{
	if (value != 100 || current_ID > server_count)
		return;

	ui->_total_progress_bar->setValue (int (100 * current_ID / server_count));
}

void Widget::server_selected(int index)
{
	if (index == 0)
	{
		ui->pushButton->setEnabled(false);
		return;
	}
	ui->pushButton->setEnabled(true);
}

void Widget::add_server()
{
	static bool first_init = true;
	if (first_init)
	{
		qsrand (time (NULL));
		first_init = false;
		ui->textEdit->clear();
	}

	QString key_char = "ABCDEF0123456789", key;
	int rnd = 0;

	for (int i = 0 ; i < key_char.length() ; ++i)
	{
		rnd = qrand() % (key_char.size() - 1);
		key += key_char.at (rnd);
	}

	std::string file_name = std::string("hexid(") + QString::number(servers[ui->comboBox->currentIndex()].first).toStdString() + std::string(").txt");
	std::ofstream file (file_name.c_str());
	if (!file);

	file << key.toStdString();

	ui->textEdit->append("File <" + QString::fromStdString(file_name) + "> has been created for " + servers[ui->comboBox->currentIndex()].second + " server ");
	ui->comboBox->setCurrentIndex(0);
}
