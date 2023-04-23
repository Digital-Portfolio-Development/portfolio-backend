CREATE TABLE IF NOT EXISTS public.users (
	user_id SERIAL NOT NULL PRIMARY KEY,
    name TEXT NOT NULL,
    last_name TEXT NOT NULL,
    username TEXT NOT NULL,
    email TEXT NOT NULL,
    password TEXT NOT NULL,
    about TEXT,
    avatar TEXT,
    banner TEXT
);

CREATE TABLE IF NOT EXISTS public.projects (
	project_id SERIAL NOT NULL PRIMARY KEY,
    name TEXT NOT NULL,
    description TEXT NOT NULL,
    username TEXT NOT NULL,
    user_id INTEGER NOT NULL,
    email TEXT NOT NULL,
    avatar TEXT,
    comments_count INTEGER DEFAULT(0),
    likes INTEGER DEFAULT(0),
    views INTEGER DEFAULT(0),
    created_at TIMESTAMP DEFAULT(now()),
    updated_at TIMESTAMP
);