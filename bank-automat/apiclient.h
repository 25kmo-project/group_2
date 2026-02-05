// This file is the header for the ApiClient class. It defines:
// - small DTO structures (data from API to UI)
// - The public interface of the ApiClient class (what the UI can call)
// Qt signals for reporting results and errors
// Private helper methods for HTTP/JSON processing

// Ensures the header is included only once per translation unit
#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QUrl>
#include <QJsonObject>
#include <QJsonDocument>
#include <QVector>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QCoreApplication>
#include <QDebug>

class QNetworkReply;

// Represents a unified API error returned to the UI layer
struct ApiError {
    int httpStatus = 0; // HTTP status code (e.g. 401, 500)
    QString message;    // Human-readable error message
    QString rawBody;    // Raw response body (useful for debugging)
};

// Data Transfer Object for a bank account
struct AccountDto {
    int idAccount = 0;          // Account ID
    QString type;               // Account type (e.g. "debit", "credit")
    double balance = 0.0;       // Current balance
    double creditLimit = 0.0;   // Credit limit (0 for debit accounts)
};

// Data Transfer Object for a successful login response
struct LoginResultDto {
    QString token;                          // JWT token
    QString idCard;                         // Card identifier
    QString idUser;                         // User identifier
    QString fName;                          // User first name
    QString role;                           // User role
    bool isLocked = false;                  // Whether the card is locked
    QVector<AccountDto> accounts;           // Accounts linked to the card
    bool requiresAccountSelection = false;  // True if user must choose an account
};

// Data Transfer Object for a single log entry
struct LogItemDto {
    int idLog = 0;              // Log entry ID
    QString time;               // Timestamp (ISO string from backend)
    double balanceChange = 0.0; // Amount changed (+ / -)
};

// Data Transfer Object for all users
struct AllUsers {
    QString idUser;
    QString firstName;
    QString lastName;
    QString streetAddress;
    QString role;
};

// ApiClient is responsible for all HTTP communication with the backend
// It wraps QNetworkAccessManager and exposes high-level API operations (login, logout, balance, withdrawals, logs) via Qt signals
class ApiClient : public QObject
{
    Q_OBJECT
public:
    explicit ApiClient(QObject* parent = nullptr);

    // Configure and access the backend base URL
    void setBaseUrl(const QUrl& baseUrl);
    QUrl baseUrl() const;

    // Manage JWT token used for Authorization header
    void setToken(const QString& token);
    QString token() const;
    void clearToken();

    // High-level API calls used by the UI
    void login(const QString& idCard, const QString& pin);
    void logout();

    void getBalance(int idAccount);
    void withdrawDebit(int idAccount, double amount);
    void withdrawCredit(int idAccount, double amount);
    void getAccountLogs(int idAccount);

    void getUser(QString idUser);
    void getAllUsers();
    void addUser(QString idUser, QString fname, QString lname, QString streetaddress, QString role);
    //Role cannot currenly be changed with this due to db procedure not having it
    void updateUser(QString idUser, QString fname, QString lname, QString streetaddress); //, QString role);
    void deleteUser(QString idUser);

    void getAccount(int idAccount);
    void getAllAccounts();
    void addAccount(QString idUser, double balance, double creditLimit);
    void updateCreditLimit(int idAccount, double creditLimit);
    void deleteAccount(int idAccount);

    void getAllCards();
    void getCard(QString idCard);
    void getCardAccount(QString idCard);
    void addCard(QString idCard, QString idUser, QString cardPIN);
    void deleteCard(QString idCard);
    void updatePIN(QString idCard, QString PIN);
    void linkCard(QString idCard, int idAccount);
    void updateLink(QString idCard, int idAccount, int newIdAccount);
    void deleteLink(QString idCard, int idAccount);
    void lockCard(QString idCard);
    void unlockCard(QString idCard);

    void getAdminLogs(int idAccount);

signals:
    // Emitted when login succeeds and all required data is parsed
    void loginSucceeded(const LoginResultDto& result);
    // Emitted when logout succeeds
    void logoutSucceeded();

    // Emitted when account balance information is received
    void balanceReceived(int idAccount, double balance, double creditLimit);
    // Emitted when a withdrawal succeeds (declared for future/optional use)
    void withdrawSucceeded(int idAccount, double newBalance);
    // Emitted when account transaction logs are received
    void logsReceived(int idAccount, const QVector<LogItemDto>& logs);

    // Emitted when user is received
    void userReceived(const QByteArray& userInfo);
    // Emitted when all users are receives
    void allUsersReceived(const QByteArray& allUsers);
    // Emitted when user is created
    void userCreated(QString idUser);
    // Emitted when user is updated
    void userUpdated(QString idUser);
    // Emitted when user is deleted
    void userDeleted();

    // Emitted when account is received
    void accountReceived(const QByteArray& accountInfo);
    // Emitted when all accounts are received
    void allAccountsReceived(const QByteArray& allAccounts);
    // Emitted when account is created
    void accountCreated(const QByteArray& accountInfo);
    // Emitted when creditlimit is updated
    void accountCreditUpdated(int idAccount);
    // Emittend when account is deleted
    void accountDeleted();

    // Emitted when all cards are received
    void allCardsReceived(const QByteArray& allCards);
    // Emitted when a card is received
    void cardReceived(const QByteArray& allCards);
    // Emitted when account is created
    void cardCreated(const QByteArray& cardData);
    // Emitted when account is deleted
    void cardDeleted();
    // Emitted when PIN is updated
    void PINUpdated(QString idCard);
    // Emitted when account(s) linked to card is received
    void cardAccountReceived(const QByteArray& cardAccount);
    // Emitted when account is linked to card
    void cardAccountLinked(const QByteArray& cardAccountLinked);
    // Emitted when card is locked
    void cardLocked(QString cardLocked);
    // Emitted when card is unlocked
    void cardUnlocked(QString cardUnlocked);
    // Emitted when account linked to card is updated
    void cardLinkUpdated(QString idCard);
    // Emitted when card-account link is deleted
    void cardLinkDeleted(QString idCard);

    // Emitted when all logs are received
    void adminLogsReceived(const QByteArray& adminLogs);

    // Emitted whenever any API request fails
    void requestFailed(const ApiError& error);

private:
    // Build a QNetworkRequest with base URL, path, headers and token
    QNetworkRequest makeRequest(const QString& path) const;
    // Send an HTTP request with a JSON body (POST / PUT / DELETE)
    void sendJson(const QString& method, const QString& path, const QJsonObject& body);
    // Send an HTTP request without a body (GET / POST without payload)
    void sendNoBody(const QString& method, const QString& path);

    // Parse an error response into ApiError
    static ApiError parseError(QNetworkReply* reply);
    // Read and parse JSON from a reply, also filling error metadata
    static QJsonDocument readJson(QNetworkReply* reply, ApiError& errOut);

    QString getEnvValue(QString secret);

private:
    QNetworkAccessManager m_nam;    // Qt network engine (handles async requests)
    QUrl m_baseUrl;                 // Backend base URL
    QString m_token;                // JWT token for Authorization header
};
