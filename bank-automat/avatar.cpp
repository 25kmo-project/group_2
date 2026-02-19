#include "avatar.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QPixmap>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include <QFile>
#include <QPainter>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QJsonValue>

avatar::avatar(const LoginResultDto& loginResult, ApiClient* api, QTimer* inactivityTimer, QWidget *parent)
    : QDialog(parent), m_api(api), m_login(loginResult), m_inactivityTimer(inactivityTimer)
{
    setWindowTitle("Select Avatar");
    setGeometry(100, 100, 600, 500);
    setObjectName("avatarDialog");
    setStyleSheet(
        "QDialog#avatarDialog { background-color: #f0f0f0; }"
        "QLabel { color: #1f1f1f; }"
        "QComboBox { color: #1f1f1f; background: #ffffff; border: 1px solid #9f9f9f; padding: 3px; min-height: 24px; }"
        "QPushButton { color: #1f1f1f; background: #ffffff; border: 1px solid #8f8f8f; border-radius: 4px; min-height: 28px; padding: 4px 10px; }"
        "QPushButton:disabled { color: #777777; background: #e2e2e2; border-color: #b5b5b5; }"
    );

    setupUI();
    m_avatarNam = new QNetworkAccessManager(this);
    loadPreselectedAvatars();

    // Install event filter to reset inactivity timer on user interaction
    if (m_inactivityTimer) {
        installEventFilter(this);
    }
}

avatar::~avatar()
{
}

void avatar::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Title
    lblTitle = new QLabel("Choose Your Avatar", this);
    lblTitle->setStyleSheet("font-size: 18px; font-weight: bold;");
    mainLayout->addWidget(lblTitle);

    // Avatar Display
    lblAvatarDisplay = new QLabel(this);
    lblAvatarDisplay->setFixedSize(200, 200);
    lblAvatarDisplay->setText("");
    lblAvatarDisplay->setAttribute(Qt::WA_StyledBackground, true);
    lblAvatarDisplay->setAlignment(Qt::AlignCenter);
    lblAvatarDisplay->setStyleSheet(R"(
    QLabel {
        background-color: #ffffff;
        border: 2px solid #d0d0d0;
        border-radius: 14px;
    }
    )");

    showPlaceholderAvatar();

    mainLayout->addWidget(lblAvatarDisplay, 0, Qt::AlignCenter);

    // Preselected avatars combo
    QHBoxLayout *comboLayout = new QHBoxLayout();
    comboLayout->addWidget(new QLabel("Preselected:", this));
    comboPreselected = new QComboBox(this);
    comboLayout->addWidget(comboPreselected);
    mainLayout->addLayout(comboLayout);

    // Buttons layout
    QHBoxLayout *buttonLayout = new QHBoxLayout();

    btnSelectPreselected = new QPushButton("Select Preselected", this);
    connect(btnSelectPreselected, &QPushButton::clicked, this, &avatar::on_btnSelectPreselected_clicked);
    buttonLayout->addWidget(btnSelectPreselected);

    btnUploadAvatar = new QPushButton("Upload Custom", this);
    connect(btnUploadAvatar, &QPushButton::clicked, this, &avatar::on_btnUploadAvatar_clicked);
    buttonLayout->addWidget(btnUploadAvatar);

    btnApply = new QPushButton("Apply", this);
    connect(btnApply, &QPushButton::clicked, this, &avatar::on_btnApply_clicked);
    buttonLayout->addWidget(btnApply);

    btnSkip = new QPushButton("Skip", this);
    connect(btnSkip, &QPushButton::clicked, this, &avatar::on_btnSkip_clicked);
    buttonLayout->addWidget(btnSkip);

    mainLayout->addLayout(buttonLayout);
    mainLayout->addStretch();

    setLayout(mainLayout);
}

void avatar::loadPreselectedAvatars()
{
    if (!m_api) return;

    connect(m_api, &ApiClient::preselectedAvatarsReceived, this, &avatar::on_preselectedAvatarsReceived, Qt::UniqueConnection);
    connect(m_api, &ApiClient::requestFailed, this, &avatar::on_apiError, Qt::UniqueConnection);
    m_api->getPreselectedAvatars();
}

void avatar::displayAvatar(const QString& url)
{
    if (url.isEmpty()) {
        showPlaceholderAvatar();
        return;
    }

    QUrl resolvedUrl(url);
    if (!resolvedUrl.isValid() || resolvedUrl.isRelative()) {
        if (m_api) {
            resolvedUrl = m_api->baseUrl().resolved(QUrl(url));
        }
    }

    if (resolvedUrl.scheme().startsWith("http")) {
        if (!m_avatarNam) {
            showPlaceholderAvatar();
            return;
        }
        QNetworkReply* reply = m_avatarNam->get(QNetworkRequest(resolvedUrl));
        connect(reply, &QNetworkReply::finished, this, [this, reply, resolvedUrl]() {
            const QByteArray bytes = reply->readAll();
            QPixmap pixmap;
            const bool loaded = pixmap.loadFromData(bytes);
            reply->deleteLater();

            if (!loaded || pixmap.isNull()) {
                qWarning() << "Failed to load avatar:" << resolvedUrl;
                showPlaceholderAvatar();
                return;
            }

            const QPixmap scaled = pixmap.scaled(
                lblAvatarDisplay->size(),
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation);
            lblAvatarDisplay->setPixmap(scaled);
            m_lastDisplayedPixmap = scaled;
            m_hasAvatarShown = true;
        });
        return;
    }

    QPixmap pixmap(resolvedUrl.toString());
    if (pixmap.isNull()) {
        qWarning() << "Failed to load avatar:" << resolvedUrl;
        showPlaceholderAvatar();
        return;
    }

    const QPixmap scaled = pixmap.scaled(
        lblAvatarDisplay->size(),
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation);
    lblAvatarDisplay->setPixmap(scaled);
    m_lastDisplayedPixmap = scaled;
    m_hasAvatarShown = true;
}

void avatar::displayAvatarFromBytes(const QByteArray& bytes)
{
    QPixmap pixmap;
    if (!pixmap.loadFromData(bytes)) {
        qWarning() << "Failed to preview local avatar bytes";
        return;
    }
    const QPixmap scaled = pixmap.scaled(
        lblAvatarDisplay->size(),
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation);
    lblAvatarDisplay->setPixmap(scaled);
    m_lastDisplayedPixmap = scaled;
    m_hasAvatarShown = true;
}

void avatar::loadFallbackPreselectedAvatars()
{
    if (!m_preselectedAvatars.isEmpty()) return;

    QJsonArray avatars;
    avatars.append(QJsonObject{{"id", 1}, {"name", "Robot"}, {"url", "/uploads/pre/robot.png"}});
    avatars.append(QJsonObject{{"id", 2}, {"name", "Alien"}, {"url", "/uploads/pre/alien.png"}});
    avatars.append(QJsonObject{{"id", 3}, {"name", "Cat"}, {"url", "/uploads/pre/cat.png"}});
    on_preselectedAvatarsReceived(avatars);
}


void avatar::on_btnUploadAvatar_clicked()
{
    // Reset inactivity timer before opening blocking file dialog
    resetInactivityTimer();
    
    // Open file dialog for image selection
    QString fileName = QFileDialog::getOpenFileName(this,
        "Select Avatar Image", "",
        "PNG Images (*.png);;All Files (*)");

    if (fileName.isEmpty()) {
        // Reset timer again if dialog was cancelled
        resetInactivityTimer();
        return;
    }
    
    // Reset timer again after file dialog closes
    resetInactivityTimer();

    // Read file and upload
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        qWarning() << "Could not open file: " << fileName;
        return;
    }

    QByteArray fileData = file.readAll();
    file.close();
    displayAvatarFromBytes(fileData);

    // Call API to upload
    if (m_api)
    {
        setEnabled(false);
        connect(m_api, &ApiClient::avatarUploadSucceeded, this, &avatar::on_avatarUploadSucceeded,Qt::UniqueConnection);
        connect(m_api, &ApiClient::requestFailed, this, &avatar::on_apiError,Qt::UniqueConnection);
        m_api->uploadAvatar(m_login.idUser, fileData);
    }
}

void avatar::on_btnSelectPreselected_clicked()
{
    // Get selected preselected avatar from combo
    int currentIndex = comboPreselected->currentIndex();
    if (currentIndex >= 0 && currentIndex < m_preselectedAvatars.size())
    {
        QJsonObject avatarObj = m_preselectedAvatars[currentIndex].toObject();
        const QJsonValue idValue = avatarObj["id"];
        QString avatarId = idValue.isString() ? idValue.toString() : QString::number(idValue.toInt());
        QString avatarUrl = avatarObj["url"].toString();

        // Display selected avatar
        displayAvatar(avatarUrl);
        m_hasUploadedAvatar = false;

        // Call API to save selection
        if (m_api)
        {
            connect(m_api, &ApiClient::avatarSelectSucceeded, this, &avatar::on_avatarSelectSucceeded,Qt::UniqueConnection);
            connect(m_api, &ApiClient::requestFailed, this, &avatar::on_apiError,Qt::UniqueConnection);
            m_api->selectPreselectedAvatar(m_login.idUser, avatarId);
        }
    }
}

void avatar::on_btnApply_clicked()
{
    if (!m_hasUploadedAvatar && comboPreselected && comboPreselected->currentIndex() >= 0 && comboPreselected->currentIndex() < m_preselectedAvatars.size()) {
        QJsonObject avatarObj = m_preselectedAvatars[comboPreselected->currentIndex()].toObject();
        const QJsonValue idValue = avatarObj["id"];
        const QString avatarId = idValue.isString() ? idValue.toString() : QString::number(idValue.toInt());
        const QString avatarUrl = avatarObj["url"].toString();
        if (!avatarUrl.isEmpty()) {
            m_currentAvatarUrl = avatarUrl;
            displayAvatar(avatarUrl);
        }
        if (m_api && !avatarId.isEmpty()) {
            m_api->selectPreselectedAvatar(m_login.idUser, avatarId);
        }
    }

    if (!m_lastDisplayedPixmap.isNull()) {
        emit avatarApplied(m_lastDisplayedPixmap, m_currentAvatarUrl);
    }
    close();
}

void avatar::on_btnSkip_clicked()
{
    // Close dialog without changing avatar
    // Reset timer again after file dialog closes
    resetInactivityTimer();
    close();
}

void avatar::on_preselectedAvatarsReceived(const QJsonArray& avatars)
{
    m_preselectedAvatars = avatars;
    comboPreselected->clear();

    for (const QJsonValue& value : avatars)
    {
        QJsonObject obj = value.toObject();
        QString name = obj["name"].toString();
        comboPreselected->addItem(name);
    }
}

void avatar::on_avatarUploadSucceeded(const QString& avatarUrl)
{
    setEnabled(true);
    m_currentAvatarUrl = avatarUrl;
    m_hasUploadedAvatar = true;
    displayAvatar(avatarUrl);
    qDebug() << "Avatar uploaded successfully: " << avatarUrl;

}


void avatar::on_avatarSelectSucceeded(const QString& avatarUrl)
{
    m_currentAvatarUrl = avatarUrl;
    m_hasUploadedAvatar = false;
    displayAvatar(avatarUrl);
    qDebug() << "Avatar selected successfully: " << avatarUrl;
}

void avatar::on_apiError(const ApiError& error)
{
    qWarning() << "API Error:" << error.httpStatus << error.message << error.rawBody;
    setEnabled(true);
    if (btnSelectPreselected) btnSelectPreselected->setEnabled(true);
    if (btnUploadAvatar) btnUploadAvatar->setEnabled(true);
    if (btnSkip) btnSkip->setEnabled(true);
    if (comboPreselected) comboPreselected->setEnabled(true);

    const bool isNotFound = (error.httpStatus == 404) || error.message.contains("Not Found", Qt::CaseInsensitive);
    if (isNotFound && m_preselectedAvatars.isEmpty()) {
        loadFallbackPreselectedAvatars();
    }

    if (!m_hasAvatarShown) {
        showPlaceholderAvatar();
    }
}

void avatar::resetInactivityTimer()
{
    if (m_inactivityTimer) {
        m_inactivityTimer->stop();
        m_inactivityTimer->start(30000);  // Restart for another 30 seconds
    }
}

bool avatar::eventFilter(QObject *obj, QEvent *event)
{
    // Reset inactivity timer on any user interaction (mouse, keyboard, etc.)
    if (event->type() == QEvent::KeyPress || 
        event->type() == QEvent::MouseButtonPress ||
        event->type() == QEvent::MouseButtonRelease) {
        resetInactivityTimer();
    }
    return QDialog::eventFilter(obj, event);
}

void avatar::paintEvent(QPaintEvent *event)
{
    // Optional: custom background drawing
    QDialog::paintEvent(event);
}
void avatar::showPlaceholderAvatar()
{
    QPixmap placeholder(200, 200);
    placeholder.fill(QColor("#f2f2f2"));

    QPainter p(&placeholder);
    p.setRenderHint(QPainter::Antialiasing);

    p.setPen(QPen(QColor("#cccccc"), 4));
    p.drawEllipse(20, 20, 160, 160);

    lblAvatarDisplay->setPixmap(placeholder);
    m_lastDisplayedPixmap = placeholder;
    m_hasAvatarShown = false;
}

