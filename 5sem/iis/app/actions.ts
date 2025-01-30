"use server";

import { Groups, Share, User_Group_Member, Comment, Photo, Post, Rating, Users, UserType } from "./types";
import db from "./db";
import { current_user_has_access, get_session } from "./auth";
import { redirect } from "next/navigation"
import { revalidatePath } from "next/cache";

async function add_tags(photo_id: number, tags: string[]) {
  if (! await current_user_has_access("user")) { return; }
  await Promise.all(tags.map(async (tag) => {
    await db.query("INSERT INTO photo_tag (photo_id, tag) VALUES ($1, $2)",
      [photo_id, tag]);
  }));
}

export async function add_photo(name: string, description: string, location: string, tags: string[], image_blob: Blob, is_private: boolean) {
  const user = await get_current_user();
  if (!user) { return };

  const image = Buffer.from(await image_blob.arrayBuffer());

  const { image_id } = (await db.query<{ image_id: number }>(
    `INSERT INTO image_data (image_content) VALUES ($1) RETURNING image_id`,
    [image]).one());

  const photo = await db.query<Photo>(
    `INSERT INTO photo (author_id, name, description, location, image_id, is_private)
    VALUES ($1, $2, $3, $4, $5, $6) RETURNING *`,
    [user.user_id, name, description, location, image_id, is_private]).one();

  await add_tags(photo.photo_id, tags);

  redirect("/");
}

async function photo_to_post(photo: Photo): Promise<Post> {
  const author = await db.query<Users>("SELECT * FROM users WHERE user_id = $1", [photo.author_id]).one();
  const comments = Array.from(await db.query<Comment>("SELECT * FROM comment WHERE photo_id = $1", [photo.photo_id]));
  const tags = Array.from(await db.query<{ tag: string }>(
    "SELECT tag FROM photo_tag WHERE photo_id = $1",
    [photo.photo_id]
  )).map((t) => t.tag);

  const { positive } = await db.query<{ positive: bigint }>(
    `SELECT count(*) as positive FROM user_photo_rating
    WHERE photo_id = $1 AND is_positive = true`,
    [photo.photo_id]).one();

  const { negative } = await db.query<{ negative: bigint }>(
    `SELECT count(*) as negative FROM user_photo_rating
    WHERE photo_id = $1 AND is_positive = false`,
    [photo.photo_id]).one();

  const [positive_rating, negative_rating] = [Number(positive), Number(negative)];

  const current_user = await get_current_user();
  let user_rating = undefined;
  if (current_user) {
    user_rating = (await db.query<Rating>(
      "SELECT * FROM user_photo_rating WHERE user_id = $1 AND photo_id = $2",
      [current_user.user_id, photo.photo_id]).first())?.is_positive;
  }
  return { photo, user: author, comments, tags, positive_rating, negative_rating, user_rating };
}

/// if logged in, returns all available posts for the given user
/// for anyone else, returns all public posts
export async function get_feed_posts(): Promise<Post[]> {
  const photos = Array.from(await db.query<Photo>("SELECT * FROM photo WHERE is_private = false"));

  const current_user = await get_current_user();
  if (current_user) {
    // add photos of current user
    photos.push(...Array.from(await db.query<Photo>(`SELECT photo.*
      FROM photo WHERE author_id = $1`,
      [current_user.user_id]
    )));

    // add photos based on user view_access
    photos.push(...Array.from(
      await db.query<Photo>(`SELECT photo.*
        FROM photo NATURAL JOIN photo_user_view_access
        WHERE user_id = $1`,
        [current_user.user_id]
      )));

    // add photos based on group view_access
    photos.push(...Array.from(await db.query<Photo>(`SELECT photo.*
      FROM photo
        NATURAL JOIN photo_group_view_access
        NATURAL JOIN groups
        NATURAL JOIN user_group_member
      WHERE user_id = $1`,
      [current_user.user_id]
    )));
  }

  const deduplicated = new Map(photos.map((p) => [p.photo_id, p]));
  return await Promise.all(Array.from(deduplicated.values()).map(photo_to_post));
}

/// returns all posts from a single user
export async function get_user_posts(user_id: number): Promise<Post[]> {
  let photos: Photo[] = []
  if (await current_user_has_access("moderator")) {
    photos = Array.from(await db.query<Photo>("SELECT * FROM photo WHERE author_id = $1", [user_id]));
  } else {
    photos = Array.from(await db.query<Photo>("SELECT * FROM photo WHERE author_id = $1 AND is_private = false",
      [user_id]));
  }

  return await Promise.all(photos.map(photo_to_post))
}

/// returns all posts from a single group
export async function get_group_posts(group_id: number): Promise<Post[]> {
  const photos = Array.from(await db.query<Photo>(
    "SELECT photo.* FROM photo NATURAL JOIN photo_group_view_access WHERE group_id = $1",
    [group_id]
  ));

  return await Promise.all(photos.map(photo_to_post));
}

export async function create_comment(photo_id: number, comment_text: string) {
  if (! await current_user_has_access("user")) { return; }
  const user = await get_current_user();
  if (user) {
    await db.query(
      `INSERT INTO comment (comment_text, photo_id, author_id)
      VALUES ($1, $2, $3)`,
      [comment_text, photo_id, user.user_id]
    )
  }
  revalidatePath("/")
}

export async function get_user(user_id: number): Promise<Users> {
  return await db.query<Users>("SELECT * FROM users WHERE user_id = $1", [user_id]).one();
}

export async function get_post(photo_id: number): Promise<Post> {
  const photo = await db.query<Photo>("SELECT * FROM photo WHERE photo_id = $1", [photo_id]).one();
  return await photo_to_post(photo);
}

export async function get_shared_posts(): Promise<Share[]> {
  const user = await get_current_user();
  if (user) {
    const result = await db.query<Share>(
      "SELECT * FROM share WHERE receiver_id = $1", [user.user_id]);
    return Array.from(result);
  }
  return [];
}

export async function get_groups(): Promise<User_Group_Member[] | undefined> {
  const user = await get_current_user();
  if (user?.user_type === "administrator" || user?.user_type === "moderator") {
    const result = await db.query<User_Group_Member>("SELECT DISTINCT group_id FROM user_group_member");
    const groups: User_Group_Member[] = [...result];
    return groups;
  }

  if (user) {
    const result = await db.query<User_Group_Member>("SELECT * FROM user_group_member WHERE user_id = $1", [user.user_id]);
    const groups: User_Group_Member[] = [...result];
    return groups;
  }
  return undefined;
}

export async function create_group(groupName: string, groupDescription: string): Promise<Groups | undefined> {
  const user = await get_current_user();
  if (user) {
    const result = await db.query<Groups>("INSERT INTO groups(group_name,description,owner_id) VALUES ($1,$2,$3) RETURNING *", [groupName, groupDescription, user.user_id]).first();
    if (result) {
      await add_user_to_group(result.group_id, user.user_id);
      return result;
    }
  }
  return undefined;
}

export async function add_user_to_group(groupId: number, userId: number): Promise<User_Group_Member | undefined> {
  const user = await db.query<User_Group_Member>("INSERT INTO user_group_member(user_id,group_id) VALUES ($1,$2) RETURNING *", [userId, groupId]).first();
  if (user) await remove_user_from_request(userId, groupId);
  return user;
}

export async function get_group_info(groupNumber: number): Promise<Groups | undefined> {
  const result = await db.query<Groups>("SELECT * FROM groups WHERE group_id = $1", [groupNumber]).first();
  return result;
}

export async function update_group(groupId: number, groupName: string, groupDescription: string): Promise<Groups | undefined> {
  const result = await db.query<Groups>("UPDATE groups SET group_name = $1, description = $2 WHERE group_id = $3 RETURNING *", [groupName, groupDescription, groupId]).first();
  return result;
}

export async function delete_group(groupNumber: number) {
  await db.query("DELETE FROM groups WHERE group_id = $1", [groupNumber]);
}


export async function get_user_in_group(groupId: number): Promise<Users[] | undefined> {
  const result = await db.query<Users>("SELECT users.* FROM users JOIN user_group_member USING (user_id) WHERE group_id = $1", [groupId]);
  if (result) {
    const users: Users[] = [...result];
    return users;
  }
  return undefined;
}

export async function remove_user_from_group(userId: number, groupId: number): Promise<number | undefined> {
  const result = await db.query<number>("DELETE FROM user_group_member WHERE user_id = $1 AND group_id = $2 RETURNING *", [userId, groupId]).first();
  return result;
}

export async function move_ownership_of_group(userId: number, groupId: number): Promise<Groups | undefined> {
  const result = await db.query<Groups>("UPDATE groups SET owner_id = $1 WHERE group_id = $2 RETURNING *", [userId, groupId]).first();
  return result;
}


export async function get_user_which_are_not_members(groupId: number): Promise<Users[] | undefined> {
  const result = await db.query<Users>("SELECT * FROM users WHERE NOT EXISTS (SELECT 1 FROM user_group_member WHERE user_group_member.user_id = users.user_id and user_group_member.group_id = $1)", [groupId]);
  if (result) {
    const users: Users[] = [...result];
    return users;
  }
  return undefined;
}

export async function get_current_user(): Promise<Users | undefined> {
  const session = await get_session();
  if (!session) return;
  const user = await db.query<Users>("SELECT * FROM users WHERE user_id = $1", [session.user_id]).first();
  return user;
}

export async function change_user_visibility(photo_id: number, user_id: number, has_access: boolean) {
  if (has_access) {
    await db.query("DELETE FROM photo_user_view_access WHERE photo_id = $1 AND user_id = $2",
      [photo_id, user_id]);
  } else {
    await db.query("INSERT INTO photo_user_view_access (photo_id, user_id) VALUES ($1, $2)",
      [photo_id, user_id]);
  }
  revalidatePath("/");
}

export async function change_group_visibility(photo_id: number, group_id: number, has_access: boolean) {
  if (has_access) {
    await db.query("DELETE FROM photo_group_view_access WHERE photo_id = $1 AND group_id = $2",
      [photo_id, group_id]);
  } else {
    await db.query("INSERT INTO photo_group_view_access (photo_id, group_id) VALUES ($1, $2)",
      [photo_id, group_id]);
  }
  revalidatePath("/");
}

export async function share_post(photo_id: number, receiver_id: number) {
  const user = await get_current_user();
  if (user) {
    await db.query(
      "INSERT INTO share (photo_id, sender_id, receiver_id) VALUES ($1, $2, $3)", [photo_id, user.user_id, receiver_id]
    );
  }
}


export async function get_all_other_users(): Promise<Users[]> {
  const user = await get_current_user();
  const result = await db.query<Users>(
    "SELECT * FROM users WHERE NOT user_id = $1", [user?.user_id]
  );

  const pole = [...result];
  return pole;
}

export async function get_users_for_share(photo_id: number): Promise<Users[]> {
  const user = await get_current_user();
  const result = await db.query<Users>(
    `SELECT * FROM users WHERE NOT user_id = $1 AND user_id NOT IN (
      SELECT receiver_id FROM share WHERE $1 = sender_id AND $2 = photo_id
    );`, [user?.user_id, photo_id]
  );

  const pole = [...result];
  return pole;
}

export async function change_user_type(user_id: number, user_type: UserType) {
  if (!current_user_has_access("administrator")) { return; }
  await db.query("UPDATE users SET user_type = $1 WHERE user_id = $2",
    [user_type, user_id]);
}

export async function has_moderating_rights(): Promise<boolean> {
  const user = await get_current_user();
  if (user?.user_type === "administrator" || user?.user_type === "moderator") return true;
  return false;
}

export async function update_username(userId: number, username: string): Promise<Users | undefined> {
  const data = await db.query<Users>("UPDATE users SET username = $1 WHERE user_id = $2 RETURNING *", [username, userId]).first();
  return data;
}

export async function update_biography(userId: number, biography: string): Promise<Users | undefined> {
  const data = await db.query<Users>("UPDATE users SET biography = $1 WHERE user_id = $2 RETURNING *", [biography, userId]).first();
  return data;
}

export async function update_password(userId: number, password: string): Promise<Users | undefined> {
  const data = await db.query<Users>("UPDATE users SET password_hash = crypt($1, gen_salt('bf')) WHERE user_id = $2 RETURNING *", [password, userId]).first();
  return data;
}

export async function update_block_user(userId: number, isBanned: boolean): Promise<Users | undefined> {
  const data = await db.query<Users>("UPDATE users SET is_blocked = $1 WHERE user_id = $2 RETURNING *", [isBanned, userId]).first();
  return data;
}

export async function get_banned_users(): Promise<Users[]> {
  const result = await db.query<Users>("SELECT * FROM users WHERE is_blocked = true");
  const data = [...result];
  return data;
}

export async function get_avaliable_groups(): Promise<Groups[]> {
  const user = await get_current_user();
  if (user) {
    const data = await db.query<Groups>(`SELECT * FROM groups WHERE 
        group_id NOT IN (SELECT group_id FROM user_group_member WHERE user_id = $1) AND 
        group_id NOT IN (SELECT group_id FROM user_group_request WHERE user_id = $1)`, [user.user_id]);
    const result = [...data];
    return result;
  }
  return [];
}

export async function send_join_group_request(groupId: number) {
  const user = await get_current_user();
  if (user) await db.query("INSERT INTO user_group_request(user_id,group_id) VALUES ($1,$2)", [user.user_id, groupId]);
}

export async function remove_user_from_request(userId: number, groupId: number) {
  await db.query("DELETE FROM user_group_request WHERE user_id = $1 AND group_id = $2", [userId, groupId]);
}

export async function get_join_group_requests(groupId: number): Promise<Users[]> {
  const result = await db.query<Users>("SELECT users.* FROM users LEFT JOIN user_group_request ON users.user_id = user_group_request.user_id WHERE group_id = $1", [groupId]);
  const data = [...result]
  return data;
}

export async function update_post(photo_id: number, name: string, tags: string[], is_private: boolean,
  description?: string, location?: string) {
  await db.query(`UPDATE photo SET name = $1, description = $2, location = $3, is_private = $4
    WHERE photo_id = $5`, [name, description, location, is_private, photo_id])

  await db.query("DELETE FROM photo_tag WHERE photo_id = $1", [photo_id]);
  await add_tags(photo_id, tags);
  revalidatePath("/");
}
