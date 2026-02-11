# Database – Stored Procedures

This directory contains all stored procedures used by the Bank ATM backend.

The backend communicates with the database **only through stored procedures**.

## Stored procedures

Below is the complete list of stored procedures defined in `procedures.sql`.

They are presented in CALL format (as used by the backend).

## Users

### Create user

**Signature**
```sql
CALL sp_create_user(iduser, fname, lname, streetaddress, role);
```
Creates a new user record. Includes an existence check and runs inside a transaction.

### Delete user

**Signature**
```sql
CALL sp_delete_user(iduser);
```
Deletes a user by id (and applies the procedure’s internal safety rules).

### Read user info

**Signature**
```sql
CALL sp_read_user_info(iduser);
```
Returns user details by id.

### Read all users

**Signature**
```sql
CALL sp_read_all_users();
```
Returns all users.

### Update user info

**Signature**
```sql
CALL sp_update_user_info(iduser, fname, lname, streetaddress);
```
Updates user fields by id.

## Accounts

### Add account

**Signature**
```sql
CALL sp_add_account(iduser, balance, creditlimit);
```
Creates a new account for a user, validating that the user exists and that numeric inputs are valid.

### Delete account

**Signature**
```sql
CALL sp_delete_account(idaccount);
```
Deletes an account by id (only if allowed by internal checks, e.g., card links).

### Read account info

**Signature**
```sql
CALL sp_read_account_info(idaccount);
```
Returns account details by id.

### Read all accounts

**Signature**
```sql
CALL sp_read_all_accounts();
```
Returns all accounts.

## Cards

### Create card

**Signature**
```sql
CALL sp_create_card(idcard, iduser, cardPIN);
```
Creates a new card for a user. PIN is expected to be provided as a hash from the backend.

### Read card info

**Signature**
```sql
CALL sp_read_card_info(idcard);
```
Returns card details by card id.

### Read all cards

**Signature**
```sql
CALL sp_read_all_cards();
```
Returns all cards.

### Update card PIN

**Signature**
```sql
CALL sp_update_card_pin(idcard, newPIN);
```
Updates a card’s PIN (expects hashed PIN from backend).

### Get card info

**Signature**
```sql
CALL sp_get_card_info(idcard);
```
Returns card information (including user linkage) by card id.

## Card ↔ Account Links

### Link card to account

**Signature**
```sql
CALL sp_card_to_account(idcard, idaccount);
```
Links a card to an account (creates a card–account relation).

### Remove card from account

**Signature**
```sql
CALL sp_remove_card_from_account(idcard, idaccount);
```
Removes a card–account relation.

### Update linked account

**Signature**
```sql
CALL sp_update_card_linked_account(idcard, old_account, new_account);
```
Updates an existing card–account link from one account id to another.

## Card Lifecycle

### Delete card

**Signature**
```sql
CALL sp_delete_card(idcard);
```
Deletes a card by id (subject to internal checks).

### Lock card

**Signature**
```sql
CALL sp_card_lock(idcard);
```
Locks a card (sets locked flag).

### Unlock card

**Signature**
```sql
CALL sp_card_unlock(idcard);
```
Unlocks a card (clears locked flag).

## ATM Operations

### Deposit

**Signature**
```sql
CALL sp_deposit(idaccount, amount);
```
Deposits funds to an account and records the change (per internal rules).

### Withdraw (debit)

**Signature**
```sql
CALL sp_withdraw(idaccount, amount);
```
Withdraws funds from an account and records the change (per internal rules).

### Transfer

**Signature**
```sql
CALL sp_transfer(idaccount_from, idaccount_to, amount);
```
Transfers funds between two accounts and records the change (per internal rules).

### Withdraw (credit)

**Signature**
```sql
CALL sp_credit_withdraw(idaccount, amount);
```
Performs a withdrawal on a credit account (uses credit limit logic per internal rules).

### Repay credit

**Signature**
```sql
CALL sp_credit_repay(idaccount, amount);
```
Repays credit balance on an account (per internal rules).

### Update credit limit

**Signature**
```sql
CALL sp_update_creditlimit(idaccount, creditlimit);
```
Updates an account’s credit limit.

## Logs

### Read account logs

**Signature**
```sql
CALL sp_read_account_logs(idaccount);
```
Returns an account’s log rows ordered by newest first.

## Notes

- All business logic is implemented inside stored procedures.
- Backend controllers call these procedures directly.
- ATM operations automatically generate log entries.
- Database must be initialized before running the backend.