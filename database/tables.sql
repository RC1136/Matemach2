

CREATE TABLE Board (
	id INTEGER PRIMARY KEY AUTOINCREMENT,
	name TEXT NOT NULL,
	url  TEXT NOT NULL
);
CREATE TABLE Thread (
	id INTEGER PRIMARY KEY AUTOINCREMENT,
	board_id INTEGER NOT NULL,
	
	FOREIGN KEY(board_id) REFERENCES Board(id)
);
CREATE TABLE Media(
	id INTEGER PRIMARY KEY AUTOINCREMENT,
	name     TEXT NOT NULL,
	filetype TEXT NOT NULL,
	filesize INTEGER NOT NULL
);
CREATE TABLE Post (
	id INTEGER PRIMARY KEY AUTOINCREMENT,
	--post_id   INTEGER PRIMARY KEY,
	thread_id   INTEGER NOT NULL,
	post_date   INTEGER NOT NULL,
	name        TEXT,
	description TEXT,
	message     TEXT NOT NULL,
	media_id    INTEGER,
	
	FOREIGN KEY(thread_id) REFERENCES Thread(id),
	FOREIGN KEY(media_id) REFERENCES Media(id)
);



 INSERT INTO Thread (board_id) SELECT id FROM Board WHERE url = "/test/";
 
 INSERT INTO Post (thread_id, post_date, message) 
 SELECT id, 0, "first post ever" FROM Thread WHERE id = 1;
 
 INSERT INTO Post (thread_id, post_date, message) 
 SELECT id, 0, "Second post ever" FROM Thread WHERE id = 2;
 