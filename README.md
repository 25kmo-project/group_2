# Bank ATM System

A full-stack banking application developed as part of the Software Development Project course.

This system simulates a real-world ATM environment consisting of:

- MySQL database
- Node.js REST API  
- Qt C++ desktop ATM client  

The project demonstrates layered architecture, secure authentication, database transactions and full-stack integration.


# Project Poster

![Project Poster](./bank-automat/images/background.png)
HUOM: Tähän oikea kuva posterista, kunhan tehty.

# System Architecture

```
Qt Desktop Client (C++ / Qt Widgets)
│
│ HTTP (JSON, JWT)
▼
Node.js REST API (Express)
│
│ Stored Procedures
▼
MySQL Database
```

### Communication Flow

1. Qt client sends HTTP request
2. REST API validates JWT and business rules
3. Stored procedure executes database logic
4. JSON response returned to client

# Project Goals

- Implement a layered backend architecture
- Design and implement a REST API
- Use stored procedures for database logic
- Implement authentication with JWT
- Create a working ATM UI in Qt
- Fulfill course minimum and advanced requirements

# Project Team

| Name | Responsibilities |
|------|------------------|
| [Juha Jermalainen](https://github.com/Sahid1981) | CRUD, Swagger, backend |
| [Laura Similä](https://github.com/Llaamari) | Backend, REST API, frontend |
| [Arttu Jämsä](https://github.com/Ard3J) | documentation, frontend, Qt |
| [Valtteri Tenhunen](https://github.com/TTEVAR) | Image upload |

(All members participated in planning, implementation and testing.)

# Features Implemented

## Core Requirements (Grade 1–2)

- Debit account support
- Qt application startup user interface
- Card login with PIN verification
- Balance display
- Withdrawal (20€, 40€, 50€, 100€)
- 10 latest transactions
- 10-second PIN timeout
- Full CRUD operations for all database tables

## Advanced Features (Grade 3–4)

- Credit account support
- Credit limit handling
- Withdrawal of any amount (only €20 and €50 notes at ATM)
- Persistent card locking (stored in database)
- 30-second global inactivity timeout
- Transaction history browsing (pagination)

## Excellent-Level Features (Grade 5)

- Dual card support (debit + credit in one card)
- Account selection after login
- UML state diagram created
- Role-based authorization (admin / user)
- Structured API contract documentation
- Clean MVC-style backend structure
- Additional features:
   - Uploading and displaying images
   - Swagger documentation
   - Logs
   - Adding tests to the backend
   - CI/CD
   - Extra Qt application (admin)

# Technologies Used

## Backend
- Node.js
- Express
- MySQL
- JWT authentication
- bcrypt (PIN hashing)
- Stored procedures
- Swagger documentation
- CI/CD

## Frontend
- C++ (Qt Widgets)
- QNetworkAccessManager (API client)
- CMake build system

# Backend Setup

## 1. Install dependencies

```bash
cd backend
npm install
```

## 2. Create .env file

```env
DB_HOST=localhost
DB_USER=root
DB_PASSWORD=your_password
DB_NAME=bank_db
DB_PORT=3306

JWT_SECRET=your-secret-key
PIN_PEPPER=your-pepper-value

PORT=3000
```

## 3. Initialize database

```bash
cd backend/db
mysql -u root -p bank_db < schema.sql
mysql -u root -p bank_db < procedures.sql
mysql -u root -p bank_db < seed.sql
```

## 4. Start backend

```bash
cd backend
npm start
```
Backend runs at:
```arduino
http://localhost:3000
```
Swagger documentation:
```bash
http://localhost:3000/docs
```

# Qt ATM Client

1. Open `bank-automat` in Qt Creator
2. Configure with CMake
3. Build and run

The client communicates with the backend using the REST API.

# Authentication Flow

1. User enters card ID and PIN
2. Backend validates PIN (bcrypt + pepper)
3. JWT token issued
4. Token stored in memory
5. Token sent in Authorization header
6. Account selected (if multiple)
7. ATM operations performed

# Test Credentials

### Regular User

- Card: `CARD123456`
- PIN: `1234`

### Admin User

- Card: `ADMINCARD`
- PIN: `admin123`

# Documentation

### REST API Contract

📄 [API_CONTRACT_v2.md](./backend/API_CONTRACT_v2.md)

### Stored Procedures

📄 [STORED_PROCEDURES.md](./backend/db/STORED_PROCEDURES.md)

### Backend Autostart (PM2)

📄 [SETUP_AUTOSTART.md](./SETUP_AUTOSTART.md)

# Project Management

- Git + GitHub version control
- Feature branches + Pull Requests
- Kanban board used for task management
- Weekly progress meetings
- Technical specification document created
- UML diagrams (ER, component, state diagram)

# Security Considerations

- PIN codes stored as bcrypt hashes
- Additional server-side pepper
- JWT authentication
- Role-based access control
- Database access only through stored procedures
- Server-side validation for all financial operations

# License

This project is developed for educational purposes.

MIT License can be applied if published publicly.