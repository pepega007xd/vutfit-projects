// File: app/galleries/gallery_components/components.tsx
// Author: Abas Hashmy (xhashm00)
// Date: 2024-12-13

"use client"

import { change_gallery_visibility, create_gallery, create_subgallery, delete_gallery, delete_subgallery, get_gallery_posts, remove_post_from_gallery, remove_post_from_subgallery } from "@/actions";
import { Delete } from "@mui/icons-material";
import { Box, Fab, IconButton, TextField, Typography } from "@mui/material";
import { useRouter } from "next/navigation";
import { useState } from "react";
import AddIcon from '@mui/icons-material/Add';
import LockIcon from '@mui/icons-material/Lock';
import LockOpenIcon from '@mui/icons-material/LockOpen';
import { GalleryTitle, SubGalleryTitle } from "@/post/editable_text";

type header_props = {
  gallery_id?: number;
  subgallery_id?: number;
  header: string;
  display_edit: boolean;
}

// Gallery_header displays the gallery title and a searchfiled for filtering galleries or subgalleries.
export function Gallery_header({ gallery_id, subgallery_id, header, display_edit }: header_props) {
  // State to save intupted search string.
  const [search, setSearch] = useState("");
  // Router is needed to push the search string trought the site URL.
  const router = useRouter();
  return (
    <Box sx={{
      width: "50em",
      display: "flex",
      flexDirection: "column",
      alignItems: "center",
      gap: 2,
      backgroundColor: '#2B2B2B',
      padding: 4,
      borderRadius: '8px',
      mx: "auto"
    }} >

      {/*Box for gallery or subgallery title*/}
      <Box sx={{ width: "40em", display: "flex", flexDirection: "column", alignItems: "center" }}>
        {
          gallery_id == undefined
            ?
            <Typography variant="h4">{header}</Typography>
            :
            subgallery_id == undefined
              ?
              <GalleryTitle gallery_id={gallery_id!} initial_text={header} editable={display_edit} />
              :
              <SubGalleryTitle gallery_id={gallery_id} subgallery_id={subgallery_id} initial_text={header} editable={display_edit} />
        }
      </Box>
      {/*Search filed.*/}
      <TextField sx={{ width: "40em", backgroundColor: "black" }}
        label="Search field"
        type="search"
        variant="filled"
        value={search}
        onChange={(e) => {
          // When something in searchfiled changes setSearch will hold that change.
          setSearch(e.target.value);
          // Pushing the inputed string to URL.
          router.push("?" + new URLSearchParams([["search", e.target.value]]));
        }}
      />

    </Box>

  );
}

// Button that deletes a specific post from a specific gallery.
export function DeletePostFromGallery({ post_id, gallery_id }: { post_id: number, gallery_id: number }) {
  return (
    <IconButton onClick={async () => {
      await remove_post_from_gallery(post_id, gallery_id);
    }}>
      <Delete />
    </IconButton>
  );

}

// Button that deletes a specific post from a specific subgallery.
export function DeletePostFromSubGallery(
  { post_id, gallery_id, subgallery_id }: { post_id: number, gallery_id: number, subgallery_id: number }) {
  return (
    <IconButton onClick={async () => {
      await remove_post_from_subgallery(post_id, gallery_id, subgallery_id);
    }}>
      <Delete />
    </IconButton>
  )
}

// Button that deletes a subgallery and redirects the user to a parent gallery.
export function DeleteSubGallery({ gallery_id, subgallery_id }: { gallery_id: number, subgallery_id: number }) {
  const router = useRouter();
  return (
    <Fab color="primary" aria-label="Delete gallery" onClick={async () => {
      await delete_subgallery(gallery_id, subgallery_id);
      router.push(`/galleries/gallery/${gallery_id}`);
    }}>
      <Delete />
    </Fab>

  );
}

// Button that deletes a gallery and redirects the user to all their galleries.
export function DeleteGallery({ gallery_id }: { gallery_id: number }) {
  const router = useRouter();
  return (
    <Fab color="primary" aria-label="Delete gallery" onClick={async () => {
      await delete_gallery(gallery_id);
      router.push("/galleries");
    }}>
      <Delete />
    </Fab>
  );
}

// Button to create a new gallery.
export function New_Gallery_Button() {
  const handleClick = () => {
    create_gallery("new gallery", true);
  };
  return (
    <Fab color="primary" aria-label="add" onClick={handleClick}>
      <AddIcon />
    </Fab>
  );
}

// Button to create a new subgallery.
export function New_SubGallery_button({ parent_gallery }: { parent_gallery: number }) {
  const handleClick = () => {
    create_subgallery(parent_gallery, "new subgallery");
  };
  return (
    <Fab color="primary" aria-label="add" onClick={handleClick}>
      <AddIcon />
    </Fab>
  );
}

// This function checks the current gallery visibility and displays a button that changes it to the other.
export function Toggle_gallery_visibility({ gallery_id, is_public }: { gallery_id: number, is_public: boolean }) {
  return (
    <>
      {
        is_public
          ?
          <Fab color="primary" aria-label="toggle" onClick={async () => {
            await change_gallery_visibility(gallery_id, !is_public);
            get_gallery_posts(gallery_id, "");
          }}>
            <LockOpenIcon />
          </Fab>
          :
          <Fab color="primary" aria-label="toggle" onClick={async () => {
            await change_gallery_visibility(gallery_id, !is_public);
          }}>
            <LockIcon />
          </Fab>
      }
    </>
  );
}


