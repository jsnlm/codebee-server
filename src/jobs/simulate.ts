import logger from '../env/debug';
import { Match, MatchModel, MatchState } from '../models/match';
import simulate from '../services/simulation';

const debug = logger('simulateJob');

async function findMatch() : Promise<MatchModel> {
  debug("finding match");
  return await Match.findOne({state: MatchState.NotRun});
}

async function runMatch() {
  let match:MatchModel = await findMatch();
  if (match) {
    debug("found a match");
    simulate(match);
  } else {
    debug("no match was found");
  }
}

export default runMatch;