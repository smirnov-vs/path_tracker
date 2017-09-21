CREATE DATABASE tracking;

CREATE TABLE logs
(
    id UInt64,
    time DateTime DEFAULT now(),
    latitude Float32,
    longitude Float32,
    accuracy Float32,
    speed Float32,
    EventDate Date DEFAULT now()
) ENGINE = MergeTree(EventDate, (id, time), 8192);