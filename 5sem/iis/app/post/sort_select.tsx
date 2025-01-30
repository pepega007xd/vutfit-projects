"use client";

import { Add } from "@mui/icons-material";
import { Box, Chip, IconButton, MenuItem, Stack, TextField } from "@mui/material";
import { useRouter } from "next/navigation";
import { useState } from "react";

function get_search_params(sort: string, filter_tags: string[]): string {
  return "?" + new URLSearchParams([
    ["sort", sort],
    ...filter_tags.map(tag => ["filter_tags", tag])
  ]).toString();

}

export default function SortSelect(
  { sort, filter_tags }: { sort: string, filter_tags: string[] }) {

  const [new_tag, set_new_tag] = useState("");
  const router = useRouter();

  return (
    <Stack sx={{
      backgroundColor: '#2B2B2B',
      padding: 2,
      borderRadius: 1,
      boxShadow: 3,

    }} direction="row" alignItems="center" spacing="2em">
      <TextField
        value={sort}
        label="Sort by"
        select
        slotProps={{
          select: {
            value: sort,
          }
        }}
        onChange={(e) => {
          const new_sort = e.target.value as string;
          router.push(get_search_params(new_sort, filter_tags));
        }}
      >
        <MenuItem value="newest">Newest</MenuItem>
        <MenuItem value="best_rating">Best rating</MenuItem>
      </TextField>

      <TextField
        label="Filter by tag"
        value={new_tag}
        onChange={(e) => set_new_tag(e.target.value)}
      />
      <Box flex="none">
        <IconButton

          onClick={() => {
            set_new_tag("");
            if (!filter_tags.includes(new_tag) && new_tag !== "") {
              router.push(get_search_params(sort, filter_tags.concat(new_tag)));
            }
          }}>
          <Add />
        </IconButton>
      </Box>
      {
        filter_tags.map((tag) => <Chip label={tag} key={tag} onDelete={() => {
          const new_tags = filter_tags.filter((t) => t !== tag)
          router.push(get_search_params(sort, new_tags));

        }} />)
      }

    </Stack >
  );
}
