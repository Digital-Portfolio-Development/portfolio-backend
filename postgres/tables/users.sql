CREATE TABLE IF NOT EXISTS users (
	user_id SERIAL NOT NULL PRIMARY KEY,
	token TEXT,
	name TEXT NOT NULL,
	last_name TEXT NOT NULL,
	username TEXT NOT NULL,
	email TEXT NOT NULL,
	password TEXT NOT NULL,
	about TEXT,
	avatar TEXT,
	banner TEXT,
	friends TEXT[],
	subscribers TEXT[],
	socials TEXT[]
);