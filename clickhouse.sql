CREATE DATABASE tracking;

USE tracking;

CREATE TABLE logs
(
    id String,
    time DateTime DEFAULT now(),
    latitude Float64,
    longitude Float64,
    accuracy Float64,
    speed Float64,
    EventDate Date DEFAULT now()
) ENGINE = MergeTree(EventDate, (id, time), 8192);