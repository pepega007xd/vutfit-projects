// File: app/galleries/gallery/content.tsx
// Author: Abas Hashmy (xhashm00)
// Date: 2024-12-13

import * as React from 'react';
import ImageListItem from '@mui/material/ImageListItem';
import { get_post } from '@/actions';
import { ImageListItemBar } from '@mui/material';
import { DeletePostFromGallery, DeletePostFromSubGallery } from '@/galleries/gallery_components/components';

type gallery_post_props = {
  post_id: number;
  gallery_id: number;
  is_subgallery: boolean;
  subgallery_id: number;
  display_edit: boolean;
}

// GalleryPost displays all posts in a gallery or a subgallery.
export async function GalleryPost({ post_id, gallery_id, is_subgallery, subgallery_id, display_edit }: gallery_post_props) {
  const post = await get_post(post_id);

  return (
    <ImageListItem key={post_id}>
      <ImageListItem component="a" href={`/post/${post_id}`} sx={{ my: "auto", mx: "auto" }}>
        <img
          src={`/image/${post.image_ids[0]}`}
          loading="lazy"
        />
      </ImageListItem>
      <ImageListItemBar title={post.name} actionIcon={
        // Check if edit buttons should be visible.
        display_edit
          ?
          // Chceck which edit button to display.
          is_subgallery
            ?
            <DeletePostFromSubGallery post_id={post_id} gallery_id={gallery_id} subgallery_id={subgallery_id} />
            :
            <DeletePostFromGallery post_id={post_id} gallery_id={gallery_id} />
          :
          <></>
      } />
    </ImageListItem>

  );
}
