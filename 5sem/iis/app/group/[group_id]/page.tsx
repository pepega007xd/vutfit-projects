import { get_group_posts } from "@/actions";
import { PostList, SearchParams } from "@/post/post";

export default async function ListOfGroupPosts(
    { params, searchParams }: { params: Promise<{ group_id: string }>, searchParams: SearchParams }
) {
    const group_id = Number((await params).group_id);
    return (
        <PostList group_id={group_id} posts={await get_group_posts(group_id)} searchParams={searchParams} />
    );
}
