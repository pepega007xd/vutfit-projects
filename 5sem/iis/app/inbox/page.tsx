import { get_post, get_shared_posts, get_user } from "@/actions";
import { UserLink } from "@/post/post";
import { Launch } from "@mui/icons-material";
import { Box, IconButton, Paper, Stack, Typography } from "@mui/material";

export default async function Inbox() {
  const shared_posts = await get_shared_posts();
  return (
    <Stack margin="2em" spacing="2em">
      <Box sx={{
        display: "flex",
        backgroundColor: '#2B2B2B',
        padding: 2,
        borderRadius: 1,
        boxShadow: 3,
        width: "40vw",
        justifyItems: "center",
        alignItems: "center",
        margin: "2em",
      }}>
        <Typography variant="h3">
          Inbox
        </Typography>
      </Box>
      {shared_posts.map(async (share) => {
        const post = await get_post(share.photo_id);
        const sender = await get_user(share.receiver_id);
        return (
          <Paper key={share.share_id}>
            <Stack padding="1em" direction="row" spacing="1em" alignItems="center">
              <UserLink user={sender} />
              <Typography color="textSecondary">
                sends you post
              </Typography>
              <IconButton href={`/post/${post.photo.photo_id}`} sx={{ margin: 0, padding: 0, fontSize: "150%" }}>
                {post.photo.name}
                <Launch />
              </IconButton>
              <Typography color="textSecondary">
                from
              </Typography>
              <UserLink user={post.user} />
            </Stack>
          </Paper>
        );
      })}
    </Stack>
  );
}
