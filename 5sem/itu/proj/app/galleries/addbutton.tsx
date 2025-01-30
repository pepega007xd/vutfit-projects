// File: app/galleries/addbutton.tsx
// Author: Abas Hashmy (xhashm00)
// Date: 2024-12-13

"use client"

import { add_post_to_gallery, add_post_to_subgallery, Gallery, get_current_user, get_subgalleries, get_user_galleries, Post, remove_post_from_gallery, remove_post_from_subgallery, Subgallery } from "@/actions";
import { Collections } from "@mui/icons-material";
import { Button, Collapse, IconButton, List, ListItem, ListItemButton, ListItemText, Popover } from "@mui/material";
import React, { useEffect, useState } from "react";
import ExpandLess from '@mui/icons-material/ExpandLess';
import ExpandMore from '@mui/icons-material/ExpandMore';
import { it } from "node:test";

// AddButton is a function for adding posts into galleries or sub_galleries.
export default function AddButton({ post }: { post: Post }) {
  // An anchor for the popups position in the site window.
  const [anchorEl, setAnchorEl] = useState<HTMLButtonElement | null>(null);
  // State to hold all the current galleries. 
  const [galleries, setGalleries] = useState<Gallery[]>([])

  // Async function that gets all of the users galleries.
  const fetch_galleries = async () => {
    const user = await get_current_user();
    const store_galleries = await get_user_galleries(user.user_id, false, "");
    setGalleries(store_galleries);
  }

  // useEffect needs to be used to call the async function inside a client component because it runs it after the component is rendered.
  useEffect(() => {
    fetch_galleries();
  }, [post.post_id]);
  return (
    <>
      {/*Icon for the add button*/}
      <IconButton onClick={(event) => setAnchorEl(event.currentTarget)} >
        <Collections />
      </IconButton>

      {/*Popup that displayes a list of users galleries at the positio of the add button*/}
      <Popover
        open={Boolean(anchorEl)}
        onClose={() => setAnchorEl(null)}
        anchorEl={anchorEl}
        anchorOrigin={{
          vertical: 'bottom',
          horizontal: 'center',
        }}
        transformOrigin={{
          vertical: 'top',
          horizontal: 'center',
        }}
      >
        {/*List of all galleries that can be oppened to display buttons for adding post to gallery of it's subgalleries*/}
        <List sx={{ overflow: 'auto', maxHeight: "50vh" }}>
          {galleries.map((gallery) => (
            <AddButtonCollapse gallery={gallery} fetch_galleries={fetch_galleries} post={post} key={gallery.gallery_id} />
          ))}
        </List>
      </Popover >
    </>
  );
}

type collapse_props = {
  gallery: Gallery;
  fetch_galleries: () => Promise<void>;
  post: Post
}

function AddButtonCollapse({ gallery, fetch_galleries, post }: collapse_props) {
  // State to hold if the collapsing element of a single gallery is open or closed.
  const [open, setOpen] = useState(false);
  // State to hold all of the current subgalleries..
  const [sub_galleries, set_sub_galleries] = useState<Subgallery[]>([])

  // Async function that gets all the subgalleries of the specified gallery.
  const fetch_subgalleries = async () => {
    const store_galleries = await get_subgalleries(gallery.gallery_id, "");
    set_sub_galleries(store_galleries);
  }

  // useEffect needs to be used to call the async function inside a client component because it runs it after the component is rendered.
  useEffect(() => {
    fetch_subgalleries();
  }, [post.post_id]);

  // Opening or closing options to add a post to gallery or subgalleries.
  const handleClick = () => {
    setOpen(!open);
  }

  return (
    <ListItem key={gallery.gallery_id}>
      <List sx={{ minWidth: "20em" }}>
        {/*Collapse button to show option to add or remove post from specified gallery or it's subgalleries*/}
        <ListItemButton onClick={handleClick}>
          <ListItemText primary={gallery.name} />
          {open ? <ExpandLess /> : <ExpandMore />}
        </ListItemButton>
        {/*Collapsing element for a gallery*/}
        <Collapse in={open} timeout="auto" unmountOnExit>
          {/*Button to add a post to a gallery*/}
          <ListItem secondaryAction={
            gallery.posts.includes(post.post_id)
              ? <Button variant="outlined" onClick={() => {
                remove_post_from_gallery(post.post_id, gallery.gallery_id);
                fetch_galleries();
              }}>
                Remove
              </Button>
              : <Button variant="outlined" onClick={() => {
                add_post_to_gallery(post.post_id, gallery.gallery_id);
                fetch_galleries();
              }}>
                Add
              </Button>
          }>
            <ListItemText>
              {gallery.name}
            </ListItemText>
          </ListItem>
          {/*Buttons to add a post to any of the gallery's subgalleries*/}
          {sub_galleries.map((subgallery) => (
            <ListItem
              key={subgallery.subgallery_id}
              secondaryAction={
                subgallery.posts.includes(post.post_id)
                  ? <Button variant="outlined" onClick={async () => {
                    remove_post_from_subgallery(post.post_id, gallery.gallery_id, subgallery.subgallery_id);
                    await fetch_subgalleries();
                  }}>
                    Remove
                  </Button>
                  : <Button variant="outlined" onClick={async () => {
                    add_post_to_subgallery(post.post_id, gallery.gallery_id, subgallery.subgallery_id);
                    await fetch_subgalleries();
                  }}>
                    Add
                  </Button>
              }>
              <ListItemText>
                {subgallery.name}
              </ListItemText>
            </ListItem>
          ))}
        </Collapse>
      </List>
    </ListItem>

  );
}
