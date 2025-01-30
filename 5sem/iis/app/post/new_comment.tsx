"use client"

import { Add } from "@mui/icons-material";
import { Box, IconButton, Stack, TextField } from "@mui/material";
import { useState } from "react";
import { create_comment } from "@/actions";

export function NewComment({ photo_id: photo_id }: { photo_id: number }) {
  const [comment, set_comment] = useState("");

  return (
    <Stack direction="row" alignItems="center" spacing="1em">
      <TextField
        label="Add comment"
        value={comment}
        onChange={(e) => set_comment(e.target.value)}
        multiline
      />
      <Box flex="none">
        <IconButton onClick={() => {
          if (comment === "") { return; }
          set_comment("");
          create_comment(photo_id, comment)
        }}>
          <Add />
        </IconButton>
      </Box>
    </Stack>
  );
}

