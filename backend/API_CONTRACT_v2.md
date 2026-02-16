# API Contract v2 – Bank ATM REST API

## 1) General

- **Protocol:** HTTP  
- **Data format:** JSON  
- **Base URL (dev):** `http://localhost:3000`  
- **Content-Type:** `application/json`  
- **Authentication:** JWT  
- **Swagger UI:** `GET /docs`

## 2) Authentication

### 2.1 Login

**POST** `/auth/login`

**Request**
```json
{
  "idCard": "CARD123456",
  "pin": "1234"
}
```
**Response (200 OK)**
```json
{
  "token": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...",
  "role": "user",
  "fName": "Testi",
  "avatarUrl": "/uploads/avatars/TESTUSER1.png",
  "avatarType": "uploaded",
  "card": {
    "idCard": "CARD123456",
    "idUser": "TESTUSER1",
    "isLocked": false
  },
  "accounts": [
    {
      "idAccount": 14,
      "type": "debit",
      "balance": 500.00,
      "creditLimit": 0.00
    },
    {
      "idAccount": 15,
      "type": "credit",
      "balance": 0.00,
      "creditLimit": 2000.00
    }
  ],
  "requiresAccountSelection": true
}
```

### 2.2 Logout

**POST** `/auth/logout`
- Requires header: `Authorization: Bearer <token>`
- Token may be invalidated server-side (blacklist).

## 3) JWT Usage

All protected endpoints require:
```makefile
Authorization: Bearer <token>
```

## 4) Roles and Permissions

The system supports the following roles:
- `user` (default): may only use ATM endpoints related to their own accounts
- `admin`: may access all administrative endpoints

### Admin-only endpoints

- `/users/*`
- `/cards/*`
- `/accounts/*`
- `/cardaccount/*`
- `/log/*`

### User endpoints

- `/atm/*`<br>
(requires authentication and ownership of the account)

## 5) HTTP Status Codes

| Situation                     | Status                    |
| ----------------------------- | ------------------------- |
| Successful GET                | 200 OK                    |
| Resource created              | 201 Created               |
| Successful DELETE             | 204 No Content            |
| Invalid or missing data       | 400 Bad Request           |
| Not authenticated             | 401 Unauthorized          |
| Not authorized                | 403 Forbidden             |
| Resource not found            | 404 Not Found             |
| Conflict / duplicate resource | 409 Conflict              |
| Unexpected server error       | 500 Internal Server Error |

## 6) REST API Endpoints

### 6.1 ATM Operations (User)

> Requires JWT authentication and ownership of the account.

### Get account balance

**GET `/atm/:id`**

**Response (200 OK)**
```json
{
  "idAccount": 14,
  "idUser": "TESTUSER1",
  "balance": 500.00,
  "creditLimit": 0.00
}
```

### Get account transaction history (ATM view)

**GET `/atm/:id/logs`**

Response (200 OK)
```json
{
  "items": [
    {
      "idLog": 12,
      "time": "2026-01-16T09:30:00.123000",
      "balanceChange": -20.00
    }
  ]
}
```

### Withdraw money (debit)

**POST `/atm/:id/withdraw`**

**Request**
```json
{
  "amount": 40.00
}
```
**Response (200 OK)**
```json
{
  "idAccount": 14,
  "balance": 460.00,
  "logged": true
}
```

### Withdraw money (credit)

**POST `/atm/:id/credit/withdraw`**

**Request**
```json
{
  "amount": 200.00
}
```
**Response (200 OK)**
```json
{
  "idAccount": 15,
  "balance": -200.00,
  "logged": true
}
```

## 6.2 Users (Admin)

> Requires JWT and `role: admin`.

### Get user by ID

**GET `/users/:idUser`**

**Response (200 OK)**
```json
{
  "idUser": "USER123",
  "firstName": "Matti",
  "lastName": "Meikäläinen",
  "streetAddress": "Example Street 1",
  "avatarUrl": "/uploads/avatars/USER123.png",
  "avatarType": "uploaded",
  "role": "user"
}
```

### Create new user

**POST `/users`**

**Request**
```json
{
  "idUser": "USER123",
  "firstName": "Matti",
  "lastName": "Meikäläinen",
  "streetAddress": "Example Street 1",
  "role": "user"
}
```
- `role` is optional, default is `"user"`
- Allowed values: `"user"`, `"admin"`

**Response (201 Created)**
```json
{
  "idUser": "USER123",
  "firstName": "Matti",
  "lastName": "Meikäläinen",
  "streetAddress": "Example Street 1",
  "role": "user"
}
```

### Update user

**PUT `/users/:idUser`**

**Request**
```json
{
  "firstName": "Maija",
  "lastName": "Virtanen",
  "streetAddress": "New Street 5"
}
```
**Response (200 OK)**
```json
{
  "idUser": "USER123",
  "firstName": "Maija",
  "lastName": "Virtanen",
  "streetAddress": "New Street 5"
}
```

### Delete user

**DELETE `/users/:idUser`**

**Response**
- `204 No Content`

## 6.3 Cards (Admin)

> Requires JWT and `role: admin`.

### Get all cards

**GET `/cards`**

### Get card by ID

**GET `/cards/:idCard`**

### Create card (PIN is hashed with bcrypt)

**POST `/cards`**
```json
{
  "idCard": "CARD789012",
  "idUser": "USER123",
  "cardPIN": "1234"
}
```

### Update card PIN

**PUT `/cards/:idCard/pin`**

### Lock card

**POST `/cards/:idCard/lock`**

### Unlock card

**POST `/cards/:idCard/unlock`**

### Delete card

**DELETE `/cards/:idCard`**

## 6.4 Accounts (Admin)

> Requires JWT and `role: admin`.

### Get account

**GET `/accounts/:id`**

### Create account

**POST `/accounts`**

### Update credit limit

**PUT `/accounts/:id`**

### Delete account

**DELETE `/accounts/:id`**

> An account must not be linked to any cards when deleting.

## 6.5 Card–Account Links (Admin)

> Requires JWT and `role: admin`.

### Get linked accounts for a card

**GET `/cardaccount/:idCard`**

### Link card to account

**POST `/cardaccount`**

### Update card–account link

**PUT `/cardaccount/:idCard`**

### Remove card–account link

**DELETE `/cardaccount/:idCard`**

## 6.6 Logs (Admin)

> Requires JWT and `role: admin`.

### Get account logs (newest first)

**GET `/log/:idAccount`**

## 6.7 Avatars

> These endpoints currently do not require JWT.

### Get preselected avatars

**GET `/avatars/preselected`**

**Response (200 OK)**
```json
[
  { "id": 1, "name": "Robot", "url": "/uploads/pre/robot.png" },
  { "id": 2, "name": "Alien", "url": "/uploads/pre/alien.png" }
]
```

### Upload avatar (PNG)

**POST `/avatar/upload`**

**Request (multipart/form-data)**
```
userId=TESTUSER1
avatar=<PNG file>
```

**Response (200 OK)**
```json
{ "success": true, "avatar": "/uploads/avatars/USER123.png" }
```

### Select preselected avatar

**POST `/avatar/select/preselected`**

**Request**
```json
{ "userId": "TESTUSER1", "avatarId": 1 }
```

**Response (200 OK)**
```json
{ "success": true, "avatar": "/uploads/pre/robot.png" }
```

## 7) Environment Variables

Create `backend/.env`:
```env
DB_HOST=localhost
DB_USER=root
DB_PASSWORD=your_password
DB_NAME=bank_db
DB_PORT=3306

JWT_SECRET=your-secret-key-here
PIN_PEPPER=your-pepper-here

PORT=3000
```

## 8) Database Initialization

```bash
cd backend/db
sudo mysql -u root bank_db < schema.sql
sudo mysql -u root bank_db < procedures.sql
sudo mysql -u root bank_db < seed.sql
```

## 9) Test Credentials

**Regular user**
- Card: `CARD123456`
- PIN: `1234`

**Admin user**
- Card: `ADMINCARD`
- PIN: `admin123`