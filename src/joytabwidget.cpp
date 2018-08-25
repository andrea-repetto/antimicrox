/* antimicro Gamepad to KB+M event mapper
 * Copyright (C) 2015 Travis Nickles <nickles.travis@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "joytabwidget.h"

#include "messagehandler.h"
#include "joyaxiswidget.h"
#include "joybuttonwidget.h"
#include "xmlconfigreader.h"
#include "xmlconfigwriter.h"
#include "buttoneditdialog.h"
#include "advancestickassignmentdialog.h"
#include "quicksetdialog.h"
#include "extraprofilesettingsdialog.h"
#include "setnamesdialog.h"
#include "stickpushbuttongroup.h"
#include "dpadpushbuttongroup.h"
#include "joybuttontypes/joycontrolstickbutton.h"
#include "joycontrolstick.h"
#include "joybuttontypes/joydpadbutton.h"
#include "joydpad.h"
#include "vdpad.h"
#include "common.h"
#include "joystick.h"
#include "axiseditdialog.h"
#include "inputdevice.h"
#include "antimicrosettings.h"

#include "gamecontroller/gamecontroller.h"
#include "gamecontrollermappingdialog.h"

#include <QDebug>
#include <QCoreApplication>
#include <QLayoutItem>
#include <QGroupBox>
#include <QMessageBox>
#include <QTextStream>
#include <QStringListIterator>
#include <QMenu>
#include <QAction>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QComboBox>
#include <QSpacerItem>
#include <QStackedWidget>
#include <QLabel>


JoyTabWidget::JoyTabWidget(InputDevice *joystick, AntiMicroSettings *settings, QWidget *parent) :
    QWidget(parent),
    tabHelper(joystick)
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);

    m_joystick = joystick;
    m_settings = settings;

    tabHelper.moveToThread(joystick->thread());

    comboBoxIndex = 0;
    hideEmptyButtons = false;

    verticalLayout = new QVBoxLayout (this);
    verticalLayout->setContentsMargins(4, 4, 4, 4);

    configHorizontalLayout = new QHBoxLayout();
    configBox = new QComboBox(this);
    configBox->addItem(trUtf8("<New>"), "");

    configBox->setObjectName(QString::fromUtf8("configBox"));
    configBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    configHorizontalLayout->addWidget(configBox);
    spacer1 = new QSpacerItem(30, 20, QSizePolicy::Fixed, QSizePolicy::Fixed);
    configHorizontalLayout->addItem(spacer1);

    removeButton = new QPushButton(trUtf8("Remove"), this);
    removeButton->setObjectName(QString::fromUtf8("removeButton"));
    removeButton->setToolTip(trUtf8("Remove configuration from recent list."));
    removeButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    removeButton->setIcon(QIcon::fromTheme("edit-clear-list"));
    configHorizontalLayout->addWidget(removeButton);

    loadButton = new QPushButton(trUtf8("Load"), this);
    loadButton->setObjectName(QString::fromUtf8("loadButton"));
    loadButton->setToolTip(trUtf8("Load configuration file."));
    loadButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    loadButton->setIcon(QIcon::fromTheme("document-open"));
    configHorizontalLayout->addWidget(loadButton);

    saveButton = new QPushButton(trUtf8("Save"), this);
    saveButton->setObjectName(QString::fromUtf8("saveButton"));
    saveButton->setToolTip(trUtf8("Save changes to configuration file."));
    saveButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    saveButton->setIcon(QIcon::fromTheme("document-save"));
    configHorizontalLayout->addWidget(saveButton);

    saveAsButton = new QPushButton(trUtf8("Save As"), this);
    saveAsButton->setObjectName(QString::fromUtf8("saveAsButton"));
    saveAsButton->setToolTip(trUtf8("Save changes to a new configuration file."));
    saveAsButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    saveAsButton->setIcon(QIcon::fromTheme("document-save-as"));
    configHorizontalLayout->addWidget(saveAsButton);

    verticalLayout->addLayout(configHorizontalLayout);
    verticalLayout->setStretchFactor(configHorizontalLayout, 1);

    spacer2 = new QSpacerItem(20, 5, QSizePolicy::Fixed, QSizePolicy::Fixed);
    verticalLayout->addItem(spacer2);

    verticalSpacer_2 = new QSpacerItem(20, 5, QSizePolicy::Minimum, QSizePolicy::Fixed);

    verticalLayout->addItem(verticalSpacer_2);


    stackedWidget_2 = new QStackedWidget(this);
    stackedWidget_2->setObjectName(QString::fromUtf8("stackedWidget_2"));

    page = new QWidget();
    page->setObjectName(QString::fromUtf8("page"));

    QVBoxLayout *tempVBoxLayout = new QVBoxLayout(page);
    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setObjectName(QString::fromUtf8("scrollArea1"));
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    scrollArea->setSizePolicy(sizePolicy);
    scrollArea->setWidgetResizable(true);

    QWidget *scrollAreaWidgetContents1 = new QWidget();
    scrollAreaWidgetContents1->setObjectName(QString::fromUtf8("scrollAreaWidgetContents1"));

    gridLayout = new QGridLayout(scrollAreaWidgetContents1);
    gridLayout->setSpacing(4);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));

    scrollArea->setWidget(scrollAreaWidgetContents1);
    tempVBoxLayout->addWidget(scrollArea);
    stackedWidget_2->addWidget(page);

    page_2 = new QWidget();
    page_2->setObjectName(QString::fromUtf8("page_2"));

    tempVBoxLayout = new QVBoxLayout(page_2);
    QScrollArea *scrollArea2 = new QScrollArea();
    scrollArea2->setObjectName(QString::fromUtf8("scrollArea2"));
    scrollArea2->setSizePolicy(sizePolicy);
    scrollArea2->setWidgetResizable(true);

    QWidget *scrollAreaWidgetContents2 = new QWidget();
    scrollAreaWidgetContents2->setObjectName(QString::fromUtf8("scrollAreaWidgetContents2"));

    gridLayout2 = new QGridLayout(scrollAreaWidgetContents2);
    gridLayout2->setSpacing(4);
    gridLayout2->setObjectName(QString::fromUtf8("gridLayout2"));

    scrollArea2->setWidget(scrollAreaWidgetContents2);
    tempVBoxLayout->addWidget(scrollArea2);
    stackedWidget_2->addWidget(page_2);

    page_3 = new QWidget();
    page_3->setObjectName(QString::fromUtf8("page_3"));

    tempVBoxLayout = new QVBoxLayout(page_3);
    QScrollArea *scrollArea3 = new QScrollArea();
    scrollArea3->setObjectName(QString::fromUtf8("scrollArea3"));
    scrollArea3->setSizePolicy(sizePolicy);
    scrollArea3->setWidgetResizable(true);

    QWidget *scrollAreaWidgetContents3 = new QWidget();
    scrollAreaWidgetContents3->setObjectName(QString::fromUtf8("scrollAreaWidgetContents3"));

    gridLayout3 = new QGridLayout(scrollAreaWidgetContents3);
    gridLayout3->setSpacing(4);
    gridLayout3->setObjectName(QString::fromUtf8("gridLayout3"));

    scrollArea3->setWidget(scrollAreaWidgetContents3);
    tempVBoxLayout->addWidget(scrollArea3);
    stackedWidget_2->addWidget(page_3);

    page_4 = new QWidget();
    page_4->setObjectName(QString::fromUtf8("page_4"));

    tempVBoxLayout = new QVBoxLayout(page_4);
    QScrollArea *scrollArea4 = new QScrollArea();
    scrollArea4->setObjectName(QString::fromUtf8("scrollArea4"));
    scrollArea4->setSizePolicy(sizePolicy);
    scrollArea4->setWidgetResizable(true);

    QWidget *scrollAreaWidgetContents4 = new QWidget();
    scrollAreaWidgetContents4->setObjectName(QString::fromUtf8("scrollAreaWidgetContents4"));

    gridLayout4 = new QGridLayout(scrollAreaWidgetContents4);
    gridLayout4->setSpacing(4);
    gridLayout4->setObjectName(QString::fromUtf8("gridLayout4"));

    scrollArea4->setWidget(scrollAreaWidgetContents4);
    tempVBoxLayout->addWidget(scrollArea4);
    stackedWidget_2->addWidget(page_4);

    page_5 = new QWidget();
    page_5->setObjectName(QString::fromUtf8("page_5"));

    tempVBoxLayout = new QVBoxLayout(page_5);
    QScrollArea *scrollArea5 = new QScrollArea();
    scrollArea5->setObjectName(QString::fromUtf8("scrollArea5"));
    scrollArea5->setSizePolicy(sizePolicy);
    scrollArea5->setWidgetResizable(true);

    QWidget *scrollAreaWidgetContents5 = new QWidget();
    scrollAreaWidgetContents5->setObjectName(QString::fromUtf8("scrollAreaWidgetContents5"));

    gridLayout5 = new QGridLayout(scrollAreaWidgetContents5);
    gridLayout5->setSpacing(4);
    gridLayout5->setObjectName(QString::fromUtf8("gridLayout5"));

    scrollArea5->setWidget(scrollAreaWidgetContents5);
    tempVBoxLayout->addWidget(scrollArea5);
    stackedWidget_2->addWidget(page_5);

    page_6 = new QWidget();
    page_6->setObjectName(QString::fromUtf8("page_6"));

    tempVBoxLayout = new QVBoxLayout(page_6);
    QScrollArea *scrollArea6 = new QScrollArea();
    scrollArea6->setObjectName(QString::fromUtf8("scrollArea6"));
    scrollArea6->setSizePolicy(sizePolicy);
    scrollArea6->setWidgetResizable(true);

    QWidget *scrollAreaWidgetContents6 = new QWidget();
    scrollAreaWidgetContents6->setObjectName(QString::fromUtf8("scrollAreaWidgetContents6"));

    gridLayout6 = new QGridLayout(scrollAreaWidgetContents6);
    gridLayout6->setSpacing(4);
    gridLayout6->setObjectName(QString::fromUtf8("gridLayout6"));

    scrollArea6->setWidget(scrollAreaWidgetContents6);
    tempVBoxLayout->addWidget(scrollArea6);
    stackedWidget_2->addWidget(page_6);

    page_7 = new QWidget();
    page_7->setObjectName(QString::fromUtf8("page_7"));

    tempVBoxLayout = new QVBoxLayout(page_7);
    QScrollArea *scrollArea7 = new QScrollArea();
    scrollArea7->setObjectName(QString::fromUtf8("scrollArea7"));
    scrollArea7->setSizePolicy(sizePolicy);
    scrollArea7->setWidgetResizable(true);

    QWidget *scrollAreaWidgetContents7 = new QWidget();
    scrollAreaWidgetContents7->setObjectName(QString::fromUtf8("scrollAreaWidgetContents7"));

    gridLayout7 = new QGridLayout(scrollAreaWidgetContents7);
    gridLayout7->setSpacing(4);
    gridLayout7->setObjectName(QString::fromUtf8("gridLayout7"));

    scrollArea7->setWidget(scrollAreaWidgetContents7);
    tempVBoxLayout->addWidget(scrollArea7);
    stackedWidget_2->addWidget(page_7);

    page_8 = new QWidget();
    page_8->setObjectName(QString::fromUtf8("page_8"));

    tempVBoxLayout = new QVBoxLayout(page_8);
    QScrollArea *scrollArea8 = new QScrollArea();
    scrollArea8->setObjectName(QString::fromUtf8("scrollArea8"));
    scrollArea8->setSizePolicy(sizePolicy);
    scrollArea8->setWidgetResizable(true);

    QWidget *scrollAreaWidgetContents8 = new QWidget();
    scrollAreaWidgetContents8->setObjectName(QString::fromUtf8("scrollAreaWidgetContents8"));

    gridLayout8 = new QGridLayout(scrollAreaWidgetContents8);
    gridLayout8->setSpacing(4);
    gridLayout8->setObjectName(QString::fromUtf8("gridLayout8"));

    scrollArea8->setWidget(scrollAreaWidgetContents8);
    tempVBoxLayout->addWidget(scrollArea8);
    stackedWidget_2->addWidget(page_8);

    verticalLayout->addWidget(stackedWidget_2);

    verticalSpacer_3 = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Fixed);
    verticalLayout->addItem(verticalSpacer_3);


    horizontalLayout_2 = new QHBoxLayout();
    horizontalLayout_2->setSpacing(6);
    horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));

    setsMenuButton = new QPushButton(trUtf8("Sets"), this);
    QMenu *setMenu = new QMenu(setsMenuButton);
    copySetMenu = new QMenu(trUtf8("Copy from Set"), setMenu);
    QAction *setSettingsAction = new QAction(trUtf8("Settings"), setMenu);
    connect(setSettingsAction, &QAction::triggered, this, &JoyTabWidget::showSetNamesDialog);
    setMenu->addAction(setSettingsAction);
    setMenu->addMenu(copySetMenu);
    setMenu->addSeparator();

    refreshCopySetActions();

    setAction1 = new QAction(trUtf8("Set 1"), setMenu);
    connect(setAction1, &QAction::triggered, this, &JoyTabWidget::changeSetOne);
    setMenu->addAction(setAction1);

    setAction2 = new QAction(trUtf8("Set 2"), setMenu);
    connect(setAction2, &QAction::triggered, this, &JoyTabWidget::changeSetTwo);
    setMenu->addAction(setAction2);

    setAction3 = new QAction(trUtf8("Set 3"), setMenu);
    connect(setAction3, &QAction::triggered, this, &JoyTabWidget::changeSetThree);
    setMenu->addAction(setAction3);

    setAction4 = new QAction(trUtf8("Set 4"), setMenu);
    connect(setAction4, &QAction::triggered, this, &JoyTabWidget::changeSetFour);
    setMenu->addAction(setAction4);

    setAction5 = new QAction(trUtf8("Set 5"), setMenu);
    connect(setAction5, &QAction::triggered, this, &JoyTabWidget::changeSetFive);
    setMenu->addAction(setAction5);

    setAction6 = new QAction(trUtf8("Set 6"), setMenu);
    connect(setAction6, &QAction::triggered, this, &JoyTabWidget::changeSetSix);
    setMenu->addAction(setAction6);

    setAction7 = new QAction(trUtf8("Set 7"), setMenu);
    connect(setAction7, &QAction::triggered, this, &JoyTabWidget::changeSetSeven);
    setMenu->addAction(setAction7);

    setAction8 = new QAction(trUtf8("Set 8"), setMenu);
    connect(setAction8, &QAction::triggered, this, &JoyTabWidget::changeSetEight);
    setMenu->addAction(setAction8);

    setsMenuButton->setMenu(setMenu);
    horizontalLayout_2->addWidget(setsMenuButton);

    setPushButton1 = new QPushButton("1", this);
    setPushButton1->setObjectName(QString::fromUtf8("setPushButton1"));
    setPushButton1->setProperty("setActive", true);

    horizontalLayout_2->addWidget(setPushButton1);

    setPushButton2 = new QPushButton("2", this);
    setPushButton2->setObjectName(QString::fromUtf8("setPushButton2"));
    setPushButton2->setProperty("setActive", false);

    horizontalLayout_2->addWidget(setPushButton2);

    setPushButton3 = new QPushButton("3", this);
    setPushButton3->setObjectName(QString::fromUtf8("setPushButton3"));
    setPushButton3->setProperty("setActive", false);

    horizontalLayout_2->addWidget(setPushButton3);

    setPushButton4 = new QPushButton("4", this);
    setPushButton4->setObjectName(QString::fromUtf8("setPushButton4"));
    setPushButton4->setProperty("setActive", false);

    horizontalLayout_2->addWidget(setPushButton4);

    setPushButton5 = new QPushButton("5", this);
    setPushButton5->setObjectName(QString::fromUtf8("setPushButton5"));
    setPushButton5->setProperty("setActive", false);

    horizontalLayout_2->addWidget(setPushButton5);

    setPushButton6 = new QPushButton("6", this);
    setPushButton6->setObjectName(QString::fromUtf8("setPushButton6"));
    setPushButton6->setProperty("setActive", false);

    horizontalLayout_2->addWidget(setPushButton6);

    setPushButton7 = new QPushButton("7", this);
    setPushButton7->setObjectName(QString::fromUtf8("setPushButton7"));
    setPushButton7->setProperty("setActive", false);

    horizontalLayout_2->addWidget(setPushButton7);

    setPushButton8 = new QPushButton("8", this);
    setPushButton8->setObjectName(QString::fromUtf8("setPushButton8"));
    setPushButton8->setProperty("setActive", false);

    horizontalLayout_2->addWidget(setPushButton8);

    refreshSetButtons();

    verticalLayout->addLayout(horizontalLayout_2);

    spacer3 = new QSpacerItem(20, 5, QSizePolicy::Fixed, QSizePolicy::Fixed);
    verticalLayout->addItem(spacer3);

    horizontalLayout_3 = new QHBoxLayout();
    horizontalLayout_3->setSpacing(6);
    horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));

    stickAssignPushButton = new QPushButton(trUtf8("Stick/Pad Assign"), this);
    stickAssignPushButton->setObjectName(QString::fromUtf8("stickAssignPushButton"));
    QIcon icon7(QIcon::fromTheme(QString::fromUtf8("games-config-options")));
    stickAssignPushButton->setIcon(icon7);
    horizontalLayout_3->addWidget(stickAssignPushButton);

    gameControllerMappingPushButton = new QPushButton(trUtf8("Controller Mapping"), this);
    gameControllerMappingPushButton->setObjectName(QString::fromUtf8("gameControllerMappingPushButton"));
    gameControllerMappingPushButton->setIcon(QIcon::fromTheme("games-config-options"));
    gameControllerMappingPushButton->setEnabled(false);
    gameControllerMappingPushButton->setVisible(false);
    horizontalLayout_3->addWidget(gameControllerMappingPushButton);

    quickSetPushButton = new QPushButton(trUtf8("Quick Set"), this);
    quickSetPushButton->setObjectName(QString::fromUtf8("quickSetPushButton"));
    horizontalLayout_3->addWidget(quickSetPushButton);

    QSpacerItem *horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout_3->addItem(horizontalSpacer_2);

    namesPushButton = new QPushButton(trUtf8("Names"), this);
    namesPushButton->setObjectName(QString::fromUtf8("namesPushButton"));
    namesPushButton->setToolTip(trUtf8("Toggle button name displaying."));
    namesPushButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    namesPushButton->setIcon(QIcon::fromTheme("text-field"));
    horizontalLayout_3->addWidget(namesPushButton);

    delayButton = new QPushButton(trUtf8("Pref"), this);
    delayButton->setObjectName(QString::fromUtf8("delayButton"));
    delayButton->setToolTip(trUtf8("Change global profile settings."));
    delayButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    horizontalLayout_3->addWidget(delayButton);

    resetButton = new QPushButton(trUtf8("Reset"), this);
    resetButton->setObjectName(QString::fromUtf8("resetButton"));
    resetButton->setToolTip(trUtf8("Revert changes to the configuration. Reload configuration file."));
    resetButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    resetButton->setIcon(QIcon::fromTheme("document-revert"));
    horizontalLayout_3->addWidget(resetButton);

    verticalLayout->addLayout(horizontalLayout_3);

    displayingNames = false;

    stickAssignPushButton->setEnabled(false);
    stickAssignPushButton->setVisible(false);

    gameControllerMappingPushButton->setEnabled(true);
    gameControllerMappingPushButton->setVisible(true);


#ifdef Q_OS_WIN
    deviceKeyRepeatSettings();
#endif

    checkHideEmptyOption();

    connect(loadButton, &QPushButton::clicked, this, &JoyTabWidget::openConfigFileDialog);
    connect(saveButton, &QPushButton::clicked, this, &JoyTabWidget::saveConfigFile);
    connect(resetButton, &QPushButton::clicked, this, &JoyTabWidget::resetJoystick);
    connect(namesPushButton, &QPushButton::clicked, this, &JoyTabWidget::toggleNames);

    connect(saveAsButton, &QPushButton::clicked, this, &JoyTabWidget::saveAsConfig);
    connect(delayButton, &QPushButton::clicked, this, &JoyTabWidget::showKeyDelayDialog);
    connect(removeButton, &QPushButton::clicked, this, &JoyTabWidget::removeConfig);
    connect(setPushButton1, &QPushButton::clicked, this, &JoyTabWidget::changeSetOne);
    connect(setPushButton2, &QPushButton::clicked, this, &JoyTabWidget::changeSetTwo);
    connect(setPushButton3, &QPushButton::clicked, this, &JoyTabWidget::changeSetThree);
    connect(setPushButton4, &QPushButton::clicked, this, &JoyTabWidget::changeSetFour);
    connect(setPushButton5, &QPushButton::clicked, this, &JoyTabWidget::changeSetFive);
    connect(setPushButton6, &QPushButton::clicked, this, &JoyTabWidget::changeSetSix);
    connect(setPushButton7, &QPushButton::clicked, this, &JoyTabWidget::changeSetSeven);
    connect(setPushButton8, &QPushButton::clicked, this, &JoyTabWidget::changeSetEight);

    connect(stickAssignPushButton, &QPushButton::clicked, this, &JoyTabWidget::showStickAssignmentDialog);
    connect(gameControllerMappingPushButton, &QPushButton::clicked, this, &JoyTabWidget::openGameControllerMappingWindow);

    connect(quickSetPushButton, &QPushButton::clicked, this, &JoyTabWidget::showQuickSetDialog);
    connect(this, &JoyTabWidget::joystickConfigChanged, this, &JoyTabWidget::refreshSetButtons);
    connect(this, &JoyTabWidget::joystickConfigChanged, this, &JoyTabWidget::refreshCopySetActions);
    connect(joystick, &InputDevice::profileUpdated, this, &JoyTabWidget::displayProfileEditNotification);

    connect(joystick, &InputDevice::requestProfileLoad, this, &JoyTabWidget::loadConfigFile, Qt::QueuedConnection);

    reconnectCheckUnsavedEvent();
    reconnectMainComboBoxEvents();

}

void JoyTabWidget::openConfigFileDialog()
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);

    m_settings->getLock()->lock();

    int numberRecentProfiles = m_settings->value("NumberRecentProfiles", DEFAULTNUMBERPROFILES).toInt();
    QString lookupDir = PadderCommon::preferredProfileDir(m_settings);

    QString filename = QFileDialog::getOpenFileName(this, trUtf8("Open Config"), lookupDir, trUtf8("Config Files (*.amgp *.xml)"));

    m_settings->getLock()->unlock();

    if (!filename.isNull() && !filename.isEmpty())
    {
        QFileInfo fileinfo(filename);
        int searchIndex = configBox->findData(fileinfo.absoluteFilePath());
        if (searchIndex == -1)
        {
            if ((numberRecentProfiles > 0) && (configBox->count() == (numberRecentProfiles + 1)))
            {
                configBox->removeItem(numberRecentProfiles);
            }

            configBox->insertItem(1, PadderCommon::getProfileName(fileinfo), fileinfo.absoluteFilePath());
            configBox->setCurrentIndex(1);
            saveDeviceSettings();
            emit joystickConfigChanged(m_joystick->getJoyNumber());
        }
        else
        {
            configBox->setCurrentIndex(searchIndex);
            saveDeviceSettings();
            emit joystickConfigChanged(m_joystick->getJoyNumber());
        }

        QString outputFilename = fileinfo.absoluteDir().absolutePath();

#if defined(Q_OS_WIN) && defined(WIN_PORTABLE_PACKAGE)
        if (fileinfo.absoluteDir().isAbsolute())
        {
            QDir tempDir = fileinfo.dir();
            tempDir.cdUp();
            if (tempDir.path() == qApp->applicationDirPath())
            {
                outputFilename = QString("%1/").arg(fileinfo.dir().dirName());
            }
        }
#endif

        m_settings->getLock()->lock();

        m_settings->setValue("LastProfileDir", outputFilename);
        m_settings->sync();

        m_settings->getLock()->unlock();
    }
}

/**
 * @brief Create and render all push buttons corresponding to joystick
 *     controls for all sets.
 */
void JoyTabWidget::fillButtons()
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);

    m_joystick->establishPropertyUpdatedConnection();
    connect(m_joystick, &InputDevice::setChangeActivated, this, &JoyTabWidget::changeCurrentSet, Qt::QueuedConnection);

    for (int i=0; i < Joystick::NUMBER_JOYSETS; i++)
    {
        SetJoystick *currentSet = m_joystick->getSetJoystick(i);
        fillSetButtons(currentSet);
    }

    refreshCopySetActions();
}

void JoyTabWidget::showButtonDialog()
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);

    JoyButtonWidget *buttonWidget = qobject_cast<JoyButtonWidget*>(sender()); // static_cast
    JoyButton *button = buttonWidget->getJoyButton();

    ButtonEditDialog *dialog = new ButtonEditDialog(button, m_joystick, this);
    dialog->show();
}

void JoyTabWidget::showAxisDialog()
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);

    JoyAxisWidget *axisWidget = qobject_cast<JoyAxisWidget*>(sender()); // static_cast
    JoyAxis *axis = axisWidget->getAxis();

    axisDialog = new AxisEditDialog (axis, this);
    axisDialog->show();
}

void JoyTabWidget::saveConfigFile()
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);

    int index = configBox->currentIndex();

    m_settings->getLock()->lock();

    int numberRecentProfiles = m_settings->value("NumberRecentProfiles", DEFAULTNUMBERPROFILES).toInt();
    QString filename = QString();
    if (index == 0)
    {
        QString lookupDir = PadderCommon::preferredProfileDir(m_settings);
        m_settings->getLock()->unlock();
        QString tempfilename = QFileDialog::getSaveFileName(this, trUtf8("Save Config"), lookupDir, trUtf8("Config File (*.%1.amgp)").arg(m_joystick->getXmlName()));
        if (!tempfilename.isEmpty())
        {
            filename = tempfilename;

            QFileInfo fileinfo(filename);
            QString deviceTypeName = m_joystick->getXmlName();
            QString fileSuffix = deviceTypeName.append(".amgp");
            if ((fileinfo.suffix() != "xml") && (fileinfo.suffix() != "amgp"))
            {
                filename = filename.append(".").append(fileSuffix);
            }
        }
    }
    else
    {
        m_settings->getLock()->unlock();
        filename = configBox->itemData(index).toString();
    }

    if (!filename.isEmpty())
    {
        QFileInfo fileinfo(filename);

        QMetaObject::invokeMethod(&tabHelper, "writeConfigFile", Qt::BlockingQueuedConnection,
                                  Q_ARG(QString, fileinfo.absoluteFilePath()));
        XMLConfigWriter *writer = tabHelper.getWriter();

        if (writer->hasError() && this->window()->isEnabled())
        {
            QMessageBox msg;
            msg.setStandardButtons(QMessageBox::Close);
            msg.setText(writer->getErrorString());
            msg.setModal(true);
            msg.exec();
        }
        else if (writer->hasError() && !this->window()->isEnabled())
        {
            QTextStream error(stderr);
            error << writer->getErrorString() << endl;
        }
        else
        {
            int existingIndex = configBox->findData(fileinfo.absoluteFilePath());

            if (existingIndex == -1)
            {
                if ((numberRecentProfiles > 0) && (configBox->count() == (numberRecentProfiles + 1)))
                {
                    configBox->removeItem(numberRecentProfiles);
                }

                m_joystick->revertProfileEdited();
                QString tempProfileName = PadderCommon::getProfileName(fileinfo);
                if (!m_joystick->getProfileName().isEmpty())
                {
                    oldProfileName = m_joystick->getProfileName();
                    tempProfileName = oldProfileName;
                }

                disconnectCheckUnsavedEvent();
                disconnectMainComboBoxEvents();

                configBox->insertItem(1, tempProfileName, fileinfo.absoluteFilePath());

                reconnectCheckUnsavedEvent();
                reconnectMainComboBoxEvents();

                configBox->setCurrentIndex(1);
                saveDeviceSettings(true);

                emit joystickConfigChanged(m_joystick->getJoyNumber());
            }
            else
            {
                m_joystick->revertProfileEdited();
                if (!m_joystick->getProfileName().isEmpty())
                {
                    oldProfileName = m_joystick->getProfileName();
                }

                configBox->setItemIcon(existingIndex, QIcon());
                saveDeviceSettings(true);

                emit joystickConfigChanged(m_joystick->getJoyNumber());
            }
        }
    }
}

void JoyTabWidget::resetJoystick()
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);

    int currentIndex = configBox->currentIndex();
    if (currentIndex != 0)
    {
        QString filename = configBox->itemData(currentIndex).toString();

        removeCurrentButtons();

        QMetaObject::invokeMethod(&tabHelper, "readConfigFileWithRevert", Qt::BlockingQueuedConnection,
                                  Q_ARG(QString, filename));

        fillButtons();
        refreshSetButtons();
        refreshCopySetActions();

        XMLConfigReader *reader = tabHelper.getReader();
        if (!reader->hasError())
        {
            configBox->setItemIcon(currentIndex, QIcon());

            QString tempProfileName = QString();
            if (!m_joystick->getProfileName().isEmpty())
            {
                tempProfileName = m_joystick->getProfileName();
                configBox->setItemText(currentIndex, tempProfileName);
            }
            else
            {
                tempProfileName = oldProfileName;
                configBox->setItemText(currentIndex, oldProfileName);
            }

            oldProfileName = tempProfileName;
        }
        else if (reader->hasError() && this->window()->isEnabled())
        {
            QMessageBox msg;
            msg.setStandardButtons(QMessageBox::Close);
            msg.setText(reader->getErrorString());
            msg.setModal(true);
            msg.exec();
        }
        else if (reader->hasError() && !this->window()->isEnabled())
        {
            QTextStream error(stderr);
            error << reader->getErrorString() << endl;
        }
    }
    else
    {
        configBox->setItemText(0, trUtf8("<New>"));
        configBox->setItemIcon(0, QIcon());

        removeCurrentButtons();

        QMetaObject::invokeMethod(&tabHelper, "reInitDevice", Qt::BlockingQueuedConnection);

        fillButtons();
        refreshSetButtons();
        refreshCopySetActions();
    }
}

void JoyTabWidget::saveAsConfig()
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);

    int index = configBox->currentIndex();

    m_settings->getLock()->lock();

    int numberRecentProfiles = m_settings->value("NumberRecentProfiles", DEFAULTNUMBERPROFILES).toInt();
    QString filename = QString();
    if (index == 0)
    {
        QString lookupDir = PadderCommon::preferredProfileDir(m_settings);
        m_settings->getLock()->unlock();

        QString tempfilename = QFileDialog::getSaveFileName(this, trUtf8("Save Config"), lookupDir, trUtf8("Config File (*.%1.amgp)").arg(m_joystick->getXmlName()));
        if (!tempfilename.isEmpty())
        {
            filename = tempfilename;
        }
    }
    else
    {
        m_settings->getLock()->unlock();

        QString configPath = configBox->itemData(index).toString();
        QFileInfo temp(configPath);
        QString tempfilename = QFileDialog::getSaveFileName(this, trUtf8("Save Config"), temp.absoluteDir().absolutePath(), trUtf8("Config File (*.%1.amgp)").arg(m_joystick->getXmlName()));
        if (!tempfilename.isEmpty())
        {
            filename = tempfilename;
        }
    }

    if (!filename.isEmpty())
    {
        QFileInfo fileinfo(filename);
        QString deviceTypeName = m_joystick->getXmlName();
        QString fileSuffix = deviceTypeName.append(".amgp");
        if ((fileinfo.suffix() != "xml") && (fileinfo.suffix() != "amgp"))
        {
            filename = filename.append(".").append(fileSuffix);
        }
        fileinfo.setFile(filename);

        QMetaObject::invokeMethod(&tabHelper, "writeConfigFile", Qt::BlockingQueuedConnection,
                                  Q_ARG(QString, fileinfo.absoluteFilePath()));
        XMLConfigWriter *writer = tabHelper.getWriter();

        if (writer->hasError() && this->window()->isEnabled())
        {
            QMessageBox msg;
            msg.setStandardButtons(QMessageBox::Close);
            msg.setText(writer->getErrorString());
            msg.setModal(true);
            msg.exec();
        }
        else if (writer->hasError() && !this->window()->isEnabled())
        {
            QTextStream error(stderr);
            error << writer->getErrorString() << endl;
        }
        else
        {
            int existingIndex = configBox->findData(fileinfo.absoluteFilePath());
            if (existingIndex == -1)
            {
                disconnectCheckUnsavedEvent();
                disconnectMainComboBoxEvents();

                if ((numberRecentProfiles > 0) && (configBox->count() == (numberRecentProfiles + 1)))
                {
                    configBox->removeItem(numberRecentProfiles);
                }

                m_joystick->revertProfileEdited();
                QString tempProfileName = PadderCommon::getProfileName(fileinfo);
                if (!m_joystick->getProfileName().isEmpty())
                {
                    oldProfileName = m_joystick->getProfileName();
                    tempProfileName = oldProfileName;
                }

                configBox->insertItem(1, tempProfileName, fileinfo.absoluteFilePath());

                reconnectCheckUnsavedEvent();
                reconnectMainComboBoxEvents();

                configBox->setCurrentIndex(1);
                saveDeviceSettings(true);
                emit joystickConfigChanged(m_joystick->getJoyNumber());
            }
            else
            {
                m_joystick->revertProfileEdited();
                if (!m_joystick->getProfileName().isEmpty())
                {
                    oldProfileName = m_joystick->getProfileName();
                }

                configBox->setItemIcon(existingIndex, QIcon());
                saveDeviceSettings(true);
                emit joystickConfigChanged(m_joystick->getJoyNumber());
            }
        }
    }
}

void JoyTabWidget::changeJoyConfig(int index)
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);

    disconnect(m_joystick, &InputDevice::profileUpdated, this, &JoyTabWidget::displayProfileEditNotification);

    QString filename = QString();
    if (index > 0)
    {
        filename = configBox->itemData(index).toString();
    }

    if (!filename.isEmpty())
    {
        removeCurrentButtons();
        emit forceTabUnflash(this);

        QMetaObject::invokeMethod(&tabHelper, "readConfigFile", Qt::BlockingQueuedConnection,
                                  Q_ARG(QString, filename));

        fillButtons();
        refreshSetButtons();
        refreshCopySetActions();
        configBox->setItemText(0, trUtf8("<New>"));
        XMLConfigReader *reader = tabHelper.getReader();

        if (!reader->hasError())
        {
            QString profileName = QString();
            if (!m_joystick->getProfileName().isEmpty())
            {
                profileName = m_joystick->getProfileName();
                oldProfileName = profileName;
            }
            else
            {
                QFileInfo profile(filename);
                oldProfileName = PadderCommon::getProfileName(profile);
                profileName = oldProfileName;
            }
            configBox->setItemText(index, profileName);
        }
        else if (reader->hasError() && this->window()->isEnabled())
        {
            QMessageBox msg;
            msg.setStandardButtons(QMessageBox::Close);
            msg.setText(reader->getErrorString());
            msg.setModal(true);
            msg.exec();
        }
        else if (reader->hasError() && !this->window()->isEnabled())
        {
            QTextStream error(stderr);
            error << reader->getErrorString() << endl;
        }
    }
    else if (index == 0)
    {
        removeCurrentButtons();
        emit forceTabUnflash(this);

        QMetaObject::invokeMethod(&tabHelper, "reInitDevice", Qt::BlockingQueuedConnection);

        fillButtons();
        refreshSetButtons();
        refreshCopySetActions();
        configBox->setItemText(0, trUtf8("<New>"));
        oldProfileName = "";
    }

    comboBoxIndex = index;

    connect(m_joystick, &InputDevice::profileUpdated, this, &JoyTabWidget::displayProfileEditNotification);
}

void JoyTabWidget::saveSettings()
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);

    QString filename = "";
    QString lastfile = "";

    m_settings->getLock()->lock();

    int index = configBox->currentIndex();
    int currentjoy = 1;

    QString identifier = m_joystick->getStringIdentifier();

    QString controlEntryPrefix = QString("Controller%1").arg(identifier);
    QString controlEntryString = QString("Controller%1ConfigFile%2").arg(identifier);
    QString controlEntryLastSelected = QString("Controller%1LastSelected").arg(identifier);
    QString controlEntryProfileName = QString("Controller%1ProfileName%2").arg(m_joystick->getStringIdentifier());

    // Remove current settings for a controller
    QStringList tempkeys = m_settings->allKeys();
    QStringListIterator iter(tempkeys);
    while (iter.hasNext())
    {
        QString tempstring = iter.next();
        if (!identifier.isEmpty() && tempstring.startsWith(controlEntryPrefix))
        {
            m_settings->remove(tempstring);
        }
    }

    // Output currently selected profile as first profile on the list
    if (index != 0)
    {
        filename = lastfile = configBox->itemData(index).toString();
        QString profileText = configBox->itemText(index);
        if (!identifier.isEmpty())
        {
            QFileInfo profileBaseFile(filename);
            QString outputFilename = filename;

#if defined(Q_OS_WIN) && defined(WIN_PORTABLE_PACKAGE)
            if (profileBaseFile.isAbsolute())
            {
                QDir tempDir = profileBaseFile.dir();
                tempDir.cdUp();
                if (tempDir.path() == qApp->applicationDirPath())
                {
                    outputFilename = QString("%1/%2")
                            .arg(profileBaseFile.dir().dirName())
                            .arg(profileBaseFile.fileName());
                }
            }
#endif

            m_settings->setValue(controlEntryString.arg(currentjoy), outputFilename);

            if (PadderCommon::getProfileName(profileBaseFile) != profileText)
            {
                m_settings->setValue(controlEntryProfileName.arg(currentjoy), profileText);
            }
        }

        currentjoy++;
    }
    else
    {
        lastfile = "";
    }

    // Write the remaining profile locations to the settings file
    for (int i=1; i < configBox->count(); i++)
    {
        if (i != index)
        {
           filename = configBox->itemData(i).toString();
           QString profileText = configBox->itemText(i);
           if (!identifier.isEmpty())
           {
               QFileInfo profileBaseFile(filename);
               QString outputFilename = filename;

#if defined(Q_OS_WIN) && defined(WIN_PORTABLE_PACKAGE)
               if (profileBaseFile.isAbsolute())
               {
                   QDir tempDir = profileBaseFile.dir();
                   tempDir.cdUp();
                   if (tempDir.path() == qApp->applicationDirPath())
                   {
                       outputFilename = QString("%1/%2")
                               .arg(profileBaseFile.dir().dirName())
                               .arg(profileBaseFile.fileName());
                   }
               }
#endif
               m_settings->setValue(controlEntryString.arg(currentjoy), outputFilename);

               if (PadderCommon::getProfileName(profileBaseFile) != profileText)
               {
                   m_settings->setValue(controlEntryProfileName.arg(currentjoy), profileText);
               }
           }

           currentjoy++;
        }
    }

    if (!identifier.isEmpty())
    {
        QFileInfo profileBaseFile(lastfile);
        QString outputFilename = lastfile;

#if defined(Q_OS_WIN) && defined(WIN_PORTABLE_PACKAGE)
       if (profileBaseFile.isAbsolute())
       {
           QDir tempDir = profileBaseFile.dir();
           tempDir.cdUp();
           if (tempDir.path() == qApp->applicationDirPath())
           {
               outputFilename = QString("%1/%2")
                       .arg(profileBaseFile.dir().dirName())
                       .arg(profileBaseFile.fileName());
           }
       }
#endif

        m_settings->setValue(controlEntryLastSelected, outputFilename);
    }

    m_settings->getLock()->unlock();
}

void JoyTabWidget::loadSettings(bool forceRefresh)
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);

    disconnect(configBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &JoyTabWidget::changeJoyConfig);

    m_settings->getLock()->lock();

    if (configBox->count() > 1)
    {
        configBox->clear();
        configBox->addItem(trUtf8("<New>"), "");
        configBox->setCurrentIndex(-1);
    }
    else if (forceRefresh)
    {
        configBox->setCurrentIndex(-1);
    }

    int shouldisplaynames = m_settings->value("DisplayNames", "0").toInt();
    if (shouldisplaynames == 1)
    {
        changeNameDisplay(shouldisplaynames);
    }

    int numberRecentProfiles = m_settings->value("NumberRecentProfiles", DEFAULTNUMBERPROFILES).toInt();
    bool autoOpenLastProfile = m_settings->value("AutoOpenLastProfile", true).toBool();

    m_settings->beginGroup("Controllers");

    QString controlEntryString = QString("Controller%1ConfigFile%2").arg(m_joystick->getStringIdentifier());
    QString controlEntryLastSelected = QString("Controller%1LastSelected").arg(m_joystick->getStringIdentifier());
    QString controlEntryProfileName = QString("Controller%1ProfileName%2").arg(m_joystick->getStringIdentifier());

    bool finished = false;
    for (int i=1; !finished; i++)
    {
        QString tempfilepath = QString();

        if (!m_joystick->getStringIdentifier().isEmpty())
        {
            tempfilepath = m_settings->value(controlEntryString.arg(i), "").toString();
        }

        if (!tempfilepath.isEmpty())
        {
            QFileInfo fileInfo(tempfilepath);

            if (fileInfo.exists() && (configBox->findData(fileInfo.absoluteFilePath()) == -1))
            {
                QString profileName = m_settings->value(controlEntryProfileName.arg(i), "").toString();
                profileName = !profileName.isEmpty() ? profileName : PadderCommon::getProfileName(fileInfo);
                configBox->addItem(profileName, fileInfo.absoluteFilePath());
            }
        }
        else
        {
            finished = true;
        }

        if ((numberRecentProfiles > 0) && (i == numberRecentProfiles))
        {
            finished = true;
        }
    }

    connect(configBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &JoyTabWidget::changeJoyConfig, Qt::QueuedConnection);

    QString lastfile = QString();

    if (!m_joystick->getStringIdentifier().isEmpty() && autoOpenLastProfile)
    {
        lastfile = m_settings->value(controlEntryLastSelected, "").toString();
    }

    m_settings->endGroup();
    m_settings->getLock()->unlock();

    if (!lastfile.isEmpty())
    {
        QString lastFileAbsolute = lastfile;

#if defined(Q_OS_WIN) && defined(WIN_PORTABLE_PACKAGE)
        QFileInfo lastFileInfo(lastfile);
        lastFileAbsolute = lastFileInfo.absoluteFilePath();
#endif

        // preventing checkForUnsavedProfile(int) at program start-up
        disconnectCheckUnsavedEvent();

        int lastindex = configBox->findData(lastFileAbsolute);
        if (lastindex > 0)
        {
            configBox->setCurrentIndex(lastindex);
            emit joystickConfigChanged(m_joystick->getJoyNumber());
        }
        else if (configBox->currentIndex() != 0)
        {
            configBox->setCurrentIndex(0);
            emit joystickConfigChanged(m_joystick->getJoyNumber());
        }

        reconnectCheckUnsavedEvent();
    }
    else if (configBox->currentIndex() != 0)
    {
        configBox->setCurrentIndex(0);
        emit joystickConfigChanged(m_joystick->getJoyNumber());
    }
}

QHash<int, QString>* JoyTabWidget::recentConfigs()
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);

    QHash<int, QString> *temp = new QHash<int, QString>();
    for (int i=1; i < configBox->count(); i++)
    {
        QString current = configBox->itemText(i);
        temp->insert(i, current);
    }

    return temp;
}

void JoyTabWidget::setCurrentConfig(int index)
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);

    // Allow 0 to select new/'null' config and therefore disable any mapping
    if ((index >= 0) && (index < configBox->count()))
    {
        configBox->setCurrentIndex(index);
    }
}

int JoyTabWidget::getCurrentConfigIndex()
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);

    return configBox->currentIndex();
}

QString JoyTabWidget::getCurrentConfigName()
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);

    return configBox->currentText();
}

QString JoyTabWidget::getConfigName(int index)
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);

    return configBox->itemText(index);
}

void JoyTabWidget::changeCurrentSet(int index)
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);
    int currentPage = stackedWidget_2->currentIndex();
    QPushButton *oldSetButton = nullptr;
    QPushButton *activeSetButton = nullptr;

    switch (currentPage)
    {
        case 0: oldSetButton = setPushButton1; break;
        case 1: oldSetButton = setPushButton2; break;
        case 2: oldSetButton = setPushButton3; break;
        case 3: oldSetButton = setPushButton4; break;
        case 4: oldSetButton = setPushButton5; break;
        case 5: oldSetButton = setPushButton6; break;
        case 6: oldSetButton = setPushButton7; break;
        case 7: oldSetButton = setPushButton8; break;
        default: break;
    }

    if (oldSetButton != nullptr)
    {
        oldSetButton->setProperty("setActive", false);
        oldSetButton->style()->unpolish(oldSetButton);
        oldSetButton->style()->polish(oldSetButton);
    }

    m_joystick->setActiveSetNumber(index);
    stackedWidget_2->setCurrentIndex(index);

    switch (index)
    {
        case 0: activeSetButton = setPushButton1; break;
        case 1: activeSetButton = setPushButton2; break;
        case 2: activeSetButton = setPushButton3; break;
        case 3: activeSetButton = setPushButton4; break;
        case 4: activeSetButton = setPushButton5; break;
        case 5: activeSetButton = setPushButton6; break;
        case 6: activeSetButton = setPushButton7; break;
        case 7: activeSetButton = setPushButton8; break;
        default: break;
    }

    if (activeSetButton != nullptr)
    {
        activeSetButton->setProperty("setActive", true);
        activeSetButton->style()->unpolish(activeSetButton);
        activeSetButton->style()->polish(activeSetButton);
    }
}

void JoyTabWidget::changeSetOne()
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);

    changeCurrentSet(0);
}

void JoyTabWidget::changeSetTwo()
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);

    changeCurrentSet(1);
}

void JoyTabWidget::changeSetThree()
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);

    changeCurrentSet(2);
}

void JoyTabWidget::changeSetFour()
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);

    changeCurrentSet(3);
}

void JoyTabWidget::changeSetFive()
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);

    changeCurrentSet(4);
}

void JoyTabWidget::changeSetSix()
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);

    changeCurrentSet(5);
}

void JoyTabWidget::changeSetSeven()
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);

    changeCurrentSet(6);
}

void JoyTabWidget::changeSetEight()
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);

    changeCurrentSet(7);
}

void JoyTabWidget::showStickAssignmentDialog()
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);

    Joystick *temp = qobject_cast<Joystick*>(m_joystick);  // static_cast
    AdvanceStickAssignmentDialog *dialog = new AdvanceStickAssignmentDialog(temp, this);
    connect(dialog, &AdvanceStickAssignmentDialog::finished, this, &JoyTabWidget::refreshButtons);
    dialog->show();
}

void JoyTabWidget::loadConfigFile(QString fileLocation)
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);

    checkForUnsavedProfile(-1);

    if (!m_joystick->isDeviceEdited())
    {
        int numberRecentProfiles = m_settings->value("NumberRecentProfiles", DEFAULTNUMBERPROFILES).toInt();
        QFileInfo fileinfo(fileLocation);
        if (fileinfo.exists() && ((fileinfo.suffix() == "xml") || (fileinfo.suffix() == "amgp")))
        {
            int searchIndex = configBox->findData(fileinfo.absoluteFilePath());
            if (searchIndex == -1)
            {
                disconnectCheckUnsavedEvent();
                disconnectMainComboBoxEvents();

                if ((numberRecentProfiles > 0) && (configBox->count() == (numberRecentProfiles + 1)))
                {
                    configBox->removeItem(numberRecentProfiles - 1);
                    //configBox->removeItem(5);
                }
                configBox->insertItem(1, PadderCommon::getProfileName(fileinfo), fileinfo.absoluteFilePath());

                reconnectCheckUnsavedEvent();
                reconnectMainComboBoxEvents();

                configBox->setCurrentIndex(1);
                emit joystickConfigChanged(m_joystick->getJoyNumber());
            }
            else if (searchIndex != configBox->currentIndex())
            {
                configBox->setCurrentIndex(searchIndex);
                emit joystickConfigChanged(m_joystick->getJoyNumber());
            }
        }
    }
}

void JoyTabWidget::showQuickSetDialog()
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);

    ButtonEditDialog *dialog = new ButtonEditDialog(m_joystick, this);
    connect(dialog, &ButtonEditDialog::finished, this, &JoyTabWidget::refreshButtons);
    dialog->show();
}

void JoyTabWidget::showKeyDelayDialog()
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);

    ExtraProfileSettingsDialog *dialog = new ExtraProfileSettingsDialog(m_joystick, this);
    dialog->show();
}

void JoyTabWidget::showSetNamesDialog()
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);

    SetNamesDialog *dialog = new SetNamesDialog(m_joystick, this);
    connect(dialog, &SetNamesDialog::accepted, this, &JoyTabWidget::refreshSetButtons);
    connect(dialog, &SetNamesDialog::accepted, this, &JoyTabWidget::refreshCopySetActions);
    dialog->show();
}

void JoyTabWidget::removeCurrentButtons()
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);

    m_joystick->disconnectPropertyUpdatedConnection();
    disconnect(m_joystick, &InputDevice::setChangeActivated, this, &JoyTabWidget::changeCurrentSet);

    for (int i=0; i < Joystick::NUMBER_JOYSETS; i++)
    {
        SetJoystick *currentSet = m_joystick->getSetJoystick(i);
        removeSetButtons(currentSet);
    }
}

InputDevice *JoyTabWidget::getJoystick()
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);

    return m_joystick;
}

void JoyTabWidget::removeConfig()
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);

    int currentIndex = configBox->currentIndex();
    if (currentIndex > 0)
    {
        configBox->removeItem(currentIndex);
        saveDeviceSettings(true);
        emit joystickConfigChanged(m_joystick->getJoyNumber());
    }
}

void JoyTabWidget::toggleNames()
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);

    displayingNames = !displayingNames;
    namesPushButton->setProperty("isDisplayingNames", displayingNames);
    namesPushButton->style()->unpolish(namesPushButton);
    namesPushButton->style()->polish(namesPushButton);
    emit namesDisplayChanged(displayingNames);
}

void JoyTabWidget::unloadConfig()
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);

    configBox->setCurrentIndex(0);
}

void JoyTabWidget::saveDeviceSettings(bool sync)
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);

    m_settings->getLock()->lock();
    m_settings->beginGroup("Controllers");
    m_settings->getLock()->unlock();

    saveSettings();

    m_settings->getLock()->lock();
    m_settings->endGroup();
    if (sync)
    {
        m_settings->sync();
    }
    m_settings->getLock()->unlock();
}

void JoyTabWidget::loadDeviceSettings()
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);

    //settings.beginGroup("Controllers");
    loadSettings();
    //settings.endGroup();
}

bool JoyTabWidget::isDisplayingNames()
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);

    return displayingNames;
}

void JoyTabWidget::changeNameDisplay(bool displayNames)
{
    displayingNames = displayNames;
    namesPushButton->setProperty("isDisplayingNames", displayingNames);
    namesPushButton->style()->unpolish(namesPushButton);
    namesPushButton->style()->polish(namesPushButton);
}

void JoyTabWidget::refreshSetButtons()
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);

    for (int i = 0; i < InputDevice::NUMBER_JOYSETS; i++)
    {
        QPushButton *tempSetButton = nullptr;
        QAction *tempSetAction = nullptr;
        SetJoystick *tempSet = m_joystick->getSetJoystick(i);
        switch (i)
        {
            case 0:
                tempSetButton = setPushButton1;
                tempSetAction = setAction1;
                break;
            case 1:
                tempSetButton = setPushButton2;
                tempSetAction = setAction2;
                break;
            case 2:
                tempSetButton = setPushButton3;
                tempSetAction = setAction3;
                break;
            case 3:
                tempSetButton = setPushButton4;
                tempSetAction = setAction4;
                break;
            case 4:
                tempSetButton = setPushButton5;
                tempSetAction = setAction5;
                break;
            case 5:
                tempSetButton = setPushButton6;
                tempSetAction = setAction6;
                break;
            case 6:
                tempSetButton = setPushButton7;
                tempSetAction = setAction7;
                break;
            case 7:
                tempSetButton = setPushButton8;
                tempSetAction = setAction8;
                break;
            default:
                break;
        }

        if (!tempSet->getName().isEmpty())
        {
            QString tempName = tempSet->getName();
            QString tempNameEscaped = tempName;
            tempNameEscaped.replace("&", "&&");
            tempSetButton->setText(tempNameEscaped);
            tempSetButton->setToolTip(tempName);

            tempSetAction->setText(trUtf8("Set").append(" %1: %2").arg(i+1).arg(tempNameEscaped));
        }
        else
        {
            tempSetButton->setText(QString::number(i+1));
            tempSetButton->setToolTip("");

            tempSetAction->setText(trUtf8("Set").append(" %1").arg(i+1));
        }
    }
}

void JoyTabWidget::displayProfileEditNotification()
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);

    int currentIndex = configBox->currentIndex();
    configBox->setItemIcon(currentIndex, QIcon::fromTheme("document-save-as",
                                         QIcon(":/icons/16x16/actions/document-save.png")));
}

void JoyTabWidget::removeProfileEditNotification()
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);

    for (int i=0; i < configBox->count(); i++)
    {
        if (!configBox->itemIcon(i).isNull())
        {
            configBox->setItemIcon(i, QIcon());
        }
    }
}

void JoyTabWidget::retranslateUi()
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);

    removeButton->setText(trUtf8("Remove"));
    removeButton->setToolTip(trUtf8("Remove configuration from recent list."));

    loadButton->setText(trUtf8("Load"));
    loadButton->setToolTip(trUtf8("Load configuration file."));

    saveButton->setText(trUtf8("Save"));
    saveButton->setToolTip(trUtf8("Save changes to configuration file."));

    saveAsButton->setText(trUtf8("Save As"));
    saveAsButton->setToolTip(trUtf8("Save changes to a new configuration file."));

    setsMenuButton->setText(trUtf8("Sets"));
    setAction1->setText(trUtf8("Set 1"));
    setAction2->setText(trUtf8("Set 2"));
    setAction3->setText(trUtf8("Set 3"));
    setAction4->setText(trUtf8("Set 4"));
    setAction5->setText(trUtf8("Set 5"));
    setAction6->setText(trUtf8("Set 6"));
    setAction7->setText(trUtf8("Set 7"));
    setAction8->setText(trUtf8("Set 8"));

    refreshSetButtons();
    refreshCopySetActions();

    gameControllerMappingPushButton->setText(trUtf8("Controller Mapping"));
    stickAssignPushButton->setText(trUtf8("Stick/Pad Assign"));
    quickSetPushButton->setText(trUtf8("Quick Set"));
    resetButton->setText(trUtf8("Reset"));

    namesPushButton->setText(trUtf8("Names"));
    namesPushButton->setToolTip(trUtf8("Toggle button name displaying."));

    delayButton->setText(trUtf8("Pref"));
    delayButton->setToolTip(trUtf8("Change global profile settings."));

    resetButton->setText(trUtf8("Reset"));
    resetButton->setToolTip(trUtf8("Revert changes to the configuration. Reload configuration file."));
    refreshButtons();
}

void JoyTabWidget::checkForUnsavedProfile(int newindex)
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);

    if (m_joystick->isDeviceEdited())
    {
        disconnectCheckUnsavedEvent();
        disconnectMainComboBoxEvents();

        if (configBox->currentIndex() != comboBoxIndex)
        {
            configBox->setCurrentIndex(comboBoxIndex);
        }

        QMessageBox msg;
        msg.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msg.setWindowTitle(trUtf8("Save Profile Changes?"));
        if (comboBoxIndex == 0)
        {
            msg.setText(trUtf8("Changes to the new profile have not been saved. Would you like to save or discard the current profile?"));
        }
        else
        {
            msg.setText(trUtf8("Changes to the profile \"%1\" have not been saved. Would you like to save or discard changes to the current profile?")
                        .arg(configBox->currentText()));
        }

        int status = msg.exec();

        switch(status)
        {

            case QMessageBox::Save:
            {
                saveConfigFile();
                reconnectCheckUnsavedEvent();
                reconnectMainComboBoxEvents();

                if (newindex > -1)
                {
                    configBox->setCurrentIndex(newindex);
                }

                break;

            }
            case QMessageBox::Discard:
            {
                m_joystick->revertProfileEdited();
                configBox->setItemText(comboBoxIndex, oldProfileName);
                reconnectCheckUnsavedEvent();
                reconnectMainComboBoxEvents();

                if (newindex > -1)
                {
                    configBox->setCurrentIndex(newindex);
                }

                break;

            }
            case QMessageBox::Cancel:
            {
                reconnectCheckUnsavedEvent();
                reconnectMainComboBoxEvents();

                break;
            }

        }
    }
}

bool JoyTabWidget::discardUnsavedProfileChanges()
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);

    bool discarded = true;

    if (m_joystick->isDeviceEdited())
    {
        disconnectCheckUnsavedEvent();

        QMessageBox msg;
        msg.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msg.setWindowTitle(trUtf8("Save Profile Changes?"));
        int currentIndex = configBox->currentIndex();
        if (currentIndex == 0)
        {
            msg.setText(trUtf8("Changes to the new profile have not been saved. Would you like to save or discard the current profile?"));
        }
        else
        {
            msg.setText(trUtf8("Changes to the profile \"%1\" have not been saved. Would you like to save or discard changes to the current profile?")
                        .arg(configBox->currentText()));
        }

        int status = msg.exec();

        switch(status)
        {

            case QMessageBox::Save:
            {
                saveConfigFile();
                if ((currentIndex == 0) && (currentIndex == configBox->currentIndex()))
                {
                    discarded = false;
                }

                break;
            }
            case QMessageBox::Discard:
            {
                m_joystick->revertProfileEdited();
                configBox->setItemText(currentIndex, oldProfileName);
                resetJoystick();

                break;
            }
            case QMessageBox::Cancel:
            {
                discarded = false;
                break;
            }

        }

        disconnectMainComboBoxEvents();
        reconnectCheckUnsavedEvent();
        reconnectMainComboBoxEvents();
    }

    return discarded;
}

void JoyTabWidget::disconnectMainComboBoxEvents()
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);

    disconnect(configBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &JoyTabWidget::changeJoyConfig);
    disconnect(configBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &JoyTabWidget::removeProfileEditNotification);
    disconnect(m_joystick, &InputDevice::profileNameEdited, this, &JoyTabWidget::editCurrentProfileItemText);
}

void JoyTabWidget::reconnectMainComboBoxEvents()
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);

    connect(configBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &JoyTabWidget::changeJoyConfig, Qt::QueuedConnection);
    connect(configBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &JoyTabWidget::removeProfileEditNotification, Qt::QueuedConnection);
    connect(m_joystick, &InputDevice::profileNameEdited, this, &JoyTabWidget::editCurrentProfileItemText);
}

void JoyTabWidget::disconnectCheckUnsavedEvent()
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);

    disconnect(configBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &JoyTabWidget::checkForUnsavedProfile);
}

void JoyTabWidget::reconnectCheckUnsavedEvent()
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);

    connect(configBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &JoyTabWidget::checkForUnsavedProfile);
}

void JoyTabWidget::refreshButtons()
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);

    removeCurrentButtons();
    fillButtons();
}

void JoyTabWidget::checkStickDisplay()
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);

    JoyControlStickButton *button = qobject_cast<JoyControlStickButton*>(sender()); // static_cast
    JoyControlStick *stick = button->getStick();
    if ((stick != nullptr) && stick->hasSlotsAssigned())
    {
        SetJoystick *currentSet = m_joystick->getActiveSetJoystick();
        removeSetButtons(currentSet);
        fillSetButtons(currentSet);
    }
}

void JoyTabWidget::checkDPadButtonDisplay()
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);

    JoyDPadButton *button = qobject_cast<JoyDPadButton*>(sender()); // static_cast
    JoyDPad *dpad = button->getDPad();
    if ((dpad != nullptr) && dpad->hasSlotsAssigned())
    {
        SetJoystick *currentSet = m_joystick->getActiveSetJoystick();
        removeSetButtons(currentSet);
        fillSetButtons(currentSet);
    }
}

void JoyTabWidget::checkAxisButtonDisplay()
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);

    JoyAxisButton *button = qobject_cast<JoyAxisButton*>(sender()); // static_cast
    if (button->getAssignedSlots()->count() > 0)
    {
        SetJoystick *currentSet = m_joystick->getActiveSetJoystick();
        removeSetButtons(currentSet);
        fillSetButtons(currentSet);
    }
}

void JoyTabWidget::checkButtonDisplay()
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);

    JoyButton *button = qobject_cast<JoyButton*>(sender()); // static_cast
    if (button->getAssignedSlots()->count() > 0)
    {
        SetJoystick *currentSet = m_joystick->getActiveSetJoystick();
        removeSetButtons(currentSet);
        fillSetButtons(currentSet);
    }
}

void JoyTabWidget::checkStickEmptyDisplay()
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);

    StickPushButtonGroup *group = qobject_cast<StickPushButtonGroup*>(sender()); // static_cast
    JoyControlStick *stick = group->getStick();
    //JoyControlStickButton *button = static_cast<JoyControlStickButton*>(sender());
    //JoyControlStick *stick = button->getStick();
    if ((stick != nullptr) && !stick->hasSlotsAssigned())
    {
        SetJoystick *currentSet = m_joystick->getActiveSetJoystick();
        removeSetButtons(currentSet);
        fillSetButtons(currentSet);
    }
}

void JoyTabWidget::checkDPadButtonEmptyDisplay()
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);

    DPadPushButtonGroup *group = qobject_cast<DPadPushButtonGroup*>(sender()); // static_cast
    JoyDPad *dpad = group->getDPad();
    //JoyDPadButton *button = static_cast<JoyDPadButton*>(sender());
    //JoyDPad *dpad = button->getDPad();
    if ((dpad != nullptr) && !dpad->hasSlotsAssigned())
    {
        SetJoystick *currentSet = m_joystick->getActiveSetJoystick();
        removeSetButtons(currentSet);
        fillSetButtons(currentSet);
    }
}

void JoyTabWidget::checkAxisButtonEmptyDisplay()
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);

    JoyAxisButton *button = qobject_cast<JoyAxisButton*>(sender()); // static_cast
    if (button->getAssignedSlots()->count() == 0)
    {
        SetJoystick *currentSet = m_joystick->getActiveSetJoystick();
        removeSetButtons(currentSet);
        fillSetButtons(currentSet);
    }
}

void JoyTabWidget::checkButtonEmptyDisplay()
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);

    JoyButton *button = qobject_cast<JoyButton*>(sender()); // static_cast
    if (button->getAssignedSlots()->count() == 0)
    {
        SetJoystick *currentSet = m_joystick->getActiveSetJoystick();
        removeSetButtons(currentSet);
        fillSetButtons(currentSet);
    }
}

void JoyTabWidget::checkHideEmptyOption()
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);

    bool currentHideEmptyButtons = m_settings->value("HideEmptyButtons", false).toBool();
    if (currentHideEmptyButtons != hideEmptyButtons)
    {
        hideEmptyButtons = currentHideEmptyButtons;
        refreshButtons();
    }
}

void JoyTabWidget::fillSetButtons(SetJoystick *set)
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);

    int row = 0;
    int column = 0;

    //QWidget *child = 0;
    QGridLayout *current_layout = nullptr;
    switch (set->getIndex())
    {
        case 0:
        {
            current_layout = gridLayout;
            break;
        }
        case 1:
        {
            current_layout = gridLayout2;
            break;
        }
        case 2:
        {
            current_layout = gridLayout3;
            break;
        }
        case 3:
        {
            current_layout = gridLayout4;
            break;
        }
        case 4:
        {
            current_layout = gridLayout5;
            break;
        }
        case 5:
        {
            current_layout = gridLayout6;
            break;
        }
        case 6:
        {
            current_layout = gridLayout7;
            break;
        }
        case 7:
        {
            current_layout = gridLayout8;
            break;
        }
        default:
            break;
    }

    SetJoystick *currentSet = set;
    currentSet->establishPropertyUpdatedConnection();

    QGridLayout *stickGrid = nullptr;
    QGroupBox *stickGroup = nullptr;
    int stickGridColumn = 0;
    int stickGridRow = 0;

    for (int j=0; j < m_joystick->getNumberSticks(); j++)
    {
        JoyControlStick *stick = currentSet->getJoyStick(j);
        stick->establishPropertyUpdatedConnection();
        QHash<JoyControlStick::JoyStickDirections, JoyControlStickButton*> *stickButtons = stick->getButtons();

        if (!hideEmptyButtons || stick->hasSlotsAssigned())
        {
            if (stickGroup == nullptr)
            {
                stickGroup = new QGroupBox(trUtf8("Sticks"), this);
            }

            if (stickGrid == nullptr)
            {
                stickGrid = new QGridLayout();
                stickGridColumn = 0;
                stickGridRow = 0;
            }

            QWidget *groupContainer = new QWidget(stickGroup);
            StickPushButtonGroup *stickButtonGroup = new StickPushButtonGroup(stick, displayingNames, groupContainer);
            if (hideEmptyButtons)
            {
                connect(stickButtonGroup, &StickPushButtonGroup::buttonSlotChanged, this, &JoyTabWidget::checkStickEmptyDisplay);
            }

            connect(namesPushButton, &QPushButton::clicked, stickButtonGroup, &StickPushButtonGroup::toggleNameDisplay);

            if (stickGridColumn > 1)
            {
                stickGridColumn = 0;
                stickGridRow++;
            }

            groupContainer->setLayout(stickButtonGroup);
            stickGrid->addWidget(groupContainer, stickGridRow, stickGridColumn);
            stickGridColumn++;
        }
        else
        {
            QHashIterator<JoyControlStick::JoyStickDirections, JoyControlStickButton*> tempiter(*stickButtons);
            while (tempiter.hasNext())
            {
                JoyControlStickButton *button = tempiter.next().value();
                button->establishPropertyUpdatedConnections();
                connect(button, &JoyControlStickButton::slotsChanged, this, &JoyTabWidget::checkStickDisplay);
            }
        }
    }

    if (stickGroup != nullptr)
    {
        QSpacerItem *tempspacer = new QSpacerItem(10, 4, QSizePolicy::Minimum, QSizePolicy::Fixed);
        QVBoxLayout *tempvbox = new QVBoxLayout;
        tempvbox->addLayout(stickGrid);
        tempvbox->addItem(tempspacer);
        stickGroup->setLayout(tempvbox);
        current_layout->addWidget(stickGroup, row, column, 1, 2, Qt::AlignTop);
        row++;
    }

    column = 0;

    QGridLayout *hatGrid = nullptr;
    QGroupBox *hatGroup = nullptr;
    int hatGridColumn = 0;
    int hatGridRow = 0;
    for (int j = 0; j < m_joystick->getNumberHats(); j++)
    {
        JoyDPad *dpad = currentSet->getJoyDPad(j);
        dpad->establishPropertyUpdatedConnection();
        QHash<int, JoyDPadButton*> *buttons = dpad->getJoyButtons();

        if (!hideEmptyButtons || dpad->hasSlotsAssigned())
        {
            if (hatGroup == nullptr)
            {
                hatGroup = new QGroupBox(trUtf8("DPads"), this);
            }

            if (hatGrid == nullptr)
            {
                hatGrid = new QGridLayout();
                hatGridColumn = 0;
                hatGridRow = 0;
            }

            QWidget *groupContainer = new QWidget(hatGroup);
            DPadPushButtonGroup *dpadButtonGroup = new DPadPushButtonGroup(dpad, displayingNames, groupContainer);
            if (hideEmptyButtons)
            {
                connect(dpadButtonGroup, &DPadPushButtonGroup::buttonSlotChanged, this, &JoyTabWidget::checkDPadButtonEmptyDisplay);
            }

            connect(namesPushButton, &QPushButton::clicked, dpadButtonGroup, &DPadPushButtonGroup::toggleNameDisplay);

            if (hatGridColumn > 1)
            {
                hatGridColumn = 0;
                hatGridRow++;
            }

            groupContainer->setLayout(dpadButtonGroup);
            hatGrid->addWidget(groupContainer, hatGridRow, hatGridColumn);
            hatGridColumn++;
        }
        else
        {
            QHashIterator<int, JoyDPadButton*> tempiter(*buttons);
            while (tempiter.hasNext())
            {
                JoyDPadButton *button = tempiter.next().value();
                button->establishPropertyUpdatedConnections();
                connect(button, &JoyDPadButton::slotsChanged, this, &JoyTabWidget::checkDPadButtonDisplay);
            }
        }
    }

    for (int j = 0; j < m_joystick->getNumberVDPads(); j++)
    {
        VDPad *vdpad = currentSet->getVDPad(j);
        vdpad->establishPropertyUpdatedConnection();
        QHash<int, JoyDPadButton*> *buttons = vdpad->getButtons();

        if (!hideEmptyButtons || vdpad->hasSlotsAssigned())
        {
            if (hatGroup == nullptr)
            {
                hatGroup = new QGroupBox(trUtf8("DPads"), this);
            }

            if (hatGrid == nullptr)
            {
                hatGrid = new QGridLayout();
                hatGridColumn = 0;
                hatGridRow = 0;
            }

            QWidget *groupContainer = new QWidget(hatGroup);
            DPadPushButtonGroup *dpadButtonGroup = new DPadPushButtonGroup(vdpad, displayingNames, groupContainer);
            if (hideEmptyButtons)
            {
                connect(dpadButtonGroup, &DPadPushButtonGroup::buttonSlotChanged, this, &JoyTabWidget::checkDPadButtonEmptyDisplay);
            }

            connect(namesPushButton, &QPushButton::clicked, dpadButtonGroup, &DPadPushButtonGroup::toggleNameDisplay);

            if (hatGridColumn > 1)
            {
                hatGridColumn = 0;
                hatGridRow++;
            }

            groupContainer->setLayout(dpadButtonGroup);
            hatGrid->addWidget(groupContainer, hatGridRow, hatGridColumn);
            hatGridColumn++;

        }
        else
        {
            QHashIterator<int, JoyDPadButton*> tempiter(*buttons);
            while (tempiter.hasNext())
            {
                JoyDPadButton *button = tempiter.next().value();
                button->establishPropertyUpdatedConnections();
                connect(button, &JoyDPadButton::slotsChanged, this, &JoyTabWidget::checkDPadButtonDisplay);
            }
        }
    }

    if (hatGroup != nullptr)
    {
        QSpacerItem *tempspacer = new QSpacerItem(10, 4, QSizePolicy::Minimum, QSizePolicy::Fixed);
        QVBoxLayout *tempvbox = new QVBoxLayout;
        tempvbox->addLayout(hatGrid);
        tempvbox->addItem(tempspacer);
        hatGroup->setLayout(tempvbox);
        current_layout->addWidget(hatGroup, row, column, 1, 2, Qt::AlignTop);
        row++;
    }

    column = 0;

    for (int j = 0; j < m_joystick->getNumberAxes(); j++)
    {
        JoyAxis *axis = currentSet->getJoyAxis(j);

        if (!axis->isPartControlStick() && axis->hasControlOfButtons())
        {
            JoyAxisButton *paxisbutton = axis->getPAxisButton();
            JoyAxisButton *naxisbutton = axis->getNAxisButton();

            if (!hideEmptyButtons ||
               ((paxisbutton->getAssignedSlots()->count() > 0) ||
                (naxisbutton->getAssignedSlots()->count() > 0)))
            {
                JoyAxisWidget *axisWidget = new JoyAxisWidget(axis, displayingNames, this);
                axisWidget->setText(axis->getName());
                axisWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
                axisWidget->setMinimumSize(200, 24);

                connect(axisWidget, &JoyAxisWidget::clicked, this, &JoyTabWidget::showAxisDialog);
                connect(namesPushButton, &QPushButton::clicked, axisWidget, &JoyAxisWidget::toggleNameDisplay);
                if (hideEmptyButtons)
                {
                    connect(paxisbutton, &JoyAxisButton::slotsChanged, this, &JoyTabWidget::checkAxisButtonEmptyDisplay);
                    connect(naxisbutton, &JoyAxisButton::slotsChanged, this, &JoyTabWidget::checkAxisButtonEmptyDisplay);
                }

                if (column > 1)
                {
                    column = 0;
                    row++;
                }
                current_layout->addWidget(axisWidget, row, column);
                column++;
            }
            else
            {
                paxisbutton->establishPropertyUpdatedConnections();
                naxisbutton->establishPropertyUpdatedConnections();

                connect(paxisbutton, &JoyAxisButton::slotsChanged, this, &JoyTabWidget::checkAxisButtonDisplay);
                connect(naxisbutton, &JoyAxisButton::slotsChanged, this, &JoyTabWidget::checkAxisButtonDisplay);
            }
        }
    }

    for (int j = 0; j < m_joystick->getNumberButtons(); j++)
    {
        JoyButton *button = currentSet->getJoyButton(j);
        if ((button != nullptr) && !button->isPartVDPad())
        {
            button->establishPropertyUpdatedConnections();

            if (!hideEmptyButtons || (button->getAssignedSlots()->count() > 0))
            {
                JoyButtonWidget *buttonWidget = new JoyButtonWidget (button, displayingNames, this);
                buttonWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
                buttonWidget->setText(buttonWidget->text());
                buttonWidget->setMinimumSize(200, 24);

                connect(buttonWidget, &JoyButtonWidget::clicked, this, &JoyTabWidget::showButtonDialog);
                connect(namesPushButton, &QPushButton::clicked, buttonWidget, &JoyButtonWidget::toggleNameDisplay);
                if (hideEmptyButtons)
                {
                    connect(button, &JoyButton::slotsChanged, this, &JoyTabWidget::checkButtonEmptyDisplay);
                }

                if (column > 1)
                {
                    column = 0;
                    row++;
                }

                current_layout->addWidget(buttonWidget, row, column);
                column++;
            }
            else
            {
                button->establishPropertyUpdatedConnections();
                connect(button, &JoyButton::slotsChanged, this, &JoyTabWidget::checkButtonDisplay);
            }
        }
    }

    if (current_layout->count() == 0)
    {
        QLabel *newlabel = new QLabel(trUtf8("No buttons have been assigned. Please use Quick Set to assign keys\nto buttons or disable hiding empty buttons."));
        current_layout->addWidget(newlabel, 0, 0, Qt::AlignCenter);
    }
}

void JoyTabWidget::removeSetButtons(SetJoystick *set)
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);

    SetJoystick *currentSet = set;
    currentSet->disconnectPropertyUpdatedConnection();

    QLayoutItem *child = nullptr;
    QGridLayout *current_layout = nullptr;
    switch (currentSet->getIndex())
    {
        case 0:
        {
            current_layout = gridLayout;
            break;
        }
        case 1:
        {
            current_layout = gridLayout2;
            break;
        }
        case 2:
        {
            current_layout = gridLayout3;
            break;
        }
        case 3:
        {
            current_layout = gridLayout4;
            break;
        }
        case 4:
        {
            current_layout = gridLayout5;
            break;
        }
        case 5:
        {
            current_layout = gridLayout6;
            break;
        }
        case 6:
        {
            current_layout = gridLayout7;
            break;
        }
        case 7:
        {
            current_layout = gridLayout8;
            break;
        }
        default:
        {
            break;
        }
    }

    while (current_layout && ((child = current_layout->takeAt(0)) != nullptr))
    {
        current_layout->removeWidget(child->widget());
        delete child->widget();
        delete child;
        child = nullptr;
    }

    for (int j=0; j < m_joystick->getNumberSticks(); j++)
    {
        JoyControlStick *stick = currentSet->getJoyStick(j);
        stick->disconnectPropertyUpdatedConnection();
        QHash<JoyControlStick::JoyStickDirections, JoyControlStickButton*> *stickButtons = stick->getButtons();

        QHashIterator<JoyControlStick::JoyStickDirections, JoyControlStickButton*> tempiter(*stickButtons);
        while (tempiter.hasNext())
        {
            JoyControlStickButton *button = tempiter.next().value();
            button->disconnectPropertyUpdatedConnections();
            disconnect(button, &JoyControlStickButton::slotsChanged, this, &JoyTabWidget::checkStickDisplay);
            disconnect(button, &JoyControlStickButton::slotsChanged, this, &JoyTabWidget::checkStickEmptyDisplay);
        }
    }

    for (int j=0; j < m_joystick->getNumberHats(); j++)
    {
        JoyDPad *dpad = currentSet->getJoyDPad(j);
        dpad->establishPropertyUpdatedConnection();
        QHash<int, JoyDPadButton*> *buttons = dpad->getJoyButtons();

        QHashIterator<int, JoyDPadButton*> tempiter(*buttons);
        while (tempiter.hasNext())
        {
            JoyDPadButton *button = tempiter.next().value();
            button->disconnectPropertyUpdatedConnections();
            disconnect(button, &JoyDPadButton::slotsChanged, this, &JoyTabWidget::checkDPadButtonDisplay);
            disconnect(button, &JoyDPadButton::slotsChanged, this, &JoyTabWidget::checkDPadButtonEmptyDisplay);
        }
    }

    for (int j=0; j < m_joystick->getNumberVDPads(); j++)
    {
        VDPad *vdpad = currentSet->getVDPad(j);
        vdpad->establishPropertyUpdatedConnection();
        QHash<int, JoyDPadButton*> *buttons = vdpad->getButtons();

        QHashIterator<int, JoyDPadButton*> tempiter(*buttons);
        while (tempiter.hasNext())
        {
            JoyDPadButton *button = tempiter.next().value();
            button->disconnectPropertyUpdatedConnections();
            disconnect(button, &JoyDPadButton::slotsChanged, this, &JoyTabWidget::checkDPadButtonDisplay);
            disconnect(button, &JoyDPadButton::slotsChanged, this, &JoyTabWidget::checkDPadButtonEmptyDisplay);
        }
    }

    for (int j=0; j < m_joystick->getNumberAxes(); j++)
    {
        JoyAxis *axis = currentSet->getJoyAxis(j);

        if (!axis->isPartControlStick() && axis->hasControlOfButtons())
        {
            JoyAxisButton *paxisbutton = axis->getPAxisButton();
            JoyAxisButton *naxisbutton = axis->getNAxisButton();

            paxisbutton->disconnectPropertyUpdatedConnections();
            naxisbutton->disconnectPropertyUpdatedConnections();

            disconnect(paxisbutton, &JoyAxisButton::slotsChanged, this, &JoyTabWidget::checkAxisButtonDisplay);
            disconnect(naxisbutton, &JoyAxisButton::slotsChanged, this, &JoyTabWidget::checkAxisButtonDisplay);
            disconnect(paxisbutton, &JoyAxisButton::slotsChanged, this, &JoyTabWidget::checkAxisButtonEmptyDisplay);
            disconnect(naxisbutton, &JoyAxisButton::slotsChanged, this, &JoyTabWidget::checkAxisButtonEmptyDisplay);
        }
    }

    for (int j=0; j < m_joystick->getNumberButtons(); j++)
    {
        JoyButton *button = currentSet->getJoyButton(j);
        if ((button != nullptr) && !button->isPartVDPad())
        {
            button->disconnectPropertyUpdatedConnections();
            disconnect(button, &JoyButton::slotsChanged, this, &JoyTabWidget::checkButtonDisplay);
            disconnect(button, &JoyButton::slotsChanged, this, &JoyTabWidget::checkButtonEmptyDisplay);
        }
    }
}

void JoyTabWidget::editCurrentProfileItemText(QString text)
{
   qInstallMessageHandler(MessageHandler::myMessageOutput);

    int currentIndex = configBox->currentIndex();
    if (currentIndex >= 0)
    {
        if (!text.isEmpty())
        {
            configBox->setItemText(currentIndex, text);
        }
        else if (currentIndex == 0)
        {
            configBox->setItemText(currentIndex, trUtf8("<New>"));
        }
        else if (currentIndex > 0)
        {
            QFileInfo profileName(configBox->itemData(currentIndex).toString());
            configBox->setItemText(currentIndex, PadderCommon::getProfileName(profileName));
        }
    }
}

#ifdef Q_OS_WIN
void JoyTabWidget::deviceKeyRepeatSettings()
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);

    bool keyRepeatActive = m_settings->value("KeyRepeat/KeyRepeatEnabled", true).toBool();
    int keyRepeatDelay = m_settings->value("KeyRepeat/KeyRepeatDelay", InputDevice::DEFAULTKEYREPEATDELAY).toInt();
    int keyRepeatRate = m_settings->value("KeyRepeat/KeyRepeatRate", InputDevice::DEFAULTKEYREPEATRATE).toInt();

    m_joystick->setKeyRepeatStatus(keyRepeatActive);
    m_joystick->setKeyRepeatDelay(keyRepeatDelay);
    m_joystick->setKeyRepeatRate(keyRepeatRate);
}
#endif

void JoyTabWidget::refreshCopySetActions()
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);

    copySetMenu->clear();

    for (int i=0; i < InputDevice::NUMBER_JOYSETS; i++)
    {
        SetJoystick *tempSet = m_joystick->getSetJoystick(i);
        QAction *newaction = nullptr;
        if (!tempSet->getName().isEmpty())
        {
            QString tempName = tempSet->getName();
            QString tempNameEscaped = tempName;
            tempNameEscaped.replace("&", "&&");
            newaction = new QAction(trUtf8("Set %1: %2").arg(i+1).arg(tempNameEscaped), copySetMenu);
        }
        else
        {
            newaction = new QAction(trUtf8("Set %1").arg(i+1), copySetMenu);
        }

        newaction->setData(i);
        connect(newaction, &QAction::triggered, this, &JoyTabWidget::performSetCopy);
        copySetMenu->addAction(newaction);
    }

    connect(copySetMenu, &QMenu::aboutToShow, this, &JoyTabWidget::disableCopyCurrentSet);
}

void JoyTabWidget::performSetCopy()
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);

    QAction *action = qobject_cast<QAction*>(sender()); // static_cast
    int sourceSetIndex = action->data().toInt();
    SetJoystick *sourceSet = m_joystick->getSetJoystick(sourceSetIndex);
    QString sourceName = QString();
    if (!sourceSet->getName().isEmpty())
    {
        QString tempNameEscaped = sourceSet->getName();
        tempNameEscaped.replace("&", "&&");
        sourceName = trUtf8("Set %1: %2").arg(sourceSetIndex+1).arg(tempNameEscaped);
    }
    else
    {
        sourceName = trUtf8("Set %1").arg(sourceSetIndex+1);
    }

    SetJoystick *destSet = m_joystick->getActiveSetJoystick();
    if ((sourceSet != nullptr) && (destSet != nullptr))
    {
        QMessageBox msgBox;
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setWindowTitle(trUtf8("Copy Set Assignments"));
        msgBox.setText(trUtf8("Are you sure you want to copy the assignments and device properties from %1?").arg(sourceName));
        int status = msgBox.exec();
        if (status == QMessageBox::Yes)
        {
            PadderCommon::lockInputDevices();

            removeSetButtons(destSet);

            QMetaObject::invokeMethod(sourceSet, "copyAssignments", Qt::BlockingQueuedConnection,
                                      Q_ARG(SetJoystick*, destSet));

            //sourceSet->copyAssignments(destSet);
            fillSetButtons(destSet);

            PadderCommon::unlockInputDevices();
        }
    }
}

void JoyTabWidget::disableCopyCurrentSet()
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);

    SetJoystick *activeSet = m_joystick->getActiveSetJoystick();
    QMenu *menu = qobject_cast<QMenu*>(sender()); // static_cast
    QList<QAction*> actions = menu->actions();
    QListIterator<QAction*> iter(actions);
    while (iter.hasNext())
    {
        QAction *action = iter.next();
        if (action->data().toInt() == activeSet->getIndex())
        {
            action->setEnabled(false);
        }
        else
        {
            action->setEnabled(true);
        }
    }
}

void JoyTabWidget::openGameControllerMappingWindow()
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);

    GameControllerMappingDialog *dialog = new GameControllerMappingDialog(m_joystick, m_settings, this);
    dialog->show();
    connect(dialog, &GameControllerMappingDialog::mappingUpdate, this, &JoyTabWidget::propogateMappingUpdate);
}

void JoyTabWidget::propogateMappingUpdate(QString mapping, InputDevice *device)
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);

    emit mappingUpdated(mapping, device);
}


void JoyTabWidget::refreshHelperThread()
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);

    tabHelper.moveToThread(m_joystick->thread());
}

void JoyTabWidget::changeEvent(QEvent *event)
{
    qInstallMessageHandler(MessageHandler::myMessageOutput);

    if (event->type() == QEvent::LanguageChange)
    {
        retranslateUi();
    }

    QWidget::changeEvent(event);
}

