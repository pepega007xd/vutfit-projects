import { connect } from 'ts-postgres';


const db = await connect({
  user: "postgres",
  database: "fitstagram",
  password: "postgres" // the db is running on localhost anyway
});

export default db;
