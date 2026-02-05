// This 'ApiClient' is Qt's HTTP client class that communicates with backend (default: http://127.0.0.1:3000). It handles:
// - base URL and JWT token storage
// - 'QNetworkAccessManager' calls (GET/POST/PUT/DELETE)
// - JSON request/response serialization
// - error parsing
// - "business" signals (login ok/fail, logout ok, balance, logs)

#include "apiclient.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QNetworkReply>

// ApiClient is a thin wrapper around QNetworkAccessManager for calling the backend API
// It stores the base URL and the current JWT token, builds requests, sends JSON, parses JSON responses and emits high-level signals for the UI
ApiClient::ApiClient(QObject* parent)
    : QObject(parent)
    // Default backend address (local development)
    //m_baseUrl(QUrl("http://127.0.0.1:3000"))
{
    QString db_url = getEnvValue("DB_IP");
    m_baseUrl = (QUrl(db_url));
}


// Configure where the backend lives (useful for changing envs)
void ApiClient::setBaseUrl(const QUrl& baseUrl) { m_baseUrl = baseUrl; }
QUrl ApiClient::baseUrl() const { return m_baseUrl; }

// Store / clear JWT token used in Authorization header
void ApiClient::setToken(const QString& token) { m_token = token; }
QString ApiClient::token() const { return m_token; }
void ApiClient::clearToken() { m_token.clear(); }

// Build a QNetworkRequest for a given API path
// Adds Content-Type and Authorization (Bearer token) if token is set
QNetworkRequest ApiClient::makeRequest(const QString& path) const
{
    QUrl url(m_baseUrl);
    url.setPath(path);

    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // Attach JWT token if available
    if (!m_token.isEmpty()) {
        req.setRawHeader("Authorization", QByteArray("Bearer ") + m_token.toUtf8());
    }
    return req;
}

// Send an HTTP request that has a JSON body (POST/PUT/DELETE/custom)
// When the reply finishes, it parses JSON and emits either requestFailed or domain signals
void ApiClient::sendJson(const QString& method, const QString& path, const QJsonObject& body)
{
    QNetworkRequest req = makeRequest(path);
    // Serialize request body to compact JSON
    const QByteArray payload = QJsonDocument(body).toJson(QJsonDocument::Compact);

    QNetworkReply* reply = nullptr;
    // Choose the correct QNetworkAccessManager method
    if (method == "POST") reply = m_nam.post(req, payload);
    else if (method == "PUT") reply = m_nam.put(req, payload);
    else if (method == "DELETE") {
        // Qt doesn't have delete-with-body in a dedicated function -> use custom request
        reply = m_nam.sendCustomRequest(req, "DELETE", payload);
    } else {
        // Fallback for any custom method
        reply = m_nam.sendCustomRequest(req, method.toUtf8(), payload);
    }

    // Handle response asynchronously
    connect(reply, &QNetworkReply::finished, this, [this, reply, path, body, method]() {

        //DELETOI nämä 2 myöhemmin
        qDebug() << "--- REAL-TIME DEBUG ---";
        qDebug() << "Actual Path:" << path << " | Status:" << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        ApiError err;
        QJsonDocument doc = readJson(reply, err);

        // Consider "ok" only if Qt saw no network error AND HTTP status is 2xx
        const int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        const bool ok = (reply->error() == QNetworkReply::NoError) && status >= 200 && status < 300;

        if (!ok) {
            // If readJson didn't fill status (rare), fill it from the reply attribute
            if (err.httpStatus == 0) err.httpStatus = status;

            // If the backend returned JSON with "message", use it as a friendly error
            if (err.message.isEmpty() && doc.isObject()) {
                const QJsonObject o = doc.object();
                const QString msg = o.value("message").toString();
                if (!msg.isEmpty()) err.message = msg;
            }

            // Fallback to Qt's error string
            if (err.message.isEmpty()) err.message = reply->errorString();

            //DELETOI nämä 2 myöhemmin
            qDebug() << "Server Error Message:" << doc.object().value("message").toString();
            qDebug() << "Full Error Body:" << doc.toJson();

            emit requestFailed(err);
            reply->deleteLater();
            return;
        }

        // Special-case handling for login response
        // Parse token + card + accounts, store token, then emit loginSucceeded
        if (path == "/auth/login") {
            LoginResultDto out;
            const QJsonObject o = doc.object();

            out.token = o.value("token").toString();
            out.fName = o.value("fName").toString();
            out.role  = o.value("role").toString();
            if (out.token.isEmpty()) {
                // Treat missing token as a server-side contract error
                ApiError e;
                e.httpStatus = 500;
                e.message = "Login response missing token";
                emit requestFailed(e);
                reply->deleteLater();
                return;
            }

            out.requiresAccountSelection = o.value("requiresAccountSelection").toBool();

            // Parse nested "card" object
            const QJsonObject card = o.value("card").toObject();
            out.idCard = card.value("idCard").toString();
            out.idUser = card.value("idUser").toString();
            out.isLocked = card.value("isLocked").toBool();

            // Parse "accounts" array
            const QJsonArray accounts = o.value("accounts").toArray();
            for (const auto& v : accounts) {
                const QJsonObject a = v.toObject();
                AccountDto acc;
                acc.idAccount = a.value("idAccount").toInt();
                acc.type = a.value("type").toString();
                acc.balance = a.value("balance").toDouble();
                acc.creditLimit = a.value("creditLimit").toDouble();
                out.accounts.push_back(acc);
            }

            // Save token for future calls and notify UI
            setToken(out.token);
            emit loginSucceeded(out);
        }

        // Special-case handling for withdraw responses (debit and credit)
        // Expected backend response example:
        // { "idAccount": 14, "balance": 460.00, "logged": true, ... }
        if (path.endsWith("/withdraw")) {
            const QJsonObject o = doc.object();

            // Prefer idAccount from JSON; fallback to parsing from URL: "/atm/{id}/withdraw" or "/atm/{id}/credit/withdraw"
            int idAccount = o.value("idAccount").toInt();
            if (idAccount <= 0) {
                idAccount = path.section('/', 2, 2).toInt();
            }

            // Prefer balance from JSON; if missing, use 0.0 (or consider treating as contract error)
            const double newBalance = o.value("balance").toDouble();

            emit withdrawSucceeded(idAccount, newBalance);
        }

        // Special case for updating user
        if (path.startsWith("/users/") && method == "PUT") {
            QString idFromPath = path.section('/',2,2);
            emit userUpdated(idFromPath);
        }

        // Special case for creating a user
        if (path.startsWith("/users") && method == "POST") {
            QString id = body.value("idUser").toString();
            emit userCreated(id);
        }

        // Special case for deleting a user
        if (path.startsWith("/users") && method == "DELETE") {
            //QString id = body.value("idUser").toString();
            emit userDeleted();
        }

        // Special case for creating account
        if (path.startsWith("/accounts") && method == "POST") {
            QByteArray responseData = doc.toJson(QJsonDocument::Compact);
            emit accountCreated(responseData);
        }

        // Special case for updating creditlimit
        if (path.startsWith("/accounts") && method == "PUT") {
            int idFromPath = path.section('/',2,2).toInt();
            emit accountCreditUpdated(idFromPath);
        }

        // Special case for deleting an account
        if (path.startsWith("/accounts") && method == "DELETE") {
            emit accountDeleted();
        }

        // Special case for linking account to card
        if (path.startsWith("/cardaccount") && method == "POST") {
            QByteArray responseData = doc.toJson(QJsonDocument::Compact);
            emit cardAccountLinked(responseData);
        }

        // Special case for updating link between card and account
        if (path.startsWith("/cardaccount") && method == "PUT") {
            QString idFromPath = path.section('/',2,2);
            emit cardLinkUpdated(idFromPath);
        }

        // Special case for deleting a link between account and card
        if (path.startsWith("/cardaccount") && method == "DELETE") {
            QString idFromPath = path.section('/',2,2);
            emit cardLinkDeleted(idFromPath);
        }

        // Special case for locking card
        if (path.startsWith("/cards/") && path.endsWith("/lock") && method == "POST") {
            QString idFromPath = path.section('/',2,2);
            emit cardLocked(idFromPath);
        }

        // Special case for unlocking card
        if (path.startsWith("/cards/") && path.endsWith("/unlock") && method == "POST") {
            QString idFromPath = path.section('/',2,2);
            emit cardUnlocked(idFromPath);
        }

        // Special case for creating card
        if (path.startsWith("/cards") && method == "POST") {
            QByteArray responseData = doc.toJson(QJsonDocument::Compact);
            emit cardCreated(responseData);
        }

        // Special case for deleting card
        if (path.startsWith("/cards") && method == "DELETE") {
            emit cardDeleted();
        }

        // Special case for updating PIN
        if (path.startsWith("/cards/") && path.endsWith("/pin") && method == "PUT") {
            QString idFromPath = path.section('/',2,2);
            emit PINUpdated(idFromPath);
        }

        reply->deleteLater();
    });
}

// Send an HTTP request without a request body (GET, POST with empty body, custom methods)
// Parses response JSON (if any) and emits domain signals based on the path
void ApiClient::sendNoBody(const QString& method, const QString& path)
{
    QNetworkRequest req = makeRequest(path);

    QNetworkReply* reply = nullptr;
    if (method == "GET") reply = m_nam.get(req);
    else if (method == "POST") reply = m_nam.post(req, QByteArray());
    else reply = m_nam.sendCustomRequest(req, method.toUtf8());

    connect(reply, &QNetworkReply::finished, this, [this, reply, path, method]() {
        const int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        const bool ok = (reply->error() == QNetworkReply::NoError) && status >= 200 && status < 300;

        if (!ok) {
            // For non-2xx responses, parse and emit a unified error
            emit requestFailed(parseError(reply));
            reply->deleteLater();
            return;
        }

        // Try reading JSON (may be empty for 204)
        ApiError err;
        QJsonDocument doc = readJson(reply, err);

        // Special-case logout: clear token and notify UI
        if (path == "/auth/logout") {
            clearToken();
            emit logoutSucceeded();
            reply->deleteLater();
            return;
        }

        // Special-case: GET /atm/{idAccount} returns account balance info (not logs)
        if (path.startsWith("/atm/") && !path.endsWith("/logs")) {
            const QJsonObject o = doc.object();
            const int idAccount = o.value("idAccount").toInt();
            const double balance = o.value("balance").toDouble();
            const double creditLimit = o.value("creditLimit").toDouble();
            emit balanceReceived(idAccount, balance, creditLimit);
        }

        // Special-case: GET /atm/{idAccount}/logs returns an object { items: [...] }
        if (path.endsWith("/logs")) {
            // Extract idAccount from the URL path: "/atm/{id}/logs"
            const int idAccount = path.section('/', 2, 2).toInt();
            QVector<LogItemDto> items;

            const QJsonObject o = doc.object();
            const QJsonArray arr = o.value("items").toArray();
            for (const auto& v : arr) {
                const QJsonObject li = v.toObject();
                LogItemDto dto;
                dto.idLog = li.value("idLog").toInt();
                dto.time = li.value("time").toString();
                dto.balanceChange = li.value("balanceChange").toDouble();
                items.push_back(dto);
            }
            emit logsReceived(idAccount, items);
        }

        // Special-case: GET /users/{idUser} returns user data as an object
        if (path.startsWith("/users/")) {
            QByteArray user = doc.toJson(QJsonDocument::Compact);
            emit userReceived(user);
        }

        // Special_case: GET /users returns all users as an QByteArray
        if (path.startsWith("/users")) {
            QByteArray users = doc.toJson(QJsonDocument::Compact);
            emit allUsersReceived(users);
        }

        // Special-case: GET /accounts/{idAccount} returns account data as an object
        if (path.startsWith("/accounts") && method == "GET") {
            QByteArray o = doc.toJson(QJsonDocument::Compact);
            emit accountReceived(o);
        }

        // Special-case: GET /accounts returns all users as an QByteArray
        if (path.startsWith("/accounts") && method == "GET") {
            QByteArray accounts = doc.toJson(QJsonDocument::Compact);
            emit allAccountsReceived(accounts);
        }

        // Special-case: Get /logs/{idAccount} returns logs as an QByteArray
        if (path.startsWith("/log/") && method == "GET") {
            QByteArray logs = doc.toJson(QJsonDocument::Compact);
            emit adminLogsReceived(logs);
        }

        if (path.startsWith("/cards/") && method == "GET") {
            QByteArray card = doc.toJson(QJsonDocument::Compact);
            emit cardReceived(card);
        }

        // Special-case: Get /cards returns all cards as an QByteArray
        if (path.startsWith("/cards") && method == "GET") {
            QByteArray cards = doc.toJson(QJsonDocument::Compact);
            emit allCardsReceived(cards);
        }

        // Special-case: Create new card
        if (path.startsWith("/cardaccount/") && method == "GET") {
            QByteArray cardAccount = doc.toJson(QJsonDocument::Compact);
            emit cardAccountReceived(cardAccount);
        }

        reply->deleteLater();
    });
}

// Parse an error reply:
// - reads HTTP status
// - reads raw response body as text
// - if JSON { message: "..." } exists, use it
// - otherwise fallback to Qt error string
ApiError ApiClient::parseError(QNetworkReply* reply)
{
    ApiError e;
    e.httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    const QByteArray body = reply->readAll();
    e.rawBody = QString::fromUtf8(body);

    QJsonParseError pe;
    QJsonDocument doc = QJsonDocument::fromJson(body, &pe);
    if (pe.error == QJsonParseError::NoError && doc.isObject()) {
        const QJsonObject o = doc.object();
        const QString msg = o.value("message").toString();
        if (!msg.isEmpty()) e.message = msg;
    }

    if (e.message.isEmpty()) {
        e.message = reply->errorString();
        if (e.message.isEmpty()) e.message = "Request failed";
    }
    return e;
}

// Read and parse JSON from the reply body
// Also fills errOut.rawBody and errOut.httpStatus for debugging
// If the body is empty (e.g., 204), returns an empty QJsonDocument
QJsonDocument ApiClient::readJson(QNetworkReply* reply, ApiError& errOut)
{
    const QByteArray body = reply->readAll();
    errOut.rawBody = QString::fromUtf8(body);
    errOut.httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    if (body.isEmpty()) return QJsonDocument();

    QJsonParseError pe;
    QJsonDocument doc = QJsonDocument::fromJson(body, &pe);
    if (pe.error != QJsonParseError::NoError) {
        errOut.message = "Invalid JSON in response";
        return QJsonDocument();
    }
    return doc;
}

// Used to read from .env. Currently only IP is stored there.
QString ApiClient::getEnvValue(QString secret)
{
    QFile file(QCoreApplication::applicationDirPath() + "/.env");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Opening .env failed. looked at" << QCoreApplication::applicationDirPath();
        // returns empty to prevent crashing
        return "";
    }
    QTextStream in(&file);
    while(!in.atEnd()) {
        QString line = in.readLine();
        if (line.contains(secret + "=")) {
            QString secretKey = line.split("=").at(1).trimmed();
            return secretKey;
        }
    }
    return "";  //failsafe
}

// Convenience API methods used by the UI:
// POST /auth/login with { idCard, pin }
void ApiClient::login(const QString& idCard, const QString& pin)
{
    sendJson("POST", "/auth/login", QJsonObject{
        {"idCard", idCard},
        {"pin", pin}
    });
}

// POST /auth/logout (no body)
void ApiClient::logout()
{
    sendNoBody("POST", "/auth/logout");
}

// GET /atm/{idAccount}
void ApiClient::getBalance(int idAccount)
{
    sendNoBody("GET", QString("/atm/%1").arg(idAccount));
}

// GET /atm/{idAccount}/logs
void ApiClient::getAccountLogs(int idAccount)
{
    sendNoBody("GET", QString("/atm/%1/logs").arg(idAccount));
}

void ApiClient::getUser(QString idUser)
{
    sendNoBody("GET", QString("/users/%1").arg(idUser));
}

void ApiClient::getAllUsers()
{
    sendNoBody("GET", QString("/users"));
}

void ApiClient::addUser(QString idUser, QString firstName, QString lastName, QString streetaddress, QString role)
{
    sendJson("POST", QString("/users"), QJsonObject{{"idUser", idUser},{"firstName", firstName},{"lastName",lastName},{"streetAddress", streetaddress}, {"role", role}});
}

//Role cannot currently be changed due to mysql procedure limitation
void ApiClient::updateUser(QString idUser, QString firstName, QString lastName, QString streetaddress) //, QString role)
{
    sendJson("PUT", QString("/users/%1").arg(idUser), QJsonObject{{"idUser", idUser},{"firstName", firstName},{"lastName",lastName},{"streetAddress", streetaddress}});
}

void ApiClient::deleteUser(QString idUser)
{
    sendJson("DELETE", QString("/users/%1").arg(idUser), QJsonObject{{}});
}

void ApiClient::getAccount(int idAccount)
{
    sendNoBody("GET", QString("/accounts/%1").arg(idAccount));
}

void ApiClient::getAllAccounts()
{
    sendNoBody("GET", QString("/accounts"));
}

void ApiClient::addAccount(QString idUser, double balance, double creditLimit)
{
    sendJson("POST", QString("/accounts"), QJsonObject{{"idUser", idUser},{"balance", balance},{"creditLimit", creditLimit}});
}

void ApiClient::updateCreditLimit(int idAccount, double creditLimit)
{
    sendJson("PUT", QString("/accounts/%1").arg(idAccount), QJsonObject{{"creditLimit", creditLimit}});
}

void ApiClient::deleteAccount(int idAccount)
{
    sendJson("DELETE", QString("/accounts/%1").arg(idAccount), QJsonObject{{}});
}

void ApiClient::addCard(QString idCard, QString idUser, QString cardPIN)
{
    sendJson("POST", QString("/cards"), QJsonObject{{"idCard", idCard},{"idUser", idUser},{"cardPIN", cardPIN}});
}

void ApiClient::deleteCard(QString idCard)
{
    sendJson("DELETE", QString("/cards/%1").arg(idCard), QJsonObject{{}});
}

void ApiClient::updatePIN(QString idCard, QString PIN)
{
    sendJson("PUT", QString("/cards/%1/pin").arg(idCard), QJsonObject{{"cardPIN",PIN}});
}

void ApiClient::linkCard(QString idCard, int idAccount)
{
    sendJson("POST", QString("/cardaccount"), QJsonObject{{"idCard", idCard},{"idAccount", idAccount}});
}

void ApiClient::updateLink(QString idCard, int idAccount, int newIdAccount)
{
    sendJson("PUT", QString("/cardaccount/%1").arg(idCard), QJsonObject{{"idAccount", idAccount},{"newIdAccount", newIdAccount}});
}

void ApiClient::deleteLink(QString idCard, int idAccount)
{
    sendJson("DELETE", QString("/cardaccount/%1").arg(idCard), QJsonObject{{"idAccount", idAccount}});
}

void ApiClient::lockCard(QString idCard)
{
    sendJson("POST", QString("/cards/%1/lock").arg(idCard), QJsonObject{{"idCard", idCard}});
}

void ApiClient::unlockCard(QString idCard)
{
    sendJson("POST", QString("/cards/%1/unlock").arg(idCard), QJsonObject{{"idCard", idCard}});
}

void ApiClient::getAllCards()
{
    sendNoBody("GET", QString("/cards"));
}

void ApiClient::getCard(QString idCard)
{
    sendNoBody("GET", QString("/cards/%1").arg(idCard));
}

void ApiClient::getCardAccount(QString idCard)
{
    sendNoBody("GET", QString("/cardaccount/%1").arg(idCard));
}

void ApiClient::getAdminLogs(int idAccount)
{
    sendNoBody("GET", QString("/log/%1").arg(idAccount));
}

// POST /atm/{idAccount}/withdraw with { amount }
void ApiClient::withdrawDebit(int idAccount, double amount)
{
    sendJson("POST", QString("/atm/%1/withdraw").arg(idAccount), QJsonObject{{"amount", amount}});
}

// POST /atm/{idAccount}/credit/withdraw with { amount }
void ApiClient::withdrawCredit(int idAccount, double amount)
{
    sendJson("POST", QString("/atm/%1/credit/withdraw").arg(idAccount), QJsonObject{{"amount", amount}});
}
