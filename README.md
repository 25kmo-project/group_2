# Bank ATM Application

Bank ATM is a full-stack school project that simulates a real ATM system.  
The project consists of a **Node.js REST API backend** and a **Qt-based desktop client** that communicates with the backend over HTTP.

The goal of the project is to demonstrate:
- layered backend architecture
- REST API design
- database procedures and transactions
- authentication and authorization
- a desktop client consuming a REST API

## Project Structure (core)

```
pankkiautomaatti/
├── backend/ # Node.js REST API
│ ├── controllers/ # Route controllers
│ ├── db/ # SQL schema, procedures, seed data
│ ├── middleware/ # Auth, authorization, error handling
│ ├── routes/ # Express router resource files
│ ├── app.js # Express app entry
│ └── API_CONTRACT_v2.md # REST API specification
│
├── bank-automat/ # Qt (C++) ATM client
│ ├── *.cpp / *.h # UI and logic
│ └── CMakeLists.txt
│
├── SETUP_AUTOSTART.md # Backend automatic startup
└── README.md # This file
```

## Technologies Used

### Backend
- Node.js
- Express
- MySQL
- JWT authentication
- bcrypt (PIN hashing)

### Frontend
- C++ / Qt
- QNetworkAccessManager (REST client)
- Qt Widgets

## Prerequisites

Before running the project, make sure you have:

- Node.js (v18 or newer recommended)
- MySQL / MariaDB
- Qt (Qt Creator recommended)
- CMake
- Git

## Backend Setup

### 1. Install dependencies

```bash
cd backend
npm install
```

### 2. Environment variables

Create a `.env` file inside `backend/`:
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

### 3. Database initialization

Create the database and import schema, procedures and seed data:
```bash
cd backend/db
mysql -u root -p bank_db < schema.sql
mysql -u root -p bank_db < procedures.sql
mysql -u root -p bank_db < seed.sql
```
Seed data includes test users, cards and accounts for development.

### 4. Start backend server

```bash
cd backend
npm start
```
Backend will run at:
```arduino
http://localhost:3000
```
Swagger UI (API documentation):
```bash
http://localhost:3000/docs
```

## Frontend (Qt ATM Client)

1. Open the `bank-automat` folder in Qt Creator
2. Configure the project with CMake
3. Build and run the application

The client communicates with the backend using the REST API defined in `API_CONTRACT_v2.md`.

## Authentication Flow (High Level)

1. User inserts card number and PIN
2. Backend validates PIN and issues JWT token
3. Client stores token in memory
4. Token is sent in `Authorization: Bearer <token>` header
5. User selects account and performs ATM operations

## Test Credentials

### Regular user

- **Card**: `CARD123456`
- **PIN**: `1234`

### Admin user

- **Card**: `ADMINCARD`
- **PIN**: `admin123`

## Documentation

All REST endpoints, request/response formats, roles and permissions are documented in:<br>
📄 [**API_CONTRACT_v2.md**](./backend/API_CONTRACT_v2.md)<br>
This file is the **single source of truth** for the backend API.

Backend automatic startup are documented in:<br>
📄 [**SETUP_AUTOSTART.md**](./SETUP_AUTOSTART.md)<br>
This file documents PM2 installation and configuration

Stored procedures are documented in:<br>
📄 [**STORED_PROCEDURES.md**](./backend/db/STORED_PROCEDURES.md)<br>
This file documents all stored procedures used by the backend.

## Notes

- PIN codes are never stored in plain text
- Database operations use stored procedures
- ATM actions automatically create transaction logs
- Role-based authorization is enforced on the backend

## License

This project is created for educational purposes.