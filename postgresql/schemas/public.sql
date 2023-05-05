CREATE TABLE IF NOT EXISTS public.users (
	user_id SERIAL NOT NULL PRIMARY KEY,
    name TEXT NOT NULL,
    last_name TEXT NOT NULL,
    username TEXT NOT NULL,
    email TEXT,
    phone_number TEXT,
    password TEXT NOT NULL,
    profession TEXT,
    key_skills TEXT[],
    about TEXT,
    country TEXT,
    city TEXT,
    avatar TEXT,
    banner TEXT,
    created_at TIMESTAMP DEFAULT(now()),
    updated_at TIMESTAMP
);

CREATE TABLE IF NOT EXISTS public.projects (
	project_id SERIAL NOT NULL PRIMARY KEY,
	user_id INTEGER NOT NULL,
    name TEXT NOT NULL,
    short_description TEXT NOT NULL,
    full_description TEXT NOT NULL,
    tags TEXT[] NOT NULL,
    link TEXT,
    avatar TEXT,
    priority INTEGER NOT NULL,
    visibility BOOLEAN NOT NULL DEFAULT(false),
    views INTEGER NOT NULL DEFAULT(0),
    created_at TIMESTAMP NOT NULL DEFAULT(now()),
    updated_at TIMESTAMP
);

CREATE TABLE IF NOT EXISTS public.posts (
	post_id SERIAL NOT NULL PRIMARY KEY,
	user_id INTEGER NOT NULL,
	name TEXT NOT NULL,
	text TEXT NOT NULL,
	media TEXT[],
	tags TEXT[] NOT NULL,
	created_at TIMESTAMP NOT NULL DEFAULT(now()),
	updated_at TIMESTAMP
);

CREATE TABLE IF NOT EXISTS public.comments (
	comment_id SERIAL NOT NULL PRIMARY KEY,
	target_id INTEGER NOT NULL,
	user_id INTEGER NOT NULL,
	target_type TEXT NOT NULL,
	text TEXT NOT NULL,
	media TEXT[],
	created_at TIMESTAMP NOT NULL DEFAULT(now()),
	updated_at TIMESTAMP
);

CREATE TABLE IF NOT EXISTS public.socials (
	user_id INTEGER NOT NULL,
	name TEXT NOT NULL,
	url TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS public.likes (
	target_id INTEGER NOT NULL,
	user_id INTEGER NOT NULL,
	target_type TEXT NOT NULL,
	username TEXT NOT NULL,
	created_at TIMESTAMP NOT NULL DEFAULT(now())
);