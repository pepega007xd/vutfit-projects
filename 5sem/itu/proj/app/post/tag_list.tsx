// File: app/post/tag_list.tsx
// Author: Tomáš Brablec (xbrabl04)
// Date: 2024-12-13

"use client";

import { Add, LocalOffer } from "@mui/icons-material";
import { Box, Chip, IconButton, InputAdornment, Paper, Stack, TextField } from "@mui/material";
import { ReactElement, useState } from "react";

// Component for showing a list of tags with an option to add or remove tags.
// Action of changing a list of tags is exposed to the caller using `on_change`.
// If `in_post` is true, the list is displayed without a background to fit into
// the side panel of the post.
export default function TagList({ initial_tags, on_change, label, icon, in_post }: { initial_tags: string[]; on_change: (new_tags: string[]) => any; label?: string; icon?: ReactElement; in_post?: boolean }) {
  // list of tags to be displayed
  const [tags, set_tags] = useState<string[]>(initial_tags);

  // text content of the new tag to be created
  const [new_tag, set_new_tag] = useState("");

  const update_tags = (tags: string[]) => {
    set_tags(tags);
    on_change(tags);
  };

  icon = icon ?? <LocalOffer fontSize="small" />;

  const component = (
    <Stack margin="1em" spacing="1em" direction={in_post ? "column" : "row"} alignItems={in_post ? "inherit" : "center"}>
      <Stack direction="row" spacing="1em" alignItems="center">
        {/* textbox for adding a new tag */}
        <TextField
          label={label ?? "Filter by tag"}
          value={new_tag}
          onChange={(e) => set_new_tag(e.target.value)}
          slotProps={{
            input: {
              startAdornment: <InputAdornment sx={{ color: "lightgrey" }} position="start">{icon}</InputAdornment>
            }
          }}
        />

        {/* Button for adding a new tag */}
        <Box flex="none">
          <IconButton
            onClick={() => {
              set_new_tag("");
              if (!tags.includes(new_tag) && new_tag !== "") {
                update_tags(tags.concat(new_tag))
              }
            }}>
            <Add />
          </IconButton>
        </Box>
      </Stack>

      {/* List of tags */}
      <Stack direction="row" spacing="1em">
        {
          tags.map((tag) => <Chip
            icon={icon ?? <LocalOffer fontSize="small" />}
            sx={{ paddingLeft: "0.5em" }}
            label={tag}
            key={tag}
            onDelete={() =>
              update_tags(tags.filter((t) => t !== tag))
            } />)
        }
      </Stack>
    </Stack>
  );

  return (in_post
    ? component
    : <Paper elevation={5}>
      {component}
    </Paper>
  );
}
