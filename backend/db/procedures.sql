-- PROCEDURES

-- ========================================
-- TIETOKANNAN HALLINTA (Database Management)
-- ========================================

-- CREATE USER
-- example CALL sp_create_user(iduser, fname, lname,streetaddress);

DELIMITER $$

CREATE PROCEDURE sp_create_user(
  IN p_iduser VARCHAR(45),
  IN p_fname VARCHAR(45),
  IN p_lname VARCHAR(45),
  IN p_streetaddress VARCHAR(45)
)
BEGIN
  DECLARE v_exists INT;

  START TRANSACTION;

  -- Check if user already exists with lock
  SELECT COUNT(*) INTO v_exists 
  FROM users WHERE iduser = p_iduser
  FOR UPDATE;

  IF v_exists > 0 THEN
    ROLLBACK;
    SIGNAL SQLSTATE '45000'
      SET MESSAGE_TEXT = 'User already exists';
  END IF;

  -- Insert new user
  INSERT INTO users (iduser, fname, lname, streetaddress)
  VALUES (p_iduser, p_fname, p_lname, p_streetaddress);

  COMMIT;
END$$
DELIMITER ;

-- DELETE USER
-- example CALL sp_delete_user(iduser);

DELIMITER $$

CREATE PROCEDURE sp_delete_user(
  IN p_iduser VARCHAR(45)
)
BEGIN
  DECLARE v_user_count INT;
  DECLARE v_account_count INT;
  DECLARE v_card_count INT;

  START TRANSACTION;

  -- Check if user exists
  SELECT COUNT(*) INTO v_user_count
  FROM users
  WHERE iduser = p_iduser
  FOR UPDATE;

  IF v_user_count = 0 THEN
    ROLLBACK;
    SIGNAL SQLSTATE '45000'
      SET MESSAGE_TEXT = 'User not found';
  END IF;

  -- Check if user has accounts
  SELECT COUNT(*) INTO v_account_count
  FROM accounts
  WHERE iduser = p_iduser;

  IF v_account_count > 0 THEN
    ROLLBACK;
    SIGNAL SQLSTATE '45000'
      SET MESSAGE_TEXT = 'Cannot delete user with existing accounts. Delete accounts first.';
  END IF;

  -- Check if user has cards
  SELECT COUNT(*) INTO v_card_count
  FROM cards
  WHERE iduser = p_iduser;

  IF v_card_count > 0 THEN
    ROLLBACK;
    SIGNAL SQLSTATE '45000'
      SET MESSAGE_TEXT = 'Cannot delete user with existing cards. Delete cards first.';
  END IF;

  -- Delete user
  DELETE FROM users
  WHERE iduser = p_iduser;

  COMMIT;
END $$
DELIMITER ;

-- READ USER INFO
-- example CALL sp_read_user_info(iduser);

DELIMITER $$

CREATE PROCEDURE sp_read_user_info(
  IN p_iduser VARCHAR(45)
)
BEGIN 
  DECLARE v_user_count INT;

  -- Validate parameter
  IF p_iduser IS NULL THEN
    SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'User ID cannot be NULL';
  END IF;

  -- Check if user exists
  SELECT COUNT(*) INTO v_user_count
  FROM users
  WHERE iduser = p_iduser;

  IF v_user_count = 0 THEN
    SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'User not found';
  END IF;

  -- Retrieve user info
  SELECT iduser, fname, lname, streetaddress
  FROM users
  WHERE iduser = p_iduser;
END$$
DELIMITER ;

-- UPDATE USER INFO
-- example CALL sp_update_user_info(iduser, fname, lname, streetaddress);

DELIMITER $$

CREATE PROCEDURE sp_update_user_info(
  IN p_iduser VARCHAR(45),
  IN p_fname VARCHAR(45),
  IN p_lname VARCHAR(45),
  IN p_streetaddress VARCHAR(45)
)
BEGIN
  DECLARE v_user_count INT;

  -- Validate parameters
  IF p_iduser IS NULL THEN
    SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'User ID cannot be NULL';
  END IF;

  IF p_fname IS NULL OR TRIM(p_fname) = '' THEN
    SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'First name cannot be NULL or empty';
  END IF;

  IF p_lname IS NULL OR TRIM(p_lname) = '' THEN
    SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Last name cannot be NULL or empty';
  END IF;

  IF p_streetaddress IS NULL OR TRIM(p_streetaddress) = '' THEN
    SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Street address cannot be NULL or empty';
  END IF;

  START TRANSACTION;

  -- Check if user exists with lock
  SELECT COUNT(*) INTO v_user_count
  FROM users
  WHERE iduser = p_iduser
  FOR UPDATE;

  IF v_user_count = 0 THEN
    ROLLBACK;
    SIGNAL SQLSTATE '45000'
      SET MESSAGE_TEXT = 'User not found';
  END IF;

  -- Update user information
  UPDATE users
  SET fname = p_fname,
      lname = p_lname,
      streetaddress = p_streetaddress
  WHERE iduser = p_iduser;

  COMMIT;
END$$
DELIMITER ;

-- ADD ACCOUNT
-- example CALL sp_add_account(iduser, balance, creditlimit);

DELIMITER $$
CREATE PROCEDURE sp_add_account(
  IN p_iduser VARCHAR(45),
  IN p_balance DECIMAL(10,2),
  IN p_creditlimit DECIMAL(10,2)
)
BEGIN
  DECLARE v_user_count INT;

  START TRANSACTION;

  -- Check if user exists
  SELECT COUNT(*) INTO v_user_count
  FROM users
  WHERE iduser = p_iduser
  FOR UPDATE;

  -- Validate balance and credit limit
  IF p_balance IS NULL OR p_balance < 0 THEN
  SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Balance cannot be NULL or negative';
  END IF;

  IF p_creditlimit IS NULL OR p_creditlimit < 0 THEN
  SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Credit limit must be 0 or greater';
  END IF;

  IF v_user_count = 0 THEN
    ROLLBACK;
    SIGNAL SQLSTATE '45000'
      SET MESSAGE_TEXT = 'User not found';
  END IF;

  -- Insert new account
  INSERT INTO accounts (iduser, balance, creditlimit)
  VALUES (p_iduser, p_balance, p_creditlimit);

  COMMIT;
END$$
DELIMITER ;

-- DELETE ACCOUNT
-- example CALL sp_delete_account(idaccount);

DELIMITER $$

CREATE PROCEDURE sp_delete_account(
  IN p_idaccount INT
)
BEGIN
  DECLARE v_account_count INT;
  DECLARE v_card_count INT;

  START TRANSACTION; 

  -- Check if account exists
  SELECT COUNT(*) INTO v_account_count
  FROM accounts
  WHERE idaccount = p_idaccount
  FOR UPDATE;

  IF v_account_count = 0 THEN
    ROLLBACK;
    SIGNAL SQLSTATE '45000'
      SET MESSAGE_TEXT = 'Account not found';
  END IF;

  -- Check if account has cards
  SELECT COUNT(*) INTO v_card_count
  FROM accounts_cards
  WHERE idaccount = p_idaccount;

  IF v_card_count > 0 THEN
    ROLLBACK;
    SIGNAL SQLSTATE '45000'
      SET MESSAGE_TEXT = 'Cannot delete account with existing cards. Delete cards first.';
  END IF;

  -- Delete log entries first
  DELETE FROM log
  WHERE idaccount = p_idaccount;

  -- Remove any lingering links
  DELETE FROM accounts_cards
  WHERE idaccount = p_idaccount;
  
  -- Delete account
  DELETE FROM accounts
  WHERE idaccount = p_idaccount;

  COMMIT;
END$$
DELIMITER ;

-- READ ACCOUNT INFO
-- example CALL sp_read_account_info(idaccount);

DELIMITER $$

CREATE PROCEDURE sp_read_account_info(
  IN p_idaccount INT
)
BEGIN
  DECLARE v_account_count INT;

  -- Validate parameter
  IF p_idaccount IS NULL THEN
    SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Account ID cannot be NULL';
  END IF;

  -- Check if account exists
  SELECT COUNT(*) INTO v_account_count
  FROM accounts
  WHERE idaccount = p_idaccount;

  IF v_account_count = 0 THEN
    SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Account not found';
  END IF;

  -- Retrieve account info
  SELECT idaccount, iduser, balance, creditlimit
  FROM accounts
  WHERE idaccount = p_idaccount;
END$$
DELIMITER ;

-- ========================================
-- KORTTIEN HALLINTA (Card Management)
-- ========================================

-- GET CARD INFO
-- example CALL sp_get_card_info(idcard);

DELIMITER $$

CREATE PROCEDURE sp_get_card_info(
  IN p_idcard VARCHAR(45)
)

BEGIN
  DECLARE v_card_count INT;

  -- Validate parameter
  IF p_idcard IS NULL THEN
    SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Card ID cannot be NULL';
  END IF;

  -- Check if card exists
  SELECT COUNT(*) INTO v_card_count
  FROM cards
  WHERE idcard = p_idcard;

  IF v_card_count = 0 THEN
    SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Card not found';
  END IF;

  -- Retrieve card's linked accounts from accounts_cards table
  SELECT idcard, idaccount
  FROM accounts_cards
  WHERE idcard = p_idcard;

END$$
DELIMITER ;

--ADD CARD TO ACCOUNT
-- example CALL sp_card_to_account(idcard, idaccount);

DELIMITER $$
CREATE PROCEDURE sp_card_to_account(
  IN p_idcard VARCHAR(45),
  IN p_idaccount INT
)
BEGIN
  DECLARE v_card_count INT;
  DECLARE v_account_count INT;
  DECLARE v_link_exists INT;
  DECLARE v_is_locked TINYINT;

  -- Validate parameters
  IF p_idcard IS NULL OR p_idaccount IS NULL THEN
    SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Card ID and Account ID cannot be NULL';
  END IF;

  START TRANSACTION;

  -- Check if card exists and get lock status
  SELECT is_locked
    INTO v_is_locked
  FROM cards
  WHERE idcard = p_idcard
  FOR UPDATE;

  IF ROW_COUNT() = 0 THEN
    ROLLBACK;
    SIGNAL SQLSTATE '45000'
      SET MESSAGE_TEXT = 'Card not found';
  END IF;

  -- Check if card is locked
  IF v_is_locked = 1 THEN
    ROLLBACK;
    SIGNAL SQLSTATE '45000'
      SET MESSAGE_TEXT = 'Cannot link a locked card to an account';
  END IF;

  -- Check if account exists
  SELECT COUNT(*) INTO v_account_count
  FROM accounts
  WHERE idaccount = p_idaccount
  FOR UPDATE;

  IF v_account_count = 0 THEN
    ROLLBACK;
    SIGNAL SQLSTATE '45000'
      SET MESSAGE_TEXT = 'Account not found';
  END IF;
  -- Check if link already exists
  SELECT COUNT(*) INTO v_link_exists
  FROM accounts_cards
  WHERE idcard = p_idcard AND idaccount = p_idaccount;
  IF v_link_exists > 0 THEN
    ROLLBACK;
    SIGNAL SQLSTATE '45000'
      SET MESSAGE_TEXT = 'Card is already linked to this account';
  END IF;
  
  -- Link card to account
  INSERT INTO accounts_cards (idcard, idaccount)
  VALUES (p_idcard, p_idaccount);
  
  COMMIT;
END$$
DELIMITER ;

--REMOVE CARD FROM ACCOUNT
-- example CALL sp_remove_card_from_account(idcard, idaccount);

DELIMITER $$
CREATE PROCEDURE sp_remove_card_from_account(
  IN p_idcard VARCHAR(45),
  IN p_idaccount INT
)


BEGIN
  DECLARE v_link_exists INT;

  START TRANSACTION;

  -- Check if link exists
  SELECT COUNT(*) INTO v_link_exists
  FROM accounts_cards
  WHERE idcard = p_idcard AND idaccount = p_idaccount
  FOR UPDATE;

  IF v_link_exists = 0 THEN
    ROLLBACK;
    SIGNAL SQLSTATE '45000'
      SET MESSAGE_TEXT = 'Link between card and account not found';
  END IF;

  -- Remove link
  DELETE FROM accounts_cards
  WHERE idcard = p_idcard AND idaccount = p_idaccount;

  COMMIT;
END$$
DELIMITER ;

  START TRANSACTION;

  -- Check if link exists
  SELECT COUNT(*) INTO v_link_exists
  FROM accounts_cards
  WHERE idcard = p_idcard AND idaccount = p_idaccount
  FOR UPDATE;

  IF v_link_exists = 0 THEN
    ROLLBACK;
    SIGNAL SQLSTATE '45000'
      SET MESSAGE_TEXT = 'Link between card and account not found';
  END IF;

  -- Remove link
  DELETE FROM accounts_cards
  WHERE idcard = p_idcard AND idaccount = p_idaccount;

  COMMIT;
END$$
DELIMITER ;

-- UPDATE CARD LINKED ACCOUNT
-- example CALL sp_update_card_linked_account(idcard, old_idaccount, new_idaccount);

DELIMITER $$
CREATE PROCEDURE sp_update_card_linked_account( 
  IN p_idcard VARCHAR(45),
  IN p_old_idaccount INT,
  IN p_new_idaccount INT
)
BEGIN
  DECLARE v_link_exists INT;
  DECLARE v_new_account_count INT;
  DECLARE v_old_creditlimit DECIMAL(10,2);
  DECLARE v_new_creditlimit DECIMAL(10,2);
  
  START TRANSACTION;
  -- Check if existing link exists
  SELECT COUNT(*) INTO v_link_exists
  FROM accounts_cards
  WHERE idcard = p_idcard AND idaccount = p_old_idaccount
  FOR UPDATE;
  IF v_link_exists = 0 THEN
    ROLLBACK;
    SIGNAL SQLSTATE '45000'
      SET MESSAGE_TEXT = 'Existing link between card and old account not found';
  END IF;
  
  -- Get old account credit limit
  SELECT creditlimit INTO v_old_creditlimit
  FROM accounts
  WHERE idaccount = p_old_idaccount
  FOR UPDATE;
  
  -- Check if new account exists and get credit limit
  SELECT creditlimit INTO v_new_creditlimit
  FROM accounts
  WHERE idaccount = p_new_idaccount
  FOR UPDATE;
  
  IF ROW_COUNT() = 0 THEN
    ROLLBACK;
    SIGNAL SQLSTATE '45000'
      SET MESSAGE_TEXT = 'New account not found';
  END IF;
  
  -- Check if both accounts are same type (both credit or both debit)
  IF (v_old_creditlimit > 0 AND v_new_creditlimit = 0) OR (v_old_creditlimit = 0 AND v_new_creditlimit > 0) THEN
    ROLLBACK;
    SIGNAL SQLSTATE '45000'
      SET MESSAGE_TEXT = 'Cannot change card between credit and debit accounts';
  END IF;
  
  -- Update link to new account
  UPDATE accounts_cards
  SET idaccount = p_new_idaccount
  WHERE idcard = p_idcard AND idaccount = p_old_idaccount;
  COMMIT;
END$$
DELIMITER ;

-- DELETE CARD
-- example CALL sp_delete_card(idcard);

DELIMITER $$

CREATE PROCEDURE sp_delete_card(
  IN p_idcard VARCHAR(45)
)
BEGIN
  DECLARE v_card_count INT;

  START TRANSACTION;

  -- Check if card exists
  SELECT COUNT(*) INTO v_card_count
  FROM cards
  WHERE idcard = p_idcard
  FOR UPDATE;

  IF v_card_count = 0 THEN
    ROLLBACK;
    SIGNAL SQLSTATE '45000'
      SET MESSAGE_TEXT = 'Card not found';
  END IF;

  -- Remove links first to avoid FK errors
  DELETE FROM accounts_cards
  WHERE idcard = p_idcard;
  
  -- Delete card
  DELETE FROM cards
  WHERE idcard = p_idcard;
  COMMIT;
END$$
DELIMITER ;

-- CARD LOCK
-- example CALL sp_card_lock(idcard);

DELIMITER $$

CREATE PROCEDURE sp_card_lock(
  IN p_idcard VARCHAR(45)
)
BEGIN

  START TRANSACTION;

  -- Locks card 
  UPDATE cards
  SET is_locked = 1
  WHERE idcard = p_idcard;

  IF ROW_COUNT() = 0 THEN
    ROLLBACK;
    SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Card not found';
  END IF;

  COMMIT;
END$$ 
DELIMITER ;

-- CARD UNLOCK
-- example CALL sp_card_unlock(idcard);

DELIMITER $$

CREATE PROCEDURE sp_card_unlock(
  IN p_idcard VARCHAR(45)
)
BEGIN

  START TRANSACTION;

  -- unlock card 
  UPDATE cards
  SET is_locked = 0
  WHERE idcard = p_idcard;

  IF ROW_COUNT() = 0 THEN
    ROLLBACK;
    SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Card not found';
  END IF;

  COMMIT;
END$$

DELIMITER ;

-- ========================================
-- TILIN TRANSAKTIOT (Account Transactions)
-- ========================================

-- DEPOSIT
-- example CALL sp_deposit(idaccount, amount);

DELIMITER $$

CREATE PROCEDURE sp_deposit(
  IN p_idaccount INT,
  IN p_amount DECIMAL(10,2)
)
BEGIN
  DECLARE v_balance DECIMAL(10,2);

  -- checks that added amount is not 0 or negative
  IF p_amount IS NULL OR p_amount <= 0 THEN
    SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Amount must be greater than 0';
  END IF;
  
  START TRANSACTION;
  
  -- Locks rows 
  SELECT balance
    INTO v_balance
    FROM accounts
  WHERE idaccount = p_idaccount
  FOR UPDATE;  
  
  IF ROW_COUNT() = 0 THEN
    ROLLBACK;
    SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Account not found';
  END IF;
  
  -- Update balance
  UPDATE accounts
    SET balance = v_balance + p_amount
  WHERE idaccount = p_idaccount;
  
  -- Log transaction
  INSERT INTO log (idaccount, time, balancechange)
  VALUES (p_idaccount, NOW(), p_amount);
  
  COMMIT;
END$$

DELIMITER ;

-- WITHDRAW
-- example CALL sp_withdraw(idaccount, amount);

DELIMITER $$

CREATE PROCEDURE sp_withdraw(
  IN p_idaccount INT,
  IN p_amount DECIMAL(10,2)
)
BEGIN
  DECLARE v_balance DECIMAL(10,2);

  -- checks that added amount is not 0 or negative
  IF p_amount IS NULL OR p_amount <= 0 THEN
    SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Amount must be greater than 0';
  END IF;
  
  START TRANSACTION;
  
  -- Locks rows 
  SELECT balance
    INTO v_balance
    FROM accounts
  WHERE idaccount = p_idaccount
  FOR UPDATE;  
  
  IF ROW_COUNT() = 0 THEN
    ROLLBACK;
    SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Account not found';
  END IF;

  -- Checks balance 
  IF v_balance < p_amount THEN
    ROLLBACK;
    SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Insufficient balance';
  END IF; 
  
  -- Update balance
  UPDATE accounts
    SET balance = v_balance - p_amount
  WHERE idaccount = p_idaccount;
  
  -- Log transaction
  INSERT INTO log (idaccount, time, balancechange)
  VALUES (p_idaccount, NOW(), -p_amount);
  
  COMMIT;
END$$

DELIMITER ;

-- TRANSFER
-- example CALL sp_transfer(idaccount_from, idaccount_to, amount);

DELIMITER $$

CREATE PROCEDURE sp_transfer(
  IN p_idaccount_from INT,
  IN p_idaccount_to INT,
  IN p_amount DECIMAL(10,2)
)
BEGIN
  DECLARE v_balance_from DECIMAL(10,2);
  DECLARE v_balance_to DECIMAL(10,2);

  -- checks that added amount is not 0 or negative
  IF p_amount IS NULL OR p_amount <= 0 THEN
    SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Amount must be greater than 0';
  END IF;

  START TRANSACTION;

  -- Locks rows
  SELECT balance
    INTO v_balance_from
    FROM accounts
  WHERE idaccount = p_idaccount_from
  FOR UPDATE;
  
  -- Check if source account exists
  IF ROW_COUNT() = 0 THEN
    ROLLBACK;
    SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Source account not found';
  END IF;

  SELECT balance
    INTO v_balance_to
    FROM accounts
  WHERE idaccount = p_idaccount_to
  FOR UPDATE;

  -- Check if destination account exists
  IF ROW_COUNT() = 0 THEN
    ROLLBACK;
    SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Destination account not found';
  END IF;

  -- Prevents transfer to same account
  IF p_idaccount_from = p_idaccount_to THEN
  SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Cannot transfer to the same account';
  END IF;

  -- Checks balance
  IF v_balance_from < p_amount THEN
    ROLLBACK;
    SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Insufficient balance in source account';
  END IF; 

  -- Update balances
  UPDATE accounts
    SET balance = v_balance_from - p_amount
  WHERE idaccount = p_idaccount_from;

  UPDATE accounts
    SET balance = v_balance_to + p_amount
  WHERE idaccount = p_idaccount_to;

  -- Log transactions
  INSERT INTO log (idaccount, time, balancechange)
  VALUES (p_idaccount_from, NOW(), -p_amount);

  INSERT INTO log (idaccount, time, balancechange)
  VALUES (p_idaccount_to, NOW(), p_amount);
  COMMIT;
END$$
DELIMITER ;

-- ========================================
-- LUOTTOTILI (Credit Account)
-- ========================================

-- CREDIT WITHDRAW
-- example CALL sp_credit_withdraw(idaccount, amount);
DELIMITER $$

CREATE PROCEDURE sp_credit_withdraw (
  IN p_idaccount INT,
  IN p_amount DECIMAL(10,2)
)
BEGIN
  DECLARE v_creditlimit DECIMAL(10,2);
  DECLARE v_balance DECIMAL(10,2);
  
  -- checks that added amount is not 0 or negative
  IF p_amount IS NULL OR p_amount <= 0 THEN
    SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Amount must be greater than 0';
  END IF;
  
  START TRANSACTION;
  
  -- Locks rows 
  SELECT creditlimit, balance
    INTO v_creditlimit, v_balance
    FROM accounts
  WHERE idaccount = p_idaccount
  FOR UPDATE;
  
  IF ROW_COUNT() = 0 THEN
    ROLLBACK;
    SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Account not found';
  END IF;
  
  -- Checks credit
  IF (v_creditlimit - v_balance) < p_amount THEN
    ROLLBACK;
    SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Insufficient available credit';
  END IF;
  
  -- Update balance
  UPDATE accounts
    set balance = v_balance + p_amount
  WHERE idaccount = p_idaccount;
  
  -- Log transaction
  INSERT INTO log (idaccount, time, balancechange)
  VALUES (p_idaccount, NOW(), -p_amount);
  
  COMMIT;
END $$

DELIMITER ;

-- CREDIT REPAY
-- example CALL sp_credit_repay(idaccount, amount);

DELIMITER $$

CREATE PROCEDURE sp_credit_repay (
  IN p_idaccount INT,
  IN p_amount DECIMAL(10,2)
)
BEGIN
  DECLARE v_creditlimit DECIMAL(10,2);
  DECLARE v_balance DECIMAL(10,2);
  
  -- checks that added amount is not 0 or negative
  IF p_amount IS NULL OR p_amount <= 0 THEN
    SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Amount must be greater than 0';
  END IF;
  
  START TRANSACTION;
  
  -- Locks rows 
  SELECT creditlimit, balance
    INTO v_creditlimit, v_balance
    FROM accounts
  WHERE idaccount = p_idaccount
  FOR UPDATE;
  
  IF ROW_COUNT() = 0 THEN
    ROLLBACK;
    SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Account not found';
  END IF;
  
  -- Checks used credit amount
  IF v_balance < p_amount THEN
    ROLLBACK;
    SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Tried to pay too much';
  END IF;
  
  -- Update balance
  UPDATE accounts
    SET balance = v_balance - p_amount
  WHERE idaccount = p_idaccount;
  
  -- Log transaction
  INSERT INTO log (idaccount, time, balancechange)
  VALUES (p_idaccount, NOW(), p_amount);
  
  COMMIT;
END$$

DELIMITER ;

-- UPDATE CREDITLIMIT
-- example CALL sp_update_creditlimit(idaccount, creditlimit);

DELIMITER $$
CREATE PROCEDURE sp_update_creditlimit(
  IN p_idaccount INT,
  IN p_creditlimit DECIMAL(10,2)
)
BEGIN
  DECLARE v_account_count INT;
  
  IF p_creditlimit IS NULL OR p_creditlimit < 0 THEN
    SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Credit limit must be 0 or greater';
  END IF;

  START TRANSACTION;
  
  SELECT COUNT(*) INTO v_account_count
  FROM accounts
  WHERE idaccount = p_idaccount
  FOR UPDATE;
  
  IF v_account_count = 0 THEN
    ROLLBACK;
    SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Account not found';
  END IF;
  
  UPDATE accounts
  SET creditlimit = p_creditlimit
  WHERE idaccount = p_idaccount;
  
  COMMIT;
END$$
DELIMITER ;

-- END PROCEDURES