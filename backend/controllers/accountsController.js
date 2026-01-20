// Tämä tiedosto on accountsController, täysin proseduuripohjainen versio

// Importit
const AppError = require('../middleware/AppError');
const pool = require('../db');

// Muuntaa tietokantarivien kentät API-muotoon
function mapAccountRow(row) {
  return {
    idAccount: row.idaccount,
    idUser: row.iduser,
    balance: Number(row.balance),
    creditLimit: Number(row.creditlimit),
  };
}

// Tämä on virhekääntäjä
function mapProcedureSignalToHttp(err) {
  const raw = err?.sqlMessage || err?.message || '';
  const msg = raw.toLowerCase();
  
  if (msg.includes('account not found')) return new AppError('Account not found', 404);
  if (msg.includes('user not found')) return new AppError('User not found', 404);
  
  if (
    msg.includes('cannot be null') ||
    msg.includes('must be 0 or greater') ||
    msg.includes('must be greater than') ||
    msg.includes('negative')
  ) {
    return new AppError(raw, 400);
  }
  
  if (msg.includes('cannot delete account')) {
    return new AppError('Account cannot be deleted because it has related data', 409);
  }
  
  if (err?.code === 'ER_SIGNAL_EXCEPTION') return new AppError(raw || 'Bad Request', 400);
  
  return null;
}

// getAccountById
// Tämä hakee tilin proseduurilla
async function getAccountById(req, res, next) {
  try {
    const idAccount = req.params.id;
    if (!idAccount) throw new AppError('id is required', 400);
    
    // Proseduurin haku
    const [resultSets] = await pool.execute('CALL sp_read_account_info(?)', [idAccount]);
    const rows = resultSets?.[0] ?? [];
    
    if (!rows.length) throw new AppError('Account not found', 404);
    
    res.status(200).json(mapAccountRow(rows[0]));
  } catch (err) {
    const mapped = mapProcedureSignalToHttp(err);
    if (mapped) return next(mapped);
    next(err);
  }
}

// createAccount
// Luo tilin proseduurilla
async function createAccount(req, res, next) {
  let conn;
  try {
    const { idUser, balance, creditLimit } = req.body ?? {};
    
    if (!idUser) throw new AppError('idUser is required', 400);
    if (balance === undefined || creditLimit === undefined) {
      throw new AppError('balance and creditLimit are required', 400);
    }
    
    conn = await pool.getConnection();
    
    // Proseduurin haku
    await conn.execute('CALL sp_add_account(?, ?, ?)', [idUser, balance, creditLimit]);
    
    const [idRows] = await conn.execute('SELECT LAST_INSERT_ID() AS idaccount');
    const idAccount = idRows?.[0]?.idaccount;
    
    res.status(201).json({
      idAccount,
      idUser,
      balance: Number(balance),
      creditLimit: Number(creditLimit),
    });
  } catch (err) {
    const mapped = mapProcedureSignalToHttp(err);
    if (mapped) return next(mapped);
    next(err);
  } finally {
    if (conn) conn.release();
  }
}

// updateAccountCreditLimit
// Päivittää tilin creditlimitin proseduurilla
async function updateAccountCreditLimit(req, res, next) {
  try {
    const idAccount = req.params.id;
    if (!idAccount) throw new AppError('id is required', 400);

    const { creditLimit } = req.body ?? {};
    if (creditLimit === undefined) throw new AppError('creditLimit is required', 400);

    // Proseduurin haku
    await pool.execute('CALL sp_update_creditlimit(?, ?)', [idAccount, creditLimit]);

    const [resultSets] = await pool.execute('CALL sp_read_account_info(?)', [idAccount]);
    const rows = resultSets?.[0] ?? [];
    if (!rows.length) throw new AppError('Account not found', 404);

    res.status(200).json(mapAccountRow(rows[0]));
  } catch (err) {
    const mapped = mapProcedureSignalToHttp(err);
    if (mapped) return next(mapped);
    next(err);
  }
}

// deleteAccount
// Poistaa tilin proseduurilla
async function deleteAccount(req, res, next) {
  try {
    const idAccount = req.params.id;
    if (!idAccount) throw new AppError('id is required', 400);

    // Proseduurin haku
    await pool.execute('CALL sp_delete_account(?)', [idAccount]);

    res.status(204).end();
  } catch (err) {
    const mapped = mapProcedureSignalToHttp(err);
    if (mapped) return next(mapped);

    if (err?.code === 'ER_ROW_IS_REFERENCED_2') {
      return next(new AppError('Account cannot be deleted because it has related data', 409));
    }

    next(err);
  }
}

// Export
module.exports = {
  getAccountById,
  createAccount,
  updateAccountCreditLimit,
  deleteAccount,
};
