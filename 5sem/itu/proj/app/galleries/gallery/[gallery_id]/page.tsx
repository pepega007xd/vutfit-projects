// File: app/galleries/gallery/page.tsx
// Author: Abas Hashmy (xhashm00)
// Date: 2024-12-13

import { ImageList, Stack } from "@mui/material";
import { get_current_user, get_gallery, get_gallery_posts, get_subgalleries, SearchParams } from "@/actions";
import { DeleteGallery, Gallery_header, New_SubGallery_button, Toggle_gallery_visibility } from "@/galleries/gallery_components/components";
import { GalleryPost } from "./content";
import GalleriesList from "@/galleries/content";

// A single open gallery
export default async function Gallery({ params, searchParams }: { params: Promise<{ gallery_id: string }>, searchParams: SearchParams }) {
  const gallery_id = Number((await params).gallery_id);
  const gallery = await get_gallery(gallery_id);
  const search_string = (await searchParams).search as string ?? "";
  const subgalleries = await get_subgalleries(gallery_id, search_string);
  const current_user = await get_current_user();
  // Check if the current user is also a gallery owner, if so display_edit will be true.
  const display_edit = current_user.user_id == gallery.owner_id;
  const is_public = gallery.is_public;
  const post_ids = await get_gallery_posts(gallery_id, search_string);


  return (
    <Stack sx={{ justifyItems: "center", mx: "auto" }} >
      <Gallery_header gallery_id={gallery_id} header={gallery.name} display_edit={display_edit} />
      <Stack mx="auto" direction="row" spacing="3em" my="3em">
        {
          display_edit
            ?
            <>
              <New_SubGallery_button parent_gallery={gallery_id} />
              <Toggle_gallery_visibility gallery_id={gallery_id} is_public={is_public} />
              <DeleteGallery gallery_id={gallery_id} />
            </>
            :
            <></>
        }
      </Stack>
      {/*GalleriesList displays subgalleries in a gallery*/}
      <GalleriesList
        content={subgalleries}
        current_gallery_id={gallery_id}
        display_edit={display_edit}
      />
      <ImageList variant="masonry" cols={3} gap={20} sx={{ px: "5vw" }}>
        {post_ids.map((post_id) =>
          // is_subgallery is false, because this component is an already open gallery so it will never be a subgallery
          <GalleryPost
            post_id={post_id}
            gallery_id={gallery_id}
            key={post_id}
            is_subgallery={false}
            subgallery_id={0}
            display_edit={display_edit}
          />
        )}
      </ImageList>

    </Stack>
  );
}
