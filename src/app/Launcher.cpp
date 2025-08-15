#include "Launcher.h"

Launcher::Launcher(QWidget* parent)
{
    // Set window attributes.
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);

    // Setup user interface.
    setupUi();

    show();
}
void Launcher::setupUi()
{
    // Main layout.
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);
    m_mainLayout = new QVBoxLayout(m_centralWidget);
    m_mainLayout->setContentsMargins(WINDOW_MARGIN, WINDOW_MARGIN, WINDOW_MARGIN, WINDOW_MARGIN);
    m_centralWidget->setLayout(m_mainLayout);

    // Search area.
    m_searchFrame = new QFrame(this);
    m_searchFrame->setObjectName("searchFrame");
    m_searchLayout = new QHBoxLayout(m_searchFrame);
    m_searchIcon = new QLabel(this);
    m_searchIcon->setText("ICON");  //TODO: Change this into a font icon.
    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setPlaceholderText("Start typing...");
    m_searchLayout->addWidget(m_searchIcon);
    m_searchLayout->addWidget(m_searchEdit);

    // Results list.
    m_resultsList = new QListWidget(this);

    m_mainLayout->addWidget(m_searchFrame);
    m_mainLayout->addWidget(m_resultsList);
}
