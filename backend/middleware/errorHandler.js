function errorHandler(err, req, res, next) {
  const status = err.statusCode || err.status || 500;

  // Tämä on kehitystilassa -> debuggaus helppoa. Lopullisessa versiossa stack piilotetaan. Lopullinen koodi alhaalla kommenttien sisällä.
  const response = {
    error: err.name || 'Error',
    message: err.message || 'Internal Server Error',
  };

  if (process.env.NODE_ENV !== 'production') {
    response.stack = err.stack;
  }

  res.status(status).json(response);
}

module.exports = { errorHandler };

// Lopullinen koodi
/*
function errorHandler(err, req, res, next) {
  const status = err.statusCode || 500;

  res.status(status).json({
    error: err.name || 'Error',
    message: err.message || 'Internal Server Error',
  });
}

module.exports = { errorHandler };
*/