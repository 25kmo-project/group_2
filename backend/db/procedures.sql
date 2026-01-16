-- PROCEDURES

-- CREDIT WHITDRAW
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

-- WHITDRAW
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

  IF ROW_COUNT() = 0 THEN
    ROLLBACK;
    SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Source account not found';
  END IF;

  SELECT balance
    INTO v_balance_to
    FROM accounts
  WHERE idaccount = p_idaccount_to
  FOR UPDATE;

  IF ROW_COUNT() = 0 THEN
    ROLLBACK;
    SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Destination account not found';
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
  DECLARE v_count INT;

  START TRANSACTION;

  -- Check if user exists
  SELECT COUNT(*) INTO v_count
  FROM users
  WHERE iduser = p_iduser
  FOR UPDATE;

  IF v_count = 0 THEN
    ROLLBACK;
    SIGNAL SQLSTATE '45000'
      SET MESSAGE_TEXT = 'User not found';
  END IF;

  -- Delete user
  DELETE FROM users
  WHERE iduser = p_iduser;

  COMMIT;
END$$
DELIMITER ;


-- END PROCEDURES