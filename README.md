# Bank ATM System

A full-stack banking application developed as part of the Software Development Project course.

This system simulates a real-world ATM environment consisting of:

![MySQL](https://img.shields.io/badge/mysql-4479A1.svg?style=for-the-badge&logo=mysql&logoColor=white)
![NodeJS](https://img.shields.io/badge/node.js-6DA55F?style=for-the-badge&logo=node.js&logoColor=white)
![Qt](https://img.shields.io/badge/Qt-%23217346.svg?style=for-the-badge&logo=Qt&logoColor=white)
![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![JWT](https://img.shields.io/badge/JWT-black?style=for-the-badge&logo=JSON%20web%20tokens)
![Express.js](https://img.shields.io/badge/express.js-%23404d59.svg?style=for-the-badge&logo=express&logoColor=%2361DAFB)
![Swagger](https://img.shields.io/badge/-Swagger-%23Clojure?style=for-the-badge&logo=swagger&logoColor=white)
![Bash Script](https://img.shields.io/badge/bash_script-%23121011.svg?style=for-the-badge&logo=gnu-bash&logoColor=white)
![CMake](https://img.shields.io/badge/CMake-%23008FBA.svg?style=for-the-badge&logo=cmake&logoColor=white)
![GitHub Actions](https://img.shields.io/badge/github%20actions-%232671E5.svg?style=for-the-badge&logo=githubactions&logoColor=white)
![GitHub](https://img.shields.io/badge/github-%23121011.svg?style=for-the-badge&logo=github&logoColor=white)
![Markdown](https://img.shields.io/badge/markdown-%23000000.svg?style=for-the-badge&logo=markdown&logoColor=white)

The project demonstrates layered architecture, secure authentication, database transactions and full-stack integration.


# Project Poster

![Project Poster](./bank-automat/images/background.png)
HUOM: Tähän oikea kuva posterista, kunhan tehty.

## Table of Contents

- [Project Overview](#bank-atm-system)
- [Project Poster](#project-poster)
- [System Architecture](#system-architecture)
- [Project Goals](#project-goals)
- [Project Team](#project-team)
- [Features Implemented](#features-implemented)
- [Technologies Used](#technologies-used)
- [Backend Setup](#backend-setup)
- [Qt ATM Client](#qt-atm-client)
- [Authentication Flow](#authentication-flow)
- [Test Credentials](#test-credentials)
- [Documentation](#documentation)
- [Project Management](#project-management)
- [Security Considerations](#security-considerations)
- [License](#license)

# System Architecture

## System Architecture

```text
+----------------------+
|  Qt Desktop Client   |
|  (C++ / Qt Widgets)  |
+----------+-----------+
           |
           | HTTP (JSON, JWT)
           v
+----------------------+
|  Node.js REST API    |
|  (Express)           |
+----------+-----------+
           |
           | Stored Procedures
           v
+----------------------+
|  MySQL Database      |
+----------------------+
```

### Communication Flow

1. Qt client sends HTTP request
2. REST API validates JWT and business rules
3. Stored procedure executes database logic
4. JSON response returned to client

<div align="right">
<a href="#table-of-contents">Back to top</a>
</div>

# Project Goals

- Implement a layered backend architecture
- Design and implement a REST API
- Use stored procedures for database logic
- Implement authentication with JWT
- Create a working ATM UI in Qt
- Fulfill course minimum and advanced requirements

<div align="right">
<a href="#table-of-contents">Back to top</a>
</div>

# Project Team

| Name | Responsibilities |
|------|------------------|
| [Juha Jermalainen](https://github.com/Sahid1981) | CRUD, Swagger, backend |
| [Laura Similä](https://github.com/Llaamari) | Backend, REST API, frontend |
| [Arttu Jämsä](https://github.com/Ard3J) | documentation, frontend, Qt |
| [Valtteri Tenhunen](https://github.com/TTEVAR) | Image upload |

(All members participated in planning, implementation and testing.)

<div align="right">
<a href="#table-of-contents">Back to top</a>
</div>

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

<div align="right">
<a href="#table-of-contents">Back to top</a>
</div>

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

<div align="right">
<a href="#table-of-contents">Back to top</a>
</div>

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

## 4. Start backend (running in development)

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

## Production Deployment (PM2) & Process Management

Backend is managed using **PM2** to ensure:
- Automatic restart on crashes
- Process monitoring
- Background execution
- Production-style process control

PM2 configuration:
- ecosystem.config.js
- Startup instructions documented in:

  SETUP_AUTOSTART.md

If deployed, backend can be started using:

```bash
pm2 start ecosystem.config.js
```
This setup enables production-style backend management and supports CI/CD workflows.

## GitHub Actions CI/CD

The project includes CI configuration for backend and frontend.

Continuous Integration:
- Automatic build checks
- Linting
- Optional test execution

This ensures code quality and maintainability.

<div align="right">
<a href="#table-of-contents">Back to top</a>
</div>

# Qt ATM Client

1. Open `bank-automat` in Qt Creator
2. Configure with CMake
3. Build and run

The client communicates with the backend using the REST API.

<div align="right">
<a href="#table-of-contents">Back to top</a>
</div>

# Authentication Flow

1. User enters card ID and PIN
2. Backend validates PIN (bcrypt + pepper)
3. JWT token issued
4. Token stored in memory
5. Token sent in Authorization header
6. Account selected (if multiple)
7. ATM operations performed

<div align="right">
<a href="#table-of-contents">Back to top</a>
</div>

# Test Credentials

### Regular User

- Card: `CARD123456`
- PIN: `1234`

### Admin User

- Card: `ADMINCARD`
- PIN: `admin123`

<div align="right">
<a href="#table-of-contents">Back to top</a>
</div>

# Documentation

### REST API Contract

📄 [API_CONTRACT_v2.md](./backend/API_CONTRACT_v2.md)

### Stored Procedures

📄 [STORED_PROCEDURES.md](./backend/db/STORED_PROCEDURES.md)

### Backend Autostart (PM2)

📄 [SETUP_AUTOSTART.md](./SETUP_AUTOSTART.md)

<div align="right">
<a href="#table-of-contents">Back to top</a>
</div>

# Project Management

- Git + GitHub version control
- Feature branches + Pull Requests
- Kanban board used for task management
- Weekly progress meetings
- Technical specification document created
- UML diagrams (ER, component, state diagram)

<div align="right">
<a href="#table-of-contents">Back to top</a>
</div>

# Security Considerations

- PIN codes stored as bcrypt hashes
- Additional server-side pepper
- JWT authentication
- Role-based access control
- Database access only through stored procedures
- Server-side validation for all financial operations

<div align="right">
<a href="#table-of-contents">Back to top</a>
</div>

# License

This project is developed for educational purposes.

MIT License can be applied if published publicly.

<div align="right">
<a href="#table-of-contents">Back to top</a>
</div>