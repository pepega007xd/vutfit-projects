// File: app/post/[post_id]/page.tsx
// Author: Tomáš Brablec (xbrabl04)
// Date: 2024-12-13

import { get_post } from "@/actions";
import Post from "@/post/post";
import { Box } from "@mui/material";

// Renders a single post on a page based on the `post_id` URL parameter
export default async function SinglePostPage(
    { params }: { params: Promise<{ post_id: string }> }
) {
    const post_id = Number((await params).post_id);
    return (
        <Box margin="2em">
            <Post post={await get_post(post_id)} is_single_page />
        </Box>
    );

}
