// File: app/new_post/page.tsx
// Author: Tomáš Brablec (xbrabl04)
// Date: 2024-12-13

import { get_draft } from "@/actions";
import NewPost from "./new_post";

// Page for creating a new post - this exists just to fetch
// the draft in a server compoenent and avoid using useEffect
export default async function NewPostPage() {
  return (
    <NewPost initial_draft={await get_draft()} />
  );
}
