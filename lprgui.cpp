#include <QtWidgets>

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

int main(int argc, char **argv)
{
	QApplication			app(argc, argv);
	QDialog				window(nullptr,Qt::Dialog);
	QVBoxLayout			*vlayout=new QVBoxLayout;
	QHBoxLayout			*hlayout=new QHBoxLayout;
	QStringList			printers;
	QComboBox			plist;
	QComboBox			qual;
	QComboBox			size;
	QLabel				*label=new QLabel("Printers:");
	QLineEdit			filenames;
	QStringList			printFiles;
	QPushButton			*openfiles;
	QPushButton			*printTheFiles;
	QCommandLineParser	parser;
	int					quality=4;
	QStringList			paperSizes;
	QStringList			defaultSize;
	QCheckBox			landscape("Landscape");
	QCheckBox			monochrome("Monochrome");
	QLineEdit			customOptions;
	QLineEdit			copies("1");

	parser.addOptions(
		{
			{{"p","printer"},"Set printer to use","printername"},
			{{"q","quality"},"Use quality (draft|normal|best)","quality"},
			{{"s","size"},"Paper size","size"},
			{{"l","landscape"},"Set landscape"},
			{{"m","monochrome"},"Monochrome"},
			{{"o","options"},"Custom options","options"},
			{{"c","copies"},"Number of copies","copies"}
		});
	parser.process(app);

	window.setWindowTitle("Print");

	vlayout->setAlignment(Qt::AlignTop);
//get printers
	printers<<runPipeAndCapture("lpstat -p | awk '{print $2}'");
	plist.addItems(printers);
	hlayout->addWidget(label);
	hlayout->addWidget(&plist);
	vlayout->addLayout(hlayout);
	QObject::connect(&plist,QOverload<int>::of(&QComboBox::activated),[&defaultSize,&paperSizes,&size,&plist](int index)
		{
			paperSizes.clear();
			defaultSize<<runPipeAndCapture(QString("lpoptions -p %1 -l|grep \"PageSize\"|awk -F\"*\" '{print $2}'|awk '{print $1}'").arg(plist.currentText()));
			paperSizes<<runPipeAndCapture(QString("lpoptions -p %1 -l|grep \"PageSize\"|awk -F: '{print $2}'|sed 's/*//g'|sed 's/Custom.WIDTHxHEIGHT//g'").arg(plist.currentText()));
			paperSizes=paperSizes.at(0).split(" ");
			size.clear();
			size.addItems(paperSizes);
			size.setCurrentText(defaultSize.at(0));
		});

//quality
	qual.addItem("Draft");
	qual.addItem("Normal");
	qual.addItem("Best");	
	QObject::connect(&qual,QOverload<int>::of(&QComboBox::activated),[&quality](int index)
		{
			quality=index+3;
		});

	hlayout=new QHBoxLayout;
	label=new QLabel("Quality:");
	hlayout->addWidget(label);
	hlayout->addWidget(&qual);
	vlayout->addLayout(hlayout);

//paper size
	hlayout=new QHBoxLayout;
	label=new QLabel("Paper Size:");
	hlayout->addWidget(label);
	hlayout->addWidget(&size);
	vlayout->addLayout(hlayout);

//landscape mode
	hlayout=new QHBoxLayout;
	hlayout->addWidget(&landscape);
	hlayout->addWidget(&monochrome);
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
	hlayout->addWidget(&customOptions);
	vlayout->addLayout(hlayout);

//number of copies
	hlayout=new QHBoxLayout;
	label=new QLabel("Copies:\t");
	hlayout->addWidget(label);
	hlayout->addWidget(&copies);
	vlayout->addLayout(hlayout);

	hlayout=new QHBoxLayout;
	printTheFiles=new QPushButton("Print files");
	QObject::connect(printTheFiles,&QPushButton::clicked,[&printFiles,&filenames,&plist,&quality,&size,&landscape,&customOptions,&monochrome,&copies]()
		{
			QStringList args;
			QStringList coptions;
			printFiles.clear();
			printFiles=filenames.text().trimmed().split(':');
			for(int j=0;j<printFiles.count();j++)
				{
					args.clear();
					args<<"-P"<<plist.currentText();
					args<<"-o"<<QString("print-quality=%1").arg(quality);
					args<<"-o"<<QString("media=%1").arg(size.currentText());
					if(customOptions.text().isEmpty()==false)
						{
							coptions.clear();
							coptions=customOptions.text().split(':');
							for(int k=0;k<coptions.count();k++)
								args<<"-o"<<coptions.at(k);
						}
					if(landscape.isChecked()==true)
						args<<"-o"<<"orientation-requested=4";
					if(monochrome.isChecked()==true)
						args<<"-o"<<"print-color-mode=monochrome";
					else
						args<<"-o"<<"print-color-mode=color";
					args<<"-#"<<copies.text().trimmed();
					args<<printFiles.at(j).trimmed();
					QProcess::startDetached("lpr",args);
					qDebug()<<"lpr"<<args.join(" ");
				}
		});
	hlayout->addWidget(printTheFiles);
	vlayout->addLayout(hlayout);

//files to print
	printFiles=parser.positionalArguments();
	if(printFiles.count())
		filenames.setText(printFiles.join(':'));
//set printer
	if(parser.isSet("printer"))
		plist.setCurrentText(parser.value("printer"));
//set default paper size
	defaultSize<<runPipeAndCapture(QString("lpoptions -p %1 -l|grep \"PageSize\"|awk -F\"*\" '{print $2}'|awk '{print $1}'").arg(plist.currentText()));
	paperSizes<<runPipeAndCapture(QString("lpoptions -p %1 -l|grep \"PageSize\"|awk -F: '{print $2}'|sed 's/*//g'|sed 's/Custom.WIDTHxHEIGHT//g'").arg(plist.currentText()));
	paperSizes=paperSizes.at(0).split(" ");
	size.addItems(paperSizes);
	size.setCurrentText(defaultSize.at(0));
//set quality
	if(parser.isSet("quality"))
		{
			if(parser.value("quality").compare("draft")==0)
				quality=3;
			if(parser.value("quality").compare("normal")==0)
				quality=4;
			if(parser.value("quality").compare("best")==0)
				quality=5;
		}
	qual.setCurrentIndex(quality-3);
//set paper size
	if(parser.isSet("size"))
		size.setCurrentText(parser.value("size"));
//set landscape
	if(parser.isSet("landscape"))
		landscape.setChecked(true);
//set custom options
	if(parser.isSet("options"))
		customOptions.setText(parser.value("options"));
//set monochrome
	if(parser.isSet("monochrome"))
		monochrome.setChecked(true);
//set number of copies to print
	if(parser.isSet("copies"))
		copies.setText(parser.value("copies"));

	window.setMinimumWidth(400);
	window.setLayout(vlayout);
	window.adjustSize();
	window.show();

	return app.exec();
}