-- oastat. General create table file

-- Note: Lines containing RESTRICT2MYSQL are MySQL specific
-- Note: Lines containing RESTRICT2POSTGRESQL are PostgreSQL specific

-- Warning. InnoDb is the only MySQL backend supporting transactions RESTRICT2MYSQL
SET table_type=INNODB; -- RESTRICT2MYSQL

CREATE TABLE oastat_awards (
    eventNumber SERIAL PRIMARY KEY,
    gamenumber bigint NOT NULL,
    second integer,
    player character varying(64),
    award integer NOT NULL
);


CREATE TABLE oastat_team_events (
    eventnumber SERIAL PRIMARY KEY,
    gamenumber bigint NOT NULL,
    second integer NOT NULL,
    team integer,
    amount integer DEFAULT 1,
    gametype character varying(20) NOT NULL,
    player character varying(64),
    player2 character varying(64),
    eventtype integer NOT NULL,
    generic1 integer
);



CREATE TABLE oastat_challenges (
    eventnumber SERIAL PRIMARY KEY,
    gamenumber bigint NOT NULL,
    player character varying(64) NOT NULL,
    challenge integer NOT NULL,
    amount integer DEFAULT 1 NOT NULL
);



CREATE TABLE oastat_games (
    gamenumber SERIAL PRIMARY KEY,
    gametype integer NOT NULL,
    mapname character varying(64) NOT NULL,
    time DATETIME NOT NULL,
    basegame character varying(64) DEFAULT 'baseoa' NOT NULL,
    second integer,
    servername character varying(64) DEFAULT 'noname' NOT NULL
);

COMMENT ON TABLE oastat_games IS 'Has the information common for an entire match/map'; -- RESTRICT2POSTGRESQL

CREATE TABLE oastat_gamecvars (
    gamenumber bigint NOT NULL, -- will be used as primary key later
    cvar character varying(100) NOT NULL, -- will be used as primary key later
    value character varying(256), -- defined as MAX_CVAR_VALUE_STRING in q_shared.h
    numericvalue float -- if value is a float or int, then the value will also be stored here for easier comparison
);

CREATE TABLE oastat_kills (
    eventnumber SERIAL PRIMARY KEY,
    gamenumber bigint NOT NULL,
    second integer NOT NULL,
    attacker character varying(64) NOT NULL,
    target character varying(64) NOT NULL,
    modtype integer NOT NULL
);


CREATE TABLE oastat_players (
    guid character varying(64) NOT NULL,
    playerid SERIAL NOT NULL,
    lastseen DATETIME NOT NULL,
    isbot char(1) DEFAULT 'n' NOT NULL,
    model character varying(64) NOT NULL,
    headmodel character varying(64) NOT NULL,
    nickname character varying(64) NOT NULL
);



CREATE TABLE oastat_points (
    eventnumber SERIAL PRIMARY KEY,
    player character varying(64) NOT NULL,
    second integer NOT NULL,
    score integer NOT NULL,
    gamenumber bigint NOT NULL
);



CREATE TABLE oastat_userinfo (
    eventNumber SERIAL PRIMARY KEY,
    gamenumber bigint NOT NULL,
    guid character varying(64) NOT NULL,
    team integer,
    model character varying(64) NOT NULL,
    skill integer,
    second integer NOT NULL
);

CREATE TABLE oastat_uservars (
    userinfoevent bigint NOT NULL, -- will be used as primary key
    thekey character varying(100) NOT NULL, -- will be used as primary key later
    thevalue character varying(256),
    numericvalue float -- if value is a float or int, then the value will also be stored here for easier comparison
);

-- CHECKS START

ALTER TABLE oastat_games
    ADD CONSTRAINT duplet_check UNIQUE (time, servername);

-- CHECKs END

-- PRIMARY KEYS START


ALTER TABLE oastat_players
    ADD CONSTRAINT oastat_players_pk pRIMARY KEY (guid);

ALTER TABLE oastat_gamecvars
    ADD CONSTRAINT oastat_gamecvars_pk pRIMARY KEY (gamenumber,cvar);

ALTER TABLE oastat_uservars
    ADD CONSTRAINT oastat_gamevars_pk pRIMARY KEY (userinfoevent,thekey);


-- PRIMARY KEYS END

-- FOREIGN KEYS START

set foreign_key_checks = 0 ; -- RESTRICT2MYSQL

ALTER TABLE oastat_awards -- RESTRICT2POSTGRESQL
    ADD CONSTRAINT oastat_awards_gn_fk FOREIGN KEY (gamenumber) REFERENCES oastat_games(gamenumber) ON UPDATE CASCADE ON DELETE CASCADE; -- RESTRICT2POSTGRESQL


ALTER TABLE oastat_team_events -- RESTRICT2POSTGRESQL
    ADD CONSTRAINT oastat_captures_fk FOREIGN KEY (gamenumber) REFERENCES oastat_games(gamenumber) ON UPDATE CASCADE ON DELETE CASCADE; -- RESTRICT2POSTGRESQL


ALTER TABLE oastat_challenges -- RESTRICT2POSTGRESQL
    ADD CONSTRAINT oastat_challenges_fk1 FOREIGN KEY (gamenumber) REFERENCES oastat_games(gamenumber) ON UPDATE CASCADE ON DELETE CASCADE; -- RESTRICT2POSTGRESQL


ALTER TABLE oastat_challenges -- RESTRICT2POSTGRESQL
    ADD CONSTRAINT oastat_challenges_fk2 FOREIGN KEY (player) REFERENCES oastat_players(guid) ON UPDATE CASCADE ON DELETE RESTRICT; -- RESTRICT2POSTGRESQL


ALTER TABLE oastat_kills -- RESTRICT2POSTGRESQL
    ADD CONSTRAINT oastat_kills_gn_fk FOREIGN KEY (gamenumber) REFERENCES oastat_games(gamenumber) ON UPDATE CASCADE ON DELETE CASCADE; -- RESTRICT2POSTGRESQL


ALTER TABLE oastat_points -- RESTRICT2POSTGRESQL
    ADD CONSTRAINT oastat_points_fk FOREIGN KEY (gamenumber) REFERENCES oastat_games(gamenumber) ON UPDATE CASCADE ON DELETE CASCADE; -- RESTRICT2POSTGRESQL


ALTER TABLE oastat_points -- RESTRICT2POSTGRESQL
    ADD CONSTRAINT oastat_points_fk2 FOREIGN KEY (player) REFERENCES oastat_players(guid) ON UPDATE CASCADE ON DELETE RESTRICT; -- RESTRICT2POSTGRESQL


ALTER TABLE oastat_userinfo -- RESTRICT2POSTGRESQL
    ADD CONSTRAINT oastat_userinfo_gn_fk FOREIGN KEY (gamenumber) REFERENCES oastat_games(gamenumber) ON UPDATE CASCADE ON DELETE CASCADE; -- RESTRICT2POSTGRESQL

ALTER TABLE oastat_gamecvars -- RESTRICT2POSTGRESQL
    ADD CONSTRAINT oastat_gamecvars_gn_fk FOREIGN KEY (gamenumber) REFERENCES oastat_games(gamenumber)  ON UPDATE CASCADE ON DELETE CASCADE; -- RESTRICT2POSTGRESQL

ALTER TABLE oastat_uservars -- RESTRICT2POSTGRESQL
    ADD CONSTRAINT oastat_uservars_ui_fk FOREIGN KEY (userinfoevent) REFERENCES oastat_userinfo(eventNumber) ON UPDATE CASCADE ON DELETE CASCADE; -- RESTRICT2POSTGRESQL

set foreign_key_checks = 1 ; -- RESTRICT2MYSQL

-- FOREIGN KEYS END
