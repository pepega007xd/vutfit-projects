-------------------------------------------------------------

-- Creation of database begins here

-------------------------------------------------------------

DROP TABLE item_category CASCADE CONSTRAINTS;

DROP TABLE item CASCADE CONSTRAINTS;

DROP TABLE shop_order CASCADE CONSTRAINTS;

DROP TABLE user_account CASCADE CONSTRAINTS;

DROP TABLE employee_account CASCADE CONSTRAINTS;

DROP TABLE basket CASCADE CONSTRAINTS;

DROP TABLE order_contains_item CASCADE CONSTRAINTS;

-- create tables

CREATE TABLE item_category (
    category_id   INT
        GENERATED BY DEFAULT AS IDENTITY
    PRIMARY KEY NOT NULL,
    category_name VARCHAR2(255) NOT NULL
);

CREATE TABLE item (
    item_id          INT
        GENERATED BY DEFAULT AS IDENTITY
    PRIMARY KEY NOT NULL,
    item_name        VARCHAR2(255) NOT NULL,
    item_description CLOB,
    price            NUMBER(20, 2) NOT NULL,
    amount_in_stock  INTEGER NOT NULL,
    category_id      INTEGER
        REFERENCES item_category
    NOT NULL
);

-- user and employee accounts had a generalization in the original model,
-- the table "Person". Since the specialization is disjunct (no person can be
-- User and Employee at the same time - the small storage optimization of not storing
-- the name and email of a few people twice is not worth the added complexity),
-- and total (there can be no Person, which is not User, nor Employee), we decided
-- to simply include Person's attributes in User and Employee, and make them separate tables
CREATE TABLE user_account (
    personal_email VARCHAR2(255) PRIMARY KEY NOT NULL,
    first_name     NVARCHAR2(255) NOT NULL,
    last_name      NVARCHAR2(255) NOT NULL,
    username       NVARCHAR2(255) NOT NULL,
    password_hash  CHAR(64) NOT NULL,
    -- the following fields will not be required for user registration
    phone_number   VARCHAR2(15),
    street_name    VARCHAR2(128),
    city_name      VARCHAR2(128),
    zip_code       CHAR(5),
    CONSTRAINT user_pw_hash CHECK ( REGEXP_LIKE ( password_hash,
                                                  '^[0-9a-f]{64}$' ) ),
    CONSTRAINT user_zip_code CHECK ( REGEXP_LIKE ( zip_code,
                                                   '^\d{5}$' ) )
);

CREATE TABLE employee_account (
    personal_email VARCHAR2(255) PRIMARY KEY NOT NULL,
    first_name     NVARCHAR2(255) NOT NULL,
    last_name      NVARCHAR2(255) NOT NULL,
    username       NVARCHAR2(255) NOT NULL,
    password_hash  CHAR(64) NOT NULL,
    company_email  VARCHAR2(255) NOT NULL,
    salary         NUMBER(20, 2) NOT NULL,
    CONSTRAINT employee_pw_hash CHECK ( REGEXP_LIKE ( password_hash,
                                                      '^[0-9a-f]{64}$' ) )
);

CREATE TABLE shop_order (
    order_id              INT
        GENERATED BY DEFAULT AS IDENTITY
    PRIMARY KEY NOT NULL,
    shipping_company      NVARCHAR2(255) NOT NULL,
    shipping_street_name  VARCHAR2(128) NOT NULL,
    shipping_city_name    VARCHAR2(128) NOT NULL,
    shipping_zip_code     CHAR(5) CHECK ( REGEXP_LIKE ( shipping_zip_code,
                                                    '^\d{5}$' ) ) NOT NULL,
    billing_street_name   VARCHAR2(128) NOT NULL,
    billing_city_name     VARCHAR2(128) NOT NULL,
    billing_zip_code      CHAR(5) CHECK ( REGEXP_LIKE ( billing_zip_code,
                                                   '^\d{5}$' ) ) NOT NULL,
    delivery_phone_number VARCHAR2(15),
    creation_date         TIMESTAMP DEFAULT current_timestamp NOT NULL,
    user_account          VARCHAR2(255)
        REFERENCES user_account ( personal_email )
    NOT NULL,
    employee_account      VARCHAR2(255)
        REFERENCES employee_account ( personal_email )
);

CREATE TABLE basket (
    personal_email
        REFERENCES user_account
    NOT NULL,
    item_id
        REFERENCES item
    NOT NULL,
    PRIMARY KEY ( personal_email,
                  item_id ),
    amount INTEGER NOT NULL
);

CREATE TABLE order_contains_item (
    item_id
        REFERENCES item
    NOT NULL,
    order_id
        REFERENCES shop_order
    NOT NULL,
    PRIMARY KEY ( item_id,
                  order_id ),
    amount INTEGER NOT NULL
);

-- insert example users

INSERT INTO user_account (
    personal_email,
    first_name,
    last_name,
    username,
    password_hash,
    phone_number,
    street_name,
    city_name,
    zip_code
) VALUES (
    'kentus@blentus.com',
    'Kentus',
    'Blentus',
    'kekel',
    '7253ec94b9ad29b292ad31664999e650b97cda5590407ff9c27cb589c7d7eef9',
    '666666666',
    'Kentusova',
    'Brnisko',
    '12757'
);

INSERT INTO user_account (
    personal_email,
    first_name,
    last_name,
    username,
    password_hash,
    phone_number,
    street_name,
    city_name,
    zip_code
) VALUES (
    'other@blentus.com',
    'Someone',
    'Else',
    'anotherone',
    '8e77b7602faa50484c5e9ef8ee6ab44e9c83ab6dc0bc30a6c5a078f0893efd78',
    '605123456',
    'Ulice 2',
    'Knowhere',
    '11234'
);

INSERT INTO user_account (
    personal_email,
    first_name,
    last_name,
    username,
    password_hash
) VALUES (
    'abc@def.com',
    'Abc',
    'Def',
    'abc',
    'bfd33f5eae21080b8b2cae7f7109cc7325107c04ad71573273d64beea063c067'
);

-- insert example employees

INSERT INTO employee_account (
    personal_email,
    first_name,
    last_name,
    username,
    password_hash,
    company_email,
    salary
) VALUES (
    'whatever@wherever.net',
    'name',
    'lastname',
    'employee1',
    '4f9c69753794e821cfd17646269689b7acc1e3be21e1ff62ba59bca06bd1e7b2',
    'employee1@koteseni.cz',
    12500.00
);

INSERT INTO employee_account (
    personal_email,
    first_name,
    last_name,
    username,
    password_hash,
    company_email,
    salary
) VALUES (
    'name@server.tld',
    'another',
    'last',
    'something',
    '3d5f44d8f5477011fc241cf602cb7c0f0bec0976f0d8dc7b8ad77733ada6ca7e',
    'employee2@koteseni.cz',
    62501.00
);

-- insert example item categories

INSERT INTO item_category ( category_name ) VALUES ( 'Smartphones' );

INSERT INTO item_category ( category_name ) VALUES ( 'Desktops' );

INSERT INTO item_category ( category_name ) VALUES ( 'Laptops' );

INSERT INTO item_category ( category_name ) VALUES ( 'Toasters' );

INSERT INTO item (
    item_name,
    item_description,
    price,
    amount_in_stock,
    category_id
) VALUES (
    'some phone',
    'this one has locked bootloader, you aint installing lineageos',
    14000.00,
    251,
    1 -- smartphones
);

INSERT INTO item (
    item_name,
    item_description,
    price,
    amount_in_stock,
    category_id
) VALUES (
    'another phone',
    'this one comes with apfelOS, you aint installing no apps on this one',
    42000.00,
    1532,
    1 -- smartphones
);

INSERT INTO item (
    item_name,
    item_description,
    price,
    amount_in_stock,
    category_id
) VALUES (
    'lonevo laptop 3000',
    'the keyboard will definitely not stop working after few years',
    22000.00,
    3,
    3 -- laptops
);

INSERT INTO item (
    item_name,
    item_description,
    price,
    amount_in_stock,
    category_id
) VALUES (
    'RGB gaming desktop 31',
    'you wont need any other lights in your home with this one',
    22000.00,
    160,
    2 -- desktops
);

INSERT INTO item (
    item_name,
    item_description,
    price,
    amount_in_stock,
    category_id
) VALUES (
    'TOTER 3000',
    'built-in discord moderation functionality',
    125000.00,
    12,
    4 -- toasters
);

-- insert example order

INSERT INTO shop_order (
    shipping_company,
    shipping_street_name,
    shipping_city_name,
    shipping_zip_code,
    billing_street_name,
    billing_city_name,
    billing_zip_code,
    delivery_phone_number,
    user_account
) VALUES (
    'Boxes S.R.O.',
    'Blue street',
    'Blue city',
    '04132',
    'Red street',
    'Red city',
    '23140',
    '666111000',
    'kentus@blentus.com'
);

-- inserting items contained in orders

INSERT INTO order_contains_item (
    item_id,
    order_id,
    amount
) VALUES (
    1, -- phone
    1,
    2
);

INSERT INTO order_contains_item (
    item_id,
    order_id,
    amount
) VALUES (
    2,
    1,
    3
);

INSERT INTO order_contains_item (
    item_id,
    order_id,
    amount
) VALUES (
    3,
    1,
    4
);

INSERT INTO order_contains_item (
    item_id,
    order_id,
    amount
) VALUES (
    4,
    1,
    15
);

-- inserting items in user baskets

INSERT INTO basket (
    personal_email,
    item_id,
    amount
) VALUES (
    'kentus@blentus.com',
    1, -- phone
    1
);

INSERT INTO basket (
    personal_email,
    item_id,
    amount
) VALUES (
    'other@blentus.com',
    5, -- toaster
    1
);

INSERT INTO basket (
    personal_email,
    item_id,
    amount
) VALUES (
    'other@blentus.com',
    3, -- laptop
    5
);

-------------------------------------------------------------

-- Solution of assignment 4 begins here

-------------------------------------------------------------

-- database triggers ----------------------------------------

-- check that the number of items in stock will never go below zero
CREATE OR REPLACE TRIGGER prevent_negative_stock BEFORE
    UPDATE OF amount_in_stock ON item
    FOR EACH ROW
BEGIN
    IF :new.amount_in_stock < 0 THEN
        raise_application_error(
                               -20001,
                               'Negative value is not allowed for item in-stock amount'
        );
    END IF;
END;
/

-- setting amount to zero passes
UPDATE item
SET
    amount_in_stock = 0
WHERE
    item_id = 1;

-- setting amount to minus one emits an exception
UPDATE item
SET
    amount_in_stock = - 1
WHERE
    item_id = 1;

-- when user creates an order, delete the content of their basket
CREATE OR REPLACE TRIGGER delete_basket_after_order AFTER
    INSERT ON shop_order
    FOR EACH ROW
BEGIN
    DELETE FROM basket
    WHERE
        personal_email = :new.user_account;

END;
/

-- put some item into user's basket
INSERT INTO basket (
    personal_email,
    item_id,
    amount
) VALUES (
    'kentus@blentus.com',
    2,
    1
);

-- check that user's basket is not empty
SELECT
    u.first_name,
    u.last_name,
    i.item_name,
    b.amount
FROM
    user_account u,
    basket       b,
    item         i
WHERE
    u.personal_email = b.personal_email
    AND i.item_id = b.item_id
    AND u.personal_email = 'kentus@blentus.com';

-- create order for user
INSERT INTO shop_order (
    shipping_company,
    shipping_street_name,
    shipping_city_name,
    shipping_zip_code,
    billing_street_name,
    billing_city_name,
    billing_zip_code,
    delivery_phone_number,
    user_account
) VALUES (
    'Boxes S.R.O.',
    'Blue street',
    'Blue city',
    '04132',
    'Red street',
    'Red city',
    '23140',
    '666111000',
    'kentus@blentus.com'
);

-- check that user's basket is empty
SELECT
    u.first_name,
    u.last_name,
    i.item_name,
    b.amount
FROM
    user_account u,
    basket       b,
    item         i
WHERE
    u.personal_email = b.personal_email
    AND i.item_id = b.item_id
    AND u.personal_email = 'kentus@blentus.com';

-- the trigger would interfere with the following code
DROP TRIGGER delete_basket_after_order;

-- stored procedures ---------------------------------------

-- stored procedure: create an order for a user, and add their basket content to the order
CREATE OR REPLACE PROCEDURE create_order (
    shipping_company      IN shop_order.shipping_company%TYPE,
    shipping_street_name  IN shop_order.shipping_street_name%TYPE,
    shipping_city_name    IN shop_order.shipping_city_name%TYPE,
    shipping_zip_code     IN shop_order.shipping_zip_code%TYPE,
    billing_street_name   IN shop_order.billing_street_name%TYPE,
    billing_city_name     IN shop_order.billing_city_name%TYPE,
    billing_zip_code      IN shop_order.billing_zip_code%TYPE,
    delivery_phone_number IN shop_order.delivery_phone_number%TYPE,
    user_account          IN user_account.personal_email%TYPE
) AS

    CURSOR items_in_basket IS
    SELECT
        i.item_id,
        b.amount
    FROM
        basket b,
        item   i
    WHERE
        user_account = b.personal_email
        AND i.item_id = b.item_id;

    order_id shop_order.order_id%TYPE;
    item_id  item.item_id%TYPE;
    amount   basket.amount%TYPE;
BEGIN
    INSERT INTO shop_order (
        shipping_company,
        shipping_street_name,
        shipping_city_name,
        shipping_zip_code,
        billing_street_name,
        billing_city_name,
        billing_zip_code,
        delivery_phone_number,
        user_account
    ) VALUES (
        shipping_company,
        shipping_street_name,
        shipping_city_name,
        shipping_zip_code,
        billing_street_name,
        billing_city_name,
        billing_zip_code,
        delivery_phone_number,
        user_account
    ) RETURNING order_id INTO order_id;

    OPEN items_in_basket;
    LOOP
        FETCH items_in_basket INTO
            item_id,
            amount;
        EXIT WHEN items_in_basket%notfound;
        INSERT INTO order_contains_item (
            item_id,
            order_id,
            amount
        ) VALUES (
            item_id,
            order_id,
            amount
        );

    END LOOP;

    CLOSE items_in_basket;
END;
/

-- demonstration

-- put some item into user's basket
INSERT INTO basket (
    personal_email,
    item_id,
    amount
) VALUES (
    'abc@def.com',
    1,
    1
);

-- check that user's basket is not empty
SELECT
    u.first_name,
    u.last_name,
    i.item_name,
    b.amount
FROM
    user_account u,
    basket       b,
    item         i
WHERE
    u.personal_email = b.personal_email
    AND i.item_id = b.item_id
    AND u.personal_email = 'abc@def.com';
    
-- execute stored procedure
BEGIN
    create_order(
                'Boxes S.R.O.',
                'Blue street',
                'Blue city',
                '04132',
                'Red street',
                'Red city',
                '23140',
                '666111000',
                'abc@def.com'
    );
END;
/

-- check that the new order contains the added item
SELECT
    u.first_name,
    u.last_name,
    o.order_id,
    i.item_name,
    c.amount
FROM
    user_account        u,
    shop_order          o,
    order_contains_item c,
    item                i
WHERE
    u.personal_email = o.user_account
    AND o.order_id = c.order_id
    AND c.item_id = i.item_id
    AND u.personal_email = 'abc@def.com';

-- stored procedure: reset password for user, print error when user does not exist
CREATE OR REPLACE PROCEDURE reset_password (
    uname         IN user_account.username%TYPE,
    password_hash user_account.personal_email%TYPE
) AS
    email user_account.personal_email%TYPE;
BEGIN
    SELECT
        u.personal_email
    INTO email
    FROM
        user_account u
    WHERE
        u.username = uname;

    UPDATE user_account u
    SET
        u.password_hash = password_hash
    WHERE
        u.personal_email = personal_email;

EXCEPTION
    WHEN no_data_found THEN
        dbms_output.put_line('Username does not correspond to any user account');
END;
/

-- demonstration

-- set password for existing user
SELECT
    u.personal_email
FROM
    user_account u
WHERE
    u.username = 'kekel';

BEGIN
    reset_password(
                  'kekel',
                  '123'
    );
END;
/

-- set password for nonexistent user
SELECT
    u.personal_email
FROM
    user_account u
WHERE
    u.username = 'AAAAAAAAAAAAAAAAAAAAAAAAAA';

BEGIN
    reset_password(
                  'AAAAAAAAAAAAAAAAAAAAAAAAAA',
                  '123'
    );
END;
/

-- EXPLAIN PLAN, query optimization using index --------------------

EXPLAIN PLAN
    FOR
-- (calculate the average amount of items in basket for each user)
SELECT
    u.first_name,
    u.last_name,
    AVG(b.amount) AS average_items_in_basket
FROM
    user_account u,
    basket       b
WHERE
    u.personal_email = b.personal_email
GROUP BY
    u.first_name,
    u.last_name;

SELECT
    *
FROM
    TABLE ( dbms_xplan.display );     
    
-- ------------------------------------------------------------------------------------------------
-- | Id  | Operation                     | Name           | Rows  | Bytes | Cost (%CPU)| Time     |
-- ------------------------------------------------------------------------------------------------
-- |   0 | SELECT STATEMENT              |                |     3 |  2355 |     7  (15)| 00:00:01 |
-- |   1 |  HASH GROUP BY                |                |     3 |  2355 |     7  (15)| 00:00:01 |
-- |   2 |   NESTED LOOPS                |                |     3 |  2355 |     6   (0)| 00:00:01 |
-- |   3 |    NESTED LOOPS               |                |     3 |  2355 |     6   (0)| 00:00:01 |
-- |   4 |     TABLE ACCESS FULL         | BASKET         |     3 |   426 |     3   (0)| 00:00:01 |
-- |*  5 |     INDEX UNIQUE SCAN         | SYS_C001505507 |     1 |       |     0   (0)| 00:00:01 |
-- 
-- PLAN_TABLE_OUTPUT                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                
-- -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
-- |   6 |    TABLE ACCESS BY INDEX ROWID| USER_ACCOUNT   |     1 |   643 |     1   (0)| 00:00:01 |
-- ------------------------------------------------------------------------------------------------
--  
-- Predicate Information (identified by operation id):
-- ---------------------------------------------------
--  
--    5 - access("U"."PERSONAL_EMAIL"="B"."PERSONAL_EMAIL")

CREATE INDEX idx_user_first_name ON
    user_account (
        first_name
    );

CREATE INDEX idx_user_last_name ON
    user_account (
        last_name
    );

CREATE INDEX idx_basket_personal_email ON
    basket (
        personal_email
    );

EXPLAIN PLAN
    FOR
-- (calculate the average amount of items in basket for each user)
SELECT
    u.first_name,
    u.last_name,
    AVG(b.amount) AS average_items_in_basket
FROM
    user_account u,
    basket       b
WHERE
    u.personal_email = b.personal_email
GROUP BY
    u.first_name,
    u.last_name;

SELECT
    *
FROM
    TABLE ( dbms_xplan.display ); 

-- ----------------------------------------------------------------------------------------------------------------------
-- | Id  | Operation                                | Name                      | Rows  | Bytes | Cost (%CPU)| Time     |
-- ----------------------------------------------------------------------------------------------------------------------
-- |   0 | SELECT STATEMENT                         |                           |     3 |  2394 |     6  (17)| 00:00:01 |
-- |   1 |  HASH GROUP BY                           |                           |     3 |  2394 |     6  (17)| 00:00:01 |
-- |   2 |   NESTED LOOPS                           |                           |     3 |  2394 |     5   (0)| 00:00:01 |
-- |   3 |    NESTED LOOPS                          |                           |     3 |  2394 |     5   (0)| 00:00:01 |
-- |   4 |     VIEW                                 | VW_GBC_5                  |     3 |   465 |     2   (0)| 00:00:01 |
-- |   5 |      HASH GROUP BY                       |                           |     3 |   426 |     2   (0)| 00:00:01 |
-- 
-- PLAN_TABLE_OUTPUT                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                
-- -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
-- |   6 |       TABLE ACCESS BY INDEX ROWID BATCHED| BASKET                    |     3 |   426 |     2   (0)| 00:00:01 |
-- |   7 |        INDEX FULL SCAN                   | IDX_BASKET_PERSONAL_EMAIL |     3 |       |     1   (0)| 00:00:01 |
-- |*  8 |     INDEX UNIQUE SCAN                    | SYS_C001505507            |     1 |       |     0   (0)| 00:00:01 |
-- |   9 |    TABLE ACCESS BY INDEX ROWID           | USER_ACCOUNT              |     1 |   643 |     1   (0)| 00:00:01 |
-- ----------------------------------------------------------------------------------------------------------------------
--  
-- Predicate Information (identified by operation id):
-- ---------------------------------------------------
--  
--    8 - access("U"."PERSONAL_EMAIL"="ITEM_1")


-- accress rights for XHASHM00 -----------------------------

GRANT ALL ON item_category TO xhashm00;

GRANT ALL ON item TO xhashm00;

GRANT ALL ON user_account TO xhashm00;

GRANT ALL ON employee_account TO xhashm00;

GRANT ALL ON shop_order TO xhashm00;

GRANT ALL ON basket TO xhashm00;

GRANT ALL ON order_contains_item TO xhashm00;

-- materialized view (execute as user xhashm00) ------------

-- create materialized view which shows the total cost of each user's basket

CREATE MATERIALIZED VIEW user_basket_cost
    REFRESH
        COMPLETE
AS
    SELECT
        u.first_name,
        u.last_name,
        SUM(b.amount * i.price)
    FROM
        xbrabl04.user_account u,
        xbrabl04.basket       b,
        xbrabl04.item         i
    WHERE
        u.personal_email = b.personal_email
        AND i.item_id = b.item_id
    GROUP BY
        u.first_name,
        u.last_name;

-- demonstration

-- original state
SELECT
    *
FROM
    user_basket_cost;

-- change source tables
INSERT INTO basket (
    personal_email,
    item_id,
    amount
) VALUES (
    'kentus@blentus.com',
    4,
    2
);

-- the view will not change automatically, this should match the original state
SELECT
    *
FROM
    user_basket_cost;
    
-- refresh the materialized view
BEGIN
    dbms_mview.refresh('user_basket_cost');
END;
/

-- the view will contain new data
SELECT
    *
FROM
    user_basket_cost;

-- complex SELECT which uses WITH clause and CASE operator ------------

-- select which shows the total income from each item, as well as text description of the amount in stock

WITH item_income AS (
    SELECT
        i.item_name,
        i.amount_in_stock,
        SUM(c.amount * i.price) AS total_income
    FROM
        item                i,
        order_contains_item c
    WHERE
        i.item_id = c.item_id
    GROUP BY
        i.item_name,
        i.amount_in_stock
)
SELECT
    item_name,
    total_income,
    amount_in_stock,
    CASE
    WHEN amount_in_stock = 0 THEN
    'Out of stock'
    WHEN amount_in_stock < 5 THEN
    'Almost out of stock'
    ELSE
    'In stock'
    END AS is_in_stock
FROM
    item_income;
