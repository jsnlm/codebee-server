import * as http from 'http';

import env from './env/env';
import { initialize as initializeDb } from './env/db';
import jobs from './jobs';
import logger from './env/debug';
import simulate from './services/simulation';
import { seedSubmissions } from './test/submission.seed';

const debug = logger('server');

const seedProgram = `#include <time.h>
#include <cstdlib>
#include <iostream>
#include <fstream>

#include "bot.h"
#include "action.h"
#include "insect.h"

using namespace std;

fstream fs;

class RandomBot : public Bot {
public:
    RandomBot() : Bot("RandomBot") { }

protected:
    vector<Action> getMoves(int id, const Map& curMap) {
        vector<Action> moves;
        for (int y = 0; y < curMap.height; ++y) {
            for (int x = 0; x < curMap.width; ++x) {
                shared_ptr<Insect> insect = curMap.map[y][x]->getInsect();
                if (insect && insect->getBotId() == id) {
                    int move = rand() % 5;
                    moves.push_back(Action(x, y, move, move > 0 ? move - 1 : insect->getFace(), rand() % (insect->getCount() + 1)));
                }
            }
        }
        return moves;
    }
};

int main() {
    srand(time(0));

    fs.open("random_bot_log.txt", fstream::out | fstream::trunc);

    cout.sync_with_stdio(false);

    RandomBot randomBot = RandomBot();

    randomBot.run();

    fs.close();
}`;

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
  new Promise( async () => {
    let submissions = await seedSubmissions(2, undefined, {
      language: 'c++',
      code: seedProgram,
    });
    simulate(submissions);
  });
  // jobs();

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
