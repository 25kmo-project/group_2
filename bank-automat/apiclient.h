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

private:
    QNetworkAccessManager m_nam;    // Qt network engine (handles async requests)
    QUrl m_baseUrl;                 // Backend base URL
    QString m_token;                // JWT token for Authorization header
};
