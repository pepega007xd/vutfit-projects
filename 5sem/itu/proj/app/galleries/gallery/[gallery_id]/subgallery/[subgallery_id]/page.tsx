// File: app/galleries/gallery/subgallery/page.tsx
// Author: Abas Hashmy (xhashm00)
// Date: 2024-12-13

import { get_current_user, get_gallery, get_subgallery, get_subgallery_posts, SearchParams } from "@/actions";
import { DeleteSubGallery, Gallery_header } from "@/galleries/gallery_components/components";
import { ImageList, Stack } from "@mui/material";
import { GalleryPost } from "../../content";

// Displays a subgallery. Header, delete button and subgallery images.
export default async function Subgallery({ params, searchParams }: { params: Promise<{ gallery_id: string, subgallery_id: string }>, searchParams: SearchParams }) {
  const awaited_params = await params;
  const gallery_id = Number(awaited_params.gallery_id);
  const subgallery_id = Number(awaited_params.subgallery_id);

  // Check if current user is also a gallery owner, if so display_edit will be true.
  const gallery = await get_gallery(gallery_id);
  const current_user = await get_current_user();
  const display_edit = current_user.user_id == gallery.owner_id;

  const search_string = (await searchParams).search as string ?? "";
  const post_ids = await get_subgallery_posts(gallery_id, subgallery_id, search_string);
  const subgallery = await get_subgallery(gallery_id, subgallery_id);
  return (
    <Stack sx={{ justifyItems: "center", display: "flex", placeItems: "center", mx: "auto" }}>
      <Gallery_header gallery_id={gallery_id} subgallery_id={subgallery_id} header={subgallery.name} display_edit={display_edit} />
      <Stack mx="auto" direction="row" spacing="3em" my="3em">
        {// && checks if display_edit is true, if so it will display the delete button
          display_edit &&
          <DeleteSubGallery gallery_id={gallery_id} subgallery_id={subgallery_id} />
        }
      </Stack>
      <ImageList variant="masonry" cols={3} gap={20} sx={{ px: "5vw" }}>
        {post_ids.map((post_id) =>
          <GalleryPost
            post_id={post_id}
            gallery_id={gallery_id}
            key={post_id}
            is_subgallery={true}
            subgallery_id={subgallery_id}
            display_edit={display_edit}
          />
        )}
      </ImageList>
    </Stack>
  );
}
