// File: app/public/page.tsx
// Author: Abas Hashmy (xhashm00)
// Date: 2024-12-13

import { get_public_galleries, SearchParams } from "@/actions";
import { DisplayGalleries } from "@/galleries/content";

// Displays all public galleries of all users.
export default async function PublicGalleries({ searchParams }: { searchParams: SearchParams }) {
  const search_string = (await searchParams).search as string ?? "";

  const public_galleries = await get_public_galleries(search_string);
  return (
    <DisplayGalleries
      galleries={public_galleries}
      only_public
      header="Public Galleries"
    />
  );
}
