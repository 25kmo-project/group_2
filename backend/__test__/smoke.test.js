// smoke tests for backend API endpoints

/* Could be enhanced with more detailed tests later
    like:
    - test-dp for each endpoint
    - test with and without auth tokens
    - test with different user roles
    - or just few tests for important endpoints
*/


const request = require('supertest');
const app = require('../app');

describe('API Smoke Tests', () => {
    describe('Public endpoints', () => {
        test('GET / - health check responds', async () => {
            const response = await request(app).get('/');
            expect(response.status).toBeDefined();
        });

        test('POST /auth/login - responds with some status', async () => {
            const response = await request(app)
                .post('/auth/login')
                .send({});
            expect(response.status).toBeDefined();
        });

        test('POST /auth/logout - responds', async () => {
            const response = await request(app).post('/auth/logout');
            expect(response.status).toBeDefined();
        });
    });

    describe('ATM endpoints - protected', () => {
        test('GET /atm/1 - get balance responds', async () => {
            const response = await request(app).get('/atm/1');
            expect([200, 401, 403, 404]).toContain(response.status);
        });

        test('GET /atm/1/logs - get logs responds', async () => {
            const response = await request(app).get('/atm/1/logs');
            expect([200, 401, 403, 404]).toContain(response.status);
        });

        test('POST /atm/1/withdraw - withdraw responds', async () => {
            const response = await request(app).post('/atm/1/withdraw').send({});
            expect([200, 400, 401, 403, 404]).toContain(response.status);
        });

        test('POST /atm/1/credit/withdraw - credit withdraw responds', async () => {
            const response = await request(app).post('/atm/1/credit/withdraw').send({});
            expect([200, 400, 401, 403, 404]).toContain(response.status);
        });
    });

    describe('Users endpoints - admin only', () => {
        test('GET /users/testuser - get user responds', async () => {
            const response = await request(app).get('/users/testuser');
            expect([200, 401, 403, 404]).toContain(response.status);
        });

        test('POST /users - create user responds', async () => {
            const response = await request(app).post('/users').send({});
            expect([200, 400, 401, 403]).toContain(response.status);
        });

        test('PUT /users/testuser - update user responds', async () => {
            const response = await request(app).put('/users/testuser').send({});
            expect([200, 400, 401, 403, 404]).toContain(response.status);
        });

        test('DELETE /users/testuser - delete user responds', async () => {
            const response = await request(app).delete('/users/testuser');
            expect([200, 401, 403, 404]).toContain(response.status);
        });
    });

    describe('Cards endpoints - admin only', () => {
        test('GET /cards - get all cards responds', async () => {
            const response = await request(app).get('/cards');
            expect([200, 401, 403, 404]).toContain(response.status);
        });

        test('GET /cards/CARD123 - get card responds', async () => {
            const response = await request(app).get('/cards/CARD123');
            expect([200, 401, 403, 404]).toContain(response.status);
        });

        test('POST /cards - create card responds', async () => {
            const response = await request(app).post('/cards').send({});
            expect([200, 400, 401, 403]).toContain(response.status);
        });

        test('PUT /cards/CARD123/pin - update PIN responds', async () => {
            const response = await request(app).put('/cards/CARD123/pin').send({});
            expect([200, 400, 401, 403, 404]).toContain(response.status);
        });

        test('DELETE /cards/CARD123 - delete card responds', async () => {
            const response = await request(app).delete('/cards/CARD123');
            expect([200, 401, 403, 404]).toContain(response.status);
        });

        test('POST /cards/CARD123/lock - lock card responds', async () => {
            const response = await request(app).post('/cards/CARD123/lock');
            expect([200, 401, 403, 404]).toContain(response.status);
        });

        test('POST /cards/CARD123/unlock - unlock card responds', async () => {
            const response = await request(app).post('/cards/CARD123/unlock');
            expect([200, 401, 403, 404]).toContain(response.status);
        });
    });

    describe('Accounts endpoints - admin only', () => {
        test('GET /accounts/1 - get account responds', async () => {
            const response = await request(app).get('/accounts/1');
            expect([200, 401, 403, 404]).toContain(response.status);
        });

        test('POST /accounts - create account responds', async () => {
            const response = await request(app).post('/accounts').send({});
            expect([200, 400, 401, 403]).toContain(response.status);
        });

        test('PUT /accounts/1 - update account responds', async () => {
            const response = await request(app).put('/accounts/1').send({});
            expect([200, 400, 401, 403, 404]).toContain(response.status);
        });

        test('DELETE /accounts/1 - delete account responds', async () => {
            const response = await request(app).delete('/accounts/1');
            expect([200, 401, 403, 404]).toContain(response.status);
        });
    });

    describe('CardAccount endpoints - admin only', () => {
        test('GET /cardaccount/CARD123 - get card-account link responds', async () => {
            const response = await request(app).get('/cardaccount/CARD123');
            expect([200, 401, 403, 404]).toContain(response.status);
        });

        test('POST /cardaccount - create card-account link responds', async () => {
            const response = await request(app).post('/cardaccount').send({});
            expect([200, 400, 401, 403]).toContain(response.status);
        });

        test('PUT /cardaccount/CARD123 - update card-account link responds', async () => {
            const response = await request(app).put('/cardaccount/CARD123').send({});
            expect([200, 400, 401, 403, 404]).toContain(response.status);
        });

        test('DELETE /cardaccount/CARD123 - delete card-account link responds', async () => {
            const response = await request(app).delete('/cardaccount/CARD123');
            expect([200, 401, 403, 404]).toContain(response.status);
        });
    });

    describe('Log endpoints - admin only', () => {
        test('GET /log/1 - get logs by account responds', async () => {
            const response = await request(app).get('/log/1');
            expect([200, 401, 403, 404]).toContain(response.status);
        });
    });
});
