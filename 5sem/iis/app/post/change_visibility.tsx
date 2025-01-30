"use client";

import { Button, Checkbox, Dialog, DialogContent, DialogTitle, Stack, Typography } from "@mui/material";
import { useState } from "react";
import { Groups, Post, Users } from "@/types";
import { change_group_visibility, change_user_visibility } from "@/actions";

export default function ChangeVisibility({ post, user_access, group_access }: { post: Post, user_access: [Users, boolean][], group_access: [Groups, boolean][] }) {
  const [open, set_open] = useState(false);
  return (
    <>
      <Button onClick={() => set_open(!open)} >Change Visibility</Button>
      <Dialog open={open} onClose={() => set_open(false)}>
        <Stack direction="row">
          <Stack>
            <DialogTitle>User access</DialogTitle>
            <DialogContent>
              {user_access.map(([user, has_access]) =>
                <Stack key={user.user_id} direction="row" alignItems="center">
                  <Checkbox checked={has_access} onClick={async () => {
                    await change_user_visibility(post.photo.photo_id, user.user_id, has_access);
                  }} />
                  <Typography>{user.username}</Typography>
                </Stack>
              )}
            </DialogContent>
          </Stack>
          <Stack>
            <DialogTitle>Group access</DialogTitle>
            <DialogContent>
              {group_access.map(([group, has_access]) =>
                <Stack key={group.group_id} direction="row" alignItems="center">
                  <Checkbox checked={has_access} onClick={async () => {
                    await change_group_visibility(post.photo.photo_id, group.group_id, has_access);
                  }} />
                  <Typography>{group.group_name}</Typography>
                </Stack>
              )}
            </DialogContent>
          </Stack>
        </Stack>
      </Dialog>
    </>
  );
}
