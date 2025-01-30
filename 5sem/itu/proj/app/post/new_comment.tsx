// File: app/post/new_comment.tsx
// Author: Tomáš Brablec (xbrabl04)
// Date: 2024-12-13

"use client"

import { Add } from "@mui/icons-material";
import { Box, IconButton, Stack, TextField } from "@mui/material";
import { useState } from "react";
import { create_comment } from "@/actions";

// Component for adding a new comment
// (extracted to another file to use useState)
export function NewComment({ post_id }: { post_id: number }) {
  const [comment, set_comment] = useState("");

  return (
    <Stack direction="row" alignItems="center" spacing="1em" margin="1em">
      {/* text field for entering the comment */}
      <TextField
        label="Add comment"
        value={comment}
        onChange={(e) => set_comment(e.target.value)}
        multiline
      />

      {/* button for creating the comment */}
      <Box flex="none">
        <IconButton onClick={() => {
          if (comment === "") { return; }
          set_comment("");
          create_comment(post_id, comment)
        }}>
          <Add />
        </IconButton>
      </Box>
    </Stack>
  );
}

