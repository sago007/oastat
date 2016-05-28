-- oastat. General create table file RESTRICT2MYSQL RESTRICT2POSTGRESQL
-- oastat. Create table file for PostgreSQL    RESTRICT2POSTGRESQL
-- oastat. Create table file for MySQL         RESTRICT2MYSQL

-- Note: Lines containing RESTRICT2MYSQL are MySQL specific
-- Note: Lines containing RESTRICT2POSTGRESQL are PostgreSQL specific

-- Warning. InnoDb is the only MySQL backend supporting transactions RESTRICT2MYSQL
SET storage_engine=INNODB; -- RESTRICT2MYSQL

CREATE TABLE oastat_awards (
    eventNumber SERIAL PRIMARY KEY,
    gamenumber bigint unsigned NOT NULL,
    second integer NOT NULL,
    player bigint unsigned NOT NULL,
    award integer NOT NULL
);


CREATE TABLE oastat_team_events (
    eventnumber SERIAL PRIMARY KEY,
    gamenumber bigint unsigned NOT NULL,
    second integer NOT NULL,
    team integer,
    amount integer DEFAULT 1,
    gametype character varying(20) NOT NULL,
    player bigint unsigned,
    player2 bigint unsigned,
    eventtype integer NOT NULL,
    generic1 integer
);



CREATE TABLE oastat_challenges (
    eventnumber SERIAL PRIMARY KEY,
    gamenumber bigint unsigned NOT NULL,
    player bigint unsigned NOT NULL,
    challenge integer NOT NULL,
    amount integer DEFAULT 1 NOT NULL
);



CREATE TABLE oastat_games (
    gamenumber SERIAL PRIMARY KEY,
    gametype integer NOT NULL,
    mapname character varying(64) NOT NULL,
    time DATETIME NOT NULL,
    basegame character varying(64) DEFAULT 'baseoa' NOT NULL,
    second integer, -- is null until the match is over
    servername character varying(64) DEFAULT 'noname' NOT NULL
);

COMMENT ON TABLE oastat_games IS 'Has the information common for an entire match/map'; -- RESTRICT2POSTGRESQL

CREATE TABLE oastat_gamecvars (
    gamenumber bigint unsigned NOT NULL, -- will be used as primary key later
    cvar character varying(100) NOT NULL, -- will be used as primary key later
    value character varying(256), -- defined as MAX_CVAR_VALUE_STRING in q_shared.h
    numericvalue NUMERIC(15,2) -- if value is a float or int, then the value will also be stored here for easier comparison
);

CREATE TABLE oastat_kills (
    eventnumber SERIAL PRIMARY KEY,
    gamenumber bigint unsigned NOT NULL,
    second integer NOT NULL,
    attacker bigint unsigned NOT NULL,
    target bigint unsigned NOT NULL,
    modtype integer NOT NULL
);


CREATE TABLE oastat_players (
    guid character varying(64) NOT NULL,
    playerid SERIAL unique NOT NULL, -- all small UNIQUE will be removed in mySQL, required for postgres
    lastseen DATETIME NOT NULL,
    isbot char(1) DEFAULT 'n' NOT NULL,
    model character varying(64) NOT NULL,
    headmodel character varying(64) NOT NULL,
    nickname character varying(64) NOT NULL
);



CREATE TABLE oastat_points (
    eventnumber SERIAL PRIMARY KEY,
    player bigint unsigned NOT NULL,
    second integer NOT NULL,
    score integer NOT NULL,
    gamenumber bigint unsigned NOT NULL
);



CREATE TABLE oastat_userinfo (
    eventNumber SERIAL PRIMARY KEY,
    gamenumber bigint unsigned NOT NULL,
    player bigint unsigned NOT NULL,
    team integer,
    model character varying(64) NOT NULL,
    skill integer,
    second integer NOT NULL
);

CREATE TABLE oastat_uservars (
    userinfoevent bigint unsigned NOT NULL, -- will be used as primary key
    thekey character varying(100) NOT NULL, -- will be used as primary key later
    thevalue character varying(256),
    numericvalue NUMERIC(15,2) -- if value is a float or int, then the value will also be stored here for easier comparison
);

CREATE TABLE oastat_config (
    thekey character varying(64) pRIMARY KEY,
    thevalue character varying(256)
);

CREATE TABLE oastat_config_uservars2save (
    thekey character varying(100) pRIMARY KEY
);

CREATE TABLE oastat_config_gamevars2save (
    cvar character varying(100) pRIMARY KEY
);

-- CHECKS START

ALTER TABLE oastat_games
    ADD CONSTRAINT oastat_duplet_check UNIQUE (time, servername);

ALTER TABLE oastat_userinfo
    ADD CONSTRAINT oastat_userinfo_second_limit UNIQUE (gamenumber,player,second);

-- CHECKs END

-- pRIMARY KEYS START

-- Notice that pRIMARY KEY must not be only uppercase letters or it will be replaced by a script! 
ALTER TABLE oastat_players
    ADD CONSTRAINT oastat_players_pk pRIMARY KEY (guid);

ALTER TABLE oastat_gamecvars
    ADD CONSTRAINT oastat_gamecvars_pk pRIMARY KEY (gamenumber,cvar);

ALTER TABLE oastat_uservars
    ADD CONSTRAINT oastat_gamevars_pk pRIMARY KEY (userinfoevent,thekey);


-- pRIMARY KEYS END

-- FOREIGN KEYS START

set foreign_key_checks = 0 ; -- RESTRICT2MYSQL

ALTER TABLE oastat_awards 
    ADD CONSTRAINT oastat_awards_gn_fk FOREIGN KEY (gamenumber) REFERENCES oastat_games(gamenumber) ON UPDATE CASCADE ON DELETE CASCADE; 

ALTER TABLE oastat_team_events 
    ADD CONSTRAINT oastat_team_events_fk FOREIGN KEY (gamenumber) REFERENCES oastat_games(gamenumber) ON UPDATE CASCADE ON DELETE CASCADE; 

ALTER TABLE oastat_challenges 
    ADD CONSTRAINT oastat_challenges_fk1 FOREIGN KEY (gamenumber) REFERENCES oastat_games(gamenumber) ON UPDATE CASCADE ON DELETE CASCADE; 

ALTER TABLE oastat_points 
    ADD CONSTRAINT oastat_points_fk FOREIGN KEY (gamenumber) REFERENCES oastat_games(gamenumber) ON UPDATE CASCADE ON DELETE CASCADE; 

ALTER TABLE oastat_userinfo 
    ADD CONSTRAINT oastat_userinfo_gn_fk FOREIGN KEY (gamenumber) REFERENCES oastat_games(gamenumber) ON UPDATE CASCADE ON DELETE CASCADE; 

ALTER TABLE oastat_gamecvars 
    ADD CONSTRAINT oastat_gamecvars_gn_fk FOREIGN KEY (gamenumber) REFERENCES oastat_games(gamenumber)  ON UPDATE CASCADE ON DELETE CASCADE; 

ALTER TABLE oastat_kills 
    ADD CONSTRAINT oastat_kills_gn_fk FOREIGN KEY (gamenumber) REFERENCES oastat_games(gamenumber) ON UPDATE CASCADE ON DELETE CASCADE; 

ALTER TABLE oastat_uservars 
    ADD CONSTRAINT oastat_uservars_ui_fk FOREIGN KEY (userinfoevent) REFERENCES oastat_userinfo(eventNumber) ON UPDATE CASCADE ON DELETE CASCADE; 

ALTER TABLE oastat_challenges 
    ADD CONSTRAINT oastat_challenges_fk2 FOREIGN KEY (player) REFERENCES oastat_players(playerid) ON UPDATE CASCADE ON DELETE RESTRICT; 

ALTER TABLE oastat_points 
    ADD CONSTRAINT oastat_points_fk2 FOREIGN KEY (player) REFERENCES oastat_players(playerid) ON UPDATE CASCADE ON DELETE RESTRICT; 

ALTER TABLE oastat_kills 
    ADD CONSTRAINT oastat_kills_fk1 FOREIGN KEY (attacker) REFERENCES oastat_players(playerid) ON UPDATE CASCADE ON DELETE RESTRICT;

ALTER TABLE oastat_kills 
    ADD CONSTRAINT oastat_kills_fk2 FOREIGN KEY (target) REFERENCES oastat_players(playerid) ON UPDATE CASCADE ON DELETE RESTRICT;

ALTER TABLE oastat_awards
    ADD CONSTRAINT oastat_awards_fk1 FOREIGN KEY (player) REFERENCES oastat_players(playerid) ON UPDATE CASCADE ON DELETE RESTRICT;

ALTER TABLE oastat_team_events
    ADD CONSTRAINT oastat_team_events_fk2 FOREIGN KEY (player) REFERENCES oastat_players(playerid) ON UPDATE CASCADE ON DELETE RESTRICT;

ALTER TABLE oastat_team_events
    ADD CONSTRAINT oastat_team_events_fk3 FOREIGN KEY (player2) REFERENCES oastat_players(playerid) ON UPDATE CASCADE ON DELETE RESTRICT;

ALTER TABLE oastat_userinfo
    ADD CONSTRAINT oastat_userinfo_fk1 FOREIGN KEY (player) REFERENCES oastat_players(playerid) ON UPDATE CASCADE ON DELETE RESTRICT;

set foreign_key_checks = 1 ; -- RESTRICT2MYSQL

-- FOREIGN KEYS END

-- STANDARD VALUES

BEGIN;

-- default uservars to save:
INSERT INTO oastat_config_uservars2save (thekey) VALUES ('hc');

-- default cvars to save:
INSERT INTO oastat_config_gamevars2save (cvar) VALUES ('g_instantgib');
INSERT INTO oastat_config_gamevars2save (cvar) VALUES ('g_rockets');
INSERT INTO oastat_config_gamevars2save (cvar) VALUES ('dmflags');
INSERT INTO oastat_config_gamevars2save (cvar) VALUES ('g_lms_mode');
INSERT INTO oastat_config_gamevars2save (cvar) VALUES ('elimination_roundtime');
INSERT INTO oastat_config_gamevars2save (cvar) VALUES ('capturelimit');
INSERT INTO oastat_config_gamevars2save (cvar) VALUES ('sv_maxclients');
INSERT INTO oastat_config_gamevars2save (cvar) VALUES ('fraglimit');
INSERT INTO oastat_config_gamevars2save (cvar) VALUES ('bot_minplayers');


-- default player for WORLD events:
INSERT INTO oastat_players (guid,lastseen, isbot, model,headmodel,nickname) VALUES ('WORLD',now(),'n','-','-','-');
UPDATE oastat_players SET playerid = 0 WHERE guid = 'WORLD';

COMMIT;

-- STANDARD VALUES END

-- VERSION 2

CREATE TABLE oastat_accuracy (
    gamenumber bigint unsigned NOT NULL,
    player bigint unsigned NOT NULL,
    shotsfired integer NOT NULL,
	shotshit integer NOT NULL,
    modtype integer NOT NULL
);

ALTER TABLE oastat_accuracy
    ADD CONSTRAINT oastat_accuracy_pk pRIMARY KEY (gamenumber,player);

ALTER TABLE oastat_accuracy 
    ADD CONSTRAINT oastat_accuracy_gn_fk FOREIGN KEY (gamenumber) REFERENCES oastat_games(gamenumber) ON UPDATE CASCADE ON DELETE CASCADE; 

ALTER TABLE oastat_accuracy 
    ADD CONSTRAINT oastat_accuracy_pi_fk FOREIGN KEY (player) REFERENCES oastat_players(playerid) ON UPDATE CASCADE ON DELETE RESTRICT; 

