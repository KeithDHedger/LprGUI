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

QComboBox	*plist;
QComboBox	*qual;
QComboBox	*size;
QCheckBox	*landscape;
QCheckBox	*monochrome;
QLineEdit	*customOptions;
QLineEdit	*copies;
QStringList	paperSizes;
QStringList	defaultSize;
bool			resetPaperSizes=false;

QSettings	prefs("KDHedger","LprGUI");

QStringList runPipeAndCapture(QString command)
{
	QStringList	dump;
	FILE		*fp=NULL;
	char		line[1024];

	dump.clear();
	fp=popen(command.toStdString().c_str(), "r");
	if(fp!=NULL)
		{
			while(fgets(line,1024,fp))
				dump<<QString(line).trimmed();
			pclose(fp);
		}
	return(dump);
}

void remakePaperSizes(void)
{
	size->clear();
	paperSizes.clear();
	defaultSize<<runPipeAndCapture(QString("lpoptions -p %1 -l|grep \"PageSize\"|awk -F\"*\" '{print $2}'|awk '{print $1}'").arg(plist->currentText()));
	paperSizes<<runPipeAndCapture(QString("lpoptions -p %1 -l|grep \"PageSize\"|awk -F: '{print $2}'|sed 's/*//g'|sed 's/Custom.WIDTHxHEIGHT//g'").arg(plist->currentText()));
	paperSizes=paperSizes.at(0).split(" ");
	size->addItems(paperSizes);
	size->setCurrentText(defaultSize.at(0));
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
}

void retsoreSettings(void)
{
/*
//editor
	this->prefsFunctionMenuLayout=this->prefs.value("editor/funcsort",4).toInt();
	this->prefsDepth=this->prefs.value("editor/prefsdepth",1).toInt();
	this->prefsToolBarLayout=this->prefs.value("editor/toolbarlayout","NSOsURsBWsFGsE9ELEDEE").toString();
	this->prefsMaxTabChars=this->prefs.value("editor/maxtabchars",20).toInt();
	this->prefsMaxMenuChars=this->prefs.value("editor/maxfuncchars",64).toInt();
	this->prefsTerminalCommand=this->prefs.value("editor/terminalcommand","xterm -e").toString();
	this->prefsRootCommand=this->prefs.value("editor/rootcommand","gtksu -- ").toString();
	this->prefsQtDocDir=this->prefs.value("editor/qtdocdir","/usr/share/doc/qt5").toString();
	this->prefsNoOpenduplicate=this->prefs.value("editor/noopendup",QVariant(bool(true))).value<bool>();
	this->prefsNoWarnings=this->prefs.value("editor/nowarnings",QVariant(bool(false))).value<bool>();
	this->recentFiles->maxFiles=this->prefs.value("editor/maxrecents",10).toInt();

*/
	size->setCurrentText(prefs.value("size","A4").toString());
	if(prefs.contains("printer")==true)
		{
			plist->setCurrentText(prefs.value("printer").toString());
			resetPaperSizes=true;
		}
	qual->setCurrentText(prefs.value("quality","Normal").toString());

//	plist->setCurrentText(prefs.value("printer"));
//	plist->setCurrentText(prefs.value("printer"));
//	plist->setCurrentText(prefs.value("printer"));
//	plist->setCurrentText(prefs.value("printer"));
//	plist->setCurrentText(prefs.value("printer"));
//	prefs.setValue("printer",);
//	prefs.setValue("quality",qual->currentText());

//	prefs.setValue("size",size->currentText());
//	prefs.setValue("landscape",landscape->isChecked());
//	prefs.setValue("monochrome",monochrome->isChecked());
//	prefs.setValue("options",customOptions->text().trimmed());
//	prefs.setValue("copies",copies->text().trimmed());
}

int main(int argc, char **argv)
{
	QApplication			app(argc, argv);
	QDialog				window(nullptr,Qt::Dialog);
	QVBoxLayout			*vlayout=new QVBoxLayout;
	QHBoxLayout			*hlayout=new QHBoxLayout;
	QStringList			printers;
	//QComboBox			plist;
//	QComboBox			qual;
//	QComboBox			size;
	QLabel				*label=new QLabel("Printers:");
	QLineEdit			filenames;
	QStringList			printFiles;
	QPushButton			*openfiles;
	QPushButton			*printTheFiles;
	QCommandLineParser	parser;
	int					quality=4;
//	QStringList			paperSizes;
//	QStringList			defaultSize;
//	QCheckBox			landscape("Landscape");
//	QCheckBox			monochrome("Monochrome");
//	QLineEdit			customOptions;
//	QLineEdit			copies("1");
	QPushButton			*button;

	plist=new QComboBox;
	qual=new QComboBox;
	size=new QComboBox;
	landscape=new QCheckBox("Landscape");
	monochrome=new QCheckBox("Monochrome");
	customOptions=new QLineEdit("");
	copies=new QLineEdit("1");

	app.setOrganizationName("KDHedger");
	app.setApplicationName("LprGui");

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
	parser.process(app);

	window.setWindowTitle("Print");

	vlayout->setAlignment(Qt::AlignTop);
//get printers
	printers<<runPipeAndCapture("lpstat -p | awk '{print $2}'");
	plist->addItems(printers);
	hlayout->addWidget(label);
	hlayout->addWidget(plist);
	vlayout->addLayout(hlayout);
	QObject::connect(plist,QOverload<int>::of(&QComboBox::activated),[](int index)
		{
			remakePaperSizes();
		});

//quality
	qual->addItem("Draft");
	qual->addItem("Normal");
	qual->addItem("Best");	
	QObject::connect(qual,QOverload<int>::of(&QComboBox::activated),[&quality](int index)
		{
			quality=index+3;
		});

	hlayout=new QHBoxLayout;
	label=new QLabel("Quality:");
	hlayout->addWidget(label);
	hlayout->addWidget(qual);
	vlayout->addLayout(hlayout);

//paper size
	hlayout=new QHBoxLayout;
	label=new QLabel("Paper Size:");
	hlayout->addWidget(label);
	hlayout->addWidget(size);
	vlayout->addLayout(hlayout);

//landscape mode
	hlayout=new QHBoxLayout;
	hlayout->addWidget(landscape);
	hlayout->addWidget(monochrome);
	vlayout->addLayout(hlayout);

//files
	hlayout=new QHBoxLayout;
	openfiles=new QPushButton("Select files");
	QObject::connect(openfiles,&QPushButton::clicked,[&window,&filenames,&printFiles]()
		{
			QFileDialog dialog(nullptr,Qt::Dialog|Qt::WindowStaysOnTopHint);
			dialog.setWindowModality(Qt::WindowModal);
			dialog.setFileMode(QFileDialog::AnyFile);
			printFiles=dialog.getOpenFileNames((QDialog *)&window,"Open Files");
			if(printFiles.count())
				filenames.setText(printFiles.join(':'));
		});

	hlayout->addWidget(openfiles);
	hlayout->addWidget(&filenames);
	vlayout->addLayout(hlayout);

//options
	hlayout=new QHBoxLayout;
	label=new QLabel("Options:\t");
	hlayout->addWidget(label);
	hlayout->addWidget(customOptions);
	vlayout->addLayout(hlayout);

//number of copies
	hlayout=new QHBoxLayout;
	label=new QLabel("Copies:\t");
	hlayout->addWidget(label);
	hlayout->addWidget(copies);
	vlayout->addLayout(hlayout);

	hlayout=new QHBoxLayout;
	label=new QLabel;
	label->setFrameStyle(QFrame::Sunken | QFrame::HLine);
	vlayout->addWidget(label);

//quit
	hlayout=new QHBoxLayout;
	button=new QPushButton("Quit");
	QObject::connect(button,&QPushButton::clicked,[&app]()
		{
			app.quit();
		});
	hlayout->addWidget(button);
	hlayout->addStretch();

//print
	printTheFiles=new QPushButton("Print files");
	QObject::connect(printTheFiles,&QPushButton::clicked,[&printFiles,&filenames,&quality]()
		{
			QStringList args;
			QStringList coptions;
			printFiles.clear();
			printFiles=filenames.text().trimmed().split(':');
			for(int j=0;j<printFiles.count();j++)
				{
					args.clear();
					args<<"-P"<<plist->currentText();
					args<<"-o"<<QString("print-quality=%1").arg(quality);
					args<<"-o"<<QString("media=%1").arg(size->currentText());
					if(customOptions->text().isEmpty()==false)
						{
							coptions.clear();
							coptions=customOptions->text().split(':');
							for(int k=0;k<coptions.count();k++)
								args<<"-o"<<coptions.at(k);
						}
					if(landscape->isChecked()==true)
						args<<"-o"<<"orientation-requested=4";
					if(monochrome->isChecked()==true)
						args<<"-o"<<"print-color-mode=monochrome";
					else
						args<<"-o"<<"print-color-mode=color";
					args<<"-#"<<copies->text().trimmed();
					args<<printFiles.at(j).trimmed();
					QProcess::startDetached("lpr",args);
					qDebug()<<"lpr"<<args.join(" ");
				}
		});
	hlayout->addWidget(printTheFiles);
	vlayout->addLayout(hlayout);

//set default paper size
	size->clear();
	defaultSize<<runPipeAndCapture(QString("lpoptions -p %1 -l|grep \"PageSize\"|awk -F\"*\" '{print $2}'|awk '{print $1}'").arg(plist->currentText()));
	paperSizes<<runPipeAndCapture(QString("lpoptions -p %1 -l|grep \"PageSize\"|awk -F: '{print $2}'|sed 's/*//g'|sed 's/Custom.WIDTHxHEIGHT//g'").arg(plist->currentText()));
	paperSizes=paperSizes.at(0).split(" ");
	size->addItems(paperSizes);
	size->setCurrentText(defaultSize.at(0));

	retsoreSettings();
	if(resetPaperSizes==true)
		{
			size->clear();
			paperSizes.clear();
			paperSizes<<runPipeAndCapture(QString("lpoptions -p %1 -l|grep \"PageSize\"|awk -F: '{print $2}'|sed 's/*//g'|sed 's/Custom.WIDTHxHEIGHT//g'").arg(plist->currentText()));
			paperSizes=paperSizes.at(0).split(" ");
			size->addItems(paperSizes);
			size->setCurrentText(prefs.value("size","A4").toString());
		}
	if(qual->currentText().compare("draft",Qt::CaseInsensitive)==0)
		quality=3;
	if(qual->currentText().compare("normal",Qt::CaseInsensitive)==0)
		quality=4;
	if(qual->currentText().compare("best",Qt::CaseInsensitive)==0)
		quality=5;
//	defaultSize<<runPipeAndCapture(QString("lpoptions -p %1 -l|grep \"PageSize\"|awk -F\"*\" '{print $2}'|awk '{print $1}'").arg(plist->currentText()));
//	paperSizes<<runPipeAndCapture(QString("lpoptions -p %1 -l|grep \"PageSize\"|awk -F: '{print $2}'|sed 's/*//g'|sed 's/Custom.WIDTHxHEIGHT//g'").arg(plist->currentText()));
//	paperSizes=paperSizes.at(0).split(" ");
//	size->clear();
//	size->addItems(paperSizes);
//	size->setCurrentText(defaultSize.at(0));
//	retsoreSettings();

//			paperSizes.clear();
//			defaultSize<<runPipeAndCapture(QString("lpoptions -p %1 -l|grep \"PageSize\"|awk -F\"*\" '{print $2}'|awk '{print $1}'").arg(plist->currentText()));
//			paperSizes<<runPipeAndCapture(QString("lpoptions -p %1 -l|grep \"PageSize\"|awk -F: '{print $2}'|sed 's/*//g'|sed 's/Custom.WIDTHxHEIGHT//g'").arg(plist->currentText()));
//			paperSizes=paperSizes.at(0).split(" ");
//			size->clear();
//			size->addItems(paperSizes);
//			size->setCurrentText(defaultSize.at(0));





//files to print
	printFiles=parser.positionalArguments();
	if(printFiles.count())
		filenames.setText(printFiles.join(':'));
//set printer
	if(parser.isSet("printer"))
		{
			plist->setCurrentText(parser.value("printer"));
			remakePaperSizes();
		}
//set quality
	if(parser.isSet("quality"))
		{
			if(parser.value("quality").compare("draft",Qt::CaseInsensitive)==0)
				quality=3;
			if(parser.value("quality").compare("normal",Qt::CaseInsensitive)==0)
				quality=4;
			if(parser.value("quality").compare("best",Qt::CaseInsensitive)==0)
				quality=5;
			qual->setCurrentIndex(quality-3);
		}
//set paper size
	if(parser.isSet("size"))
		size->setCurrentText(parser.value("size"));
//set landscape
	if(parser.isSet("landscape"))
		landscape->setChecked(true);
//set custom options
	if(parser.isSet("options"))
		customOptions->setText(parser.value("options"));
//set monochrome
	if(parser.isSet("monochrome"))
		monochrome->setChecked(true);
//set number of copies to print
	if(parser.isSet("copies"))
		copies->setText(parser.value("copies"));

	window.setMinimumWidth(400);
	window.setLayout(vlayout);
	window.adjustSize();
	window.show();

	app.exec();

	saveSettings();

	return 0;
}