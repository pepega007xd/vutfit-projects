// File: app/new_post/new_post.tsx
// Author: Tomáš Brablec (xbrabl04)
// Date: 2024-12-13

"use client";

import { Box, Button, Collapse, IconButton, ImageList, ImageListItem, ImageListItemBar, InputAdornment, Paper, Stack, TextField, Typography } from "@mui/material";
import { useState } from "react";
import { Delete, ExpandLess, ExpandMore, LocationOn, Upload } from "@mui/icons-material";
import { create_post, Draft, update_draft as update_draft_action, remove_image, upload_image } from "@/actions";
import { fromBlob } from "image-resize-compress";
import TagList from "@/post/tag_list";

// used to clear the draft
const empty_draft = {
  name: "",
  description: "",
  tags: [],
  location: "",
  image_ids: []
}

// The whole page for adding a new post, it is separated to a client component to
// avoid using useEffect (`initial_draft` is passed from a server component).
export default function NewPost({ initial_draft }: { initial_draft: Draft }) {
  const [draft, set_draft] = useState<Draft>(initial_draft);

  // called each time the draft is updated -> sends the draft to the server
  const update_draft = async (draft: Draft) => {
    set_draft(draft);
    await update_draft_action(draft);
  }

  // condition for empty post name -> cannot create post, show error
  const [empty_name, set_empty_name] = useState(false);
  // condition for no images is most -> cannot create post, show error
  const [empty_images, set_empty_images] = useState(false);
  // condition for showing extra form fields
  const [show_more_properties, set_show_more_properties] = useState(false);

  return (
    <Stack margin="3em" spacing="3em" direction="row">
      <Paper sx={{ maxWidth: "50%" }}>
        <Stack alignItems="center" spacing="2em" margin="2em" minWidth="20em">
          {/* button for uploading images */}
          <Button component="label" variant="contained" startIcon={<Upload />} >
            Add Images
            <input type="file" hidden multiple onChange={async (event) => {
              if (!event.target.files) { return };
              const images = await Promise.all(Array.from(event.target.files).map(async (image) => {
                // compress images to a reasonable size before sending to server
                const compressed = await fromBlob(image, 85, 1000, "auto", "webp");
                const image_id = await upload_image(compressed);
                set_empty_images(false);
                return image_id;
              }));

              update_draft({ ...draft, image_ids: [...draft.image_ids, ...images] });
            }}
            />
          </Button>

          {empty_images &&
            <Typography color="error" variant="caption">
              Please add at least one image
            </Typography>
          }

          {/* list of uploaded images with an option to delete them */}
          <ImageList variant="masonry" cols={Math.min(draft.image_ids.length, 3)}>
            {draft.image_ids.map((image_id) => (
              <ImageListItem key={image_id}>
                <img src={`/image/${image_id}`} alt="draft image" />
                <ImageListItemBar
                  actionIcon={
                    <IconButton
                      onClick={async () => {
                        await remove_image(image_id);
                        update_draft({
                          ...draft,
                          image_ids: draft.image_ids.filter((id) => id != image_id)
                        });
                      }}>
                      <Delete />
                    </IconButton>
                  }
                />
              </ImageListItem>
            ))}
          </ImageList>
        </Stack>
      </Paper>

      <Paper sx={{ width: "100%" }}>
        <Stack component="form" spacing="1em" margin="2em">
          {/* name field */}
          <TextField
            label="Name"
            variant="outlined"
            fullWidth
            required
            error={empty_name}
            helperText={empty_name && "Please add a name"}
            value={draft.name}
            onChange={(event) => {
              update_draft({ ...draft, name: event.target.value });
              set_empty_name(false);
            }}
          />

          {/* collapse button for showing more fields */}
          <Stack direction="row" spacing="1em" alignItems="center">
            <Box flex="none">
              <IconButton onClick={() => set_show_more_properties(!show_more_properties)}>
                {show_more_properties ? <ExpandLess /> : <ExpandMore />}
              </IconButton>
            </Box>
            <Typography color="textSecondary">
              More properties
            </Typography>
          </Stack>

          <Collapse in={show_more_properties}>
            <Stack spacing="1em">
              {/* description field */}
              <TextField
                label="Description"
                variant="outlined"
                type="text"
                multiline
                fullWidth
                rows={4}
                value={draft.description}
                onChange={(event) => update_draft({ ...draft, description: event.target.value })}
              />

              {/* component for adding tags to post */}
              <TagList
                initial_tags={draft.tags}
                on_change={(tags) => update_draft({ ...draft, tags })}
                label="Add tag"
              />

              {/* location field */}
              <TextField
                label="Location"
                variant="outlined"
                fullWidth
                type="text"
                value={draft.location}
                onChange={(event) => update_draft({ ...draft, location: event.target.value })}
                slotProps={{
                  input: {
                    startAdornment:
                      <InputAdornment sx={{ color: "lightgrey" }} position="start">
                        <LocationOn />
                      </InputAdornment>
                  }
                }}
              />
            </Stack>
          </Collapse>

          <Stack direction="row" spacing="1em" width="100%" justifyContent="flex-end">
            {/* button for clearing the draft content */}
            <Button
              variant="outlined"
              color="error"
              onClick={async () => {
                draft.image_ids.forEach(remove_image);
                update_draft(empty_draft);
              }}
            >
              Clear Draft
            </Button>

            {/* button for creating the post */}
            <Button
              variant="contained"
              onClick={async () => {
                if (draft.name === "") {
                  set_empty_name(true);
                }

                if (draft.image_ids.length === 0) {
                  set_empty_images(true);
                }

                if (draft.name !== "" && draft.image_ids.length > 0) {
                  await create_post();
                }
              }}>
              Create Post
            </Button>
          </Stack>
        </Stack>
      </Paper>
    </Stack>
  );
}
