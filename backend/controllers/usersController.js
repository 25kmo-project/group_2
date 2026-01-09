// Esimerkkiversio, jotta saan testattua virheitä
const AppError = require('../middleware/AppError');

// GET /users/:id  -> 200 tai 404
function getUserById(req, res, next) {
  try {
    const id = req.params.id;

    // 400 Bad Request (validointi)
    if (!id) {
      throw new AppError('id is required', 400);
    }

    // TODO: hae käyttäjä tietokannasta
    const user = null; // placeholder

    // 404 Not Found
    if (!user) {
      throw new AppError('User not found', 404);
    }

    // 200 OK
    res.status(200).json(user);
  } catch (err) {
    next(err);
  }
}

// POST /users  -> 201 tai 400 tai 409
function createUser(req, res, next) {
  try {
    const { name, email } = req.body;

    // 400 Bad Request (validointi)
    if (!name || !email) {
      throw new AppError('name and email are required', 400);
    }

    // TODO: tarkista duplikaatti esim. email
    const emailExists = false; // placeholder. Jos haluat testata 409 Conflict koodia, vaihda true

    // 409 Conflict (duplikaatti)
    if (emailExists) {
      throw new AppError('Email already exists', 409);
    }

    // TODO: luo käyttäjä tietokantaan
    const created = { id: 123, name, email };

    // 201 Created
    res.status(201).json(created);
  } catch (err) {
    next(err);
  }
}

// DELETE /users/:id -> 204 tai 404
function deleteUser(req, res, next) {
  try {
    const id = req.params.id;

    if (!id) {
      throw new AppError('id is required', 400);
    }

    // TODO: poista tietokannasta
    const deleted = true; // placeholder, jos haluaa testata 204 koodia, niin muuta true

    // 404 Not Found
    if (!deleted) {
      throw new AppError('User not found', 404);
    }

    // 204 No Content
    res.status(204).end();
  } catch (err) {
    next(err);
  }
}

module.exports = { getUserById, createUser, deleteUser };
