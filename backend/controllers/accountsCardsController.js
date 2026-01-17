/* Tämä tiedosto on accounts_cards -liitostaulun controller. Tämä toteuttaa linkityksen "kortti ↔ tili" CRUD-tyyppisesti -> luo linkin, listaa
linkit eri tavoilla ja poistaa linkin. Ei sisällä UPDATEa */

// Importit
const AppError = require('../middleware/AppError');
const pool = require('../db');

// Apufunktiot
function parseAccountId(value) {
  const id = Number(value);
  if (!Number.isInteger(id) || id <= 0) {
    throw new AppError('idAccount must be a positive integer', 400);
  }
  return id;
}

function requireNonEmptyString(value, fieldName) {
  if (typeof value !== 'string' || value.trim().length === 0) {
    throw new AppError(`${fieldName} is required (non-empty string)`, 400);
  }
}

function mapRow(row) {
  return {
    idAccount: row.idaccount,
    idCard: row.idcard,
  };
}

// createLink
// Validioi idAccount ja idCard
async function createLink(req, res, next) {
  try {
    const { idAccount, idCard } = req.body ?? {};
    const accountId = parseAccountId(idAccount);
    requireNonEmptyString(idCard, 'idCard');
    
    const [arows] = await pool.execute(
      // Tarkistaa, että account löytyy
      'SELECT idaccount FROM accounts WHERE idaccount = ?',
      [accountId]
    );
    if (arows.length === 0) throw new AppError('Account not found', 404);
    
    const [crows] = await pool.execute(
      // Tarkistaa, että card löytyy
      'SELECT idcard FROM cards WHERE idcard = ?',
      [idCard]
    );
    if (crows.length === 0) throw new AppError('Card not found', 404);
    
    await pool.execute(
      // Lisää linkin liitostauluun
      'INSERT INTO accounts_cards (idaccount, idcard) VALUES (?, ?)',
      [accountId, idCard]
    );
    
    res.status(201).json({ idAccount: accountId, idCard });
  } catch (err) {
    if (err.code === 'ER_DUP_ENTRY') {
      return next(new AppError('Link already exists', 409));
    }
    if (err.code === 'ER_NO_REFERENCED_ROW_2') {
      return next(new AppError('Account or card not found', 404));
    }
    next(err);
  }
}

// getAllLinks
// Listaa kaikki linkit
async function getAllLinks(req, res, next) {
  try {
    const [rows] = await pool.execute(
      'SELECT idaccount, idcard FROM accounts_cards ORDER BY idaccount, idcard'
    );
    res.status(200).json(rows.map(mapRow));
  } catch (err) {
    next(err);
  }
}

// getLinksByAccount
// Listaa kaikki kortit tietylle tilille
async function getLinksByAccount(req, res, next) {
  try {
    const idAccount = parseAccountId(req.params.idAccount);
    
    const [rows] = await pool.execute(
      'SELECT idaccount, idcard FROM accounts_cards WHERE idaccount = ? ORDER BY idcard',
      [idAccount]
    );
    
    res.status(200).json(rows.map(mapRow));
  } catch (err) {
    next(err);
  }
}

// getLinksByCard
// Listaa kaikki tilit tietylle kortille
async function getLinksByCard(req, res, next) {
  try {
    const idCard = req.params.idCard;
    requireNonEmptyString(idCard, 'idCard');
    
    const [rows] = await pool.execute(
      'SELECT idaccount, idcard FROM accounts_cards WHERE idcard = ? ORDER BY idaccount',
      [idCard]
    );
    
    res.status(200).json(rows.map(mapRow));
  } catch (err) {
    next(err);
  }
}

// deleteLink
// Poistaa linkin kortin ja tilin väliltä
async function deleteLink(req, res, next) {
  try {
    const { idAccount, idCard } = req.body ?? {};
    const accountId = parseAccountId(idAccount);
    requireNonEmptyString(idCard, 'idCard');
    
    const [result] = await pool.execute(
      'DELETE FROM accounts_cards WHERE idaccount = ? AND idcard = ?',
      [accountId, idCard]
    );
    
    if (result.affectedRows === 0) {
      throw new AppError('Link not found', 404);
    }
    
    res.status(204).end();
  } catch (err) {
    next(err);
  }
}

module.exports = {
  createLink,
  getAllLinks,
  getLinksByAccount,
  getLinksByCard,
  deleteLink,
};