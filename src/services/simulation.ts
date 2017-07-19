import logger from '../env/debug';
import DockerSandbox from './docker/dockerSandbox';
import { MatchModel, MatchState } from './../models/match';
import { File } from './fsWrapper';

const debug = logger('Simulation');

async function processResults(match: MatchModel, result: string) {
  debug("result : ", result);
  match.state = MatchState.Finished;
  //TODO: process the result
  match.save();
}

async function processError(match: MatchModel, error: string) {
  debug("error : ", error);
  match.state = MatchState.RuntimeError; //TODO change this to the correct MatchState
  //TODO: process the error
  match.save();
}

async function simulate(match: MatchModel) {
  try {
    let files:File [] = match.submissions.map(
      matchSubmission => ({
        name: matchSubmission.submission._id,
        contents: matchSubmission.submission.code,
      })
    );    
    let sandbox = new DockerSandbox();
    await sandbox.initialize(files);
    let result = await sandbox.run('a b c d e f g');
    processResults(match, result);
  } catch(error) {
    processError(match, error);
  }
}

export default simulate;
