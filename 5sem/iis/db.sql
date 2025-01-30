--
-- PostgreSQL database dump
--

-- Dumped from database version 17.1
-- Dumped by pg_dump version 17.1

SET statement_timeout = 0;
SET lock_timeout = 0;
SET idle_in_transaction_session_timeout = 0;
SET transaction_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SELECT pg_catalog.set_config('search_path', '', false);
SET check_function_bodies = false;
SET xmloption = content;
SET client_min_messages = warning;
SET row_security = off;

--
-- Name: pgcrypto; Type: EXTENSION; Schema: -; Owner: -
--

CREATE EXTENSION IF NOT EXISTS pgcrypto WITH SCHEMA public;


--
-- Name: EXTENSION pgcrypto; Type: COMMENT; Schema: -; Owner: 
--

COMMENT ON EXTENSION pgcrypto IS 'cryptographic functions';


SET default_tablespace = '';

SET default_table_access_method = heap;

--
-- Name: comment; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.comment (
    comment_id integer NOT NULL,
    comment_text text NOT NULL,
    creation_time timestamp with time zone DEFAULT CURRENT_TIMESTAMP NOT NULL,
    photo_id integer NOT NULL,
    author_id integer NOT NULL
);


ALTER TABLE public.comment OWNER TO postgres;

--
-- Name: comment_comment_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.comment_comment_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.comment_comment_id_seq OWNER TO postgres;

--
-- Name: comment_comment_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.comment_comment_id_seq OWNED BY public.comment.comment_id;


--
-- Name: groups; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.groups (
    group_id integer NOT NULL,
    group_name character varying(255) NOT NULL,
    description text,
    creation_time timestamp with time zone DEFAULT CURRENT_TIMESTAMP NOT NULL,
    owner_id integer NOT NULL
);


ALTER TABLE public.groups OWNER TO postgres;

--
-- Name: group_group_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.group_group_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.group_group_id_seq OWNER TO postgres;

--
-- Name: group_group_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.group_group_id_seq OWNED BY public.groups.group_id;


--
-- Name: image_data; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.image_data (
    image_id integer NOT NULL,
    image_content bytea NOT NULL
);


ALTER TABLE public.image_data OWNER TO postgres;

--
-- Name: image_image_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.image_image_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.image_image_id_seq OWNER TO postgres;

--
-- Name: image_image_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.image_image_id_seq OWNED BY public.image_data.image_id;


--
-- Name: photo; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.photo (
    photo_id integer NOT NULL,
    name character varying(255),
    description text,
    location character varying(255),
    is_private boolean NOT NULL,
    creation_time timestamp with time zone DEFAULT CURRENT_TIMESTAMP NOT NULL,
    author_id integer NOT NULL,
    image_id integer NOT NULL
);


ALTER TABLE public.photo OWNER TO postgres;

--
-- Name: photo_group_view_access; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.photo_group_view_access (
    group_id integer NOT NULL,
    photo_id integer NOT NULL
);


ALTER TABLE public.photo_group_view_access OWNER TO postgres;

--
-- Name: photo_photo_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.photo_photo_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.photo_photo_id_seq OWNER TO postgres;

--
-- Name: photo_photo_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.photo_photo_id_seq OWNED BY public.photo.photo_id;


--
-- Name: photo_tag; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.photo_tag (
    photo_id integer NOT NULL,
    tag character varying(64) NOT NULL
);


ALTER TABLE public.photo_tag OWNER TO postgres;

--
-- Name: photo_user_view_access; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.photo_user_view_access (
    photo_id integer NOT NULL,
    user_id integer NOT NULL
);


ALTER TABLE public.photo_user_view_access OWNER TO postgres;

--
-- Name: share; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.share (
    share_id integer NOT NULL,
    comment text,
    creation_time timestamp with time zone DEFAULT CURRENT_TIMESTAMP NOT NULL,
    photo_id integer NOT NULL,
    sender_id integer NOT NULL,
    receiver_id integer NOT NULL
);


ALTER TABLE public.share OWNER TO postgres;

--
-- Name: share_share_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.share_share_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.share_share_id_seq OWNER TO postgres;

--
-- Name: share_share_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.share_share_id_seq OWNED BY public.share.share_id;


--
-- Name: user_group_member; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_group_member (
    user_id integer NOT NULL,
    group_id integer NOT NULL
);


ALTER TABLE public.user_group_member OWNER TO postgres;

--
-- Name: user_group_request; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_group_request (
    user_id integer NOT NULL,
    group_id integer NOT NULL
);


ALTER TABLE public.user_group_request OWNER TO postgres;

--
-- Name: user_photo_rating; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.user_photo_rating (
    user_id integer NOT NULL,
    photo_id integer NOT NULL,
    is_positive boolean NOT NULL
);


ALTER TABLE public.user_photo_rating OWNER TO postgres;

--
-- Name: users; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.users (
    user_id integer NOT NULL,
    username character varying(255) NOT NULL,
    full_name character varying(255),
    biography text,
    is_blocked boolean NOT NULL,
    creation_time timestamp with time zone DEFAULT CURRENT_TIMESTAMP NOT NULL,
    password_hash character varying(255) NOT NULL,
    user_type character varying(16) NOT NULL
);


ALTER TABLE public.users OWNER TO postgres;

--
-- Name: user_user_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.user_user_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.user_user_id_seq OWNER TO postgres;

--
-- Name: user_user_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.user_user_id_seq OWNED BY public.users.user_id;


--
-- Name: comment comment_id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.comment ALTER COLUMN comment_id SET DEFAULT nextval('public.comment_comment_id_seq'::regclass);


--
-- Name: groups group_id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.groups ALTER COLUMN group_id SET DEFAULT nextval('public.group_group_id_seq'::regclass);


--
-- Name: image_data image_id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.image_data ALTER COLUMN image_id SET DEFAULT nextval('public.image_image_id_seq'::regclass);


--
-- Name: photo photo_id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.photo ALTER COLUMN photo_id SET DEFAULT nextval('public.photo_photo_id_seq'::regclass);


--
-- Name: share share_id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.share ALTER COLUMN share_id SET DEFAULT nextval('public.share_share_id_seq'::regclass);


--
-- Name: users user_id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.users ALTER COLUMN user_id SET DEFAULT nextval('public.user_user_id_seq'::regclass);


--
-- Data for Name: comment; Type: TABLE DATA; Schema: public; Owner: postgres
--



--
-- Data for Name: groups; Type: TABLE DATA; Schema: public; Owner: postgres
--

INSERT INTO public.groups VALUES (1, 'my group', 'some description', '2024-11-23 21:34:44.933562+01', 16);


--
-- Data for Name: image_data; Type: TABLE DATA; Schema: public; Owner: postgres
--



--
-- Data for Name: photo; Type: TABLE DATA; Schema: public; Owner: postgres
--



--
-- Data for Name: photo_group_view_access; Type: TABLE DATA; Schema: public; Owner: postgres
--



--
-- Data for Name: photo_tag; Type: TABLE DATA; Schema: public; Owner: postgres
--



--
-- Data for Name: photo_user_view_access; Type: TABLE DATA; Schema: public; Owner: postgres
--



--
-- Data for Name: share; Type: TABLE DATA; Schema: public; Owner: postgres
--



--
-- Data for Name: user_group_member; Type: TABLE DATA; Schema: public; Owner: postgres
--

INSERT INTO public.user_group_member VALUES (16, 1);
INSERT INTO public.user_group_member VALUES (17, 1);


--
-- Data for Name: user_group_request; Type: TABLE DATA; Schema: public; Owner: postgres
--

INSERT INTO public.user_group_request VALUES (18, 1);


--
-- Data for Name: user_photo_rating; Type: TABLE DATA; Schema: public; Owner: postgres
--



--
-- Data for Name: users; Type: TABLE DATA; Schema: public; Owner: postgres
--

INSERT INTO public.users VALUES (16, 'administrator', NULL, NULL, false, '2024-11-23 21:32:42.107464+01', '$2a$06$DQ5s.SPcPUvJq3H2/JO9Yui0sKjmeVQ7EGQ8C9stNAcHniXZ34kj6', 'administrator');
INSERT INTO public.users VALUES (17, 'moderator', NULL, NULL, false, '2024-11-23 21:33:38.859551+01', '$2a$06$rgpPP.bFpJJkXjQodT9cYOwB2XXvcsxe.X2a38lSZa7SLytBBeMzy', 'moderator');
INSERT INTO public.users VALUES (18, 'user', NULL, NULL, false, '2024-11-23 21:33:48.614223+01', '$2a$06$B2Zp1jFn9UaYk4xIrLOLVe233zC.RwUNk7n7IP68H8m5AHgt3RzVS', 'user');


--
-- Name: comment_comment_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('public.comment_comment_id_seq', 8, true);


--
-- Name: group_group_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('public.group_group_id_seq', 1, true);


--
-- Name: image_image_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('public.image_image_id_seq', 6, true);


--
-- Name: photo_photo_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('public.photo_photo_id_seq', 11, true);


--
-- Name: share_share_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('public.share_share_id_seq', 1, false);


--
-- Name: user_user_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('public.user_user_id_seq', 18, true);


--
-- Name: comment comment_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.comment
    ADD CONSTRAINT comment_pkey PRIMARY KEY (comment_id);


--
-- Name: groups group_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.groups
    ADD CONSTRAINT group_pkey PRIMARY KEY (group_id);


--
-- Name: image_data image_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.image_data
    ADD CONSTRAINT image_pkey PRIMARY KEY (image_id);


--
-- Name: photo_group_view_access photo_group_view_access_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.photo_group_view_access
    ADD CONSTRAINT photo_group_view_access_pkey PRIMARY KEY (group_id, photo_id);


--
-- Name: photo photo_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.photo
    ADD CONSTRAINT photo_pkey PRIMARY KEY (photo_id);


--
-- Name: photo_tag photo_tag_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.photo_tag
    ADD CONSTRAINT photo_tag_pkey PRIMARY KEY (photo_id, tag);


--
-- Name: photo_user_view_access photo_user_view_access_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.photo_user_view_access
    ADD CONSTRAINT photo_user_view_access_pkey PRIMARY KEY (photo_id, user_id);


--
-- Name: share share_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.share
    ADD CONSTRAINT share_pkey PRIMARY KEY (share_id);


--
-- Name: user_group_member user_group_member_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_group_member
    ADD CONSTRAINT user_group_member_pkey PRIMARY KEY (user_id, group_id);


--
-- Name: user_group_request user_group_request_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_group_request
    ADD CONSTRAINT user_group_request_pkey PRIMARY KEY (user_id, group_id);


--
-- Name: user_photo_rating user_photo_rating_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_photo_rating
    ADD CONSTRAINT user_photo_rating_pkey PRIMARY KEY (user_id, photo_id);


--
-- Name: users user_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.users
    ADD CONSTRAINT user_pkey PRIMARY KEY (user_id);


--
-- Name: fki_fk_author_id; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX fki_fk_author_id ON public.photo USING btree (author_id);


--
-- Name: fki_fk_group_id; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX fki_fk_group_id ON public.photo_group_view_access USING btree (group_id);


--
-- Name: fki_fk_image_id; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX fki_fk_image_id ON public.photo USING btree (image_id);


--
-- Name: fki_fk_owner_id; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX fki_fk_owner_id ON public.groups USING btree (owner_id);


--
-- Name: fki_fk_photo_id; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX fki_fk_photo_id ON public.photo_tag USING btree (photo_id);


--
-- Name: fki_fk_sender_id; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX fki_fk_sender_id ON public.share USING btree (sender_id);


--
-- Name: fki_fk_user_id; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX fki_fk_user_id ON public.photo_user_view_access USING btree (user_id);


--
-- Name: fki_phot; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX fki_phot ON public.share USING btree (photo_id);


--
-- Name: fki_receiver_id; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX fki_receiver_id ON public.share USING btree (receiver_id);


--
-- Name: fki_u; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX fki_u ON public.comment USING btree (photo_id);


--
-- Name: fki_user_id; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX fki_user_id ON public.comment USING btree (author_id);


--
-- Name: photo fk_author_id; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.photo
    ADD CONSTRAINT fk_author_id FOREIGN KEY (author_id) REFERENCES public.users(user_id) ON DELETE CASCADE NOT VALID;


--
-- Name: photo_group_view_access fk_group_id; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.photo_group_view_access
    ADD CONSTRAINT fk_group_id FOREIGN KEY (group_id) REFERENCES public.groups(group_id) ON DELETE CASCADE NOT VALID;


--
-- Name: user_group_member fk_group_id; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_group_member
    ADD CONSTRAINT fk_group_id FOREIGN KEY (group_id) REFERENCES public.groups(group_id) ON DELETE CASCADE NOT VALID;


--
-- Name: user_group_request fk_group_id; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_group_request
    ADD CONSTRAINT fk_group_id FOREIGN KEY (group_id) REFERENCES public.groups(group_id) ON DELETE CASCADE NOT VALID;


--
-- Name: photo fk_image_id; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.photo
    ADD CONSTRAINT fk_image_id FOREIGN KEY (image_id) REFERENCES public.image_data(image_id) ON DELETE CASCADE NOT VALID;


--
-- Name: comment fk_owner_id; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.comment
    ADD CONSTRAINT fk_owner_id FOREIGN KEY (author_id) REFERENCES public.users(user_id) ON DELETE CASCADE NOT VALID;


--
-- Name: groups fk_owner_id; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.groups
    ADD CONSTRAINT fk_owner_id FOREIGN KEY (owner_id) REFERENCES public.users(user_id) ON DELETE CASCADE NOT VALID;


--
-- Name: comment fk_photo_id; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.comment
    ADD CONSTRAINT fk_photo_id FOREIGN KEY (photo_id) REFERENCES public.photo(photo_id) ON DELETE CASCADE NOT VALID;


--
-- Name: share fk_photo_id; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.share
    ADD CONSTRAINT fk_photo_id FOREIGN KEY (photo_id) REFERENCES public.photo(photo_id) ON DELETE CASCADE NOT VALID;


--
-- Name: photo_tag fk_photo_id; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.photo_tag
    ADD CONSTRAINT fk_photo_id FOREIGN KEY (photo_id) REFERENCES public.photo(photo_id) ON DELETE CASCADE NOT VALID;


--
-- Name: photo_group_view_access fk_photo_id; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.photo_group_view_access
    ADD CONSTRAINT fk_photo_id FOREIGN KEY (photo_id) REFERENCES public.photo(photo_id) ON DELETE CASCADE NOT VALID;


--
-- Name: photo_user_view_access fk_photo_id; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.photo_user_view_access
    ADD CONSTRAINT fk_photo_id FOREIGN KEY (photo_id) REFERENCES public.photo(photo_id) ON DELETE CASCADE NOT VALID;


--
-- Name: user_photo_rating fk_photo_id; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_photo_rating
    ADD CONSTRAINT fk_photo_id FOREIGN KEY (photo_id) REFERENCES public.photo(photo_id) ON DELETE CASCADE NOT VALID;


--
-- Name: share fk_receiver_id; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.share
    ADD CONSTRAINT fk_receiver_id FOREIGN KEY (receiver_id) REFERENCES public.users(user_id) ON DELETE CASCADE NOT VALID;


--
-- Name: share fk_sender_id; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.share
    ADD CONSTRAINT fk_sender_id FOREIGN KEY (sender_id) REFERENCES public.users(user_id) ON DELETE CASCADE NOT VALID;


--
-- Name: photo_user_view_access fk_user_id; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.photo_user_view_access
    ADD CONSTRAINT fk_user_id FOREIGN KEY (user_id) REFERENCES public.users(user_id) ON DELETE CASCADE NOT VALID;


--
-- Name: user_group_member fk_user_id; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_group_member
    ADD CONSTRAINT fk_user_id FOREIGN KEY (user_id) REFERENCES public.users(user_id) ON DELETE CASCADE NOT VALID;


--
-- Name: user_photo_rating fk_user_id; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_photo_rating
    ADD CONSTRAINT fk_user_id FOREIGN KEY (user_id) REFERENCES public.users(user_id) ON DELETE CASCADE NOT VALID;


--
-- Name: user_group_request fk_user_id; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.user_group_request
    ADD CONSTRAINT fk_user_id FOREIGN KEY (user_id) REFERENCES public.users(user_id) ON DELETE CASCADE NOT VALID;


--
-- PostgreSQL database dump complete
--

