// File: user/page.tsx
// Author: Abas Hashmy (xhashm00)
// Date: 2024-12-13

import { SearchParams } from "@/actions";
import { UserGalleries } from "@/galleries/content";

// Displays galleries of a specific user.
export default async function User(
    { params, searchParams }: { params: Promise<{ user_id: string }>; searchParams: SearchParams }) {
    const user_id = Number((await params).user_id);
    const search_string = (await searchParams).search as string ?? "";
    return (
        <UserGalleries user_id={user_id} search_string={search_string} />
    );

}
