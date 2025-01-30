import { get_feed_posts } from "@/actions";
import { PostList, SearchParams } from "@/post/post"

export default async function Home({ searchParams }: { searchParams: SearchParams }) {
  return (
    <PostList posts={await get_feed_posts()} searchParams={searchParams} />
  );
}
