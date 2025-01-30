"use client";

import { Box, Button, Checkbox, Chip, FormControlLabel, IconButton, ImageListItem, Stack, TextField } from "@mui/material";
import { useState } from "react";
import { Add, Upload } from "@mui/icons-material";
import { add_photo } from "@/actions";
import { fromURL } from "image-resize-compress";

export default function UploadForm() {
  const [photo, set_photo] = useState("");
  const [new_tag, set_new_tag] = useState("");
  const [tags, set_tags] = useState<string[]>([]);

  return (
    <Stack
      direction="row"
      component="form"
      width="100%"
      margin="2em"
      spacing="2em"
      action={async (form_data) => {
        const compressed = await fromURL(photo, 85, 1000, "auto", "webp");
        await add_photo(
          form_data.get("name") as string,
          form_data.get("description") as string,
          form_data.get("location") as string,
          tags,
          compressed,
          form_data.get("is_private") as string === "on",
        )
      }}
    >
      <Stack
        width="100%"
        spacing="2em"
      >
        <Button component="label" variant="contained" startIcon={<Upload />}>
          {photo.length > 0 ? "Change Image" : "Add Image"}
          <input type="file" hidden onChange={(event) => {
            if (!event.target.files) { return };
            set_photo(URL.createObjectURL(event.target.files[0]));
          }}
          />
        </Button>

        <ImageListItem key="photo">
          { // eslint-disable-next-line @next/next/no-img-element
            <img src={photo === "" ? undefined : photo} alt="" />}
        </ImageListItem>
      </Stack>
      <Stack
        width="100%"
        spacing="2em"
      >
        <TextField
          label="Name"
          variant="outlined"
          name="name"
          fullWidth={false}
          required
        />
        <TextField
          label="Description"
          variant="outlined"
          name="description"
          type="text"
          multiline
          fullWidth
          rows={4}
        />
        <TextField
          label="Location"
          variant="outlined"
          name="location"
          type="text"
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
          control={<Checkbox defaultChecked name="is_private" />}
          label="Private photo" />
        <Button
          variant="contained"
          type="submit"
        >
          Submit
        </Button>
      </Stack>
    </Stack>
  );
}
