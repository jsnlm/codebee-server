import * as dotenv from 'dotenv';

interface EnvironmentVars {
  NODE_ENV: string;
  DEBUG: string;
  MONGO_URI: string;
  MONGO_USER: string;
  MONGO_PASSWORD: string;
  PORT: string;
}

dotenv.config();

export default process.env as EnvironmentVars;
