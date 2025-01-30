// File: app/post/arrange_options.tsx
// Author: Tomáš Brablec (xbrabl04)
// Date: 2024-12-13

"use client";

import { ExpandLess, ExpandMore, LocationOn, Search } from "@mui/icons-material";
import { Box, Collapse, IconButton, InputAdornment, MenuItem, Paper, Stack, TextField } from "@mui/material";
import { useRouter } from "next/navigation";
import { useState } from "react";
import { type ArrangeOptions } from "@/actions";
import TagList from "./tag_list";

// encodes ArrangeOptions as URL parameters
function get_search_params(options: ArrangeOptions): string {
  return "?" + new URLSearchParams([
    ["sort", options.sort],
    ["min_rating_filter", String(options.min_rating_filter)],
    ...options.location_filter.map(l => ["location_filter", l]),
    ...options.tag_filter.map(t => ["tag_filter", t]),
    ["search_filter", options.search_filter],
  ]).toString();
}

// Shows the sort and filtering options for a list of posts.
// The entered options are set as URL search parameters on every change
export default function Options({ arrange_options }: { arrange_options: ArrangeOptions }) {
  // current values for arrange options
  const [options, set_options] = useState(arrange_options);

  // condition whether to show additional options besides the main search bar
  const [show_extra_options, set_show_extra_options] = useState(false);

  const router = useRouter();

  // sets the new options as URL parameters and updates them in the component
  const update_options = (options: ArrangeOptions) => {
    set_options(options);
    router.push(get_search_params(options));
  };

  return (
    <Paper>
      <Stack margin="2em">
        <Stack direction="row" spacing="1em" alignItems="center">
          {/* button for showing more options */}
          <Box flex="none">
            <IconButton onClick={() => set_show_extra_options(!show_extra_options)}>
              {show_extra_options ? <ExpandLess /> : <ExpandMore />}
            </IconButton>
          </Box>

          {/* the main search bar - searches through all text inside the post */}
          <TextField
            label="Search posts"
            fullWidth
            value={options.search_filter}
            onChange={(e) => {
              const search_filter = e.target.value as string;
              update_options({ ...options, search_filter });
            }}
            slotProps={{
              input: {
                startAdornment:
                  <InputAdornment sx={{ color: "lightgrey" }} position="start">
                    <Search />
                  </InputAdornment>
              }
            }}
          />
        </Stack>

        <Collapse in={show_extra_options}>
          <Stack direction="row" spacing="2em" alignItems="center" marginTop="2em">
            {/* selection of sorting the posts */}
            <TextField
              value={options.sort}
              label="Sort by"
              select
              slotProps={{
                select: {
                  value: options.sort,
                }
              }}
              onChange={(e) => {
                const sort = e.target.value as string;
                update_options({ ...options, sort });
              }}
            >
              <MenuItem value="newest">Newest</MenuItem>
              <MenuItem value="best_rating">Best rating</MenuItem>
              <MenuItem value="most_views">Most views</MenuItem>
            </TextField>

            {/* textfield for filtering by minimum rating */}
            <TextField
              label="Minimum rating"
              sx={{ width: "10em" }}
              value={options.min_rating_filter}
              onChange={(e) => {
                const min_rating_filter = e.target.value
                  ? Number(e.target.value)
                  : 0;
                update_options({ ...options, min_rating_filter });
              }}
              inputMode="decimal"
              slotProps={{
                htmlInput: {
                  step: 10,
                  min: 0,
                  max: 100,
                  type: 'number',
                },
                input: {
                  endAdornment: <InputAdornment position="end">%</InputAdornment>
                }
              }}
            >
            </TextField>

            {/* list of tags to filter by - all tags must
              be present in post to show the post */}
            <TagList initial_tags={options.tag_filter} on_change={(tag_filter) =>
              update_options({ ...options, tag_filter })
            } />

            {/* list of locations to filter by - post must have one
              of these locations to show up */}
            <TagList
              initial_tags={options.location_filter}
              on_change={(location_filter) =>
                update_options({ ...options, location_filter })
              }
              label="Filter by location"
              icon={<LocationOn fontSize="small" />} />
          </Stack>
        </Collapse>
      </Stack >
    </Paper >
  );
}
