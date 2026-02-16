#ifndef AVATAR_H
#define AVATAR_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QPixmap>
#include <QTimer>
#include <QNetworkAccessManager>
#include "apiclient.h"
#include <QJsonArray>

// Avatar dialog shown after login, allowing users to upload or select a preselected avatar
class avatar : public QDialog
{
    Q_OBJECT

public:
    explicit avatar(const LoginResultDto& loginResult, ApiClient* api, QTimer* inactivityTimer = nullptr, QWidget *parent = nullptr);
    ~avatar();

protected:
    void paintEvent(QPaintEvent *event) override;
    // Reset inactivity timer on any user interaction
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void on_btnUploadAvatar_clicked();
    void on_btnSelectPreselected_clicked();
    void on_btnApply_clicked();
    void on_btnSkip_clicked();
    void on_preselectedAvatarsReceived(const QJsonArray& avatars);
    void on_avatarUploadSucceeded(const QString& avatarUrl);
    void on_avatarSelectSucceeded(const QString& avatarUrl);
    void on_apiError(const ApiError& error);

signals:
    void avatarApplied(const QPixmap& pixmap, const QString& avatarUrl);

private:
    // UI elements
    QLabel *lblAvatarDisplay = nullptr;
    QLabel *lblTitle = nullptr;
    QPushButton *btnUploadAvatar = nullptr;
    QPushButton *btnSelectPreselected = nullptr;
    QPushButton *btnApply = nullptr;
    QPushButton *btnSkip = nullptr;
    QComboBox *comboPreselected = nullptr;

    // Data
    ApiClient* m_api = nullptr;
    LoginResultDto m_login;
    QJsonArray m_preselectedAvatars;
    QString m_currentAvatarUrl;
    QTimer* m_inactivityTimer = nullptr;
    QNetworkAccessManager* m_avatarNam = nullptr;
    bool m_hasAvatarShown = false;
    QPixmap m_lastDisplayedPixmap;

    // Initialization
    void setupUI();
    void loadPreselectedAvatars();
    void displayAvatar(const QString& url);
    void loadFallbackPreselectedAvatars();
    void displayAvatarFromBytes(const QByteArray& bytes);
    // Reset the inactivity timer to prevent timeout during upload
    void resetInactivityTimer();
    void showPlaceholderAvatar();
};

#endif // AVATAR_H
