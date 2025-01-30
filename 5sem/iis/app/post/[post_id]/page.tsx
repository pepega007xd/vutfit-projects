import { get_post } from "@/actions";
import Post from "@/post/post";
import { Box } from "@mui/material";

export default async function SinglePostPage(
    { params }: { params: Promise<{ post_id: string }> }
) {
    const post_id = Number((await params).post_id);
    return (
        <Box justifyContent="center" display="flex" width="100vw" marginTop="2em">
            <Post post={await get_post(post_id)} />
        </Box>
    );
}
