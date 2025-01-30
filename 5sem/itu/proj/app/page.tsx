// File: app/page.tsx
// Author: Tomáš Brablec (xbrabl04)
// Date: 2024-12-13

import { Stack } from "@mui/material";
import { type ArrangeOptions, get_posts, SearchParams } from "@/actions";
import Post from "@/post/post"
import Options from "@/post/arrange_options";

// Page showing a list of posts with options to 
// search, sort and filter posts in the list
export default async function Home({ searchParams }: { searchParams: SearchParams }) {
  const params = await searchParams;

  // converts search param to array
  const to_array = (x: string | string[] | undefined) =>
    typeof x === "string"
      ? [x]
      : x ?? [];

  // options to sort and filter posts
  const options: ArrangeOptions = {
    sort: (params.sort as string) ?? "newest",
    search_filter: (params.search_filter as string) ?? "",
    tag_filter: to_array(params.tag_filter),
    location_filter: to_array(params.location_filter),
    min_rating_filter: Number(params.min_rating_filter ?? "0")
  };

  // fetch posts from backend
  const posts = await get_posts(options);

  return (
    <Stack spacing="2em" margin="2em">
      <Options arrange_options={options} />
      {posts.map((p) => <Post post={p} key={p.post_id} />)}
    </Stack>
  );
}
