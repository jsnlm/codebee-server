import * as http from 'http';

import env from './env/env';
import { initialize as initializeDb } from './env/db';
import jobs from './jobs';
import logger from './env/debug';

import { seedMatches } from './test/match.seed';

const debug = logger('server');

function startServer() {
  const app = require('./app').default;

  /**
  * Get port from environment and store in Express.
  */

  const port = normalizePort(env.PORT);
  app.set('port', port);

  /**
  * Create HTTP server.
  */

  const server = http.createServer(app);

  /**
  * Listen on provided port, on all network interfaces.
  */

  server.listen(port, '0.0.0.0' as any);
  server.on('error', onError);
  server.on('listening', onListening);

  /**
  * Start Jobs
  */
  jobs();

  /**
  * Normalize a port into a number, string, or false.
  */

  function normalizePort(val: string) {
    var port = parseInt(val, 10);

    if (isNaN(port)) {
      // named pipe
      return val;
    }

    if (port >= 0) {
      // port number
      return port;
    }

    return false;
  }

  /**
  * Event listener for HTTP server "error" event.
  */

  function onError(error: any) {
    if (error.syscall !== 'listen') {
      throw error;
    }

    var bind = typeof port === 'string'
      ? 'Pipe ' + port
      : 'Port ' + port;

    // handle specific listen errors with friendly messages
    switch (error.code) {
      case 'EACCES':
        console.error(bind + ' requires elevated privileges');
        process.exit(1);
        break;
      case 'EADDRINUSE':
        console.error(bind + ' is already in use');
        process.exit(1);
        break;
      default:
        throw error;
    }
  }

  /**
  * Event listener for HTTP server "listening" event.
  */

  function onListening() {
    var addr = server.address();
    var bind = typeof addr === 'string'
      ? 'pipe ' + addr
      : 'port ' + addr.port;
    debug('Listening on ' + bind);
  }
}

function setup() {
  require('source-map-support').install();

  // Catch all unhandled promise rejections and log error
  process.on('unhandledRejection', (error: Error) => {
    console.error(error);
  });
}

(async function() {
  setup();
  await initializeDb(true);
  startServer();
})();
