#include <QtWidgets>
#include <NinEmu/Menu/GraphicsMenu.h>

GraphicsMenu::GraphicsMenu(QWidget* parent)
: QMenu(parent)
{
    setTitle(tr("Graphics"));

    QAction* actionFullscreen;
    actionFullscreen = new QAction(tr("Fullscreen"), this);
    actionFullscreen->setCheckable(true);
    actionFullscreen->setShortcut(Qt::ALT + Qt::Key_Return);
    connect(actionFullscreen, &QAction::triggered, this, &GraphicsMenu::fullscreen);
    addAction(actionFullscreen);

    addSeparator();

    QAction* actionFit;
    actionFit = new QAction(tr("Fit to window"), this);
    actionFit->setCheckable(true);
    connect(actionFit, &QAction::triggered, this, &GraphicsMenu::fit);
    addAction(actionFit);

    QAction* actionIntegerScale;
    actionIntegerScale = new QAction(tr("Integer scaling"), this);
    actionIntegerScale->setCheckable(true);
    connect(actionIntegerScale, &QAction::triggered, this, &GraphicsMenu::integerScale);
    addAction(actionIntegerScale);

    addSeparator();
    createSubMenuAspectRatio();
    createSubMenuOverscan();

    if (parent)
    {
        parent->addAction(actionFullscreen);
    }
}

void GraphicsMenu::createSubMenuAspectRatio()
{
    QMenu* subMenu;
    QActionGroup* group;
    QAction* actSquare;
    QAction* actNTSC;

    subMenu = new QMenu(tr("Pixel Aspect Ratio"));
    group = new QActionGroup(subMenu);

    actSquare = new QAction(tr("Square"), group);
    actSquare->setCheckable(true);
    actSquare->setChecked(true);
    actSquare->setData((int)AspectRatio::Square);
    connect(actSquare, &QAction::triggered, this, &GraphicsMenu::selectAspectRatio);

    actNTSC = new QAction(tr("NTSC"), group);
    actNTSC->setCheckable(true);
    actNTSC->setData((int)AspectRatio::NTSC);
    connect(actNTSC, &QAction::triggered, this, &GraphicsMenu::selectAspectRatio);

    subMenu->addAction(actSquare);
    subMenu->addAction(actNTSC);
    addMenu(subMenu);
}

void GraphicsMenu::createSubMenuOverscan()
{
    QMenu* subMenu;
    QActionGroup* group;
    QAction* actNone;
    QAction* actSmall;
    QAction* actMedium;
    QAction* actLarge;

    subMenu = new QMenu(tr("Overscan"));
    group = new QActionGroup(subMenu);

    actNone = new QAction(tr("None"), group);
    actNone->setCheckable(true);
    actNone->setChecked(true);
    actNone->setData((int)Overscan::None);
    connect(actNone, &QAction::triggered, this, &GraphicsMenu::selectOverscan);

    actSmall = new QAction(tr("Small"), group);
    actSmall->setCheckable(true);
    actSmall->setData((int)Overscan::Small);
    connect(actSmall, &QAction::triggered, this, &GraphicsMenu::selectOverscan);

    actMedium = new QAction(tr("Medium"), group);
    actMedium->setCheckable(true);
    actMedium->setData((int)Overscan::Medium);
    connect(actMedium, &QAction::triggered, this, &GraphicsMenu::selectOverscan);

    actLarge = new QAction(tr("Large"), group);
    actLarge->setCheckable(true);
    actLarge->setData((int)Overscan::Large);
    connect(actLarge, &QAction::triggered, this, &GraphicsMenu::selectOverscan);

    subMenu->addAction(actNone);
    subMenu->addAction(actSmall);
    subMenu->addAction(actMedium);
    subMenu->addAction(actLarge);
    addMenu(subMenu);
}

void GraphicsMenu::selectAspectRatio()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (action)
    {
        aspectRatio((AspectRatio)action->data().toInt());
    }
}

void GraphicsMenu::selectOverscan()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (action)
    {
        overscan((Overscan)action->data().toInt());
    }
}
