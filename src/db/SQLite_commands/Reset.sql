DELETE FROM batch_pipeline_map;
DELETE FROM batch;
DELETE FROM pipeline;
DELETE FROM sqlite_sequence WHERE name IN ('batch', 'pipeline', 'batch_pipeline_map');

-- Pipelines: p1 - p10
INSERT INTO pipeline (name, date_created, date_modified) VALUES
    ('p1',  1719600000, 1719600000),
    ('p2',  1719600000, 1719600000),
    ('p3',  1719600000, 1719600000),
    ('p4',  1719600000, 1719600000),
    ('p5',  1719600000, 1719600000),
    ('p6',  1719600000, 1719600000),
    ('p7',  1719600000, 1719600000),
    ('p8',  1719600000, 1719600000),
    ('p9',  1719600000, 1719600000),
    ('p10', 1719600000, 1719600000);

-- Batches: b1 - b10, random status values
INSERT INTO batch (name, date_created, date_modified, status) VALUES
    ('b1',  1719600000, 1719600000, 2),
    ('b2',  1719600000, 1719600000, 0),
    ('b3',  1719600000, 1719600000, 1),
    ('b4',  1719600000, 1719600000, 0),
    ('b5',  1719600000, 1719600000, 2),
    ('b6',  1719600000, 1719600000, 1),
    ('b7',  1719600000, 1719600000, 0),
    ('b8',  1719600000, 1719600000, 1),
    ('b9',  1719600000, 1719600000, 2),
    ('b10', 1719600000, 1719600000, 0);

-- Batch <-> Pipeline mapping (random, some batches mapped to multiple pipelines)
INSERT INTO batch_pipeline_map (batch_id, pipeline_id) VALUES
    (1, 4),
    (1, 7),
    (2, 2),
    (3, 9),
    (3, 1),
    (4, 6),
    (5, 3),
    (5, 10),
    (6, 8),
    (7, 1),
    (7, 5),
    (8, 4),
    (9, 9),
    (9, 2),
    (10, 7);