/* contrib/my_pgext_template/my_pgext_template--1.0.sql */

-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use '''CREATE EXTENSION "my_pgext_template"''' to load this file. \quit

CREATE FUNCTION my_hello_world() RETURNS text
AS 'MODULE_PATHNAME', 'my_hello_world'
LANGUAGE C STRICT;