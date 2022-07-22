/*
 *
 * ©K. D. Hedger. Sun 17 Jul 16:00:55 BST 2022 keithdhedger@gmail.com

 * This file (lprgui.cpp) is part of LprGUI.

 * LprGUI is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * Projects is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LprGUI.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtWidgets>

QDialog		*window;
QComboBox	*plist;
QComboBox	*qual;
QComboBox	*size;
QCheckBox	*landscape;
QCheckBox	*monochrome;
QLineEdit	*customOptions;
QLineEdit	*copies;
QLineEdit	*fileNames;

QComboBox	*opts;
QDialog		*optionsWindow;
QLineEdit	*setoption;
QCheckBox	*useoption;
QLineEdit	*option;


QSettings	prefs("KDHedger","LprGUI");

QStringList runPipeAndCapture(QString command)
{
	QStringList	dump;
	FILE		*fp=NULL;
	char		line[2048];

	dump.clear();
	fp=popen(command.toStdString().c_str(), "r");
	if(fp!=NULL)
		{
			while(fgets(line,2048,fp))
				dump<<QString(line).trimmed();
			pclose(fp);
		}
	return(dump);
}

void remakePaperSizes(void)
{
	QStringList	defaultpaper;
	QStringList	papersizes;

	size->clear();
	defaultpaper<<runPipeAndCapture(QString("lpoptions -p %1 -l|grep \"PageSize\"|awk -F\"*\" '{print $2}'|awk '{print $1}'").arg(plist->currentText()));
	papersizes<<runPipeAndCapture(QString("lpoptions -p %1 -l|grep \"PageSize\"|awk -F: '{print $2}'|sed 's/*//g'|sed 's/Custom.WIDTHxHEIGHT//g'").arg(plist->currentText()));
	papersizes=papersizes.at(0).split(" ");
	size->addItems(papersizes);
	size->setCurrentText(defaultpaper.at(0));
}

void saveSettings(void)
{
	prefs.setValue("printer",plist->currentText());
	prefs.setValue("quality",qual->currentText());
	prefs.setValue("size",size->currentText());
	prefs.setValue("landscape",landscape->isChecked());
	prefs.setValue("monochrome",monochrome->isChecked());
	prefs.setValue("options",customOptions->text().trimmed());
	prefs.setValue("copies",copies->text().trimmed());
	prefs.setValue("geometry",window->saveGeometry());
}

void doPrint(void)
{
	QStringList args;
	QStringList coptions;
	QStringList filestoprint;

	filestoprint=fileNames->text().trimmed().split(':');
	if(customOptions->text().isEmpty()==false)
		{
			coptions.clear();
			coptions=customOptions->text().split(':');
		}

	for(int j=0;j<filestoprint.count();j++)
		{
			args.clear();

			args<<"-P"<<plist->currentText();
			args<<"-o"<<QString("print-quality=%1").arg(qual->currentIndex()+3);
			args<<"-o"<<QString("media=%1").arg(size->currentText());
			if(landscape->isChecked()==true)
				args<<"-o"<<"orientation-requested=4";
			if(monochrome->isChecked()==true)
				args<<"-o"<<"print-color-mode=monochrome";
			else
				args<<"-o"<<"print-color-mode=color";
			args<<"-#"<<copies->text().trimmed();
			if(customOptions->text().isEmpty()==false)
				{
					for(int k=0;k<coptions.count();k++)
						args<<"-o"<<coptions.at(k);
				}

			args<<filestoprint.at(j).trimmed();
			QProcess::startDetached("lpr",args);
//qDebug()<<"lpr"<<args.join(" ");
		}
}

void setOptionsCombo(void)
{
	QStringList options=runPipeAndCapture(QString("lpoptions -p '%1'|sed -n \"s|\\([^=]*\\)=\\([^' ]*\\s\\)|\\1=\\2\\n|pg\"|awk -F\"'\" '{print $1 $2 \"\\n\" $NF}'|sort -u|awk '{$1=$1};NF'").arg(plist->currentText()));	

	opts->clear();
	for(int j=0;j<options.count();j++)
		{
			opts->addItem(options.at(j).split('=').at(0));
			opts->setItemData(j,QString("0%1").arg(options.at(j).split('=').at(1)));
		}
	useoption->setChecked(false);
	customOptions->setText("");
}

void buildOptionsDialog(void)
{
	QVBoxLayout	*vlayout=new QVBoxLayout;
	QHBoxLayout	*hlayout;
	QPushButton	*button;
	QLabel		*label;
	optionsWindow=new QDialog(nullptr,Qt::Dialog);

	optionsWindow->setWindowTitle("Select Options");
	optionsWindow->setWindowModality(Qt::ApplicationModal);

	opts=new QComboBox;

	QObject::connect(opts,QOverload<int>::of(&QComboBox::activated),[](int index)
		{
			setoption->setText(opts->currentData().toString().right(opts->currentData().toString().length()-1));
			if(opts->currentData().toString().at(0)=='1')
				useoption->setChecked(true);
			else
				useoption->setChecked(false);
		});

	useoption=new QCheckBox("Use this");
	QObject::connect(useoption,&QCheckBox::stateChanged,[](int state)
		{
			if(useoption->isChecked()==true)
				opts->setItemData(opts->currentIndex(),QString("1%1").arg(setoption->text()));
			else
				opts->setItemData(opts->currentIndex(),QString("0%1").arg(setoption->text()));
		});

	setOptionsCombo();

	vlayout->setAlignment(Qt::AlignTop);
	vlayout->addWidget(opts);
	hlayout=new QHBoxLayout;
	hlayout->addWidget(useoption);
	setoption=new QLineEdit("");
	hlayout->addWidget(setoption);
	QObject::connect(setoption,&QLineEdit::textEdited,[](QString text)
		{
			if(useoption->isChecked()==true)
				opts->setItemData(opts->currentIndex(),QString("1%1").arg(text));
			else
				opts->setItemData(opts->currentIndex(),QString("0%1").arg(text));
		});

	vlayout->addLayout(hlayout);

//seperator
	hlayout=new QHBoxLayout;
	label=new QLabel;
	label->setFrameStyle(QFrame::Sunken | QFrame::HLine);
	vlayout->addWidget(label);

//buttons
//apply
	hlayout=new QHBoxLayout;
	button=new QPushButton("Apply");
	QObject::connect(button,&QPushButton::clicked,[]()
		{
			QString	co="";
			optionsWindow->hide();
			customOptions->clear();
			for(int j=0;j<opts->count();j++)
				{
					if(opts->itemData(j).toString().at(0)=='1')
						{
							co+=QString("%1=%2:").arg(opts->itemText(j)).arg(opts->itemData(j).toString().right(opts->itemData(j).toString().length()-1));
						}
				}
			customOptions->setText(co);
		});
	hlayout->addWidget(button);
	hlayout->addStretch();

//cancel
	button=new QPushButton("Cancel");
	QObject::connect(button,&QPushButton::clicked,[]()
		{
			optionsWindow->hide();
			
		});
	hlayout->addWidget(button);
	vlayout->addLayout(hlayout);

	optionsWindow->setLayout(vlayout);
	//optionsWindow->resize(480,200);
}

int main(int argc, char **argv)
{
	QApplication			app(argc, argv);
	QVBoxLayout			*vlayout=new QVBoxLayout;
	QHBoxLayout			*hlayout=new QHBoxLayout;
	QLabel				*label=new QLabel("Printers:");
	int					retval=0;
	QCommandLineParser	parser;
	QPushButton			*button;
	QStringList			printerslist;
	QStringList			fileslist;

	window=new QDialog(nullptr,Qt::Dialog);
	plist=new QComboBox;
	qual=new QComboBox;
	size=new QComboBox;
	landscape=new QCheckBox("Landscape");
	monochrome=new QCheckBox("Monochrome");
	customOptions=new QLineEdit("");
	copies=new QLineEdit("1");
	fileNames=new QLineEdit("");

	app.setOrganizationName("KDHedger");
	app.setApplicationName("LprGui");
	app.setApplicationVersion("0.0.2");

	parser.addOptions(
		{
			{{"p","printer"},"Set printer to use","printername"},
			{{"q","quality"},"Use quality (draft|normal|best)","quality"},
			{{"s","size"},"Paper size","size"},
			{{"l","landscape"},"Set landscape"},
			{{"m","monochrome"},"Monochrome"},
			{{"o","options"},"Custom options","option1:option2:...:optionN"},
			{{"c","copies"},"Number of copies","copies"}
		});
	parser.addHelpOption();
	parser.addVersionOption();
	parser.process(app);

	window->setWindowTitle("Lpr Print GUI");

	vlayout->setAlignment(Qt::AlignTop);

	printerslist<<runPipeAndCapture("lpstat -p | awk '{print $2}'");
	plist->addItems(printerslist);
	hlayout->addWidget(label);
	hlayout->addWidget(plist);
	vlayout->addLayout(hlayout);
	QObject::connect(plist,QOverload<int>::of(&QComboBox::activated),[](int index)
		{
			remakePaperSizes();
			setOptionsCombo();
		});

//quality
	hlayout=new QHBoxLayout;
	label=new QLabel("Quality:");
	hlayout->addWidget(label);
	qual->addItem("Draft");
	qual->addItem("Normal");
	qual->addItem("Best");	
	hlayout->addWidget(qual);
	vlayout->addLayout(hlayout);

//paper size
	hlayout=new QHBoxLayout;
	label=new QLabel("Paper Size:");
	hlayout->addWidget(label);
	hlayout->addWidget(size);
	vlayout->addLayout(hlayout);

//landscape monchrome mode
	hlayout=new QHBoxLayout;
	hlayout->addWidget(landscape);
	hlayout->addWidget(monochrome);
	vlayout->addLayout(hlayout);

//files
	hlayout=new QHBoxLayout;
	button=new QPushButton("  Select  files  ");
	QObject::connect(button,&QPushButton::clicked,[]()
		{
			QStringList	files;
			QFileDialog dialog(nullptr,Qt::Dialog|Qt::WindowStaysOnTopHint);
			dialog.setWindowModality(Qt::WindowModal);
			dialog.setFileMode(QFileDialog::AnyFile);
			files=dialog.getOpenFileNames(window,"Open Files");
			if(files.count())
				fileNames->setText(files.join(':'));
		});
	hlayout->addWidget(button);
	hlayout->addWidget(fileNames);
	vlayout->addLayout(hlayout);

//options
	hlayout=new QHBoxLayout;
	button=new QPushButton("Select options");
	QObject::connect(button,&QPushButton::clicked,[]()
		{
			optionsWindow->show();
		});

	hlayout->addWidget(button);
	hlayout->addWidget(customOptions);
	vlayout->addLayout(hlayout);

//number of copies
	hlayout=new QHBoxLayout;
	label=new QLabel("Copies:\t");
	hlayout->addWidget(label);
	hlayout->addWidget(copies);
	vlayout->addLayout(hlayout);

//seperator
	hlayout=new QHBoxLayout;
	label=new QLabel;
	label->setFrameStyle(QFrame::Sunken | QFrame::HLine);
	vlayout->addWidget(label);

//buttons
//quit
	hlayout=new QHBoxLayout;
	button=new QPushButton("Quit");
	QObject::connect(button,&QPushButton::clicked,[&app]()
		{
			app.quit();
		});
	hlayout->addWidget(button);
	hlayout->addStretch();

//print and quit
	button=new QPushButton("Print files and quit");
	QObject::connect(button,&QPushButton::clicked,[&app]()
		{
			doPrint();
			app.quit();
		});
	hlayout->addWidget(button);
	hlayout->addStretch();

//print
	button=new QPushButton("Print files");
	QObject::connect(button,&QPushButton::clicked,[]()
		{
			doPrint();
		});
	hlayout->addWidget(button);
	vlayout->addLayout(hlayout);

//set options
//set printer
	if(prefs.contains("printer")==true)
		plist->setCurrentText(prefs.value("printer").toString());

	if(parser.isSet("printer"))
		plist->setCurrentText(parser.value("printer"));

	remakePaperSizes();

//set quality
	if(prefs.contains("quality")==true)
		qual->setCurrentIndex(qual->findText(prefs.value("quality","Normal").toString(),Qt::MatchFixedString));
	if(parser.isSet("quality"))
		qual->setCurrentIndex(qual->findText(parser.value("quality"),Qt::MatchFixedString));

//set size
	if(prefs.contains("size")==true)
		size->setCurrentIndex(size->findText(prefs.value("size","A4").toString(),Qt::MatchFixedString));
	if(parser.isSet("size"))
		size->setCurrentIndex(size->findText(parser.value("size"),Qt::MatchFixedString));

//set modes
	if(prefs.contains("landscape")==true)
		landscape->setChecked(prefs.value("landscape",QVariant(bool(false))).value<bool>());
	if(parser.isSet("landscape"))
		landscape->setChecked(true);
	if(prefs.contains("monochrome")==true)
		monochrome->setChecked(prefs.value("monochrome",QVariant(bool(false))).value<bool>());
	if(parser.isSet("monochrome"))
		monochrome->setChecked(true);

//set filelist
	fileslist=parser.positionalArguments();
	if(fileslist.count())
		fileNames->setText(fileslist.join(':'));

//set custom options
	if(prefs.contains("options")==true)
		customOptions->setText(prefs.value("options").toString());
	if(parser.isSet("options"))
		customOptions->setText(parser.value("options"));

//set copies
	if(prefs.contains("copies")==true)
		copies->setText(prefs.value("copies").toString());
	if(parser.isSet("copies"))
		copies->setText(parser.value("copies"));

	window->setLayout(vlayout);
	if(window->restoreGeometry(prefs.value("geometry").toByteArray())==false)
		window->resize(480,200);

	buildOptionsDialog();

	window->show();

	retval=app.exec();

	saveSettings();

	return(retval);
}