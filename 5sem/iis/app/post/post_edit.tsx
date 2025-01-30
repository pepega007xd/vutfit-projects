"use client";

import { useState } from "react";
import { Post } from "@/types";
import { Box, Button, Checkbox, Chip, Dialog, DialogActions, DialogContent, DialogTitle, FormControlLabel, IconButton, Stack, TextField } from "@mui/material";
import { Add, Edit } from "@mui/icons-material";
import { update_post } from "@/actions";

export default function PostEdit({ post }: { post: Post; }) {
  const [open, set_open] = useState(false);
  const [tags, set_tags] = useState(post.tags);
  const [new_tag, set_new_tag] = useState("");
  const [is_private, set_is_private] = useState(post.photo.is_private);

  return (
    <>
      <IconButton onClick={() => set_open(true)}>
        <Edit />
      </IconButton>

      <Dialog open={open} onClose={() => set_open(false)} >
        <Box component="form"
          action={async (form_data) => {
            const name = form_data.get("name") as string;
            const description = form_data.get("description") as (string | undefined);
            const location = form_data.get("location") as (string | undefined);
            const is_private = form_data.get("is_private") as string === "on";
            await update_post(post.photo.photo_id, name, tags, is_private, description, location);
            set_open(false);
          }}>
          <DialogTitle>Change username</DialogTitle>
          <DialogContent>
            <TextField
              name="name"
              defaultValue={post.photo.name}
              margin="dense"
              label="Name"
            />
            <TextField
              fullWidth
              multiline
              name="description"
              defaultValue={post.photo.description}
              margin="dense"
              label="Description"
            />
            <TextField
              name="location"
              defaultValue={post.photo.location}
              margin="dense"
              label="Location"
            />
            <Stack direction="row" spacing="1em" alignItems="center">
              {tags.map((tag) => <Chip label={tag} key={tag} onDelete={() => {
                set_tags(tags.filter((t) => t != tag))
              }} />)}
              <TextField
                label="Add a tag"
                variant="filled"
                value={new_tag}
                onChange={(e) => set_new_tag(e.target.value)} />
              <Box flex="none">
                <IconButton onClick={() => {
                  set_new_tag("");
                  if (!tags.includes(new_tag) && new_tag !== "") {
                    set_tags(tags.concat(new_tag))
                  }
                }}>
                  <Add />
                </IconButton>
              </Box>
            </Stack>
            <FormControlLabel
              control={
                <Checkbox checked={is_private}
                  onChange={() => set_is_private(!is_private)}
                  name="is_private" />
              }
              label="Private photo"
            />
          </DialogContent>
          <DialogActions>
            <Button type="submit"> Update </Button>
          </DialogActions>
        </Box>
      </Dialog>
    </>
  );
}
