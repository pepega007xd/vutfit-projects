import { Box, ButtonBase, Chip, IconButton, ImageListItem, Link, Paper, Stack, Typography } from "@mui/material";
import { Groups, Users, type Comment, type Post } from "@/types";
import { AccountCircle, Block, Delete, Launch, LocalOffer, LocationOn, ThumbDown, ThumbUp } from "@mui/icons-material";
import { NewComment } from "./new_comment";
import Age from "./age";
import { get_user, get_current_user, get_group_info } from "@/actions";
import db from "@/db";
import { revalidatePath } from "next/cache";
import ChangeVisibility from "./change_visibility";
import Sharepost from "./sharepost";
import SortSelect from "./sort_select";
import { current_user_has_access } from "@/auth";
import PostEdit from "./post_edit";

export function UserLink({ user }: { user: Users; }) {
  return (
    <ButtonBase
      href={`/user/${user.user_id}`}
    >
      <Stack
        direction="row"
        spacing={1}
        alignItems="center"
      >
        <AccountCircle />
        <Link
          component="p"
          color="textPrimary"
          variant="h6"
          underline="none"
        >
          {user.username}
        </Link>
      </Stack >
    </ButtonBase>
  );
}

async function Comment({ comment }: { comment: Comment }) {
  const user = await get_current_user();
  const author = await get_user(comment.author_id);
  return (
    <Stack spacing={1}>
      <Stack direction="row" alignItems="center" spacing="1em">
        <UserLink user={author} />
        <Age age={comment.creation_time} />
        {(await current_user_has_access("moderator") || user?.user_id === author.user_id) &&
          <Box flex="none">
            <IconButton onClick={async () => {
              "use server"
              await db.query(`DELETE FROM comment WHERE $1 = comment_id`, [comment.comment_id])
              revalidatePath("/");
            }}>
              <Delete />
            </IconButton>
          </Box>
        }
      </Stack>
      <Typography overflow="hidden">{comment.comment_text}</Typography>
    </Stack>
  );
}

export async function RatingButton({ post, positive }: { post: Post, positive: boolean }) {
  const icon = positive
    ? <ThumbUp color={post.user_rating === true ? "success" : "disabled"} />
    : <ThumbDown color={post.user_rating === false ? "error" : "disabled"} />;

  const user = await get_current_user();

  return (
    <>
      {user
        ?
        <IconButton onClick={async () => {
          "use server";
          if (!user) { return };
          if (post.user_rating === positive) {
            await db.query(`DELETE FROM user_photo_rating 
              WHERE user_id = $1 AND photo_id = $2`,
              [user.user_id, post.photo.photo_id, positive]);
          } else {
            await db.query(`INSERT INTO user_photo_rating (user_id, photo_id, is_positive)
              VALUES ($1, $2, $3) ON CONFLICT (user_id, photo_id) DO UPDATE SET is_positive = $3`,
              [user.user_id, post.photo.photo_id, positive]);
          }

          revalidatePath("/");
        }}>
          {icon}
        </IconButton>
        :
        icon
      }
      <Typography color="textSecondary">
        {positive ? post.positive_rating : post.negative_rating}
      </Typography>
    </>
  );
}

export async function PostInteractions({ post, group_id }: { post: Post, group_id?: number }) {
  const user = await get_current_user();
  const group = await get_group_info(group_id ?? -1);
  const user_access = await db.query<Users & { has_access: boolean }>(
    `SELECT *,
      (SELECT count(*) 
        FROM photo_user_view_access 
        WHERE user_id = u.user_id AND photo_id = $1) = 1 
      AS has_access
    FROM users u WHERE NOT user_id = $2`,
    [post.photo.photo_id, user?.user_id]);
  const user_access_list: [Users, boolean][] = Array.from(user_access).map((row) => [row as Users, row.has_access]);

  const group_access = await db.query<Groups & { has_access: boolean }>(
    `SELECT *,
      (SELECT count(*) 
      FROM photo_group_view_access 
      WHERE group_id = g.group_id AND photo_id = $1) = 1
      AS has_access
    FROM groups g
    NATURAL JOIN user_group_member
    WHERE user_id = $2`,
    [post.photo.photo_id, user?.user_id]);
  const group_access_list: [Groups, boolean][] = Array.from(group_access).map((row) => [row as Groups, row.has_access]);

  return (
    <Stack direction="row" alignItems="center" spacing="2em">
      <UserLink user={post.user} />
      <Age age={post.photo.creation_time} />
      <Stack alignItems="center" direction="row" spacing="1em">
        <RatingButton post={post} positive={true} />
        <RatingButton post={post} positive={false} />

        {user && user?.user_id === group?.owner_id &&
          <IconButton onClick={async () => {
            "use server";
            await db.query("DELETE FROM photo_group_view_access WHERE photo_id = $1 AND group_id = $2",
              [post.photo.photo_id, group_id]);
            revalidatePath("/");
          }}>
            <Block />
          </IconButton>
        }

        <IconButton href={`/post/${post.photo.photo_id}`}>
          <Launch />
        </IconButton>

        {(user?.user_id === post.photo.author_id || await current_user_has_access("moderator")) &&
          <IconButton onClick={async () => {
            "use server"
            if (!user) { return };
            await db.query(`DELETE FROM photo WHERE $1 = photo_id`, [post.photo.photo_id]);
            revalidatePath("/");
          }}>
            <Delete />
          </IconButton>
        }
        {user?.user_id === post.photo.author_id &&
          <>
            <ChangeVisibility post={post} user_access={user_access_list} group_access={group_access_list} />


            <PostEdit post={post} />
          </>
        }

        {user &&
          <Sharepost id_photo={post.photo.photo_id} />
        }
      </Stack>
    </Stack>
  );
}

export default async function Post({ post, group_id }: { post: Post, group_id?: number }) {
  const user = await get_current_user();
  return (
    <Paper sx={{ padding: "2em" }}>
      <Stack spacing="2em">
        <PostInteractions group_id={group_id} post={post} />
        <Stack direction="row" spacing="2em">
          <ImageListItem sx={{ width: "50%" }}>
            { // eslint-disable-next-line @next/next/no-img-element
              <img src={`/image/${post.photo.image_id}`} alt="posted image" />
            }
          </ImageListItem>

          <Stack spacing="2em">
            <Typography color="textSecondary" variant="h4">{post.photo.name}</Typography>

            {post.photo.description &&
              <Typography sx={{ whiteSpace: "pre-line" }}>
                {post.photo.description}
              </Typography>
            }

            {post.tags.length > 0 &&
              <Stack direction="row" spacing="1em">
                {post.tags.map(async (tag) => {
                  const onDelete = await current_user_has_access("moderator")
                    ? (async () => {
                      "use server";
                      await db.query("DELETE FROM photo_tag WHERE tag = $1",
                        [tag]);
                      revalidatePath("/");
                    })
                    : undefined;
                  return (<Chip
                    icon={<LocalOffer fontSize="small" />}
                    sx={{ paddingLeft: "0.5em" }}
                    label={tag} key={tag}
                    onDelete={onDelete}
                  />);
                })}
              </Stack>
            }

            {post.photo.location &&
              <Stack direction="row"
                alignItems="center" spacing="1em"
                sx={{ display: post.photo.location ? "inherit" : "none" }}
              >
                <LocationOn />
                <Typography variant="subtitle2">{post.photo.location}</Typography>
              </Stack>
            }

            {(post.comments.length > 0 || user) &&
              <Paper elevation={5}>
                <Stack spacing="2em" margin="2em" maxWidth="40em">
                  {user &&
                    <NewComment key={-1} photo_id={post.photo.photo_id} />
                  }
                  {post.comments.toReversed().map((c) => <Comment key={c.creation_time.toString()} comment={c} />)}
                </Stack>
              </Paper>
            }
          </Stack>
        </Stack>
      </Stack>
    </Paper>
  );
}

export type SearchParams = Promise<{ [key: string]: string | string[] | undefined }>

export async function PostList(
  { posts, searchParams, group_id }: {
    posts: Post[], searchParams?: SearchParams, group_id?: number
  }) {
  const params = await searchParams;

  const filter_tags: string[] = (typeof params?.filter_tags) === "string"
    ? [params?.filter_tags as string]
    : (params?.filter_tags as string[] ?? []);

  const filtered = filter_tags.length > 0
    ? posts.filter((post) => filter_tags.every((filter_tag) => post.tags.includes(filter_tag)))
    : posts;

  const sort = params?.sort as string ?? "newest";

  const get_rating = (post: Post) => {
    const rating = post.positive_rating / (post.positive_rating + post.negative_rating);
    return isNaN(rating) ? 0.5 : rating;
  }

  let sorted: Post[] = []

  if (sort === "newest") {
    sorted = filtered.toSorted((lhs, rhs) =>
      rhs.photo.creation_time.getTime() - lhs.photo.creation_time.getTime());

  } else if (sort === "best_rating") {
    sorted = filtered
      .toSorted((lhs, rhs) => lhs.negative_rating - rhs.negative_rating)
      .toSorted((lhs, rhs) => rhs.positive_rating - lhs.positive_rating)
      .toSorted((lhs, rhs) => get_rating(rhs) - get_rating(lhs))
  }

  return (
    <Stack spacing="2em" margin="2em">
      <SortSelect sort={sort} filter_tags={filter_tags} />
      {sorted.map((post) => <Post group_id={group_id} post={post} key={post.photo.photo_id} />)}
    </Stack>
  );
}
