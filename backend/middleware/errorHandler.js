function errorHandler(err, req, res, next) {
  const status = err.statusCode || err.status || 500;

  // Älä vuoda stackia tuotantoon
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
