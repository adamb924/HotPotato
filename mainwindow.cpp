#include <QtGui>
#include <QtDebug>

#include "mainwindow.h"

//![0]
MainWindow::MainWindow()
{
    averagePlayTime = 15;
    maximumVariability = 10;
    pauseTime = 5;
    minimumPlayTime = 3;
    maximumPlayTime = 30;

    musicTimer = new QTimer(this);
    pauseTimer = new QTimer(this);
    musicTimer->setSingleShot(true);
    pauseTimer->setSingleShot(true);
    connect(musicTimer,SIGNAL(timeout()), this, SLOT(pauseMusic()));
    connect(pauseTimer,SIGNAL(timeout()),this, SLOT(playMusic()));


    audioOutput = new Phonon::AudioOutput(Phonon::MusicCategory, this);
    mediaObject = new Phonon::MediaObject(this);
    metaInformationResolver = new Phonon::MediaObject(this);

    mediaObject->setTickInterval(1000);
//![0]
//![2]
    connect(mediaObject, SIGNAL(tick(qint64)), this, SLOT(tick(qint64)));
    connect(mediaObject, SIGNAL(stateChanged(Phonon::State, Phonon::State)),
	    this, SLOT(stateChanged(Phonon::State, Phonon::State)));
    connect(metaInformationResolver, SIGNAL(stateChanged(Phonon::State,Phonon::State)),
	    this, SLOT(metaStateChanged(Phonon::State, Phonon::State)));
    connect(mediaObject, SIGNAL(currentSourceChanged(const Phonon::MediaSource &)),
	    this, SLOT(sourceChanged(const Phonon::MediaSource &)));
    connect(mediaObject, SIGNAL(aboutToFinish()), this, SLOT(aboutToFinish()));
//![2]

//![1]
    Phonon::createPath(mediaObject, audioOutput);
//![1]

    setupActions();
    setupMenus();
    setupUi();
}

//![6]
void MainWindow::addFiles()
{
    QStringList files = QFileDialog::getOpenFileNames(this, tr("Select Music Files"),
	QDesktopServices::storageLocation(QDesktopServices::MusicLocation));

    if (files.isEmpty())
	return;

    int index = sources.size();
    foreach (QString string, files) {
	    Phonon::MediaSource source(string);

	sources.append(source);
    }
    if (!sources.isEmpty())
	metaInformationResolver->setCurrentSource(sources.at(index));

}
//![6]

void MainWindow::about()
{
    QMessageBox::information(this, tr("About Hot Potato"),
	tr("Hot Potato is a simple application for playing hot potato. It\'s based on Qt\'s qmusicplayer demo."));
}

//![9]
void MainWindow::stateChanged(Phonon::State newState, Phonon::State /* oldState */)
{
    switch (newState) {
/*
    case Phonon::ErrorState:
	    if (mediaObject->errorType() == Phonon::FatalError) {
		QMessageBox::warning(this, tr("Fatal Error"),
		mediaObject->errorString());
	    } else {
		QMessageBox::warning(this, tr("Error"),
		mediaObject->errorString());
	    }
	    break;
	case Phonon::PlayingState:
		playAction->setEnabled(false);
		pauseAction->setEnabled(true);
		break;
*/
	case Phonon::StoppedState:
	    pause();
	    break;
    case Phonon::PausedState:
	    pause();
		break;
		/*
	case Phonon::BufferingState:
		break;
*/
	default:
	    ;
    }
}

//![11]
void MainWindow::tick(qint64 time)
{
    QTime displayTime(0, (time / 60000) % 60, (time / 1000) % 60);
}
//![11]

//![12]
void MainWindow::tableClicked(int row, int /* column */)
{
    bool wasPlaying = mediaObject->state() == Phonon::PlayingState;

    mediaObject->stop();
    mediaObject->clearQueue();

    if (row >= sources.size())
	return;

    mediaObject->setCurrentSource(sources[row]);

    if (wasPlaying)
	mediaObject->play();
    else
	mediaObject->stop();
}
//![12]

//![13]
void MainWindow::sourceChanged(const Phonon::MediaSource &source)
{
    musicTable->selectRow(sources.indexOf(source));
}
//![13]

//![14]
void MainWindow::metaStateChanged(Phonon::State newState, Phonon::State /* oldState */)
{
    if (newState == Phonon::ErrorState) {
	QMessageBox::warning(this, tr("Error opening files"),
	    metaInformationResolver->errorString());
	while (!sources.isEmpty() &&
	       !(sources.takeLast() == metaInformationResolver->currentSource())) {}  /* loop */;
	return;
    }

    if (newState != Phonon::StoppedState && newState != Phonon::PausedState)
	return;

    if (metaInformationResolver->currentSource().type() == Phonon::MediaSource::Invalid)
	    return;

    QMap<QString, QString> metaData = metaInformationResolver->metaData();

    QString title = metaData.value("TITLE");
    if (title == "")
	title = metaInformationResolver->currentSource().fileName();

    QTableWidgetItem *titleItem = new QTableWidgetItem(title);
    titleItem->setFlags(titleItem->flags() ^ Qt::ItemIsEditable);
//![14]

    int currentRow = musicTable->rowCount();
    musicTable->insertRow(currentRow);
    musicTable->setItem(currentRow, 0, titleItem);


//![15]
    if (musicTable->selectedItems().isEmpty()) {
	musicTable->selectRow(0);
	mediaObject->setCurrentSource(metaInformationResolver->currentSource());
    }

    Phonon::MediaSource source = metaInformationResolver->currentSource();
    int index = sources.indexOf(metaInformationResolver->currentSource()) + 1;
    if (sources.size() > index) {
	metaInformationResolver->setCurrentSource(sources.at(index));
    }
    else {
	musicTable->resizeColumnsToContents();
	if (musicTable->columnWidth(0) > 300)
	    musicTable->setColumnWidth(0, 300);
    }
}
//![15]

//![16]
void MainWindow::aboutToFinish()
{
    int index = sources.indexOf(mediaObject->currentSource()) + 1;
    if (sources.size() > index) {
	mediaObject->enqueue(sources.at(index));
    }
}
//![16]

void MainWindow::setupActions()
{
    settingsAction = new QAction(tr("Settings"),this);
    settingsAction->setShortcut(tr("Ctrl+S"));
    settingsAction->setDisabled(false);

    playAction = new QAction(style()->standardIcon(QStyle::SP_MediaPlay), tr("Play"), this);
    playAction->setShortcut(tr("Ctrl+P"));
    playAction->setEnabled(true);
    pauseAction = new QAction(style()->standardIcon(QStyle::SP_MediaPause), tr("Pause"), this);
    pauseAction->setShortcut(tr("Ctrl+A"));
    pauseAction->setDisabled(true);
    nextAction = new QAction(style()->standardIcon(QStyle::SP_MediaSkipForward), tr("Next"), this);
    nextAction->setShortcut(tr("Ctrl+N"));
    previousAction = new QAction(style()->standardIcon(QStyle::SP_MediaSkipBackward), tr("Previous"), this);
    previousAction->setShortcut(tr("Ctrl+R"));
    addFilesAction = new QAction(tr("Add &Files"), this);
    addFilesAction->setShortcut(tr("Ctrl+F"));
    exitAction = new QAction(tr("E&xit"), this);
    exitAction->setShortcuts(QKeySequence::Quit);
    aboutAction = new QAction(tr("A&bout"), this);
    aboutAction->setShortcut(tr("Ctrl+B"));

//![5]
//    connect(playAction, SIGNAL(triggered()), mediaObject, SLOT(play()));
//    connect(pauseAction, SIGNAL(triggered()), mediaObject, SLOT(pause()) );
    connect(playAction, SIGNAL(triggered()), this, SLOT(play()));
    connect(pauseAction, SIGNAL(triggered()), this, SLOT(pause()) );
//![5]
    connect(addFilesAction, SIGNAL(triggered()), this, SLOT(addFiles()));
    connect(settingsAction,SIGNAL(triggered()),this,SLOT(settings()));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));
}

void MainWindow::setupMenus()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(addFilesAction);
    fileMenu->addAction(settingsAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    QMenu *aboutMenu = menuBar()->addMenu(tr("&Help"));
    aboutMenu->addAction(aboutAction);
}

//![3]
void MainWindow::setupUi()
{
//![3]
    QToolBar *bar = new QToolBar;

    bar->addAction(playAction);
    bar->addAction(pauseAction);

    volumeSlider = new Phonon::VolumeSlider(this);
    volumeSlider->setAudioOutput(audioOutput);
//![4]
    volumeSlider->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

    QLabel *volumeLabel = new QLabel;
    volumeLabel->setPixmap(QPixmap("images/volume.png"));

    QPalette palette;
    palette.setBrush(QPalette::Light, Qt::darkGray);

    QStringList headers;
    headers << tr("File");

    musicTable = new QTableWidget(0, 1);
    musicTable->setHorizontalHeaderLabels(headers);
    musicTable->setSelectionMode(QAbstractItemView::SingleSelection);
    musicTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    connect(musicTable, SIGNAL(cellPressed(int, int)),
	    this, SLOT(tableClicked(int, int)));

    QPushButton *settingsButton = new QPushButton(tr("Settings"),this);

    QHBoxLayout *playbackLayout = new QHBoxLayout;
    playbackLayout->addWidget(bar);
    playbackLayout->addStretch();
    playbackLayout->addWidget(settingsButton);
    playbackLayout->addStretch();
    playbackLayout->addWidget(volumeLabel);
    playbackLayout->addWidget(volumeSlider);

    connect(settingsButton,SIGNAL(clicked()),this,SLOT(settings()));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(musicTable);
    mainLayout->addLayout(playbackLayout);

    QWidget *widget = new QWidget;
    widget->setLayout(mainLayout);

    setCentralWidget(widget);
    setWindowTitle("Hot Potato");
}

void MainWindow::settings()
{
    averagePlayTime = QInputDialog::getInt(this,tr("Settings"),tr("Average play time (in seconds)"), averagePlayTime, 0);
    maximumVariability = QInputDialog::getInt(this,tr("Settings"),tr("Range of variation in play time (in seconds)"), maximumVariability, 0, averagePlayTime);
    minimumPlayTime = QInputDialog::getInt(this,tr("Settings"),tr("Minimum play time (in seconds)"), minimumPlayTime, 1);
    maximumPlayTime = QInputDialog::getInt(this,tr("Settings"),tr("Maximum play time (in seconds)"), maximumPlayTime, 1);
    pauseTime = QInputDialog::getInt(this,tr("Settings"),tr("Time of pauses (in seconds)"), pauseTime, 1);
}

void MainWindow::play()
{
    playAction->setEnabled(false);
    pauseAction->setEnabled(true);

//    qDebug() << "Play";

    playMusic();
}

void MainWindow::pause()
{
//    qDebug() << "Pause";

    playAction->setEnabled(true);
    pauseAction->setEnabled(false);

    musicTimer->stop();
    pauseTimer->stop();
    mediaObject->pause();
}

void MainWindow::playMusic()
{
    mediaObject->play();

    int playtime, variation;
    do
    {
	variation = (int)(maximumVariability * ((double)qrand()/(double)RAND_MAX));
	if( qrand() < 0.5*RAND_MAX)
	{
	    playtime = averagePlayTime + variation;
	}
	else
	{
	    playtime = averagePlayTime - variation;
	}
    }
    while( playtime < minimumPlayTime && playtime > maximumPlayTime );

    musicTimer->start(1000*playtime);
//    qDebug() << "Playing for " << playtime << " seconds; variation " << variation;
}

void MainWindow::pauseMusic()
{
    mediaObject->pause();

    pauseTimer->start(1000*pauseTime);
//    qDebug() << "Pausing for " << pauseTime << " seconds";
}


