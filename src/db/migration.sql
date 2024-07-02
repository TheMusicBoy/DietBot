-- Ручками

CREATE USER diet_bot WITH PASSWORD '123456';

CREATE TABLE consumer (
    id bigint PRIMARY KEY,
    purpose integer,
    birthday bigint,
    height integer,
    weight integer,
    activity integer
);

GRANT ALL PRIVILEGES ON consumer TO diet_bot;

CREATE TABLE product (
    name varchar(40) PRIMARY KEY,
	kalories real,
	protein real,
	fats real,
	carbohydrates real
);

GRANT ALL PRIVILEGES ON product TO diet_bot;
