// File: app/post/post.tsx
// Author: Tomáš Brablec (xbrabl04)
// Date: 2024-12-13

import { ImageList, ImageListItem, Paper, Stack, Typography } from "@mui/material";
import { type Comment, get_current_user, get_user, get_user_post_rating, type Post } from "@/actions";
import { NewComment } from "./new_comment";
import PostInteractions, { UserLink } from "./post_interactions";
import Age from "./age";
import { PostDescription, PostLocation, PostTags } from "./editable_text";

// Component for showing a single comment
async function Comment({ comment }: { comment: Comment }) {
  const user = await get_user(comment.author_id);

  return (
    <Paper elevation={5}>
      <Stack margin="1em">
        {/* comment author and age */}
        <Stack direction="row" alignItems="center" spacing="1em">
          <UserLink user={user} />
          <Age age={comment.creation_time} />
        </Stack>

        {/* comment text */}
        <Typography sx={{ whiteSpace: "pre-line" }}>
          {comment.content}
        </Typography>
      </Stack>
    </Paper>
  );
}

// Component for showing a single post, `is_single_page` tells the component
// whether it is displayed as a single page post or in a list of posts
export default async function Post({ post, is_single_page }: { post: Post, is_single_page?: boolean }) {
  const author = await get_user(post.author_id);

  // components are editable when they post is displayed in a single page view
  // and the current user is the post author
  const editable = is_single_page && post.author_id === (await get_current_user()).user_id;
  const show_side_panel =
    editable
    || post.description
    || post.tags.length > 0
    || post.location;

  return (
    <Paper sx={{ padding: "2em" }}>
      <Stack spacing="2em">
        {/* top bar with interactive elements (rating, storing into gallery, deletion) */}
        <PostInteractions
          author={author}
          editable={editable}
          post={post}
          is_single_page={is_single_page}
          current_rating={await get_user_post_rating(post.post_id)}
        />

        <Stack direction="row" spacing="2em">
          {/* list of images in the post */}
          <ImageList variant="masonry"
            gap={10}
            cols={Math.min(3, post.image_ids.length)}
            sx={{ minWidth: "50%" }} >
            {post.image_ids.map((image) => (
              <ImageListItem key={image}>
                <img src={`/image/${image}`} alt="posted image" />
              </ImageListItem>
            ))}
          </ImageList>

          <Stack spacing="2em" width="50%" minWidth="30%">
            {/* right side panel with post metadata (description, tags, location) */}
            {show_side_panel &&
              <Paper elevation={5}>
                <Stack spacing="2em" margin="1em">
                  <PostDescription editable={editable} post_id={post.post_id} initial_text={post.description} />

                  <PostTags editable={editable} post_id={post.post_id} initial_tags={post.tags} />

                  <PostLocation editable={editable} post_id={post.post_id} initial_text={post.location} />
                </Stack>
              </Paper>
            }

            {/* component for creating a new comment */}
            <Paper elevation={5}>
              <NewComment post_id={post.post_id} />
            </Paper>

            {/* list of existing comments */}
            <Stack spacing="1em" direction="column-reverse">
              {post.comments.map((c) => <Comment key={c.creation_time.toString()} comment={c} />)}
            </Stack>
          </Stack>
        </Stack>
      </Stack >
    </Paper>
  );
}
