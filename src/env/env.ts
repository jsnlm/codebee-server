import * as dotenv from 'dotenv';

interface EnvironmentVars {
  NODE_ENV: string;
  DEBUG: string;
  MONGO_URI: string;
  PORT: string;

  DOCKER_HOST: string;
  DOCKER_MACHINE_NAME: string;
}

dotenv.config();

export default process.env as EnvironmentVars;
