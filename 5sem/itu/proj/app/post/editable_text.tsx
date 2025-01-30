// File: app/post/editable_text.tsx
// Author: Tomáš Brablec (xbrabl04)
// Date: 2024-12-13

"use client";

import { Check, Edit, LocalOffer, LocationOn } from "@mui/icons-material";
import { Chip, IconButton, Stack, TextField, Typography } from "@mui/material";
import { useState } from "react";
import { change_gallery_name, change_post_description, change_post_location, change_post_name, change_post_tags, change_subgallery_name } from "@/actions";
import TagList from "./tag_list";

interface EditableTextProps {
  initial_text: string;
  label: string;
  on_change: (new_value: string) => void;
  is_header?: boolean;
  is_description?: boolean;
  editable?: boolean;
  nonempty?: boolean;
}

// Component which displays text that can be edited using an edit button
// displayed next to it. The button can be conditionally hidden (`editable`).
// `on_change` is called when the text changes. The component can be visually adapted
// for different places using the other props.
function EditableText({ initial_text, label, on_change, is_header, is_description, editable, nonempty }: EditableTextProps) {
  // the current text in the component
  const [text, set_text] = useState(initial_text);

  // condition whether the component is currently in editing mode
  const [in_edit_mode, set_in_edit_mode] = useState(false);

  // when editable, show the component label in grey if the text is empty
  // to tell the user that there is an editable field here
  const text_element = text === ""
    ? editable && <Typography
      variant={is_header ? "h4" : "inherit"}
      sx={{ whiteSpace: "pre-line" }}
      color="textSecondary"
    >
      {label}
    </Typography>
    : <Typography
      variant={is_header ? "h4" : "inherit"}
      sx={{ whiteSpace: "pre-line" }}
    >
      {text}
    </Typography>;

  return (
    <Stack direction="row"
      spacing="1em"
      height={is_description ? "auto" : "3em"}
      alignItems="center"
    >
      {/* text element/text field */}
      {in_edit_mode
        ? <TextField
          label={label}
          value={text}
          autoFocus={true}
          onFocusCapture={(e) => {
            const input = (e.target as HTMLInputElement);
            input.setSelectionRange(input.value.length, input.value.length);
          }}
          multiline={is_description}
          onChange={(e) => set_text(e.target.value)}
        />
        : text_element
      }

      {/* button for entering/leaving edit mode */}
      {editable && (
        in_edit_mode
          ? <IconButton onClick={() => {
            set_in_edit_mode(false);

            if (nonempty && text === "") {
              set_text(initial_text);
            } else {
              on_change(text);
            }
          }}>
            <Check />
          </IconButton>

          : <IconButton onClick={() => {
            set_in_edit_mode(true);
          }}>
            <Edit />
          </IconButton>
      )}
    </Stack>
  )
}

// Component for displaying/editing the name of a post
export function PostName({ post_id, initial_text, editable }: { post_id: number, initial_text: string; editable?: boolean }) {
  return (
    <EditableText
      editable={editable}
      label="Name"
      nonempty
      is_header
      initial_text={initial_text}
      on_change={(name) =>
        change_post_name(post_id, name)
      }
    />
  );
}

// Component for displaying/editing the description of a post
export function PostDescription({ post_id, initial_text, editable }: { post_id: number, initial_text: string; editable?: boolean }) {
  return (
    <EditableText
      is_description
      editable={editable}
      initial_text={initial_text}
      label="Description"
      on_change={(description) =>
        change_post_description(post_id, description)
      }
    />
  );
}

// Component for displaying/editing the tags of a post
export function PostTags({ post_id, initial_tags, editable }: { post_id: number, initial_tags: string[]; editable?: boolean }) {
  return (editable
    ? <TagList
      in_post
      label="Add tag"
      initial_tags={initial_tags}
      on_change={(tags) => { change_post_tags(post_id, tags); }} />

    : <Stack direction="row" spacing="1em">
      {initial_tags.map((tag) =>
        <Chip
          icon={<LocalOffer fontSize="small" />}
          sx={{ paddingLeft: "0.5em" }}
          label={tag}
          key={tag}
        />
      )}
    </Stack>
  );
}

// Component for displaying/editing the location of a post
export function PostLocation({ post_id, initial_text, editable }: { post_id: number, initial_text: string; editable?: boolean }) {
  return (editable || initial_text !== "") && (
    <Stack direction="row" alignItems="center" spacing="1em">
      <LocationOn />
      <EditableText
        editable={editable}
        label="Location"
        initial_text={initial_text}
        on_change={(location) =>
          change_post_location(post_id, location)
        }
      />
    </Stack>
  );
}

// Component for displaying/editing the name of a gallery
// Author: Abas Hashmy (xhashm00)
export function GalleryTitle({ gallery_id, initial_text, editable }: { gallery_id: number, initial_text: string, editable?: boolean }) {
  return (
    <EditableText
      editable={editable}
      label="Gallery title"
      is_header initial_text={initial_text}
      on_change={(name) =>
        change_gallery_name(gallery_id, name)
      }
    />
  );
}

// Component for displaying/editing the name of a subgallery
// Author: Abas Hashmy (xhashm00)
export function SubGalleryTitle({ gallery_id, subgallery_id, initial_text, editable }:
  { gallery_id: number, subgallery_id: number, initial_text: string, editable?: boolean }) {
  return (
    <EditableText
      editable={editable}
      label="Subgallery title"
      is_header
      initial_text={initial_text}
      on_change={(name) =>
        change_subgallery_name(gallery_id, subgallery_id, name)
      }
    />
  );
}
