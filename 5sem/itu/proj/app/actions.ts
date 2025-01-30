// File: app/layout.tsx
// Author: Abas Hashmy (xhashm00) -- functions related to galleries and subgalleries
// Author: Tomáš Brablec (xbrabl04) -- everything else
// Date: 2024-12-13

"use server";

import { JSONFilePreset } from "lowdb/node";
import { revalidatePath } from "next/cache";
import { redirect } from "next/navigation";
import fs from "fs";

// -- Types used by the frontend to exchange data with the backend --

export interface Comment {
  author_id: number,
  content: string,
  creation_time: Date,
}

export interface Rating {
  user_id: number,
  rating: number // integer from 1 to 5

}

export interface Post {
  post_id: number,
  author_id: number,
  name: string,
  description: string,
  ratings: Rating[],
  views: number,
  tags: string[],
  location: string,
  image_ids: number[],
  comments: Comment[],
  creation_time: Date,
}

export interface Subgallery {
  subgallery_id: number,
  name: string,
  posts: number[],
}

export interface Gallery {
  gallery_id: number,
  owner_id: number,
  is_public: boolean,
  name: string,
  posts: number[],
  subgalleries: Subgallery[],
}

export interface Draft {
  name: string,
  description: string,
  tags: string[],
  location: string,
  image_ids: number[],
}

export interface User {
  user_id: number,
  username: string,
  draft: Draft,
}

export interface Database {
  posts: Post[],
  users: User[],
  galleries: Gallery[],
}

export interface ArrangeOptions {
  sort: string,
  search_filter: string,
  tag_filter: string[],
  location_filter: string[],
  min_rating_filter: number,
}

export type SearchParams = Promise<{ [key: string]: string | string[] | undefined }>;

// -- Default data --

// empty draft of a post
const empty_draft = {
  name: "",
  description: "",
  tags: [],
  location: "",
  image_ids: []
}

// default database data
const default_data: Database = {
  posts: [],
  users: [
    { user_id: 0, username: "Tom", draft: empty_draft }, // logged in user
    { user_id: 1, username: "Evo", draft: empty_draft },
    { user_id: 2, username: "Someone", draft: empty_draft },
    { user_id: 3, username: "Nobody", draft: empty_draft },
    { user_id: 4, username: "Macicak", draft: empty_draft },
  ],
  galleries: [],
};

// -- Database functions --

// database object
const db = await JSONFilePreset('db.json', default_data);

// this determines the currently "logged in" user
const current_user = 0;

// generates a unique identifier each time (unless you have a really bad day)
function get_id(): number {
  return Math.floor(Math.random() * Number.MAX_SAFE_INTEGER);
}

// -- Users --

export async function get_user(user_id: number): Promise<User> {
  await db.read();
  return db.data.users.find((user) => user.user_id === user_id)!;
}

export async function get_current_user(): Promise<User> {
  await db.read();
  return db.data.users.find((user) => user.user_id === current_user)!;
}

// -- Drafts --

export async function get_draft(): Promise<Draft> {
  await db.read();
  const user = db.data.users.find((user) => user.user_id === current_user)!;
  return user.draft;
}

export async function update_draft(draft: Draft) {
  await db.read();
  const user_idx = db.data.users.findIndex((user) => user.user_id === current_user);
  db.update(({ users }) => users[user_idx].draft = draft);
  revalidatePath("/new_post");
}

// -- Posts --

// returns posts with the sorting and filtering options applied
export async function get_posts(options: ArrangeOptions): Promise<Post[]> {
  await db.read();

  const rating = (post: Post) => post.ratings
    .map(({ rating }) => (rating - 1) * 25)
    .reduce((acc, elem) => acc + elem, 0) / Math.max(post.ratings.length, 1);

  return db.data.posts
    // sort posts
    .toSorted((lhs, rhs) => {
      switch (options.sort) {
        case "best_rating": return rating(rhs) - rating(lhs);
        case "most_views": return rhs.views - lhs.views;
        // default sort is by age
        default: return new Date(rhs.creation_time).getTime() - new Date(lhs.creation_time).getTime();
      }
    })
    // filter by search string
    .filter((post) => options.search_filter
      ? JSON.stringify(post).toLowerCase().includes(options.search_filter.toLowerCase())
      : true)
    // filter by tags (all tags must be present)
    .filter((post) => options.tag_filter.length > 0
      ? options.tag_filter.every((tag) =>
        post.tags.map(t => t.toLowerCase())
          .includes(tag.toLowerCase()))
      : true)
    // filter by location
    .filter((post) => options.location_filter.length > 0
      ? options.location_filter.map(l => l.toLowerCase())
        .includes(post.location.toLowerCase())
      : true
    )
    // filter by minimum rating
    .filter((post) => rating(post) >= options.min_rating_filter);
}

export async function get_post(post_id: number): Promise<Post> {
  await db.read();
  const post_idx = db.data.posts.findIndex((post) => post.post_id === post_id);
  db.update(({ posts }) => posts[post_idx].views++);
  return db.data.posts[post_idx];
}

export async function get_image(image_id: number): Promise<Buffer> {
  return await fs.promises.readFile(`images/${image_id}`);
}

export async function upload_image(image: Blob): Promise<number> {
  const image_id = get_id();
  await fs.promises.writeFile(`images/${image_id}`, Buffer.from(await image.arrayBuffer()));
  return image_id;
}

export async function remove_image(image_id: number) {
  await fs.promises.unlink(`images/${image_id}`);
}

export async function create_post() {
  await db.read();

  const user = await get_user(current_user);

  await db.update(({ posts }) => posts.push({
    post_id: get_id(),
    author_id: user.user_id,
    ...user.draft,
    ratings: [],
    views: 0,
    comments: [],
    creation_time: new Date()
  }));

  await update_draft(empty_draft);

  revalidatePath("/");
  redirect("/");
}

export async function delete_post(post_id: number) {
  await db.read();
  const idx = db.data.posts.findIndex((post) => post.post_id === post_id);
  await db.update(({ posts }) => posts.splice(idx, 1));
  redirect("/");
}

function find_post(post_id: number): Post {
  return db.data.posts.find((post) => post.post_id === post_id)!;
}

export async function change_post_name(post_id: number, name: string) {
  await db.read();
  await db.update(() => find_post(post_id).name = name);
}

export async function change_post_description(post_id: number, description: string) {
  await db.read();
  await db.update(() => find_post(post_id).description = description);
}

export async function change_post_tags(post_id: number, tags: string[]) {
  await db.read();
  await db.update(() => find_post(post_id).tags = tags);
}

export async function change_post_location(post_id: number, location: string) {
  await db.read();
  await db.update(() => find_post(post_id).location = location);
}

export async function get_user_post_rating(post_id: number): Promise<number | undefined> {
  await db.read();
  const ratings = find_post(post_id).ratings;
  const idx = ratings.findIndex((r) => r.user_id === current_user);
  if (idx !== -1) { return ratings[idx].rating; }
}

export async function update_post_rating(post_id: number, rating: number) {
  await db.read();
  const ratings = find_post(post_id).ratings;
  const idx = ratings.findIndex((r) => r.user_id === current_user);
  if (idx === -1) {
    await db.update(() => ratings.push({ user_id: current_user, rating }));
  } else {
    await db.update(() => ratings[idx].rating = rating);
  }
  revalidatePath("/");
}

export async function delete_post_rating(post_id: number) {
  await db.read();
  const ratings = find_post(post_id).ratings;
  const idx = ratings.findIndex((r) => r.user_id === current_user);
  await db.update(() => ratings.splice(idx, 1));
  revalidatePath("/");
}

// -- Comments --

export async function create_comment(post_id: number, content: string) {
  await db.read();
  const post = find_post(post_id);
  await db.update(() => post.comments.push({
    author_id: current_user,
    content,
    creation_time: new Date()
  }));
  revalidatePath("/")
}

// -- Galleries --

// get galleries with search filter applied
export async function get_user_galleries(user_id: number, only_public: boolean, search_filter: string): Promise<Gallery[]> {
  await db.read();
  const galleries = db.data.galleries.filter((gallery) => gallery.owner_id === user_id);
  return (only_public
    ? galleries.filter((g) => g.is_public)
    : galleries
  ).filter((galleries) => search_filter
    ? JSON.stringify(galleries).toLowerCase().includes(search_filter.toLowerCase())
    : true);
}

export async function get_public_galleries(search_filter: string): Promise<Gallery[]> {
  await db.read();
  return (db.data.galleries.filter((gallery) =>
    gallery.is_public)).filter((galleries) =>
      search_filter
        ? JSON.stringify(galleries).toLowerCase().includes(search_filter.toLowerCase())
        : true);
}

export async function create_gallery(name: string, is_public: boolean) {
  await db.read();
  await db.update(({ galleries }) => galleries.push({
    gallery_id: get_id(),
    owner_id: current_user,
    name,
    is_public,
    posts: [],
    subgalleries: []
  }));
  revalidatePath("/galleries");
}

export async function get_gallery(gallery_id: number) {
  await db.read();
  return find_gallery(gallery_id);
}
export async function delete_gallery(gallery_id: number) {
  await db.read();
  const idx = db.data.galleries.findIndex((gallery) => gallery.gallery_id === gallery_id);
  await db.update(({ galleries }) => galleries.splice(idx, 1));
}

function find_gallery(gallery_id: number): Gallery {
  return db.data.galleries.find((gallery) => gallery.gallery_id === gallery_id)!;
}

export async function search_galleries(name: string): Promise<Gallery[]> {
  await db.read();
  return db.data.galleries.filter((gallery) => gallery.name.includes(name));
}

export async function add_post_to_gallery(post_id: number, gallery_id: number) {
  await db.read();
  await db.update(() => find_gallery(gallery_id).posts.push(post_id));
  revalidatePath("/");
}

export async function remove_post_from_gallery(post_id: number, gallery_id: number) {
  await db.read();
  const posts = find_gallery(gallery_id).posts;
  const idx = posts.findIndex((p) => p === post_id)
  await db.update(() => posts.splice(idx, 1));
  revalidatePath("/");
}

export async function change_gallery_name(gallery_id: number, name: string) {
  await db.read();
  await db.update(() => find_gallery(gallery_id).name = name);
}

export async function change_gallery_visibility(gallery_id: number, is_public: boolean) {
  await db.read();
  await db.update(() => find_gallery(gallery_id).is_public = is_public);
  revalidatePath("/");
}

export async function get_gallery_posts(gallery_id: number, search_filter: string): Promise<number[]> {
  await db.read();
  const post_ids = (await get_gallery(gallery_id)).posts;
  const posts = await Promise.all(post_ids.map((post_id) => get_post(post_id)));
  const filtered = posts.filter((posts) => search_filter
    ? JSON.stringify(posts).toLowerCase().includes(search_filter.toLowerCase())
    : true);
  const filtered_ids = filtered.map((post) => post.post_id);
  return filtered_ids;
}

// -- Subgalleries --

function find_subgallery(gallery_id: number, subgallery_id: number): Subgallery {
  return find_gallery(gallery_id).subgalleries.find(
    (subgallery) => subgallery.subgallery_id === subgallery_id
  )!;
}

export async function get_subgalleries(gallery_id: number, search_filter: string): Promise<Subgallery[]> {
  await db.read();
  return (find_gallery(gallery_id).subgalleries)
    .filter((galleries) => search_filter
      ? JSON.stringify(galleries).toLowerCase().includes(search_filter.toLowerCase())
      : true);
}

export async function get_subgallery(gallery_id: number, subgallery_id: number): Promise<Subgallery> {
  await db.read();
  return find_subgallery(gallery_id, subgallery_id);
}

export async function create_subgallery(gallery_id: number, name: string) {
  await db.read();
  await db.update(() => find_gallery(gallery_id).subgalleries.push({
    subgallery_id: get_id(),
    name,
    posts: []
  }));
  revalidatePath("/");
}

export async function delete_subgallery(gallery_id: number, subgallery_id: number) {
  await db.read();
  const subgalleries = find_gallery(gallery_id).subgalleries;
  const idx = subgalleries.findIndex((subgallery) => subgallery.subgallery_id === subgallery_id);
  await db.update(() => subgalleries.splice(idx, 1));
}

export async function add_post_to_subgallery(post_id: number, gallery_id: number, subgallery_id: number) {
  await db.read();
  await db.update(() => find_subgallery(gallery_id, subgallery_id).posts.push(post_id));
  revalidatePath("/");
}

export async function remove_post_from_subgallery(post_id: number, gallery_id: number, subgallery_id: number) {
  await db.read();
  const posts = find_subgallery(gallery_id, subgallery_id).posts;
  const idx = posts.findIndex((p) => p === post_id);
  await db.update(() => posts.splice(idx, 1));

  revalidatePath("/");
}

export async function change_subgallery_name(gallery_id: number, subgallery_id: number, name: string) {
  await db.read();
  await db.update(() => find_subgallery(gallery_id, subgallery_id).name = name);
}

export async function get_subgallery_posts(gallery_id: number, subgallery_id: number, search_filter: string): Promise<number[]> {
  await db.read();
  const post_ids = (await get_subgallery(gallery_id, subgallery_id)).posts;
  const posts = await Promise.all(post_ids.map((post_id) => get_post(post_id)));
  const filtered = posts.filter((posts) => search_filter
    ? JSON.stringify(posts).toLowerCase().includes(search_filter.toLowerCase())
    : true);
  const filtered_ids = filtered.map((post) => post.post_id);
  return filtered_ids;
}
