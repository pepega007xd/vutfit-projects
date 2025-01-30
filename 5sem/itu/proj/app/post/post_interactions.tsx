// File: app/post/post_interactions.tsx
// Author: Tomáš Brablec (xbrabl04)
// Date: 2024-12-13

"use client";

import { ButtonBase, IconButton, Link, Paper, Rating, Stack, Typography } from "@mui/material";
import { delete_post, delete_post_rating, Post, update_post_rating, User } from "@/actions";
import { AccountCircle, Delete, Launch, Visibility } from "@mui/icons-material";
import { useState } from "react";
import Age from "@/post/age";
import AddButton from "@/galleries/addbutton";
import { PostName } from "./editable_text";

// Component with a styled link to a user's profile (their public galleries),
// contains an icon and a username
export function UserLink({ user }: { user: User }) {
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

// Top bar of a post, pulled to a separate file as a client compoennt
export default function PostInteractions({ post, author, current_rating, is_single_page, editable }: { post: Post; author: User; current_rating: number | undefined; is_single_page?: boolean, editable?: boolean }) {
  // rating of the post given in stars by the current user, undefined means no rating
  const [user_rating, set_user_rating] = useState(current_rating);

  // calculate post's total percentage rating
  const rating = post.ratings
    .map(({ rating }) => (rating - 1) * 25)
    .reduce((acc, elem) => acc + elem, 0) / post.ratings.length;

  return (
    <Paper elevation={5}>
      <Stack direction="row" alignItems="center" spacing="2em" margin="1em">
        <UserLink user={author} />

        <PostName editable={editable} post_id={post.post_id} initial_text={post.name} />

        <Age age={post.creation_time} />

        {/* number of views */}
        <Stack alignItems="center" direction="row" spacing="1em">
          <Visibility />
          <Typography color="textSecondary">{post.views}</Typography>
        </Stack>

        {/* post rating component and total rating percentage */}
        <Stack alignItems="center" direction="row" spacing="1em">
          <Rating
            value={user_rating ?? null}
            onChange={async (_, new_value) => {
              set_user_rating(new_value ?? undefined);
              if (new_value) { await update_post_rating(post.post_id, new_value); }
              else { await delete_post_rating(post.post_id) }
            }}
          />
          {post.ratings.length > 0 &&
            <Typography color="textSecondary">{Math.round(rating)} %</Typography>
          }
        </Stack>

        {/* button for adding the post into galleries */}
        <AddButton post={post} />

        {/* button for deleting post */}
        {editable &&
          <IconButton onClick={async () => await delete_post(post.post_id)}>
            <Delete />
          </IconButton>
        }

        {/* button for opening the post in single page view */}
        {is_single_page ||
          <IconButton href={`/post/${post.post_id}`}>
            <Launch />
          </IconButton>
        }
      </Stack>
    </Paper>
  );
}
