/*
* Cyan <http://cyan.fxarena.net> <https://github.com/olear/cyan>,
* Copyright (C) 2016, 2017 Ole-André Rodlie
*
* Cyan is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 2 as published
* by the Free Software Foundation.
*
* Cyan is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with Cyan.  If not, see <http://www.gnu.org/licenses/gpl-2.0.html>
*/

#include "cyan.h"
#include <QCoreApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSettings>
#include <QTimer>
#include <QDebug>
#include <QFileInfo>
#include <QFileDialog>
#include <QMatrix>
#include <QMessageBox>
#include <QIcon>
#include <QKeySequence>
#include <QUrl>
#include <QMimeData>
#include <QVBoxLayout>
#include <QDirIterator>
#include <QApplication>
#include <QStyleFactory>

#ifdef Q_OS_MAC
#define CYAN_FONT_SIZE 10
#else
#define CYAN_FONT_SIZE 8
#endif

Cyan::Cyan(QWidget *parent)
    : QMainWindow(parent)
    , scene(Q_NULLPTR)
    , view(Q_NULLPTR)
    , mainBar(Q_NULLPTR)
    , convertBar(Q_NULLPTR)
    , profileBar(Q_NULLPTR)
    , rgbProfile(Q_NULLPTR)
    , cmykProfile(Q_NULLPTR)
    , grayProfile(Q_NULLPTR)
    , inputProfile(Q_NULLPTR)
    , outputProfile(Q_NULLPTR)
    , monitorProfile(Q_NULLPTR)
    , renderingIntent(Q_NULLPTR)
    , blackPoint(Q_NULLPTR)
    , mainBarLoadButton(Q_NULLPTR)
    , mainBarSaveButton(Q_NULLPTR)
    , menuBar(Q_NULLPTR)
    , fileMenu(Q_NULLPTR)
    , helpMenu(Q_NULLPTR)
    , openImageAction(Q_NULLPTR)
    , saveImageAction(Q_NULLPTR)
    , quitAction(Q_NULLPTR)
    , exportEmbeddedProfileAction(Q_NULLPTR)
    , bitDepth(Q_NULLPTR)
    , progBar(Q_NULLPTR)
    , imageInfoDock(Q_NULLPTR)
    , imageInfoTree(Q_NULLPTR)
    //, sBar(Q_NULLPTR)
{
    qApp->setStyle(QStyleFactory::create("fusion"));

    QPalette palette;
    palette.setColor(QPalette::Window, QColor(53,53,53));
    palette.setColor(QPalette::WindowText, Qt::white);
    palette.setColor(QPalette::Base, QColor(15,15,15));
    palette.setColor(QPalette::AlternateBase, QColor(53,53,53));
    //palette.setColor(QPalette::ToolTipBase, Qt::white);
    //palette.setColor(QPalette::ToolTipText, Qt::white);
    palette.setColor(QPalette::Link, Qt::white);
    palette.setColor(QPalette::LinkVisited, Qt::white);
    palette.setColor(QPalette::ToolTipText, Qt::black);
    palette.setColor(QPalette::Text, Qt::white);
    palette.setColor(QPalette::Button, QColor(53,53,53));
    palette.setColor(QPalette::ButtonText, Qt::white);
    palette.setColor(QPalette::BrightText, Qt::red);
    //palette.setColor(QPalette::Highlight, QColor(142,45,197).lighter());
    palette.setColor(QPalette::Highlight, QColor(0,124,151));
    palette.setColor(QPalette::HighlightedText, Qt::black);
    palette.setColor(QPalette::Disabled, QPalette::Text, Qt::darkGray);
    palette.setColor(QPalette::Disabled, QPalette::ButtonText, Qt::darkGray);
    qApp->setPalette(palette);

    setWindowTitle(qApp->applicationName());
    setWindowIcon(QIcon(":/cyan.png"));

    scene = new QGraphicsScene(this);
    view = new ImageView(this);
    view->setScene(scene);
    view->fit = true;

    setCentralWidget(view);

    mainBar = new QToolBar(this);
    convertBar = new QToolBar(this);
    profileBar = new QToolBar(this);

    mainBar->setObjectName("MainToolbar");
    mainBar->setWindowTitle(tr("Toolbar"));
    mainBar->setOrientation(Qt::Vertical);
    mainBar->setAllowedAreas(Qt::LeftToolBarArea);
    mainBar->setFloatable(false);
    mainBar->setMovable(false);
    mainBar->setIconSize(QSize(22,22));

    convertBar->setObjectName("ColorConverter");
    convertBar->setWindowTitle(tr("Color Converter"));
    convertBar->setFloatable(false);
    convertBar->setMovable(false);

    profileBar->setObjectName("ColorManagement");
    profileBar->setWindowTitle(tr("Color Management"));
    profileBar->setFloatable(false);
    profileBar->setMovable(false);
    profileBar->setAllowedAreas(Qt::BottomToolBarArea);

    imageInfoDock = new QDockWidget(this);
    imageInfoDock->setWindowTitle("Image Information");
    imageInfoDock->setObjectName("imageInformation");
   // QWidget* titleWidget = new QWidget(this);
   // imageInfoDock->setTitleBarWidget(titleWidget);
    imageInfoDock->setContentsMargins(0,0,0,0);
    //imageInfoDock->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
    imageInfoDock->setFeatures(QDockWidget::DockWidgetMovable);

    //sBar = new QStatusBar(this);

    addToolBar(Qt::TopToolBarArea, mainBar);
    addToolBar(Qt::TopToolBarArea, convertBar);
    addToolBar(Qt::BottomToolBarArea, profileBar);

    addDockWidget(Qt::RightDockWidgetArea, imageInfoDock);

    //setStatusBar(sBar);

    imageInfoTree = new QTreeWidget(this);
    QStringList imageInfoTreeLabels;
    imageInfoTreeLabels << tr("Property") << tr("Value");
    imageInfoTree->setHeaderLabels(imageInfoTreeLabels);


    imageInfoDock->setWidget(imageInfoTree);

    rgbProfile = new QComboBox(this);
    cmykProfile = new QComboBox(this);
    grayProfile = new QComboBox(this);
    inputProfile = new QComboBox(this);
    outputProfile = new QComboBox(this);
    monitorProfile = new QComboBox(this);
    renderingIntent = new QComboBox(this);
    blackPoint = new QCheckBox(this);
    bitDepth = new QComboBox(this);
    //progBar = new QProgressBar(this);

    rgbProfile->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    cmykProfile->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    grayProfile->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    inputProfile->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    outputProfile->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    monitorProfile->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    renderingIntent->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    bitDepth->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
   // progBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    renderingIntent->setMaximumWidth(130);

  //  progBar->setMaximumWidth(100);
    //progBar->setMaximumHeight(20);
    //progBar->setRange(0,0);
    //progBar->setDisabled(true);
    //progBar->setRange(0,100);
    //progBar->setValue(100);
  //  progBar->setTextVisible(false);
  //  progBar->hide();

    QIcon bitDepthIcon(":/cyan-display.png");
    bitDepth->addItem(bitDepthIcon, tr("Source"));
    if (supportedDepth()>=8) {
        bitDepth->addItem(bitDepthIcon, tr("8-bit"), 8);
    }
    if (supportedDepth()>=16) {
        bitDepth->addItem(bitDepthIcon, tr("16-bit"), 16);
    }
    if (supportedDepth()>=32) {
        bitDepth->addItem(bitDepthIcon, tr("32-bit"), 32);
    }
    bitDepth->setMaximumWidth(150);

    QIcon renderIcon(":/cyan-display.png");
    renderingIntent->addItem(renderIcon, tr("Undefined"),
                             FXX::UndefinedRenderingIntent);
    renderingIntent->addItem(renderIcon, tr("Saturation"),
                             FXX::SaturationRenderingIntent);
    renderingIntent->addItem(renderIcon, tr("Perceptual"),
                             FXX::PerceptualRenderingIntent);
    renderingIntent->addItem(renderIcon, tr("Absolute"),
                             FXX::AbsoluteRenderingIntent);
    renderingIntent->addItem(renderIcon, tr("Relative"),
                             FXX::RelativeRenderingIntent);

    QLabel *inputLabel = new QLabel(this);
    QLabel *outputLabel = new QLabel(this);
    QLabel *monitorLabel = new QLabel(this);
    QLabel *renderLabel = new QLabel(this);
    QLabel *blackLabel = new QLabel(this);
    QLabel *rgbLabel = new QLabel(this);
    QLabel *cmykLabel = new QLabel(this);
    QLabel *grayLabel = new QLabel(this);
    QLabel *bitDepthLabel = new QLabel(this);

    inputLabel->setText(tr("Input"));
    outputLabel->setText(tr("Output"));
    monitorLabel->setText(tr("Monitor"));
    renderLabel->setText(tr("Intent"));
    blackLabel->setText(tr("Black Point"));
    rgbLabel->setText(tr("RGB"));
    cmykLabel->setText(tr("CMYK"));
    grayLabel->setText(tr("GRAY"));
    bitDepthLabel->setText(tr("Depth"));

    inputLabel->setToolTip(tr("Input profile for image"));
    outputLabel->setToolTip(tr("Profile used to convert image"));
    monitorLabel->setToolTip(tr("Monitor profile, used for proofing"));
    renderLabel->setToolTip(tr("Rendering intent used"));
    blackLabel->setToolTip(tr("Enable/Disable black point compensation"));
    rgbLabel->setToolTip(tr("Default RGB profile, used when image don't have an embedded profile"));
    cmykLabel->setToolTip(tr("Default CMYK profile, used when image don't have an embedded profile"));
    grayLabel->setToolTip(tr("Default GRAY profile, used when image don't have an embedded profile"));
    bitDepthLabel->setToolTip(tr("Adjust image output bit depth"));

    convertBar->addWidget(inputLabel);
    convertBar->addWidget(inputProfile);
    convertBar->addSeparator();
    convertBar->addWidget(outputLabel);
    convertBar->addWidget(outputProfile);
    convertBar->addWidget(bitDepthLabel);
    convertBar->addWidget(bitDepth);

    profileBar->addWidget(rgbLabel);
    profileBar->addWidget(rgbProfile);
    profileBar->addSeparator();
    profileBar->addWidget(cmykLabel);
    profileBar->addWidget(cmykProfile);
    profileBar->addSeparator();
    profileBar->addWidget(grayLabel);
    profileBar->addWidget(grayProfile);
    profileBar->addSeparator();
    profileBar->addWidget(monitorLabel);
    profileBar->addWidget(monitorProfile);

    profileBar->addSeparator();
    profileBar->addWidget(renderLabel);
    profileBar->addWidget(renderingIntent);
    profileBar->addSeparator();
    profileBar->addWidget(blackLabel);
    profileBar->addWidget(blackPoint);

    profileBar->addSeparator();
    //convertBar->addWidget(progBar);

    //sBar->addPermanentWidget(progBar);

    mainBarLoadButton = new QPushButton(this);
    mainBarSaveButton = new QPushButton(this);

    mainBarLoadButton->setToolTip(tr("Open image"));
    mainBarLoadButton->setIcon(QIcon(":/cyan-open.png"));
    mainBarSaveButton->setToolTip(tr("Save image"));
    mainBarSaveButton->setIcon(QIcon(":/cyan-save.png"));
    mainBarSaveButton->setDisabled(true);

    mainBar->addWidget(mainBarLoadButton);
    mainBar->addWidget(mainBarSaveButton);

    menuBar = new QMenuBar(this);
    setMenuBar(menuBar);

    fileMenu = new QMenu(tr("File"));
    helpMenu = new QMenu(tr("Help"));
    menuBar->addMenu(fileMenu);
    menuBar->addMenu(helpMenu);
    menuBar->setMaximumHeight(20);
    //menuBar->setCornerWidget(progBar, Qt::TopRightCorner);

    QAction *aboutAction = new QAction(tr("About ") + qApp->applicationName(), this);
    aboutAction->setIcon(QIcon(":/cyan.png"));
    helpMenu->addAction(aboutAction);

    QAction *aboutQtAction = new QAction(tr("About Qt"), this);
    helpMenu->addAction(aboutQtAction);

    openImageAction = new QAction(tr("Open image"), this);
    openImageAction->setIcon(QIcon(":/cyan-open.png"));
    openImageAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));
    fileMenu->addAction(openImageAction);

    saveImageAction = new QAction(tr("Save image"), this);
    saveImageAction->setIcon(QIcon(":/cyan-save.png"));
    saveImageAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));
    saveImageAction->setDisabled(true);
    fileMenu->addAction(saveImageAction);

    fileMenu->addSeparator();

    exportEmbeddedProfileAction = new QAction(tr("Save embedded profile"), this);
    exportEmbeddedProfileAction->setIcon(QIcon(":/cyan-save.png"));
    exportEmbeddedProfileAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E));
    exportEmbeddedProfileAction->setDisabled(true);
    fileMenu->addAction(exportEmbeddedProfileAction);

    fileMenu->addSeparator();

    quitAction = new QAction(tr("Quit"),this);
    quitAction->setIcon(QIcon(":/cyan-quit.png"));
    quitAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));
    fileMenu->addAction(quitAction);

    qRegisterMetaType<FXX::Image>("FXX::Image");

    connect(&loader, SIGNAL(loadedImage(FXX::Image)), this, SLOT(loadedImage(FXX::Image)));
    connect(&loader, SIGNAL(convertedImage(FXX::Image)), this, SLOT(convertedImage(FXX::Image)));

    connect(aboutAction, SIGNAL(triggered()), this, SLOT(aboutCyan()));
    connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(openImageAction, SIGNAL(triggered()), this, SLOT(openImageDialog()));
    connect(saveImageAction, SIGNAL(triggered()), this, SLOT(saveImageDialog()));
    connect(mainBarLoadButton, SIGNAL(released()), this, SLOT(openImageDialog()));
    connect(mainBarSaveButton, SIGNAL(released()), this, SLOT(saveImageDialog()));
    connect(exportEmbeddedProfileAction, SIGNAL(triggered()), this, SLOT(exportEmbeddedProfileDialog()));
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

    connect(rgbProfile, SIGNAL(currentIndexChanged(int)), this, SLOT(updateRgbDefaultProfile(int)));
    connect(cmykProfile, SIGNAL(currentIndexChanged(int)), this, SLOT(updateCmykDefaultProfile(int)));
    connect(grayProfile, SIGNAL(currentIndexChanged(int)), this, SLOT(updateGrayDefaultProfile(int)));
    connect(monitorProfile, SIGNAL(currentIndexChanged(int)), this, SLOT(updateMonitorDefaultProfile(int)));

    connect(inputProfile, SIGNAL(currentIndexChanged(int)), this, SLOT(inputProfileChanged(int)));
    connect(outputProfile, SIGNAL(currentIndexChanged(int)), this, SLOT(outputProfileChanged(int)));
    connect(bitDepth, SIGNAL(currentIndexChanged(int)), this, SLOT(bitDepthChanged(int)));

    connect(view, SIGNAL(resetZoom()), this, SLOT(resetImageZoom()));
    connect(view, SIGNAL(openImage(QString)), this, SLOT(openImage(QString)));
    connect(view, SIGNAL(openProfile(QString)), this, SLOT(openProfile(QString)));

    connect(profileDialog.profileSaveButton, SIGNAL(released()), this, SLOT(saveProfile()));

    connect(renderingIntent, SIGNAL(currentIndexChanged(int)), this, SLOT(renderingIntentUpdated(int)));
    connect(blackPoint, SIGNAL(stateChanged(int)), this, SLOT(blackPointUpdated(int)));



    setStyleSheet(QString("QLabel {margin-left:5px;margin-right:5px;} QComboBox {padding:3px;} QLabel, QComboBox, QDoubleSpinBox, QMenuBar {font-size: %1pt;}").arg(QString::number(CYAN_FONT_SIZE)));

    clearImageBuffer();
    QTimer::singleShot(0, this, SLOT(readConfig()));
}

Cyan::~Cyan()
{
    writeConfig();
}

void Cyan::readConfig()
{
    QSettings settings;

    settings.beginGroup("color");
    blackPoint->setChecked(settings.value("black").toBool());

    if (settings.value("render").isValid()) {
        renderingIntent->setCurrentIndex(settings.value("render").toInt());
    }

    settings.endGroup();

    settings.beginGroup("ui");
    if (settings.value("state").isValid()) {
        restoreState(settings.value("state").toByteArray());
    }
    if (settings.value("size").isValid()) {
        resize(settings.value("size", QSize(320, 256)).toSize());
    }
    if (settings.value("pos").isValid()) {
        move(settings.value("pos", QPoint(0,0)).toPoint());
    }
    if (settings.value("max").toBool() == true) {
        this->showMaximized();
    }
    settings.endGroup();

    loadDefaultProfiles();
    gimpPlugin();

    /*bool bitDepthNoWarn = false;
    settings.beginGroup("default");
    if (settings.value("nowarning_bitdepth").isValid()) {
        bitDepthNoWarn = settings.value("nowarning_bitdepth").toBool();
    }
    if (!bitDepthNoWarn) {
        if (proc.quantumDepth() < 32) {
            QMessageBox::warning(this, tr("Cyan Quantum Depth"),
                                 tr("Quantum depth 32 is missing from backend,"
                                    " you will not be able to handle 32-bit images."));
            settings.setValue("nowarning_bitdepth",true);
        }
    }
    settings.endGroup();*/

    QStringList args = qApp->arguments();
    bool foundArg1 = false;
    for (int i = 1; i < args.size(); ++i) {
        QString file = args.at(i);
        if (file.isEmpty()) { continue; }
        if (foundArg1) {
            lockedSaveFileName = file;
            foundArg1 = false;
        } else if (file == "-o") {
            foundArg1 = true;
        } else {
            QFile isFile(file);
            if (isFile.exists()) {
                openImage(file);
                break;
            }
        }
    }
}

void Cyan::writeConfig()
{
    QSettings settings;
    settings.beginGroup("color");
    settings.setValue("black", blackPoint->isChecked());

    settings.setValue("render", renderingIntent->itemData(renderingIntent->currentIndex()).toInt());
    settings.endGroup();

    settings.beginGroup("ui");
    settings.setValue( "state", saveState());
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    if (this->isMaximized()) {
        settings.setValue("max", "true");
    } else {
        settings.setValue("max", "false");
    }
    settings.endGroup();
    settings.sync();

    saveDefaultProfiles();
}

void Cyan::aboutCyan()
{
    QMessageBox aboutCyan;
    aboutCyan.setTextFormat(Qt::RichText);
    aboutCyan.setWindowTitle(tr("About")+" " + qApp->applicationName() + " " + qApp->applicationVersion());
    aboutCyan.setIconPixmap(QPixmap::fromImage(QImage(":/cyan.png")));
    aboutCyan.setText("<h1>" + qApp->applicationName() + " " + qApp->applicationVersion() + "</h1><p>Prepress image viewer and converter.</p>");

    QString infoText = "<p>Copyright &copy;2016, 2017 <a href=\"mailto:olear@dracolinux.org\">Ole-Andr&eacute; Rodlie</a>. All rights reserved.</p><p>Cyan is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License version 2 as published by the Free Software Foundation.<br><br>Cyan is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.</p>";
    infoText.append("<p>Includes ICC color profiles from <a href=\"http://www.basiccolor.de/\">basICColor GmbH</a>, licensed under a <a href=\"http://creativecommons.org/licenses/by-nd/3.0/\">Creative Commons Attribution-No Derivative Works 3.0</a> License. Includes icons from the <a href=\"http://tango.freedesktop.org/Tango_Icon_Library\">Tango Icon Library</a>.");

    infoText.append("</p>");
    //infoText.append(proc.version());
    infoText.append("<p><img src=\":/cyan-icc2.png\">&nbsp;<img src=\":/cyan-icc4.png\"></p>");
    infoText.append("<p>Visit <a href=\"http://prepress.sf.net\">prepress.sf.net</a> for news and updates.");
    infoText.append(" Please consider a <a href=\"https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=CKVSFAUAZLUBL&lc=NO&item_name=Cyan&item_number=CYAN&currency_code=USD&bn=PP%2dDonationsBF%3abtn_donateCC_LG%2egif%3aNonHosted\">donation</a> to keep this project alive.</p>");
    aboutCyan.setInformativeText(infoText);

    QString detailedText;
    QFile licenseOther(":/3rdparty.txt");
    if (licenseOther.open(QIODevice::ReadOnly)) {
        detailedText.append("==============================\n");
        detailedText.append(tr("THIRD-PARTY")+"\n");
        detailedText.append("==============================\n\n");
        detailedText.append(QString::fromUtf8(licenseOther.readAll()));
        detailedText.append("\n\n");
        licenseOther.close();
    }
    aboutCyan.setDetailedText(detailedText);

    aboutCyan.exec();
}

void Cyan::openImageDialog()
{
    QSettings settings;
    settings.beginGroup("default");

    QString file;
    QString dir;

    if (settings.value("lastDir").isValid()) {
        dir = settings.value("lastDir").toString();
    } else {
        dir = QDir::homePath();
    }

    file = QFileDialog::getOpenFileName(this, tr("Open image"), dir, tr("Image files (*.png *.jpg *.jpeg *.tif *.tiff *.psd *.pdf *.icc *.icm)"));
    if (!file.isEmpty()) {
        QString suffix = QFileInfo(file).suffix().toUpper();
        if (suffix == "ICC" || suffix == "ICM") {
            openProfile(file);
        } else if (suffix == "PNG" || suffix == "JPG" || suffix == "JPEG" || suffix == "TIF" || suffix == "TIFF" || suffix == "PSD" || suffix == "PDF") {
            openImage(file);
        }
        QFileInfo imageFile(file);
        settings.setValue("lastDir", imageFile.absoluteDir().absolutePath());
    }

    settings.endGroup();
    settings.sync();
}

void Cyan::saveImageDialog()
{
    if (!lockedSaveFileName.isEmpty()) {
        saveImage(lockedSaveFileName);
    } else {
        QSettings settings;
        settings.beginGroup("default");

        QString file;
        QString dir;

        if (settings.value("lastSaveDir").isValid()) {
            dir = settings.value("lastSaveDir").toString();
        } else {
            dir = QDir::homePath();
        }

        file = QFileDialog::getSaveFileName(this, tr("Save image"), dir, tr("Image files (*.tif *.psd)"));
        if (!file.isEmpty()) {
            QFileInfo imageFile(file);
            if (imageFile.suffix().isEmpty()) {
                QMessageBox::warning(this, tr("File suffix is missing"), tr("File suffix is missing, please add .tif or .psd after your filename."));
                return;
            }
            saveImage(file);
            settings.setValue("lastSaveDir", imageFile.absoluteDir().absolutePath());
        }

        settings.endGroup();
        settings.sync();
    }
}

void Cyan::openImage(QString file)
{
    if (file.isEmpty()) { return; }
    if (rgbProfile->itemData(rgbProfile->currentIndex()).isNull() ||
        cmykProfile->itemData(cmykProfile->currentIndex()).isNull() ||
        grayProfile->itemData(grayProfile->currentIndex()).isNull()) {
        QMessageBox::warning(this, "Default Color Profiles",
                             "Please set the default color profiles (RGB/CMYK/GRAY) before loading images.");
        return;
    }
    disableUI();
    loader.requestImage(file);
}

void Cyan::saveImage(QString file)
{
    qDebug() << "SAVE TODO" << file;
    /*if (!file.isEmpty()) {
        disableUI();
        QByteArray empty;
        magentaAdjust adjust;
        adjust.black = blackPoint->isChecked();
        adjust.brightness = 100;
        adjust.hue = 100;
        adjust.intent = renderingIntent->itemData(renderingIntent->currentIndex()).toInt();
        adjust.saturation = 100;
        adjust.depth = bitDepth->itemData(bitDepth->currentIndex()).toInt();
        QByteArray currentInputProfile;
        QString selectedInputProfile = inputProfile->itemData(inputProfile->currentIndex()).toString();
        if (!selectedInputProfile.isEmpty()) {
            currentInputProfile = currentImageNewProfile;
        } else {
            currentInputProfile = currentImageProfile;
        }
        proc.requestImage(false , true, file, currentImageData, currentInputProfile, getOutputProfile(), empty, adjust);
    }*/
}

void Cyan::getColorProfiles(FXX::ColorSpace colorspace, QComboBox *box, bool isMonitor)
{
    QString defaultProfile;
    int defaultIndex = -1;
    QSettings settings;
    settings.beginGroup("profiles");
    QString profileType;
    if (isMonitor) {
        profileType = "monitor";
    } else {
        profileType = QString::number(colorspace);
    }
    if (!settings.value(profileType).toString().isEmpty()) {
        defaultProfile = settings.value(profileType).toString();
    }
    settings.endGroup();

    QMap<QString,QString> profiles = genProfiles(colorspace);
    if (profiles.size() > 0) {
        box->clear();
        QIcon itemIcon(":/cyan-wheel.png");
        QString noProfileText = tr("Select ...");
        if (isMonitor) {
            noProfileText = tr("None");
        }
        box->addItem(itemIcon, noProfileText);
        box->addItem("----------");

        int it = 0;
        QMapIterator<QString, QString> profile(profiles);
        while (profile.hasNext()) {
            profile.next();
            box->addItem(itemIcon, profile.key(), profile.value());
            if (profile.value() == defaultProfile) { defaultIndex = it+2; }
            ++it;
        }
        if (defaultIndex >= 0) {
            box->setCurrentIndex(defaultIndex);
        }
    }
}

void Cyan::loadDefaultProfiles()
{
    QString icc = QString("%1/.config/Cyan/icc")
                  .arg(QDir::homePath());
    QDir dir(icc);
    if (!dir.exists(icc)) { dir.mkpath(icc); }

    QFile defRGB(QString("%1/rgb.icc").arg(icc));
    if (!defRGB.exists()) {
        QFile::copy(":/icc/rgb.icc",
                    QString("%1/rgb.icc").arg(icc));
    }
    QFile defCMYK(QString("%1/cmyk.icc").arg(icc));
    if (!defCMYK.exists()) {
        QFile::copy(":/icc/cmyk.icc",
                    QString("%1/cmyk.icc").arg(icc));
    }
    QFile defGRAY(QString("%1/gray.icc").arg(icc));
    if (!defGRAY.exists()) {
        QFile::copy(":/icc/gray.icc",
                    QString("%1/gray.icc").arg(icc));
    }

    getColorProfiles(FXX::RGBColorSpace, rgbProfile, false);
    getColorProfiles(FXX::CMYKColorSpace, cmykProfile, false);
    getColorProfiles(FXX::GRAYColorSpace, grayProfile, false);
    getColorProfiles(FXX::RGBColorSpace, monitorProfile, true /*isMonitor*/);
}

void Cyan::saveDefaultProfiles()
{
    QSettings settings;
    settings.beginGroup("profiles");
    if (rgbProfile->count() > 0) {
        settings.setValue("1", rgbProfile->itemData(rgbProfile->currentIndex()));
    }
    if (cmykProfile->count() > 0) {
        settings.setValue("2", cmykProfile->itemData(cmykProfile->currentIndex()));
    }
    if (grayProfile->count() > 0) {
        settings.setValue("3", grayProfile->itemData(grayProfile->currentIndex()));
    }
    if (monitorProfile->count() > 0) {
        settings.setValue("monitor", monitorProfile->itemData(monitorProfile->currentIndex()));
    }
    settings.endGroup();
    settings.sync();
}

void Cyan::updateRgbDefaultProfile(int index)
{
    QSettings settings;
    settings.beginGroup("profiles");
    if (!rgbProfile->itemData(index).isNull()) {
        QString currentProfile = rgbProfile->itemData(index).toString();
        QString savedProfile = settings.value("1").toString();
        if (currentProfile != savedProfile) {
            settings.setValue("1", currentProfile);
        }
    }
    settings.endGroup();
    settings.sync();
}

void Cyan::updateCmykDefaultProfile(int index)
{
    QSettings settings;
    settings.beginGroup("profiles");
    if (!cmykProfile->itemData(index).isNull()) {
        QString currentProfile = cmykProfile->itemData(index).toString();
        QString savedProfile = settings.value("2").toString();
        if (currentProfile != savedProfile) {
            settings.setValue("2", currentProfile);
        }
    }
    settings.endGroup();
    settings.sync();
}

void Cyan::updateGrayDefaultProfile(int index)
{
    QSettings settings;
    settings.beginGroup("profiles");
    if (!grayProfile->itemData(index).isNull()) {
        QString currentProfile = grayProfile->itemData(index).toString();
        QString savedProfile = settings.value("3").toString();
        if (currentProfile != savedProfile) {
            settings.setValue("3", currentProfile);
        }
    }
    settings.endGroup();
    settings.sync();
}

void Cyan::updateMonitorDefaultProfile(int index)
{
    QSettings settings;
    settings.beginGroup("profiles");
    if (!monitorProfile->itemData(index).isNull()) {
        QString currentProfile = monitorProfile->itemData(index).toString();
        QString savedProfile = settings.value("monitor").toString();
        if (currentProfile != savedProfile) {
            settings.setValue("monitor", currentProfile);
        }
    }
    settings.endGroup();
    settings.sync();

    updateImage();


}

/*void Cyan::getImage(magentaImage result)
{
    Q_UNUSED(result)
    qDebug() << "GET IMAGE";*/
    /*enableUI();
    if (result.colorspace == 0) {
        QMessageBox::warning(this, tr("Unsupported Colorspace"), tr("Image has an unsupported colorspace, unable to load."));
        imageClear();
        return;
    }
    if (result.saved && result.error.isEmpty()) {
        QFileInfo imageFile(result.filename);
        if (imageFile.exists()) {
            if (lockedSaveFileName.isEmpty()) {
                QMessageBox::information(this, tr("Image saved"), imageFile.completeBaseName() + tr(" saved to disk."));
            } else {
                qApp->quit();
            }
        } else {
            QMessageBox::warning(this, tr("Failed to save image"), tr("Failed to save image to disk"));
        }
    }
    if (!result.warning.isEmpty()) {
        QMessageBox::warning(this, tr("Cyan Warning"), result.warning);
    }
    if (result.error.isEmpty() && result.data.length() > 0 && result.profile.length() > 0) {
        if (!result.preview) {
            imageClear();
            currentImageData = result.data;
            ///TODO
            /// imageData =
            currentImageProfile = result.profile;
            QFileInfo imageFile(result.filename);
            QString imageColorspace;
            switch (result.colorspace) {
            case fxxRGB:
                imageColorspace = "RGB";
                break;
            case fxxCMYK:
                imageColorspace = "CMYK";
                break;
            case fxxGRAY:
                imageColorspace = "GRAY";
                break;
            }
            QString newWindowTitle = qApp->applicationName() + " - " + imageFile.fileName() + " [" + imageColorspace+"]" + " [" + cms.profileDescFromData(currentImageProfile) + "] [" + QString::number(result.width) + "x" + QString::number(result.height) + "]";
            setWindowTitle(newWindowTitle);
            getConvertProfiles();
            if (!result.hasProfile) {
                saveImageAction->setEnabled(true);
                mainBarSaveButton->setEnabled(true);
            }
            exportEmbeddedProfileAction->setEnabled(true);
            updateImage();
        } else {
            setImage(result.data);
        }
    } else {
        if (!result.error.isEmpty()) {
            QMessageBox::warning(this, tr("Cyan Error"), result.error);
        }
        if (!result.saved) {
            imageClear();
        }
    }*/
//}

void Cyan::imageClear()
{
    //setWindowTitle(qApp->applicationName());
    //currentImageData.clear();
    //currentImageProfile.clear();
    //currentImageNewProfile.clear();
    scene->clear();
    resetImageZoom();
    mainBarSaveButton->setDisabled(true);
    saveImageAction->setDisabled(true);
    exportEmbeddedProfileAction->setDisabled(true);
}

void Cyan::resetImageZoom()
{
    QMatrix matrix;
    matrix.scale(1.0, 1.0);
    view->setMatrix(matrix);
}

void Cyan::setImage(QByteArray image)
{
    if (image.length() == 0) { return; }
    QPixmap pixmap(QPixmap::fromImage(QImage::fromData(image)));
    if (pixmap.isNull()) { return; }
    scene->clear();
    scene->addPixmap(pixmap);
    scene->setSceneRect(0, 0, pixmap.width(), pixmap.height());
}

void Cyan::updateImage()
{
    qDebug() << "UPDATE IMAGE";

    FXX::Image image;
    image.imageBuffer = imageData.imageBuffer;
    QString selectedInputProfile = inputProfile->itemData(inputProfile->currentIndex()).toString();
    QString selectedOutputProfile = outputProfile->itemData(outputProfile->currentIndex()).toString();
    QString selectedMonitorProfile = monitorProfile->itemData(monitorProfile->currentIndex()).toString();
    int currentDepth = bitDepth->itemData(bitDepth->currentIndex()).toInt();

    image.intent = (FXX::RenderingIntent)renderingIntent->itemData(renderingIntent->currentIndex()).toInt();
    image.blackpoint = blackPoint->isChecked();
    image.depth = (size_t)currentDepth;

    // add input profile
    if (!selectedInputProfile.isEmpty()) {
        QByteArray profile = readColorProfile(selectedInputProfile);
        if (profile.size()>0) {
            std::vector<unsigned char> buffer(profile.begin(), profile.end());
            image.iccInputBuffer = buffer;
        }
    } else {
        image.iccInputBuffer = imageData.iccInputBuffer;
    }

    // add output profile
    if (!selectedOutputProfile.isEmpty()) {
        QByteArray profile = readColorProfile(selectedOutputProfile);
        if (profile.size()>0) {
            std::vector<unsigned char> buffer(profile.begin(), profile.end());
            image.iccOutputBuffer = buffer;
        }
    } else {
        image.iccOutputBuffer = imageData.iccOutputBuffer;
    }

    // add monitor profile
    if (!selectedMonitorProfile.isEmpty()) {
        QByteArray profile = readColorProfile(selectedMonitorProfile);
        if (profile.size()>0) {
            std::vector<unsigned char> buffer(profile.begin(), profile.end());
            image.iccMonitorBuffer = buffer;
        }
    }

    // check if input profile exists
    if (image.iccInputBuffer.size()==0) { return; }

    // proc
    disableUI();
    loader.requestConvert(image);
}

QByteArray Cyan::getMonitorProfile()
{
    QByteArray result;
    if (!monitorProfile->itemData(monitorProfile->currentIndex()).toString().isEmpty()) {
        QFile outProfileName(monitorProfile->itemData(monitorProfile->currentIndex()).toString());
        if (outProfileName.open(QIODevice::ReadOnly)) {
            result = outProfileName.readAll();
            outProfileName.close();
        }
    }
    return result;
}

QByteArray Cyan::getOutputProfile()
{
    QByteArray result;
    if (!outputProfile->itemData(outputProfile->currentIndex()).toString().isEmpty()) {
        QFile outProfileName(outputProfile->itemData(outputProfile->currentIndex()).toString());
        if (outProfileName.open(QIODevice::ReadOnly)) {
            result = outProfileName.readAll();
            outProfileName.close();
        }
    }
    return result;
}

QByteArray Cyan::readColorProfile(QString file)
{
    QByteArray result;
    QFile outProfileName(file);
    if (outProfileName.open(QIODevice::ReadOnly)) {
        result = outProfileName.readAll();
        outProfileName.close();
    }
    return result;
}

void Cyan::getConvertProfiles()
{
    if (imageData.iccInputBuffer.size()==0) { return; }
    FXX::ColorSpace inputColorSpace = fx.getProfileColorspace(imageData.iccInputBuffer);
    if (inputColorSpace == FXX::UnknownColorSpace) { return; }
    QMap<QString,QString> inputProfiles, outputProfiles;

    switch(inputColorSpace) {
    case FXX::RGBColorSpace:
        outputProfiles = genProfiles(FXX::CMYKColorSpace);
        outputProfiles.unite(genProfiles(FXX::GRAYColorSpace));
        break;
    case FXX::CMYKColorSpace:
        outputProfiles = genProfiles(FXX::RGBColorSpace);
        outputProfiles.unite(genProfiles(FXX::GRAYColorSpace));
        break;
    case FXX::GRAYColorSpace:
        outputProfiles = genProfiles(FXX::RGBColorSpace);
        outputProfiles.unite(genProfiles(FXX::CMYKColorSpace));
        break;
    default:;
    }
    inputProfiles = genProfiles(inputColorSpace);

    QIcon itemIcon(":/cyan-wheel.png");
    QString embeddedProfile = QString::fromStdString(fx.getProfileTag(imageData.iccInputBuffer));
    //embeddedProfile.append(tr(" (embedded)"));

    inputProfile->clear();
    outputProfile->clear();

    inputProfile->addItem(itemIcon, embeddedProfile);
    inputProfile->addItem("----------");
    outputProfile->addItem(itemIcon, tr("None"));
    outputProfile->addItem("----------");

    QMapIterator<QString, QString> inputMap(inputProfiles);
    while (inputMap.hasNext()) {
        inputMap.next();
        inputProfile->addItem(itemIcon, inputMap.key(), inputMap.value());
    }
    QMapIterator<QString, QString> outputMap(outputProfiles);
    while (outputMap.hasNext()) {
        outputMap.next();
        outputProfile->addItem(itemIcon, outputMap.key(), outputMap.value());
    }
}

void Cyan::inputProfileChanged(int)
{
    QFile inputProfileName(inputProfile->itemData(inputProfile->currentIndex()).toString());
    if (inputProfileName.open(QIODevice::ReadOnly)) {
        currentImageNewProfile = inputProfileName.readAll();
        inputProfileName.close();
    }
    handleSaveState();
    updateImage();
}

void Cyan::outputProfileChanged(int)
{
    handleSaveState();
    updateImage();
}

void Cyan::enableUI()
{
    menuBar->setEnabled(true);
    mainBar->setEnabled(true);
    convertBar->setEnabled(true);
    profileBar->setEnabled(true);
}

void Cyan::disableUI()
{
    menuBar->setDisabled(true);
    mainBar->setDisabled(true);
    convertBar->setDisabled(true);
    profileBar->setDisabled(true);
}

void Cyan::exportEmbeddedProfileDialog()
{
    QSettings settings;
    settings.beginGroup("default");

    QString file;
    QString dir;

    if (settings.value("lastSaveDir").isValid()) {
        dir = settings.value("lastSaveDir").toString();
    } else {
        dir = QDir::homePath();
    }

    dir.append(QString("/%1.icc")
               .arg(QString::fromStdString(imageData.iccDescription)));

    file = QFileDialog::getSaveFileName(this, tr("Save profile"), dir, tr("Color Profiles (*.icc)"));
    if (!file.isEmpty()) {
        QFileInfo proFile(file);
        if (proFile.suffix().isEmpty()) {
            file.append(".icc");
        }
        exportEmbeddedProfile(file);
        settings.setValue("lastSaveDir", proFile.absoluteDir().absolutePath());
    }

    settings.endGroup();
    settings.sync();
}

void Cyan::exportEmbeddedProfile(QString file)
{
    if (!file.isEmpty() && imageData.iccInputBuffer.size() > 0) {
        QFile proFile(file);
        if (proFile.open(QIODevice::WriteOnly)) {
            if (proFile.write((const char*)imageData.iccInputBuffer.data(),
                              (qint64)imageData.iccInputBuffer.size()) == -1)
            {
                QMessageBox::warning(this, tr("Unable to save profile"),
                                     tr("Unable to save profile, please check write permissions."));
            } else {
                QFileInfo proFileInfo(file);
                QMessageBox::information(this, tr("Color profile saved"),
                                         proFileInfo.completeBaseName() + tr(" Saved to disk."));
            }
            proFile.close();
        } else {
            QMessageBox::warning(this, tr("Unable to save profile"),
                                 tr("Unable to save profile, please check write permissions."));
        }
    }
}

bool Cyan::imageModified()
{
    if (!inputProfile->itemData(inputProfile->currentIndex()).toString().isEmpty()) {
        return true;
    }
    if (!outputProfile->itemData(outputProfile->currentIndex()).toString().isEmpty()) {
        return true;
    }
    if (bitDepth->itemData(bitDepth->currentIndex()).toString().toInt() > 0) {
        return true;
    }
    return false;
}

void Cyan::handleSaveState()
{
    if (!imageModified()) {
        saveImageAction->setDisabled(true);
        mainBarSaveButton->setDisabled(true);
    } else {
        saveImageAction->setEnabled(true);
        mainBarSaveButton->setEnabled(true);
    }
}

bool Cyan::hasProfiles()
{
    if (genProfiles(FXX::RGBColorSpace).size()>0) {
        return true;
    }
    return false;
}

bool Cyan::hasRGB()
{
    return hasProfiles();
}

bool Cyan::hasCMYK()
{
    if (genProfiles(FXX::CMYKColorSpace).size()>0) {
        return true;
    }
    return false;
}

bool Cyan::hasGRAY()
{
    if (genProfiles(FXX::GRAYColorSpace).size()>0) {
        return true;
    }
    return false;
}

void Cyan::bitDepthChanged(int index)
{
    Q_UNUSED(index)
    handleSaveState();
    updateImage();
}

void Cyan::gimpPlugin()
{
    QStringList versions,folders;
    versions << "2.4" << "2.6" << "2.7" << "2.8" << "2.9" << "2.10" << "2.11" << "2.12";
    foreach (QString version, versions) {
        bool hasDir = false;
        QDir gimpDir;
        QString gimpPath;
        gimpPath.append(QDir::homePath());
        gimpPath.append(QDir::separator());
#ifndef Q_OS_MAC
        gimpPath.append(".gimp-"+version);
        if (gimpDir.exists(gimpPath)) {
            hasDir = true;
        }
#else
        gimpPath.append("Library/Application Support/GIMP/"+version);
        if (gimpDir.exists(gimpPath)) {
            hasDir = true;
        }
#endif
        if (hasDir) {
            gimpPath.append(QDir::separator());
            gimpPath.append("plug-ins");
            if (!gimpDir.exists(gimpPath)) {
                gimpDir.mkdir(gimpPath);
            }
            QString result = gimpPath;
            result.append(QDir::separator());
            result.append("cyan.py");
            folders << result;
        }
    }

    QString appPath = QString("cyanbin = \"%1\"").arg(qApp->applicationFilePath());
    foreach (QString filepath, folders) {
        QFile file(filepath);
        if (file.exists(filepath)) {
            bool rmFile = false;
            if (file.open(QIODevice::ReadOnly | QFile::Text)) {
                QTextStream s(&file);
                while (!s.atEnd()) {
                    QString line = s.readLine();
                    if (line.contains("cyanbin =")) {
                        if (line != appPath) {
                            rmFile = true;
                            break;
                        }
                    }
                }
                file.close();
            }
            if (rmFile) {
                file.remove(filepath);
            }
        } else {
            QFile sourcePy(":/gimp.py");
            if (sourcePy.open(QIODevice::ReadOnly | QFile::Text)) {
                QTextStream s(&sourcePy);
                if (file.open(QIODevice::WriteOnly | QFile::Text)) {
                    QTextStream o(&file);
                    while (!s.atEnd()) {
                        QString line = s.readLine();
                        if (line.contains("cyanbin = \"cyan\"")) {
                            line = appPath;
                        }
                        o << line << "\n";
                    }
                    file.setPermissions(QFileDevice::ExeUser|QFileDevice::ExeGroup|QFileDevice::ExeOther|QFileDevice::ReadOwner|QFileDevice::ReadGroup|QFileDevice::ReadOther|QFileDevice::WriteUser);
                    file.close();
                }
                sourcePy.close();
            }
        }
    }
}

void Cyan::openProfile(QString file)
{
    if (!file.isEmpty()) {
        /*profileDialog.profileCopyright->clear();
        profileDialog.profileDescription->clear();
        profileDialog.profileFileName->clear();

        profileDialog.profileCopyright->setText(cms.profileCopyrightFromFile(file));
        profileDialog.profileDescription->setText(cms.profileDescFromFile(file));
        profileDialog.profileFileName->setText(file);

        QString specs = QString::fromStdString(fx.identify(file.toStdString()));
        if (!profileDialog.profileDescription->text().isEmpty()) {
            profileDialog.show();
        } else {
            QMessageBox::warning(this, tr("Unsupported Color Profile"), tr("Unable to read the requested color profile."));
        }*/
    }
}

void Cyan::saveProfile()
{
    if (!profileDialog.profileDescription->text().isEmpty() && !profileDialog.profileFileName->text().isEmpty()) {
        /*QSettings settings;
        settings.beginGroup("default");

        QString dir;
        if (settings.value("lastSaveDir").isValid()) {
            dir = settings.value("lastSaveDir").toString();
        } else {
            dir = QDir::homePath();
        }
        QString output = QFileDialog::getSaveFileName(this, tr("Save Color Profile"), dir, tr("Color profiles (*.icc)"));
        if (!output.isEmpty()) {
            QFileInfo outFile(output);
            if (outFile.suffix().isEmpty() || outFile.suffix() != "icc") {
                output.append(".icc");
            }
            if (cms.editProfile(profileDialog.profileFileName->text(), output, profileDialog.profileDescription->text(), profileDialog.profileCopyright->text())) {
                QMessageBox::information(this, tr("Saved Color Profile"), tr("Color profile saved to disk."));
                profileDialog.profileCopyright->clear();
                profileDialog.profileDescription->clear();
                profileDialog.profileFileName->clear();
                profileDialog.hide();
            } else {
                QMessageBox::warning(this, tr("Unable to save"), tr("Failed to save color profile."));
            }
            settings.setValue("lastSaveDir", outFile.absoluteDir().absolutePath());
        }
        settings.endGroup();
        settings.sync();*/
    } else {
        QMessageBox::warning(this, tr("Unable to save"), tr("Nothing to save, please check the profile information."));
    }
}



void Cyan::renderingIntentUpdated(int)
{
    updateImage();
}

void Cyan::blackPointUpdated(int)
{
    updateImage();
}

int Cyan::supportedDepth()
{
    QString quantum = QString::fromStdString(fx.supportedQuantumDepth());
    quantum.remove("Q");
    return quantum.toInt();
}

void Cyan::loadedImage(FXX::Image image)
{
    qDebug() << "handle loaded image" << QString::fromStdString(image.filename);
    enableUI();
    if (image.imageBuffer.size()>0 && image.error.empty()) {
        clearImageBuffer();
        if (image.previewBuffer.size()>0) {
            resetImageZoom();
            setImage(QByteArray((char*)image.previewBuffer.data(),
                                (int)image.previewBuffer.size()));
        } else {
            qDebug() << "image preview missing!!!";
        }
        imageData = image;
        exportEmbeddedProfileAction->setDisabled(imageData.iccDescription.empty());
        if (!imageData.info.empty()) { parseImageInfo(); }
        getConvertProfiles();
    } else if (!image.error.empty()) {
        qDebug() << "image error" << QString::fromStdString(image.error);
    }
}

void Cyan::convertedImage(FXX::Image image)
{
    qDebug() << "handle converted image" << QString::fromStdString(image.filename);
    enableUI();
    if (image.previewBuffer.size()>0) {
        setImage(QByteArray((char*)image.previewBuffer.data(),
                            (int)image.previewBuffer.size()));
        imageData.info = image.info;
        parseImageInfo();
    } else {
        qDebug() << "image preview missing!!!";
    }
}

void Cyan::clearImageBuffer()
{
    fx.clearImage(imageData);
    imageInfoTree->clear();
}

void Cyan::parseImageInfo()
{
    QString info = QString::fromStdString(imageData.info);
    if (!info.isEmpty()) {
        imageInfoTree->clear();
        QStringList list = info.split("\n", QString::SkipEmptyParts);
        QVector<QTreeWidgetItem*> level1items;
        QVector<QTreeWidgetItem*> level2items;
        QString level1 = "  ";
        QString level2 = "    ";
        QString level3 = "      ";
        bool foundHistogramTag = false;
        for (int i = 0; i < list.size(); ++i) {
            QString item = list.at(i);
            if (item.startsWith(level1)) {
                QTreeWidgetItem *levelItem = new QTreeWidgetItem();
                QString section1 = item.section(":",0,0).trimmed();
                QString section2 = item.section(":",1).trimmed();
                if (item.startsWith("  Pixels per second:") || item.startsWith("  User time:") || item.startsWith("  Elapsed time:") || item.startsWith("  Version: Image")) {
                    continue;
                }
                levelItem->setText(0,section1);
                levelItem->setText(1,section2);
                if (item == "  Histogram:") {
                    foundHistogramTag = true;
                }
                if (foundHistogramTag && (item.startsWith("  Rendering intent:") || item.startsWith("  Gamma:"))) {
                    foundHistogramTag = false;
                }
                if (foundHistogramTag) {
                    continue;
                }
                if (item.startsWith(level3)) {
                    int parentID = level2items.size()-1;
                    if (parentID<0) {
                        parentID=0;
                    }
                    QTreeWidgetItem *parentItem = level2items.at(parentID);
                    if (parentItem) {
                        parentItem->addChild(levelItem);
                    }

                } else if(item.startsWith(level2)) {
                    int parentID = level1items.size()-1;
                    if (parentID<0) {
                        parentID=0;
                    }
                    QTreeWidgetItem *parentItem = level1items.at(parentID);
                    if (parentItem) {
                        parentItem->addChild(levelItem);
                        level2items << levelItem;
                    }
                } else if(item.startsWith(level1)) {
                    level1items << levelItem;
                }
                continue;
            }
        }
        imageInfoTree->addTopLevelItems(level1items.toList());
        level2items.clear();
        imageInfoTree->expandAll();
    }
}

QMap<QString, QString> Cyan::genProfiles(FXX::ColorSpace colorspace)
{
    QMap<QString,QString> output;
    QStringList folders;
    folders << QDir::rootPath() + "/WINDOWS/System32/spool/drivers/color";
    folders << "/Library/ColorSync/Profiles";
    folders << QDir::homePath() + "/Library/ColorSync/Profiles";
    folders << "/usr/share/color/icc";
    folders << "/usr/local/share/color/icc";
    folders << QDir::homePath() + "/.color/icc";
    QString cyanICCPath = QDir::homePath() + "/.config/Cyan/icc";
    QDir cyanICCDir(cyanICCPath);
    if (cyanICCDir.exists(cyanICCPath)) {
        folders << cyanICCPath;
    }
    for (int i = 0; i < folders.size(); ++i) {
        QStringList filter;
        filter << "*.icc" << "*.icm";
        QDirIterator it(folders.at(i), filter, QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            QString iccFile = it.next();
            QString profile = QString::fromStdString(fx.getProfileTag(iccFile.toStdString()));
            if (iccFile.isEmpty() || profile.isEmpty()) { continue; }
            if (fx.getProfileColorspace(iccFile.toStdString())!=colorspace) { continue; }
            output[profile] = iccFile;
        }
    }
    return output;
}

QByteArray Cyan::getDefaultProfile(FXX::ColorSpace colorspace)
{
    QByteArray bytes;
    if (colorspace != FXX::UnknownColorSpace) {
        QString fileName;
        QSettings settings;
        settings.beginGroup("profiles");
        if (!settings.value(QString::number(colorspace)).isNull()) {
            fileName = settings.value(QString::number(colorspace)).toString();
        }
        settings.endGroup();
        if (!fileName.isEmpty()) {
            QFile proFile(fileName);
            if (proFile.exists()) {
                if (proFile.open(QIODevice::ReadOnly)) {
                    bytes = proFile.readAll();
                    proFile.close();
                }
            }
        }
    }
    return bytes;
}
