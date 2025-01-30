
import { PostList, SearchParams } from "@/post/post";
import UserDetails from "./profile";
import { get_user_posts } from "@/actions";


export default async function SinglePostPage(
    { params, searchParams }:
        { params: Promise<{ user_id: string }>, searchParams: SearchParams }
) {
    const userId = await params;
    return (
        <div>
            <UserDetails userId={Number(userId.user_id)} />
            <PostList posts={await get_user_posts(Number(userId.user_id))} searchParams={searchParams} />
        </div>
    );
}
