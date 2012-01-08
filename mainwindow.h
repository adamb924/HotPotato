#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <Phonon/AudioOutput>
#include <Phonon/SeekSlider>
#include <Phonon/MediaObject>
#include <Phonon/VolumeSlider>
#include <Phonon/BackendCapabilities>
#include <QList>

QT_BEGIN_NAMESPACE
class QAction;
class QTableWidget;
class QTimer;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
//    ~MainWindow();

    QSize sizeHint() const {
	return QSize(500, 300);
    }
private:
    int averagePlayTime;
    int minimumPlayTime;
    int maximumPlayTime;
    int maximumVariability;
    int pauseTime;
    QTimer *musicTimer, *pauseTimer;

private slots:
    void addFiles();
    void about();
//![1]
    void stateChanged(Phonon::State newState, Phonon::State oldState);
    void tick(qint64 time);
    void sourceChanged(const Phonon::MediaSource &source);
    void metaStateChanged(Phonon::State newState, Phonon::State oldState);
    void aboutToFinish();
    void tableClicked(int row, int column);
    void settings();
    void play();
    void pause();
    void playMusic();
    void pauseMusic();

//![1]

private:
    void setupActions();
    void setupMenus();
    void setupUi();

//![2]
    Phonon::MediaObject *mediaObject;
    Phonon::MediaObject *metaInformationResolver;
    Phonon::AudioOutput *audioOutput;
    Phonon::VolumeSlider *volumeSlider;
    QList<Phonon::MediaSource> sources;
//![2]

    QAction *playAction;
    QAction *pauseAction;
    QAction *nextAction;
    QAction *previousAction;
    QAction *addFilesAction;
    QAction *exitAction;
    QAction *aboutAction;
    QAction *settingsAction;
    QTableWidget *musicTable;
};

#endif // MAINWINDOW_H
