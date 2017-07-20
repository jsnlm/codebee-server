import logger from '../env/debug';
import { DockerSandbox, dockerEnvVar} from './docker/dockerSandbox';
import { Match, MatchClass, MatchState } from './../models/match';
import { SubmissionModel } from './../models/submission';
import { File } from './fsWrapper';

const debug = logger('Simulation');



async function processResults(result: string) {
  debug("result : ", result);
  let matchClass = new MatchClass();
  matchClass.state = MatchState.Finished;
  //TODO: process the result
  await Match.create(matchClass);
}

async function processError(error: string) {
  debug("error : ", error);
  let matchClass = new MatchClass();
  matchClass.state = MatchState.Finished;
  matchClass.state = MatchState.RuntimeError; //TODO change this to the correct MatchState
  //TODO: process the error
  await Match.create(matchClass);
}

async function simulate(submissions: SubmissionModel[]) {
  try {
    let files:File [] = submissions.map(
      submission => ({
        name: submission.programName,
        contents: submission.code,
      })
    );
    let sandbox = new DockerSandbox();
    await sandbox.initialize(files);
    let envVars: dockerEnvVar[] = [];
    for(let i = 0; i < submissions.length; i++) {
      let envVar: dockerEnvVar = {
        name: `COMPILE_ARG${i}`,
        value: submissions[i].compileCommand,
      }
      envVars.push(envVar);
    }
    let execCmd = submissions.map(sub => sub.execCommand).join(' ');
    let execEnvVar: dockerEnvVar = {
      name: "EXEC_ARGS",
      value: execCmd
    }
    envVars.push(execEnvVar);
    debug('envVars:  ', envVars);
    let result = await sandbox.run(envVars);
    processResults(result);
  } catch(error) {
    processError(error);
  }
}

export default simulate;
